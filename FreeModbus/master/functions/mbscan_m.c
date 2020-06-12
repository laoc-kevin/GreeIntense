#include "mbport_m.h"
#include "mb_m.h"
#include "mbframe.h"

#include "mbfunc_m.h"
#include "mbdict_m.h"
#include "mbtest_m.h"
#include "mbscan_m.h"

#include "system.h"

#define MB_SCAN_SLAVE_DELAY_MS             50    //主栈扫描从设备
#define MB_SCAN_SLAVE_INTERVAL_MS          50

/**********************************************************************
 * @brief   主栈轮询从设备任务
 * @param   *p_arg    
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBMasterScanSlaveDevTask(void *p_arg)
{
	UCHAR n, iIndex, ucSlaveAddr;
    
	OS_ERR err = OS_ERR_NONE;

	USHORT msReadInterval = MB_SCAN_SLAVE_INTERVAL_MS;

    eMBMasterReqErrCode   errorCode = MB_MRE_NO_ERR;
    sMBSlaveDev*       psMBSlaveDev = NULL;
    
	sMBMasterInfo*       psMBMasterInfo = (sMBMasterInfo*)p_arg;
    sMBMasterDevsInfo*   psMBDevsInfo   = &psMBMasterInfo->sMBDevsInfo;  //从设备状态信息
    sMBMasterPort*       psMBPort       = &psMBMasterInfo->sMBPort;
    
    
    UCHAR ucMaxAddr = psMBDevsInfo->ucSlaveDevMaxAddr;
    UCHAR ucMinAddr = psMBDevsInfo->ucSlaveDevMinAddr;
    
    UCHAR ucAddrSub = ucMaxAddr - ucMinAddr;  //设备地址差
	BOOL* pxDevAddrOccupy = psMBDevsInfo->xDevAddrOccupy;          //被占用的从设备通讯地址
    
    (void)OSTimeDlyHMSM(0, 0, 0, msReadInterval, OS_OPT_TIME_HMSM_STRICT, &err);
    
    /*************************首次上电后先对从设备进行在线测试，主要收集各从设备通讯地址和在线状态**********************/
    for(psMBSlaveDev = psMBDevsInfo->psMBSlaveDevsList; psMBSlaveDev != NULL; psMBSlaveDev = psMBSlaveDev->pNext)
    { 
        for(ucSlaveAddr = ucMinAddr; ucSlaveAddr <= ucMaxAddr; ucSlaveAddr++)
        {    
            if(pxDevAddrOccupy[ucSlaveAddr-ucMinAddr] == FALSE)         //该地址未被占用
            {
                vMBDevTest(psMBMasterInfo, psMBSlaveDev, ucSlaveAddr);  //确定从设备参数类型测试和设备通讯地址
                if(psMBSlaveDev->xOnLine == TRUE)
                {
                    pxDevAddrOccupy[ucSlaveAddr-ucMinAddr] = TRUE;  //从设备通讯地址占用
                    break;
                }                            
            }
        }
    }
	while (DEF_TRUE)
	{
        (void)OSTimeDlyHMSM(0, 0, 0, msReadInterval, OS_OPT_TIME_HMSM_STRICT, &err);
        
//        myprintf("****************************************************************************\n");   
#if MB_MASTER_DTU_ENABLED > 0    //GPRS模块功能支持，特殊处理      
        if( (psMBMasterInfo->bDTUEnable == TRUE) && (psMBMasterInfo->pvDTUScanDevCallBack != NULL))
        {
             psMBMasterInfo->pvDTUScanDevCallBack(psMBMasterInfo);
        }   
#endif
		/*********************************测试从设备***********************************/
		for(psMBSlaveDev = psMBDevsInfo->psMBSlaveDevsList; psMBSlaveDev != NULL; psMBSlaveDev = psMBSlaveDev->pNext)
        {
            if(psMBSlaveDev->xOnLine == FALSE)   //如果设备不在线
            {
                if(psMBSlaveDev->ucDevCurTestAddr == 0)  //刚掉线,从当前地址开始测试，一个周期只测试一个地址
                {
                    psMBSlaveDev->ucDevCurTestAddr = (psMBSlaveDev->ucDevAddr < ucMinAddr) ? ucMinAddr:psMBSlaveDev->ucDevAddr;   
                }
                if(psMBSlaveDev->ucDevCurTestAddr > ucMaxAddr)
                {
                    psMBSlaveDev->ucDevCurTestAddr = ucMinAddr;  //超过最大地址则重新从最小地址开始测试
                }                    
                if(pxDevAddrOccupy[psMBSlaveDev->ucDevCurTestAddr-ucMinAddr] == FALSE)  //该地址未被占用
                {
                    vMBDevTest(psMBMasterInfo, psMBSlaveDev, psMBSlaveDev->ucDevCurTestAddr);  //测试
                    if(psMBSlaveDev->xOnLine == TRUE)
                    {
                        pxDevAddrOccupy[psMBSlaveDev->ucDevCurTestAddr-ucMinAddr] = TRUE;  //从设备通讯地址占用
                        break;
                    }                      
                }
                psMBSlaveDev->ucDevCurTestAddr++;
            }
        }
        /*********************************轮询从设备***********************************/
        for(psMBSlaveDev = psMBDevsInfo->psMBSlaveDevsList; psMBSlaveDev != NULL; psMBSlaveDev = psMBSlaveDev->pNext)
        {    
            if(psMBSlaveDev->xOnLine == TRUE && psMBSlaveDev->ucDevAddr <= ucMaxAddr && psMBSlaveDev->ucDevAddr >= ucMinAddr )
            {
                vMBDevCurStateTest(psMBMasterInfo, psMBSlaveDev);  //检测从设备是否掉线
                if( (psMBSlaveDev->xOnLine == TRUE) && (psMBSlaveDev->ucOfflineTimes == 0) ) //在线且不处于延时阶段
                {                  
                    vMBMasterScanSlaveDev(psMBMasterInfo, psMBSlaveDev);
                }
                else if(psMBSlaveDev->xOnLine == FALSE)
                {
                    pxDevAddrOccupy[psMBSlaveDev->ucDevAddr-ucMinAddr] = FALSE;
                }
//                myprintf("vMBDevCurStateTest  %d  psMBSlaveDev->xOnLine %d\n", psMBSlaveDev->ucDevAddr, psMBSlaveDev->xOnLine);                 
            }          
        }      
	}
}

