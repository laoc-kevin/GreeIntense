#include "mbport_m.h"
#include "mb_m.h"
#include "mbframe.h"
#include "mbfunc_m.h"
#include "mbdict_m.h"
#include "user_mb_scan_m.h"

#if MB_FUNC_READ_INPUT_ENABLED > 0
/***********************************************************************************
 * @brief  轮询输入寄存器字典
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr )
{
	USHORT i, nSlaveTypes;
	USHORT iIndex, iStartAddr, iLastAddr, iCount;
    
    eMBMasterReqErrCode             eStatus = MB_MRE_NO_ERR;
    sMasterRegInData*       psRegInputValue = NULL;
    
    sMBSlaveDevInfo*       psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur ;   //当前从设备
    const sMBDevDataTable*   psRegInputBuf = psMBSlaveDevCur->psDevCurData->psMBRegInTable;         //从设备通讯协议表

    iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
  
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psRegInputBuf = psMBSlaveDevCur->psDevCurData->psMBRegInTable;
    }
	if( (psRegInputBuf->pvDataBuf == NULL) || (psRegInputBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	
	for(iIndex = 0; iIndex < psRegInputBuf->usDataCount ; iIndex++)    //轮询
	{
		psRegInputValue = (sMasterRegInData*)psRegInputBuf->pvDataBuf + iIndex;

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
        if( ((psRegInputValue->ucAccessMode == WO) || (iIndex == psRegInputBuf->usDataCount-1)) && (iCount > 0) )  //如果寄存器为只写，发送读请求
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
 * @brief  轮询读保持寄存器字典
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr )
{
	USHORT i, nRegs, nSlaveTypes;
	USHORT iIndex, iStartAddr, iStartReg, iLastAddr, iCount;
	
    USHORT          usRegHoldValue;
	ULONG			ulRegHoldValue;
	LONG            lRegHoldValue;
    SHORT           sRegHoldValue;
	int8_t          cRegHoldValue;

	eMBMasterReqErrCode            eStatus = MB_MRE_NO_ERR;
    sMasterRegHoldData*     psRegHoldValue = NULL;
    
    sMBSlaveDevInfo*       psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    const sMBDevDataTable*    psRegHoldBuf = psMBSlaveDevCur->psDevCurData->psMBRegHoldTable;         //从设备通讯协议表
    
	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;

    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psRegHoldBuf = psMBSlaveDevCur->psDevCurData->psMBRegHoldTable;
    }
	if( (psRegHoldBuf->pvDataBuf == NULL) || (psRegHoldBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	myprintf("iSlaveAddr %d  eMBMasterScanReadHoldingRegister  \n",ucSndAddr);	
	
	for(iIndex = 0; iIndex < psRegHoldBuf->usDataCount; iIndex++)  //轮询
	{
		psRegHoldValue = (sMasterRegHoldData*)psRegHoldBuf->pvDataBuf + iIndex;
		
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
				eStatus = eMBMasterReqReadHoldingRegister(psMBMasterInfo, ucSndAddr, iStartAddr, iCount-1, MB_MASTER_WAITING_DELAY);   //
                iCount = 0;	                                                                                          	
			}
			else
            {
				iCount = 0;	   
			}   
		}
        if( ((psRegHoldValue->ucAccessMode == WO) || (iIndex == psRegHoldBuf->usDataCount-1)) && ( iCount > 0))  //如果寄存器为只写，发送读请求
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

#if  MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 

/***********************************************************************************
 * @brief  轮询写保持寄存器字典
 * @param  ucSndAddr            从栈地址
 * @param  bCheckPreValue       检查先前值
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanWriteHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue )
{
	USHORT i, nRegs, bStarted, nSlaveTypes;
	USHORT iIndex, iStartAddr, iStartReg, iLastAddr, iCount, iChangedRegs, iRegs;
	
    USHORT          usRegHoldValue;
    SHORT           sRegHoldValue;
	int8_t          cRegHoldValue;
	
	eMBMasterReqErrCode            eStatus = MB_MRE_NO_ERR;
    sMasterRegHoldData*     psRegHoldValue = NULL;
    
    sMBSlaveDevInfo*       psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    const sMBDevDataTable*    psRegHoldBuf = psMBSlaveDevCur->psDevCurData->psMBRegHoldTable;         //从设备通讯协议表
	
    volatile USHORT  RegHoldValueList[MB_PDU_SIZE_MAX];
    volatile USHORT* pRegHoldPreValueList[MB_PDU_SIZE_MAX];

	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
    iChangedRegs = 0;
	iRegs = 0;
	bStarted = FALSE;
	
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psRegHoldBuf = psMBSlaveDevCur->psDevCurData->psMBRegHoldTable;
    }
    if( (psRegHoldBuf->pvDataBuf  == NULL) || (psRegHoldBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	myprintf("iSlaveAddr %d  eMBMasterScanWriteHoldingRegister   bCheckPreValue %d  \n",ucSndAddr, bCheckPreValue);	
	
	for(iIndex = 0; iIndex < psRegHoldBuf->usDataCount; iIndex++)  //轮询
	{
		psRegHoldValue = (sMasterRegHoldData*)psRegHoldBuf->pvDataBuf + iIndex;
		
		/******************* 写保持寄存器，如果对应的变量发生变化则写寄存器 ***************************/
        if( (psRegHoldValue != NULL) && (psRegHoldValue->pvValue != NULL) && (psRegHoldValue->ucAccessMode != RO) )   //寄存器非只读
		{
			switch ( psRegHoldValue->ucDataType )
            {	
               	case uint16:	
               		usRegHoldValue = *(USHORT*)psRegHoldValue->pvValue;
				    if ( psRegHoldValue->fTransmitMultiple != (float)1 )
					{
						usRegHoldValue = (USHORT)( (float)usRegHoldValue * (float)psRegHoldValue->fTransmitMultiple ); //传输因子
					}
				    if ( ((USHORT)psRegHoldValue->usPreVal != usRegHoldValue) || (!bCheckPreValue) )  //变量变化且或者不检查是否变化
               		{		
               			if( (usRegHoldValue >= (USHORT)psRegHoldValue->lMinVal) && (usRegHoldValue <= (USHORT)psRegHoldValue->lMaxVal) )
               			{
               				RegHoldValueList[iChangedRegs]  = (USHORT)usRegHoldValue;
               			    pRegHoldPreValueList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );	
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
               		if ( ((USHORT)psRegHoldValue->usPreVal != usRegHoldValue) || (!bCheckPreValue) )  //变量变化且或者不检查是否变化
               		{
               			if( (usRegHoldValue >= (USHORT)psRegHoldValue->lMinVal) && (usRegHoldValue <= (USHORT)psRegHoldValue->lMaxVal) )
               			{
               				RegHoldValueList[iChangedRegs]  = (USHORT)usRegHoldValue;
               			    pRegHoldPreValueList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );
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
               		if ( ((SHORT)psRegHoldValue->usPreVal != sRegHoldValue) || (!bCheckPreValue) )  //变量变化且或者不检查是否变化
               		{
               			if( (sRegHoldValue >= (SHORT)psRegHoldValue->lMinVal) && (sRegHoldValue <= (SHORT)psRegHoldValue->lMaxVal) )
               			{
               				RegHoldValueList[iChangedRegs]  = (USHORT)sRegHoldValue;
               			    pRegHoldPreValueList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );
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
               		if ( ((int8_t)psRegHoldValue->usPreVal != cRegHoldValue) || (!bCheckPreValue) )  //变量变化且或者不检查是否变化
               		{
               			if( (cRegHoldValue >= (int8_t)psRegHoldValue->lMinVal) && (cRegHoldValue <= (int8_t)psRegHoldValue->lMaxVal) )
               			{
               				RegHoldValueList[iChangedRegs]  = (USHORT)cRegHoldValue;
               			    pRegHoldPreValueList[iChangedRegs] = (USHORT*)( &(psRegHoldValue->usPreVal) );
               			    iChangedRegs++;
               			}		
               		}	
                break;
            }
		}
        iRegs++;
           
        if( iChangedRegs == 1 && ( bStarted != TRUE))    //记录首地址
        {
           iStartReg = psRegHoldValue->usAddr;
           bStarted = TRUE;
           iRegs = 1;
        }
	
        if( (psRegHoldValue->usAddr != (iLastAddr + 1)) && ( iChangedRegs > 0) && (iRegs > 1) )    //地址不连续，则发送写请求
        {
       	    nRegs = iChangedRegs;
       	    if(iRegs == iChangedRegs)
       	    {
       	    	eStatus = eMBMasterReqWriteMultipleHoldingRegister(psMBMasterInfo, ucSndAddr, iStartReg, iChangedRegs - 1, 
       	                                                      (USHORT*)RegHoldValueList, MB_MASTER_WAITING_DELAY);	//写寄存器
       	    	iChangedRegs = 1;
       	        iRegs = 1;
       	    	bStarted = TRUE;
       	    	iStartReg = psRegHoldValue->usAddr;
       	    }
       	    else
       	    {
       	    	eStatus = eMBMasterReqWriteMultipleHoldingRegister(psMBMasterInfo, ucSndAddr, iStartReg, iChangedRegs, 
       	                                                      (USHORT*)RegHoldValueList, MB_MASTER_WAITING_DELAY);	//写寄存器
       	    	iChangedRegs = 0;
       	        iRegs = 0;
       	    	bStarted = FALSE;
       	    }
       	    
       	    if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPortInfo) == EV_MASTER_PROCESS_SUCCESS )            //如果写入成功，更新数据
       	    {	
       	    	for( i = nRegs ; i > 0; i--)
       	    	{
       	    		*pRegHoldPreValueList[i-1] = RegHoldValueList[i-1];
       	    	}
       	    }
       	    RegHoldValueList[0]  = RegHoldValueList[nRegs-1];
            pRegHoldPreValueList[0] = pRegHoldPreValueList[nRegs-1];   
        }
		
		if( iChangedRegs > 0 )
		{
			if( (iRegs != iChangedRegs) || (iIndex == psRegHoldBuf->usDataCount-1) || ( iChangedRegs * 4 >= MB_PDU_SIZE_MAX ))  //发生变化的寄存器不连续或者地址到达字典最后，则写寄存器
            {                                                                                                                  //数据超过Modbus数据帧最大字节数，则发送写请求
                nRegs = iChangedRegs;
                eStatus = eMBMasterReqWriteMultipleHoldingRegister(psMBMasterInfo, ucSndAddr, iStartReg, iChangedRegs, 
             	                                                  (USHORT*)RegHoldValueList, MB_MASTER_WAITING_DELAY);	//写寄存器
                iChangedRegs = 0;
             	iRegs = 0;
             	bStarted = FALSE;
             	
             	if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPortInfo) == EV_MASTER_PROCESS_SUCCESS )            //如果写入成功，更新当前数据
             	{	
             		for( i = nRegs ; i > 0; i--)
             		{
             			*pRegHoldPreValueList[i-1] = RegHoldValueList[i-1];
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
 * @brief  轮询读线圈字典
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanReadCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr)
{
	UCHAR  iBitInByte, cByteValue;
	USHORT i, nBits, bStarted, nSlaveTypes;
	USHORT iIndex, iLastAddr, iStartAddr, iStartBit, iCount, iChangedBytes, iChangedBits, iBits;
   
    eMBMasterReqErrCode          eStatus = MB_MRE_NO_ERR;
	sMasterBitCoilData*      psCoilValue = NULL;

    sMBSlaveDevInfo*        psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    const sMBDevDataTable*        psCoilBuf = psMBSlaveDevCur->psDevCurData->psMBCoilTable;           //从设备通讯协议表
    
	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
	iChangedBits = 0;
	iBits = 0;
	bStarted = FALSE;
	
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psCoilBuf = psMBSlaveDevCur->psDevCurData->psMBCoilTable;
    }
	if( (psCoilBuf->pvDataBuf == NULL) || (psCoilBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	
	myprintf("iSlaveAddr %d  eMBMasterScanReadCoils \n",ucSndAddr);	
	
	for(iIndex = 0; iIndex < psCoilBuf->usDataCount ; iIndex++)
	{
		 psCoilValue = (sMasterBitCoilData*)psCoilBuf->pvDataBuf + iIndex;
        
        /***************************** 读线圈 **********************************/
         if( psCoilValue->usAddr != (iLastAddr + 1) )     //地址不连续，则发送读请求
         {
         	if( iCount > 0)
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
         else
         {
         	if(	iCount == 0 )
         	{
         		iStartAddr = psCoilValue->usAddr;
         	}	
         	if( psCoilValue->ucAccessMode != WO )
         	{
         		iCount++;
         	}	
         }
		 if( (psCoilValue->ucAccessMode == RW) && (psCoilValue->ucPreVal != *(UCHAR*)psCoilValue->pvValue ) ) 
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
         if( ((psCoilValue->ucAccessMode == WO) || (iIndex== psCoilBuf->usDataCount-1)) && ( iCount > 0) )  //如果为只写，发送读请求
         {
         	
             eStatus = eMBMasterReqReadCoils(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);	
             iCount = 0;
             iStartAddr = psCoilValue->usAddr;   
         }	
       
         if( iCount >= MB_PDU_SIZE_MAX * 4)    //数据超过Modbus数据帧最大字节数，则发送读请求
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

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
/***********************************************************************************
 * @brief  轮询写线圈字典
 * @param  ucSndAddr            从栈地址
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterScanWriteCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue)
{
	UCHAR  iBitInByte, cByteValue;
	USHORT i, nBits, bStarted, nSlaveTypes;
	USHORT iIndex, iLastAddr, iStartAddr, iStartBit, iCount, iChangedBytes, iChangedBits, iBits;
  
	eMBMasterReqErrCode          eStatus = MB_MRE_NO_ERR;
	sMasterBitCoilData*      psCoilValue = NULL;

    sMBSlaveDevInfo*       psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    const sMBDevDataTable*       psCoilBuf = psMBSlaveDevCur->psDevCurData->psMBCoilTable;           //从设备通讯协议表
    
    volatile UCHAR  BitCoilByteValueList[MB_PDU_SIZE_MAX];
    volatile UCHAR* pBitCoilPreValueList[MB_PDU_SIZE_MAX * 8];
    
	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
	iChangedBits = 0;
	iChangedBytes = 0;
	iBits = 0;
	bStarted = FALSE;
	
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psCoilBuf = psMBSlaveDevCur->psDevCurData->psMBCoilTable;
    }
	if( (psCoilBuf->pvDataBuf == NULL) || (psCoilBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	
	myprintf("iSlaveAddr %d  eMBMasterScanWriteCoils   bCheckPreValue %d  \n",ucSndAddr, bCheckPreValue);
	for(iIndex = 0; iIndex < psCoilBuf->usDataCount; iIndex++)
	{
		psCoilValue = (sMasterBitCoilData*)psCoilBuf->pvDataBuf + iIndex;

		/******************* 写线圈，如果对应的变量发生变化则写线圈 ***************************/
        if ( (psCoilValue->pvValue != NULL) && (psCoilValue->ucAccessMode != RO))
		{
			if( (psCoilValue->ucPreVal != *(UCHAR*)psCoilValue->pvValue) || (!bCheckPreValue) )  //线圈地址发生变化或者不检查是否发生变化
            {
				if( iBitInByte % 8 ==0 )   //8bit
			    {
			    	cByteValue = 0;
			    	iBitInByte = 0;
			    	iChangedBytes++;  //字节+1
			    }
				if( *(UCHAR*)psCoilValue->pvValue > 0 )    //线圈状态为1
			    {
			    	cByteValue |= ( 1 << iBitInByte);	//组成一个字节		
			    }
				iBitInByte++;  //偏移+1
			    iChangedBits++;	//发生变化的线圈数量+1

			    pBitCoilPreValueList[iChangedBits-1] =  (UCHAR*)( &(psCoilValue->ucPreVal) );  //记录先前值地址
			    BitCoilByteValueList[iChangedBytes-1] = cByteValue;	  //记录要下发的数据
			}				
		}
		iBits++;  //线圈地址+1
		
		if( iChangedBits == 1 && ( bStarted != TRUE))    //记录首地址
		{
			iStartBit = psCoilValue->usAddr;
			bStarted = TRUE;
			iBits = 1;
			iChangedBytes = 1;
		}	
		if( (psCoilValue->usAddr != (iLastAddr + 1)) && (iChangedBits > 0) && (iBits > 1) )    //地址不连续，则发送写请求
		{
			if(iBits == iChangedBits)   //线圈地址不连续，且该线圈地址也发生了变化
			{
				eStatus = eMBMasterReqWriteMultipleCoils(psMBMasterInfo, ucSndAddr, iStartBit, iChangedBits - 1, 
														(UCHAR*)BitCoilByteValueList, MB_MASTER_WAITING_DELAY);	//写线圈
			    if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPortInfo) == EV_MASTER_PROCESS_SUCCESS )            //如果写入成功，更新数据
			    {	
			    	for( i = iChangedBits; i > 0; i--)
			    	{
						nBits = i%8;
						if( nBits ==0 )
						{
							nBits = 7;
						}
						else if(nBits > 0)
						{
							nBits -= 1;
						}
			    		*(UCHAR*)pBitCoilPreValueList[i- 1] = (BitCoilByteValueList[iChangedBytes-1] & (1<<nBits)) >> nBits;
						 BitCoilByteValueList[iChangedBytes-1] = 0;	
						
						if( (nBits ==7) && (iChangedBytes > 0))
						{
							iChangedBytes -= 1;
						}
			    	}
			    }		
				iChangedBits = 1;
				iChangedBytes = 1;
				iBits = 1;
				
				cByteValue = 0;
			    iBitInByte = 0;
				bStarted = TRUE;
				iStartBit = psCoilValue->usAddr;  //写完一帧得复位所有状态
				
				if( *(UCHAR*)psCoilValue->pvValue > 0 ) //重新记录，以该线圈地址为第一个地址
			    {
			    	cByteValue |= ( 1 << iBitInByte);			
			    }		
			    pBitCoilPreValueList[iChangedBits-1] =  (UCHAR*)( &(psCoilValue->ucPreVal) ); //重新记录
			    BitCoilByteValueList[iChangedBytes-1] = cByteValue;	
			    iBitInByte++;
			}
			else if( iBits > iChangedBits )  //线圈地址不连续，但该线圈地址没发生变化
			{
				eStatus = eMBMasterReqWriteMultipleCoils(psMBMasterInfo, ucSndAddr, iStartBit, iChangedBits, 
														(UCHAR*)BitCoilByteValueList, MB_MASTER_WAITING_DELAY);	//写线圈
			
			    if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPortInfo) == EV_MASTER_PROCESS_SUCCESS )  //如果写入成功，更新数据
			    {	
			    	for( i = iChangedBits; i > 0; i--)
			    	{
						nBits = i%8;
						if( nBits ==0 )
						{
							nBits = 7;
						}
						else if(nBits > 0)
						{
							nBits -= 1;
						}
			    		*(UCHAR*)pBitCoilPreValueList[i- 1] = (BitCoilByteValueList[iChangedBytes-1] & (1<<nBits)) >> nBits;
						 BitCoilByteValueList[iChangedBytes-1] = 0;
						if( (nBits ==7) && (iChangedBytes > 0))
						{
							iChangedBytes -= 1;
						}
			    	}
			    }
				iChangedBits = 0;
				iChangedBytes = 0;
				iBits = 0;
				cByteValue = 0;
			    iBitInByte = 0;
				bStarted = FALSE;   //写完一帧得复位所有状态
			}
		}	
		if( iChangedBits > 0 ) //地址连续
		{
			if( (iBits != iChangedBits) || (iIndex == psCoilBuf->usDataCount-1) 
				|| (iChangedBits >= MB_PDU_SIZE_MAX * 4) ) //地址到达字典最后或者数据超过Modbus数据帧最大字节数，则发送写请求
		    {                                                                                                                                                             			
				eStatus = eMBMasterReqWriteMultipleCoils(psMBMasterInfo, ucSndAddr, iStartBit, iChangedBits, 
		    											(UCHAR*)BitCoilByteValueList, MB_MASTER_WAITING_DELAY);	//写线圈
				
		    	if( xMBMasterPortCurrentEvent(&psMBMasterInfo->sMBPortInfo) == EV_MASTER_PROCESS_SUCCESS ) //如果写入成功，更新数据
		    	{	
		    		for( i = iChangedBits; i > 0; i--)
			    	{
						nBits = i%8;
						if( nBits ==0 )
						{
							nBits = 7;
						}
						else if(nBits > 0)
						{
							nBits -= 1;
						}
			    		*(UCHAR*)pBitCoilPreValueList[i- 1] = (BitCoilByteValueList[iChangedBytes-1] & (1<<nBits)) >> nBits;
						 BitCoilByteValueList[iChangedBytes-1] = 0;
						if( (nBits ==7) && (iChangedBytes > 0))
						{
							iChangedBytes -= 1;
						}
			    	}
		    	}
				iChangedBits = 0;
				iChangedBytes = 0;
		    	iBits = 0;
		    	bStarted = FALSE;
		    	cByteValue = 0;
			    iBitInByte = 0;
		    }
		}
        iLastAddr = psCoilValue->usAddr ;		
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
	UCHAR i;
	USHORT iIndex, iLastAddr, iStartAddr, iCount, iBit, nSlaveTypes;
   
    eMBMasterReqErrCode             eStatus = MB_MRE_NO_ERR;
    sMasterBitDiscData*      pDiscreteValue = NULL;
    
    sMBSlaveDevInfo*     psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    const sMBDevDataTable* psDiscreteBuf = psMBSlaveDevCur->psDevCurData->psMBDiscInTable;  //从设备通讯协议表

	iLastAddr = 0;
	iStartAddr = 0;
	iCount = 0;
	
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psDiscreteBuf = psMBSlaveDevCur->psDevCurData->psMBDiscInTable;
    } 

    if( (psDiscreteBuf->pvDataBuf == NULL) || (psDiscreteBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	
	for(iIndex = 0; iIndex < psDiscreteBuf->usDataCount ; iIndex++)
	{
		pDiscreteValue = (sMasterBitDiscData*)psDiscreteBuf->pvDataBuf + iIndex;	
	
        if( pDiscreteValue->usAddr != (iLastAddr + 1) )     //地址不连续，则发送读请求
		{
			if( iCount > 0)
			{
				eStatus = eMBMasterReqReadDiscreteInputs(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);	
                iCount = 0;
				iStartAddr = pDiscreteValue->usAddr;
			}
			if( pDiscreteValue->ucAccessMode != WO )
			{
				iCount++;
				iStartAddr = pDiscreteValue->usAddr;
			}
		}
		else
		{
			if(	iCount == 0 )
			{
				iStartAddr = pDiscreteValue->usAddr;
			}	
			if( pDiscreteValue->ucAccessMode != WO )
			{
				iCount++;
			}	
		}
		if( ((pDiscreteValue->ucAccessMode == WO) || (iIndex== psDiscreteBuf->usDataCount-1)) && (iCount > 0) )  //如果为只写或者地址到达字典最后发送读请求
		{
             eStatus = eMBMasterReqReadDiscreteInputs(psMBMasterInfo, ucSndAddr, iStartAddr, iCount, MB_MASTER_WAITING_DELAY);	
             iCount = 0;
             iStartAddr = pDiscreteValue->usAddr;
		}	
		if( iCount / 8 >= MB_PDU_SIZE_MAX / 2)                      //数据超过Modbus数据帧最大字节数，则发送读请求
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