/**********************************************************************
 * @brief   创建主栈轮询从设备任务
 * @param   *p_arg    
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
BOOL xMBMasterCreateScanSlaveDevTask(sMBMasterInfo* psMBMasterInfo)
{   
    OS_ERR               err = OS_ERR_NONE;
    CPU_STK_SIZE    stk_size = MB_MASTER_SCAN_TASK_STK_SIZE; 
    
    sMBMasterTask* psMBTask = &(psMBMasterInfo->sMBTask);
    
    OS_PRIO             prio = psMBTask->ucMasterScanPrio;
    OS_TCB*            p_tcb = (OS_TCB*)(&psMBTask->sMasterScanTCB);  
    CPU_STK*      p_stk_base = (CPU_STK*)(psMBTask->usMasterScanStk);
   
    OSTaskCreate(p_tcb, "vMBMasterScanSlaveDevTask", vMBMasterScanSlaveDevTask, (void*)psMBMasterInfo, prio, p_stk_base,
                 stk_size/10u, stk_size, 0u, 0u, 0u, (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &err);
    return (err == OS_ERR_NONE);              
}

/**********************************************************************
 * @brief   主栈轮询某个从设备
 * @param   psMBMasterInfo  主栈信息块 
 * @param   psMBSlaveDev    从设备
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBMasterScanSlaveDev(sMBMasterInfo* psMBMasterInfo, sMBSlaveDev* psMBSlaveDev)
{
    eMBMasterReqErrCode errorCode    = MB_MRE_NO_ERR;
    sMBMasterDevsInfo*  psMBDevsInfo = &psMBMasterInfo->sMBDevsInfo;      //从设备列表
    UCHAR               ucSlaveAddr  = psMBSlaveDev->ucDevAddr;           //通讯地址

    psMBDevsInfo->psMBSlaveDevCur = psMBSlaveDev;                         //当前从设备
    if(psMBDevsInfo->psMBSlaveDevCur->psDevCurData == NULL)               //数据表为空则不进行轮询
    {
        return;
    }
    if( (psMBSlaveDev != NULL) && (psMBSlaveDev->xOnLine == TRUE) )       //如果设备在线则进行轮询
    {
        if(psMBSlaveDev->xDataReady == TRUE)         //从设备数据准备好了才同步上来
        {	 	    
            if(psMBSlaveDev->xSynchronized == FALSE) //重新上线的话，同步所有数据，先写后读
            {
                vMBMasterScanWriteSlaveDev(psMBMasterInfo, ucSlaveAddr, FALSE);  //同步从设备数据
                vMBMasterScanReadSlaveDev(psMBMasterInfo, ucSlaveAddr);			 //读从设备数据	
                psMBSlaveDev->xSynchronized = TRUE;                              //同步完成
            }
            else   //同步完成后，先写后读
            {
                vMBMasterScanWriteSlaveDev(psMBMasterInfo, ucSlaveAddr, TRUE);  //写有变化数据	
                vMBMasterScanReadSlaveDev(psMBMasterInfo, ucSlaveAddr);			//读从设备数据										
            }
        }
        else  //从设备数据未好，则只进行写不读
        {
            if(psMBSlaveDev->xSynchronized == FALSE) 
            {
                vMBMasterScanWriteSlaveDev(psMBMasterInfo, ucSlaveAddr, FALSE);  //同步设备数据
                psMBSlaveDev->xSynchronized = TRUE;  //同步完成
            }
        }
        psMBSlaveDev->eScanMode = (psMBSlaveDev->eScanMode == SCAN_WRITE) ? SCAN_READ:SCAN_WRITE; //切换轮询模式

//        myprintf("************vMBMasterScanSlaveDev  ucSlaveAddr %d  xDataReady %d  xSynchronized %d**************\n", 
//        ucSlaveAddr, psMBSlaveDev->xDataReady, psMBSlaveDev->xSynchronized);
    }		
}

/**********************************************************************
 * @brief   主栈轮询读某个从设备
 * @param   psMBMasterInfo  主栈信息块
 * @param   ucSlaveAddr      从设备地址
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBMasterScanReadSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR ucSlaveAddr)
{
    OS_ERR err = OS_ERR_NONE;
    
    eMBMasterReqErrCode errorCode    = MB_MRE_NO_ERR;
    sMBSlaveDev*    psMBSlaveDevCur  = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备

    if(psMBSlaveDevCur->eScanMode == SCAN_WRITE)
    {
        return;
    }
//    myprintf("ucSlaveAddr %d  vMBMasterScanReadSlaveDev\n", ucSlaveAddr);
    
#if MB_FUNC_READ_HOLDING_ENABLED > 0 			
    errorCode = eMBMasterScanReadHoldingRegister(psMBMasterInfo, ucSlaveAddr); //读保持寄存器 							
#endif
					
#if MB_FUNC_READ_COILS_ENABLED > 0
    errorCode = eMBMasterScanReadCoils(psMBMasterInfo, ucSlaveAddr);           //读线圈
#endif
					
#if MB_FUNC_READ_INPUT_ENABLED > 0				
    errorCode = eMBMasterScanReadInputRegister(psMBMasterInfo, ucSlaveAddr);	  //读输入寄存器					
#endif	
				
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    errorCode = eMBMasterScanReadDiscreteInputs(psMBMasterInfo, ucSlaveAddr);   //读离散量
    
#endif  
}

/**********************************************************************
 * @brief   主栈轮询写某个从设备
 * @param   psMBMasterInfo  主栈信息块
 * @param   bCheckPreValue  是否检查数据变化
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBMasterScanWriteSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR ucSlaveAddr, UCHAR bCheckPreValue)
{
    OS_ERR err = OS_ERR_NONE;
    
    eMBMasterReqErrCode errorCode    = MB_MRE_NO_ERR;
    sMBSlaveDev*    psMBSlaveDevCur  = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    
    if(psMBSlaveDevCur->eScanMode == SCAN_READ)
    {
        return;
    }
//    myprintf("ucSlaveAddr %d  vMBMasterScanWriteSlaveDev  bCheckPreValue %d\n", ucSlaveAddr, bCheckPreValue);
    
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 			
    errorCode = eMBMasterScanWriteHoldingRegister(psMBMasterInfo, ucSlaveAddr, bCheckPreValue);	//写保持寄存器 									
#endif
					
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    errorCode = eMBMasterScanWriteCoils(psMBMasterInfo, ucSlaveAddr, bCheckPreValue);            //写线圈 
#endif 
   
}

#if MB_FUNC_READ_INPUT_ENABLED > 0
/***********************************************************************************
 * @brief  轮询输入寄存器
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr )
{
	USHORT nSlaveTypes, iIndex, iStartAddr, iLastAddr, iCount;
    
    eMBMasterReqErrCode         eStatus = MB_MRE_NO_ERR;
    sMasterRegInData*   psRegInputValue = NULL;
    
    sMBSlaveDev*        psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur ;   //当前从设备
    sMBDevDataTable*     psMBRegInTable = &psMBSlaveDevCur->psDevCurData->sMBRegInTable;  //从设备通讯协议表

    iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
  
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBRegInTable = &psMBSlaveDevCur->psDevCurData->sMBRegInTable;
    }
	if( (psMBRegInTable->pvDataBuf == NULL) || (psMBRegInTable->usDataCount == 0) ) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	
	for(iIndex = 0; iIndex < psMBRegInTable->usDataCount ; iIndex++)    //轮询
	{
		psRegInputValue = (sMasterRegInData*)psMBRegInTable->pvDataBuf + iIndex;

	    /***************************** 读输入寄存器 **********************************/
		if( psRegInputValue->usAddr != (iLastAddr + 1) )    //地址不连续，则发送读请求
		{
			if( iCount > 0)
			{
				eStatus = eMBMasterReqReadInputRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
                iCount = 0;	
			}
           	if( psRegInputValue->ucAccessMode != WO )
			{
				iCount++;
			    iStartAddr = psRegInputValue->usAddr;
			}
		}
		else
		{	
			if( (iCount == 0) && (psRegInputValue->ucAccessMode != WO) )
			{
				iStartAddr = psRegInputValue->usAddr;
			}
			if( psRegInputValue->ucAccessMode != WO )
			{
				iCount++;
			}
		}
        if( ((psRegInputValue->ucAccessMode == WO) || (iIndex == psMBRegInTable->usDataCount-1)) && (iCount > 0) )  //如果寄存器为只写，发送读请求
        {
        	eStatus = eMBMasterReqReadInputRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
            iCount = 0; 
        }
        if( iCount * 4 >= MB_PDU_SIZE_MAX )                      //数据超过Modbus数据帧最大字节数，则发送读请求
        {
        	eStatus = eMBMasterReqReadInputRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
        	iCount = 1;
            iStartAddr = psRegInputValue->usAddr;				
        }
        iLastAddr = psRegInputValue->usAddr;		
	}
	return eStatus;
}
#endif

#if MB_FUNC_READ_HOLDING_ENABLED > 0 
/***********************************************************************************
 * @brief  轮询读保持寄存器
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr )
{
	USHORT nSlaveTypes, iIndex, iStartAddr, iStartReg, iLastAddr, iCount;
	
    USHORT          usRegHoldValue;
	ULONG			ulRegHoldValue;
	LONG            lRegHoldValue;
    SHORT           sRegHoldValue;
	int8_t          cRegHoldValue;

	eMBMasterReqErrCode        eStatus = MB_MRE_NO_ERR;
    sMasterRegHoldData* psRegHoldValue = NULL;
    
    sMBSlaveDev*      psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable* psMBRegHoldTable = &psMBSlaveDevCur->psDevCurData->sMBRegHoldTable; //从设备通讯协议表
    
	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;

    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBRegHoldTable = &psMBSlaveDevCur->psDevCurData->sMBRegHoldTable;
    }
	if( (psMBRegHoldTable->pvDataBuf == NULL) || (psMBRegHoldTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
//	myprintf("ucSlaveAddr %d  eMBMasterScanReadHoldingRegister\n",ucSndAddr);	

	for(iIndex = 0; iIndex < psMBRegHoldTable->usDataCount; iIndex++)  //轮询
	{
		psRegHoldValue = (sMasterRegHoldData*)psMBRegHoldTable->pvDataBuf + iIndex;
		
		 /***************************** 读保持寄存器 **********************************/
        if( psRegHoldValue->usAddr != (iLastAddr + 1) )    //地址不连续，则发送读请求
        {
        	if( iCount > 0)
        	{
        		eStatus = eMBMasterReqReadHoldingRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
                iCount = 0;
        	}
            if( psRegHoldValue->ucAccessMode != WO )
        	{
        		iCount++;
        	    iStartAddr = psRegHoldValue->usAddr;
        	}
        }
        else
        {	
        	if( (iCount == 0) && (psRegHoldValue->ucAccessMode != WO) )
        	{
        		iStartAddr = psRegHoldValue->usAddr;
        	}
        	if( psRegHoldValue->ucAccessMode != WO )
        	{
        		iCount++;
        	}
        }
		/*****主要针对可读写的变量，当变量发生变化要保证先写后读，所以遇到这种情况该寄存器不读了，得先写完再读****/
		if( (psRegHoldValue->ucAccessMode == RW) && (psRegHoldValue->usPreVal != *(USHORT*)psRegHoldValue->pvValue) ) 
		{
			if( iCount > 1)
			{
				eStatus = eMBMasterReqReadHoldingRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount-1, MB_MASTER_WAITING_DELAY);
                iCount = 0;	                                                                                          	
			}
			else
            {
				iCount = 0;	   
			}   
		}
        if( ((psRegHoldValue->ucAccessMode == WO) || (iIndex == psMBRegHoldTable->usDataCount-1)) && ( iCount > 0))  //如果寄存器为只写，发送读请求
        {
        	eStatus = eMBMasterReqReadHoldingRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
            iCount = 0;	 
        }
        if( iCount * 4 >= MB_PDU_SIZE_MAX )                      //数据超过Modbus数据帧最大字节数，则发送读请求
        {
        	eStatus = eMBMasterReqReadHoldingRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
        	iCount = 1;
            iStartAddr = psRegHoldValue->usAddr;		
        }
		iLastAddr = psRegHoldValue->usAddr;
	}
	return eStatus;
}
#endif

#if  MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_HOLDING_ENABLED > 0

/***********************************************************************************
 * @brief  写保持寄存器
 *************************************************************************************/
eMBMasterReqErrCode 
eMBMasterReqWriteHoldReg(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, USHORT usRegAddr, USHORT usNRegs, USHORT* pusDataBuffer, LONG lTimeOut)
{
    eMBMasterReqErrCode eStatus = MB_MRE_NO_ERR;
    
    if(usNRegs == 1)      //写单个寄存器
    {
        eStatus = eMBMasterReqWriteHoldingRegister(psMBMasterInfo, ucSndAddr, usRegAddr, *(USHORT*)pusDataBuffer, lTimeOut);
    }
    else if(usNRegs > 1) //写多个寄存器
    {
        eStatus = eMBMasterReqWriteMultipleHoldingRegister(psMBMasterInfo, ucSndAddr, usRegAddr, usNRegs, pusDataBuffer, lTimeOut);
    }
    return eStatus;
}

/***********************************************************************************
 * @brief  轮询写保持寄存器
 * @param  ucSndAddr            从栈地址
 * @param  bCheckPreValue       检查先前值
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanWriteHoldingRegister(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue)
{
    BOOL   bStarted;
	USHORT i, nRegs, nSlaveTypes;
	USHORT iIndex, iStartAddr, iStartReg, iLastAddr, iCount, iChangedRegs, iRegs;
	
    USHORT usRegHoldValue = 0;
    SHORT  sRegHoldValue  = 0;
	int8_t cRegHoldValue  =0;

	eMBMasterReqErrCode eStatus        = MB_MRE_NO_ERR;
    sMasterRegHoldData* psRegHoldValue = NULL;
    
    sMBSlaveDev*     psMBSlaveDevCur  = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable* psMBRegHoldTable = &psMBSlaveDevCur->psDevCurData->sMBRegHoldTable; //从设备通讯协议表
	
    USHORT*  RegHoldValList     = psMBMasterInfo->RegHoldValList;
    USHORT** pRegHoldPreValList = psMBMasterInfo->pRegHoldPreValList;
    
	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
    iChangedRegs = 0;
	iRegs = 0;
	bStarted = FALSE;
	
//    myprintf("ucSlaveAddr %d  eMBMasterScanWriteHoldingRegister\n", ucSndAddr);
 
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBRegHoldTable = &psMBSlaveDevCur->psDevCurData->sMBRegHoldTable;
    }
    if( (psMBRegHoldTable->pvDataBuf  == NULL) || (psMBRegHoldTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	for(iIndex = 0; iIndex < psMBRegHoldTable->usDataCount; iIndex++)  //轮询
	{
		psRegHoldValue = (sMasterRegHoldData*)psMBRegHoldTable->pvDataBuf + iIndex;
		
		/******************* 写保持寄存器，如果对应的变量发生变化则写寄存器 ***************************/
        if( (psRegHoldValue != NULL) && (psRegHoldValue->pvValue != NULL) && (psRegHoldValue->ucAccessMode != RO) )   //寄存器非只读
		{
			switch (psRegHoldValue->ucDataType)
            {	
               	case uint16:	
               		usRegHoldValue = *(USHORT*)psRegHoldValue->pvValue;
                
                    if( psRegHoldValue->fTransmitMultiple != (float)1 )
					{
						usRegHoldValue = (USHORT)( (float)usRegHoldValue * (float)psRegHoldValue->fTransmitMultiple ); //传输因子
					}
				    if ( ((USHORT)psRegHoldValue->usPreVal != usRegHoldValue) || (bCheckPreValue == FALSE) )  //变量变化且或者不检查是否变化
               		{		
               			if( (usRegHoldValue >= (LONG)psRegHoldValue->lMinVal) && (usRegHoldValue <= (LONG)psRegHoldValue->lMaxVal) )
               			{
//                            myprintf("usRegHoldValue %d  iIndex %d\n", usRegHoldValue, iIndex);
               				RegHoldValList[iChangedRegs]  = (USHORT)usRegHoldValue;
               			    pRegHoldPreValList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );	
               			    iChangedRegs++;
                        }
               		}	
               	break;
               	case uint8: 		
               		usRegHoldValue = *(UCHAR*)psRegHoldValue->pvValue;
                    if ( psRegHoldValue->fTransmitMultiple != (float)1 )
					{
						usRegHoldValue =  (UCHAR)( (float)usRegHoldValue * (float)psRegHoldValue->fTransmitMultiple ); //传输因子
					}
               		if ( ((USHORT)psRegHoldValue->usPreVal != usRegHoldValue) || (bCheckPreValue == FALSE) )  //变量变化且或者不检查是否变化
               		{
               			if( (usRegHoldValue >= (LONG)psRegHoldValue->lMinVal) && (usRegHoldValue <= (LONG)psRegHoldValue->lMaxVal) )
               			{
               				RegHoldValList[iChangedRegs]  = (USHORT)usRegHoldValue;
               			    pRegHoldPreValList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );
               			    iChangedRegs++;
               			}			
               		}	
               	break;
               	case int16:		
               		sRegHoldValue = *(SHORT*)psRegHoldValue->pvValue;
				    if ( psRegHoldValue->fTransmitMultiple != (float)1 )
					{
						 sRegHoldValue = (SHORT)( (float)sRegHoldValue * (float)psRegHoldValue->fTransmitMultiple ); //传输因子
					}
               		if ( ((SHORT)psRegHoldValue->usPreVal != sRegHoldValue) || (bCheckPreValue == FALSE) )  //变量变化且或者不检查是否变化
               		{
               			if( (sRegHoldValue >= (LONG)psRegHoldValue->lMinVal) && (sRegHoldValue <= (LONG)psRegHoldValue->lMaxVal) )
               			{
               				RegHoldValList[iChangedRegs]  = (USHORT)sRegHoldValue;
               			    pRegHoldPreValList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );
               			    iChangedRegs++;
               			}
               		}		
               	break;
               	case int8:
               		cRegHoldValue = *(int8_t*)psRegHoldValue->pvValue;
				    if ( psRegHoldValue->fTransmitMultiple != (float)1 )
					{
						 cRegHoldValue = (int8_t)( (float)cRegHoldValue * (float)psRegHoldValue->fTransmitMultiple ); //传输因子
					}
               		if ( ((int8_t)psRegHoldValue->usPreVal != cRegHoldValue) || (bCheckPreValue == FALSE) )  //变量变化且或者不检查是否变化
               		{
               			if( (cRegHoldValue >= (LONG)psRegHoldValue->lMinVal) && (cRegHoldValue <= (LONG)psRegHoldValue->lMaxVal) )
               			{
               				RegHoldValList[iChangedRegs]  = (USHORT)cRegHoldValue;
               			    pRegHoldPreValList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );
               			    iChangedRegs++;
               			}		
               		}	
                break;
            }
		}
        iRegs++;
        if(iChangedRegs == 1 && (bStarted != TRUE))    //记录首地址
        {
           iStartReg = psRegHoldValue->usAddr;
           bStarted = TRUE;
           iRegs = 1;
        }
        if( (psRegHoldValue->usAddr != (iLastAddr+1)) && (iChangedRegs>0) && (iRegs>1) )    //地址不连续，则发送写请求
        {
       	    nRegs = iChangedRegs;
       	    if(iRegs == iChangedRegs)
       	    {
       	    	eStatus = eMBMasterReqWriteHoldReg(psMBMasterInfo, ucSndAddr, iStartReg, iChangedRegs-1, 
       	                                          (USHORT*)RegHoldValList, MB_MASTER_WAITING_DELAY);	//写寄存器
       	    	iChangedRegs = 1;
       	        iRegs = 1;
       	    	bStarted = TRUE;
       	    	iStartReg = psRegHoldValue->usAddr;
       	    }
       	    else
       	    {
       	    	eStatus = eMBMasterReqWriteHoldReg(psMBMasterInfo, ucSndAddr, iStartReg, iChangedRegs, 
       	                                          (USHORT*)RegHoldValList, MB_MASTER_WAITING_DELAY);	//写寄存器
       	    	iChangedRegs = 0;
       	        iRegs = 0;
       	    	bStarted = FALSE;
       	    }
       	    if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPort) == EV_MASTER_PROCESS_SUCCESS)            //如果写入成功，更新数据
       	    {	
       	    	for(i = nRegs ; i > 0; i--)
       	    	{
       	    		*pRegHoldPreValList[i-1] = RegHoldValList[i-1];
       	    	}
       	    }
       	    RegHoldValList[0]  = RegHoldValList[nRegs-1];
            pRegHoldPreValList[0] = pRegHoldPreValList[nRegs-1];   
        }
		
		if(iChangedRegs > 0)
		{
            //发生变化的寄存器不连续或者地址到达字典最后,则写寄存器, 数据超过Modbus数据帧最大字节数，则发送写请求
			if( (iRegs != iChangedRegs) || (iIndex == psMBRegHoldTable->usDataCount-1) || (iChangedRegs*4 >= MB_PDU_SIZE_MAX)) 
            {                                                                                                                  
                nRegs = iChangedRegs;
                eStatus = eMBMasterReqWriteHoldReg(psMBMasterInfo, ucSndAddr, iStartReg, iChangedRegs, 
             	                                  (USHORT*)RegHoldValList, MB_MASTER_WAITING_DELAY);	//写寄存器
                iChangedRegs = 0;
             	iRegs = 0;
             	bStarted = FALSE;
             	
             	if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPort) == EV_MASTER_PROCESS_SUCCESS )            //如果写入成功，更新当前数据
             	{	
             		for(i = nRegs ; i > 0; i--)
             		{
             			*pRegHoldPreValList[i-1] = RegHoldValList[i-1];
             		}
             	}    
            }	
		}
        iLastAddr = psRegHoldValue->usAddr;
	}
    return eStatus;
}
#endif


#if MB_FUNC_READ_COILS_ENABLED > 0 
/***********************************************************************************
 * @brief  轮询读线圈
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanReadCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr)
{
	UCHAR  iBitInByte, cByteValue;
	USHORT nSlaveTypes, iIndex, iLastAddr, iStartAddr, iStartBit, iCount;
   
    eMBMasterReqErrCode     eStatus = MB_MRE_NO_ERR;
	sMasterBitCoilData* psCoilValue = NULL;

    sMBSlaveDev*     psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable* psMBCoilTable   = &psMBSlaveDevCur->psDevCurData->sMBCoilTable;    //从设备通讯协议表
    
	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;

    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBCoilTable = &psMBSlaveDevCur->psDevCurData->sMBCoilTable;
    }
	if( (psMBCoilTable->pvDataBuf == NULL) || (psMBCoilTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
//	myprintf("ucSlaveAddr %d  eMBMasterScanReadCoils \n",ucSndAddr);	
	
	for(iIndex = 0; iIndex < psMBCoilTable->usDataCount; iIndex++)
	{
        psCoilValue = (sMasterBitCoilData*)psMBCoilTable->pvDataBuf + iIndex;
        
        /***************************** 读线圈 **********************************/
        if( psCoilValue->usAddr != iLastAddr+1 )     //地址不连续，则发送读请求
        {
            if(iCount>0)
            {
                eStatus = eMBMasterReqReadCoils(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);	
                iCount = 0;
                iStartAddr = psCoilValue->usAddr;
            }
            if( psCoilValue->ucAccessMode != WO )
            {
                iCount++;
                iStartAddr = psCoilValue->usAddr;
            }
        }
        else  //连续
        {
        	if(iCount == 0)
        	{
        		iStartAddr = psCoilValue->usAddr;
        	}	
        	if( psCoilValue->ucAccessMode != WO )
        	{
        		iCount++;
        	}	
        }
        /*****主要针对可读写的变量，当变量发生变化要保证先写后读，所以遇到这种情况该线圈不读了，得先写完再读****/
		if( psCoilValue->ucAccessMode == RW && psCoilValue->ucPreVal != *(UCHAR*)psCoilValue->pvValue ) 
		{
	        if( iCount > 1)
	        {
	            eStatus = eMBMasterReqReadCoils(psMBMasterInfo, ucSndAddr, iStartAddr, iCount-1, MB_MASTER_WAITING_DELAY);	
                iCount = 0;   	
	        }
	        else
            {
	            iCount = 0;	   
	        }   
		}
        if( (psCoilValue->ucAccessMode == WO || iIndex == psMBCoilTable->usDataCount-1) && iCount>0 )  //如果为只写，发送读请求
        {
            eStatus = eMBMasterReqReadCoils(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);	
            iCount = 0;
            iStartAddr = psCoilValue->usAddr;   
        }	
        if( iCount >= MB_PDU_SIZE_MAX*4 )    //数据超过Modbus数据帧最大字节数，则发送读请求
        {
            eStatus = eMBMasterReqReadCoils(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
        	iCount = 1;
        	iStartAddr = psCoilValue->usAddr;			
        }
        iLastAddr = psCoilValue->usAddr ;		
	}
	return eStatus;
}
#endif

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0
/*************************************************************************************
 * @brief  写线圈
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterWriteCoil(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, USHORT usCoilAddr,
                                       USHORT usNCoils, UCHAR* pucDataBuffer, LONG lTimeOut)
{
    eMBMasterReqErrCode eStatus = MB_MRE_NO_ERR;
    USHORT usMBBitData = 0;
    
     System* pSystem = (System*)System_Core();
    
    if(usNCoils == 1)      //写单个线圈
    {
        usMBBitData = (*(UCHAR*)pucDataBuffer >0) ? 0xFF00 : 0x0000;
        
//        if(usCoilAddr == 10)
//        {
//             myprintf("eMBMasterWriteCoil ucSndAddr  %d usCoilAddr %d  usMBBitData %d %d %d\n\n",ucSndAddr, 
//            usCoilAddr, usMBBitData, *(UCHAR*)pucDataBuffer, pSystem->psModularRoofList[0]->xErrClean);
//        }   

        eStatus = eMBMasterReqWriteCoil(psMBMasterInfo, ucSndAddr, usCoilAddr, usMBBitData, lTimeOut);   
    }
    else if(usNCoils > 1)  //写多个线圈
    {
        eStatus = eMBMasterReqWriteMultipleCoils(psMBMasterInfo, ucSndAddr, usCoilAddr, usNCoils, (UCHAR*)pucDataBuffer, lTimeOut);	      
    }
    return eStatus;
}

/***********************************************************************************
 * @brief  更新线圈值
 *************************************************************************************/
void vMBMasterFreshCoils(sMBMasterInfo* psMBMasterInfo, USHORT iChangedBytes, USHORT iChangedBits)
{
    USHORT i, n, iBitInByte;
    UCHAR*  pBitCoilPreVal =NULL;

    if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPort) == EV_MASTER_PROCESS_SUCCESS )   //如果写入成功，更新数据
    {	
        for(i=0, n=0; i<iChangedBits; i++)
        {
            iBitInByte = i%8;
         
            pBitCoilPreVal  = psMBMasterInfo->pBitCoilPreValList[i];
            *pBitCoilPreVal = ( psMBMasterInfo->BitCoilByteValList[n] & (1<<iBitInByte) ) >> iBitInByte;
            	
            if(iBitInByte==0 && i>7 && n<iChangedBytes)
            {
                psMBMasterInfo->BitCoilByteValList[n] = 0;
                n++;
                
            }
        }
//        myprintf("vMBMasterFreshCoils BitCoilByteValList %d\n\n", psMBMasterInfo->BitCoilByteValList[0]);
//        myprintf("vMBMasterFreshCoils pBitCoilPreVal %d iChangedBytes %d iChangedBits %d %d\n\n", *pBitCoilPreVal,iChangedBytes, iChangedBits, n);     
    }		
}

/***********************************************************************************
 * @brief  轮询写线圈
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanWriteCoils(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue)
{
	UCHAR  iBitInByte, cByteValue;
	USHORT i, nBits, bStarted, nSlaveTypes;
	USHORT iIndex, iLastAddr, iStartAddr, iChangedBytes, iChangedBits, iBits;
  
	eMBMasterReqErrCode      eStatus = MB_MRE_NO_ERR;
	sMasterBitCoilData*  psCoilValue = NULL;

    sMBSlaveDev*     psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备 
    sMBDevDataTable* psMBCoilTable   = &psMBSlaveDevCur->psDevCurData->sMBCoilTable;    //从设备通讯协议表
   
    
	iLastAddr = 0;
	iStartAddr = 0;
	iChangedBits = 0;
	iChangedBytes = 0;
    iBitInByte = 0;
	iBits = 0;
	bStarted = FALSE;
	
//    myprintf("ucSlaveAddr %d  eMBMasterScanWriteCoils\n", ucSndAddr);
    
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBCoilTable = &psMBSlaveDevCur->psDevCurData->sMBCoilTable;
    }
	if( (psMBCoilTable == NULL) ||(psMBCoilTable->pvDataBuf == NULL) || (psMBCoilTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}

	for(iIndex = 0; iIndex < psMBCoilTable->usDataCount; iIndex++)
	{
		psCoilValue = (sMasterBitCoilData*)(psMBCoilTable->pvDataBuf) + iIndex;
        
//        if(psCoilValue->usAddr == 10)
//        {
//             myprintf("psCoilValue  ucSndAddr %d  usCoilAddr %d pvValue %d ucPreVal %d\n\n",
//                    ucSndAddr, psCoilValue->usAddr, *(UCHAR*)psCoilValue->pvValue, psCoilValue->ucPreVal);
//        }

        /******************* 写线圈，如果对应的变量发生变化则写线圈 ***************************/
        if ( (psCoilValue->pvValue != NULL) && (psCoilValue->ucAccessMode != RO) )   //非只读
		{
			if( psCoilValue->ucPreVal != *(UCHAR*)(psCoilValue->pvValue) || bCheckPreValue == FALSE ) //线圈发生变化或者不检查是否发生变化
            {
				if(iBitInByte%8 ==0)   //byte中bit的偏移
			    {
			    	cByteValue = 0;
			    	iBitInByte = 0;
			    	iChangedBytes++;     //字节+1  8bit就增加1byte
			    }
				if( *(UCHAR*)psCoilValue->pvValue > 0 )  //线圈状态为1
			    {
			    	cByteValue |= (1<<iBitInByte);	//组成一个字节		
			    }
                iBitInByte++;   //byte中bit的偏移+1
			    iChangedBits++;	//发生变化的线圈数量+1
                
			    psMBMasterInfo->pBitCoilPreValList[iChangedBits-1]  = (UCHAR*)(&psCoilValue->ucPreVal);  //记录先前值地址
			    psMBMasterInfo->BitCoilByteValList[iChangedBytes-1] = cByteValue;	                     //记录要下发的byte数据
			}				
		}
		iBits++;  //线圈计数+1
       
		if( iChangedBits == 1 && bStarted == FALSE )    
		{
			iStartAddr = psCoilValue->usAddr;     //记录线圈起始地址
			bStarted = TRUE;
			iBits = 1;
			iChangedBytes = 1;
		}	
		if( psCoilValue->usAddr != iLastAddr + 1 )   //地址不连续
		{
            if( iChangedBits>0 && iBits>1 )
            {
                if(iBits == iChangedBits)   //线圈地址不连续，且该线圈地址也发生了变化
			    {
//                    myprintf("eMBMasterScanWriteCoils 1111 ucSndAddr %d  usCoilAddr %d cByteValue %d iChangedBits %d iBits %d\n\n",
//                    ucSndAddr, iStartAddr, psMBMasterInfo->BitCoilByteValList[0], iChangedBits, iBits);
                    
			        eStatus = eMBMasterWriteCoil(psMBMasterInfo, ucSndAddr, iStartAddr, iChangedBits-1, 
			                                     psMBMasterInfo->BitCoilByteValList, MB_MASTER_WAITING_DELAY);	     //写线圈
			        vMBMasterFreshCoils(psMBMasterInfo, iChangedBytes, iChangedBits-1);
                 
			        iChangedBits = 1;    //写完一帧得复位所有状态,
			        iChangedBytes = 1;
			        iBits = 1;
			        cByteValue = 0;
			        iBitInByte = 0;
			        bStarted = TRUE;
                     
			        iStartAddr = psCoilValue->usAddr;       //以该线圈地址为第一个地址
			        if( *(UCHAR*)psCoilValue->pvValue > 0 ) //重新记录，
			        {
			            cByteValue |= (1 << iBitInByte);			
			        }		
			        psMBMasterInfo->pBitCoilPreValList[0]  = (UCHAR*)(&psCoilValue->ucPreVal); //重新记录
			        psMBMasterInfo->BitCoilByteValList[0]  = cByteValue;	
			        iBitInByte++;  
			    }
			    else if(iBits > iChangedBits)  //线圈地址不连续，但该线圈没发生变化
			    {
//                    myprintf("eMBMasterScanWriteCoils 222 ucSndAddr %d  usCoilAddr %d cByteValue %d  iChangedBits %d\n\n",
//                    ucSndAddr, iStartAddr, psMBMasterInfo->BitCoilByteValList[0], iChangedBits);
                    
			    	eStatus = eMBMasterWriteCoil(psMBMasterInfo, ucSndAddr, iStartAddr, iChangedBits, 
                                                 psMBMasterInfo->BitCoilByteValList, MB_MASTER_WAITING_DELAY);	//写线圈
			        vMBMasterFreshCoils(psMBMasterInfo, iChangedBytes, iChangedBits);
                    
			    	iChangedBits = 0;      //写完一帧得复位所有状态
			    	iChangedBytes = 0;
			    	iBits = 0;
			    	cByteValue = 0;
			        iBitInByte = 0;
			    	bStarted = FALSE;   
			    }
            }
		}  
		else //地址连续
		{
            if(iChangedBits > 0)
            {
                //1. 未发生变化  2.地址到达字典最后 3.数据超过Modbus数据帧最大字节数，则发送写请求
                if( (iBits > iChangedBits) || (iIndex == psMBCoilTable->usDataCount-1) || (iChangedBits >= MB_PDU_SIZE_MAX*4) ) 
                {     
//                    myprintf("eMBMasterScanWriteCoils 333 ucSndAddr %d  usCoilAddr %d cByteValue %d iChangedBits %d\n\n",
//                    ucSndAddr, iStartAddr, psMBMasterInfo->BitCoilByteValList[0], iChangedBits);
                    
                    eStatus = eMBMasterWriteCoil(psMBMasterInfo, ucSndAddr, iStartAddr, iChangedBits, 
                                                 psMBMasterInfo->BitCoilByteValList, MB_MASTER_WAITING_DELAY);	//写线圈	
                    vMBMasterFreshCoils(psMBMasterInfo, iChangedBytes, iChangedBits);
                    
                    iChangedBits = 0;
                    iChangedBytes = 0;
                    iBits = 0;
                    bStarted = FALSE;
                    cByteValue = 0;
                    iBitInByte = 0;    
                }
            }                
		}
        iLastAddr = psCoilValue->usAddr;
	}
	return eStatus;
}
#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
/***********************************************************************************
 * @brief  轮询读离散量字典
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanReadDiscreteInputs( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr )
{
	UCHAR  i;
	USHORT iIndex, iLastAddr, iStartAddr, iCount, iBit, nSlaveTypes;
   
    eMBMasterReqErrCode             eStatus = MB_MRE_NO_ERR;
    sMasterBitDiscData*      pDiscreteValue = NULL;
    
    sMBSlaveDev*     psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable* psMBDiscInTable = &psMBSlaveDevCur->psDevCurData->sMBDiscInTable;  //从设备通讯协议表

	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
	
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBDiscInTable = &psMBSlaveDevCur->psDevCurData->sMBDiscInTable;
    } 
    if( (psMBDiscInTable->pvDataBuf == NULL) || (psMBDiscInTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	
	for(iIndex = 0; iIndex < psMBDiscInTable->usDataCount ; iIndex++)
	{
		pDiscreteValue = (sMasterBitDiscData*)psMBDiscInTable->pvDataBuf + iIndex;	
	
        if( pDiscreteValue->usAddr != (iLastAddr + 1) )     //地址不连续，则发送读请求
		{
			if(iCount > 0)
			{
				eStatus = eMBMasterReqReadDiscreteInputs(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);	
                iCount = 0;
				iStartAddr = pDiscreteValue->usAddr;
			}
			if(pDiscreteValue->ucAccessMode != WO)
			{
				iCount++;
				iStartAddr = pDiscreteValue->usAddr;
			}
		}
		else
		{
			if(iCount == 0)
			{
				iStartAddr = pDiscreteValue->usAddr;
			}	
			if(pDiscreteValue->ucAccessMode != WO)
			{
				iCount++;
			}	
		}
		if( ((pDiscreteValue->ucAccessMode == WO) || (iIndex== psMBDiscInTable->usDataCount-1)) && (iCount > 0) )  //如果为只写或者地址到达字典最后发送读请求
		{
             eStatus = eMBMasterReqReadDiscreteInputs(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);	
             iCount = 0;
             iStartAddr = pDiscreteValue->usAddr;
		}	
		if(iCount / 8 >= MB_PDU_SIZE_MAX / 2)                      //数据超过Modbus数据帧最大字节数，则发送读请求
		{
			eStatus = eMBMasterReqReadDiscreteInputs(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);
			iCount = 1;
			iStartAddr = pDiscreteValue->usAddr;			
		}
        iLastAddr = pDiscreteValue->usAddr ;		
	}
	return eStatus;
}
#endif
