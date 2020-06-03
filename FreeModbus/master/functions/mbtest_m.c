#include "mbtest_m.h"
#include "mbfunc_m.h"

#define MB_TEST_RETRY_TIMES   2

/**********************************************************************
 * @brief   主栈对从设备发送命令
 * @param   psMBMasterInfo  主栈信息块
 * @param   psMBSlaveDev    从设备
 * @param   ucSlaveAddr     从设备地址
 * @return	eMBMasterReqErrCode
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
eMBMasterReqErrCode eMBDevCmdTest(sMBMasterInfo* psMBMasterInfo, const sMBSlaveDev* psMBSlaveDev, 
                                  const sMBTestDevCmd* psMBDevCmd, UCHAR ucSlaveAddr)
{
    eMBMasterReqErrCode errorCode = MB_MRE_EILLSTATE;
   
    if(psMBDevCmd == NULL)
    {
         return MB_MRE_ILL_ARG;
    }
    if(psMBDevCmd->eCmdMode == WRITE_REG_HOLD)
    {
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 
        errorCode = eMBMasterReqWriteHoldingRegister(psMBMasterInfo, ucSlaveAddr, psMBDevCmd->usAddr, 
                                                     psMBDevCmd->usValue, MB_MASTER_WAITING_DELAY);   //测试从设备
#endif						
    }   
    if( psMBDevCmd->eCmdMode == READ_REG_HOLD )
    {
#if MB_FUNC_READ_HOLDING_ENABLED > 0 
        errorCode = eMBMasterReqReadHoldingRegister(psMBMasterInfo, ucSlaveAddr, psMBDevCmd->usAddr, 
                                                    1, MB_MASTER_WAITING_DELAY);   //测试从设备
#endif						
    }
    if(psMBDevCmd->eCmdMode == READ_REG_IN)
    {				
#if MB_FUNC_READ_INPUT_ENABLED > 0						
        errorCode = eMBMasterReqReadInputRegister(psMBMasterInfo, ucSlaveAddr, psMBDevCmd->usAddr, 
                                                  1, MB_MASTER_WAITING_DELAY);     //测试从设备
#endif						
    }
    return errorCode;
}

#if MB_MASTER_HEART_BEAT_ENABLED
/**********************************************************************
 * @brief   主栈对从设备发送心跳帧
 * @param   psMBMasterInfo  主栈信息块
 * @param   psMBSlaveDev    从设备
 * @return	eMBMasterReqErrCode
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
eMBMasterReqErrCode eMBDevHeartBeat(sMBMasterInfo* psMBMasterInfo, const sMBSlaveDev* psMBSlaveDev)
{
    eMBMasterReqErrCode errorCode = MB_MRE_EILLSTATE;
    sMBDevHeartBeat* psDevHeartBeat = &psMBSlaveDev->psDevCurData->sMBDevHeartBeat;
    
    if( (psDevHeartBeat == NULL) || (psDevHeartBeat->xHeartBeatEnable == FALSE) )
    {
         return MB_MRE_ILL_ARG;
    }
    
    if(psDevHeartBeat->eCmdMode == WRITE_REG_HOLD)
    {
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 
        errorCode = eMBMasterReqWriteHoldingRegister(psMBMasterInfo, psMBSlaveDev->ucDevAddr, psDevHeartBeat->usAddr, 
                                                     psDevHeartBeat->usValue, MB_MASTER_WAITING_DELAY);   //心跳
#endif						
    }
    if(psDevHeartBeat->eCmdMode == READ_REG_HOLD)
    {
#if MB_FUNC_READ_HOLDING_ENABLED > 0 
        errorCode = eMBMasterReqReadHoldingRegister(psMBMasterInfo, psMBSlaveDev->ucDevAddr, psDevHeartBeat->usAddr, 
                                                    1, MB_MASTER_WAITING_DELAY);   //心跳
#endif					
    }
    if(psDevHeartBeat->eCmdMode == READ_REG_IN)
    {				
#if MB_FUNC_READ_INPUT_ENABLED > 0						
        errorCode = eMBMasterReqReadInputRegister(psMBMasterInfo, psMBSlaveDev->ucDevAddr, psDevHeartBeat->usAddr, 
                                                  1, MB_MASTER_WAITING_DELAY);     //心跳
#endif
    }        
    return errorCode;
}
#endif	

/**********************************************************************
 * @brief  主栈对从设备未知状态进行测试
 * @param  psMBMasterInfo  主栈信息块
 * @param  psMBSlaveDev    某从设备状态
 * @param  ucSlaveAddr      从设备地址
 * @return sMBSlaveDev
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBDevTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDev* psMBSlaveDev, UCHAR ucSlaveAddr)
{
    UCHAR   n, iIndex, nSlaveTypes;
    USHORT  usAddr, usDataVal;
    
    eMBMasterReqErrCode  errorCode       = MB_MRE_EILLSTATE;

    UCHAR*               pcPDUCur        = NULL;
    sMBSlaveDevCommData* psMBDevData     = NULL;       //某从设备数据域
    const sMBTestDevCmd* psMBCmd         = NULL;       //某从设备测试命令表
    
    psMBMasterInfo->xMBRunInTestMode = TRUE;  //接口处于测试从设备状态
    
    for(psMBDevData = psMBSlaveDev->psDevDataInfo;  psMBDevData != NULL; psMBDevData = psMBDevData->pNext)  
    {
        psMBCmd = &psMBDevData->sMBDevCmdTable;
        if(psMBCmd == NULL)
        {
            continue;
        }
        
    	errorCode = eMBDevCmdTest(psMBMasterInfo, psMBSlaveDev, psMBCmd, ucSlaveAddr);	
        if( errorCode == MB_MRE_NO_ERR ) //证明从设备有反应
        {
            pcPDUCur = psMBMasterInfo->pucMasterPDUCur + MB_PDU_DATA_OFF;  //当前帧的数据域
        
            usAddr = ( (USHORT)(*pcPDUCur++) ) << 8;     //地址 
            usAddr |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            usDataVal = ( (USHORT)(*pcPDUCur++) ) << 8;   //数据
            usDataVal |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            psMBSlaveDev->ucDevAddr       = ucSlaveAddr;                //从设备通讯地址
            psMBSlaveDev->xOnLine         = TRUE;                       //从设备反馈正确，则设备在线
            psMBSlaveDev->psDevCurData    = psMBDevData;                //从设备当前数据域
            psMBSlaveDev->ucProtocolID    = psMBDevData->ucProtocolID;  //从设备协议ID
            
            if(usAddr == psMBCmd->usAddr)  //地址一致
            {   
                if(psMBCmd->xCheckVal)  //测试时比较数值
                {
                    psMBSlaveDev->xDataReady = (usDataVal == psMBCmd->usValue) ? TRUE:FALSE;  //从设备数据准备好                               
                }
                else
                {
                     psMBSlaveDev->xDataReady = TRUE; 
                }
            }
            else
            {
                psMBSlaveDev->xDataReady = FALSE;
            }
            break; 				
        }
    }
    psMBMasterInfo->xMBRunInTestMode = FALSE;  //退出测试从设备状态  
}

/**********************************************************************
 * @brief   主栈对从设备当前状态测试
 * @param   psMBMasterInfo  主栈信息块
 * @param   psMBSlaveDev    从设备
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBDevCurStateTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDev* psMBSlaveDev)
{
    UCHAR   n, iIndex, nSlaveTypes;
    USHORT  usAddr, usDataVal;
    
    eMBMasterReqErrCode      errorCode = MB_MRE_EILLSTATE;
    
    UCHAR*                    pcPDUCur = NULL;
    const sMBTestDevCmd*       psMBCmd = NULL;
    sMBMasterDevsInfo*    psMBDevsInfo = NULL;
    
    if( (psMBSlaveDev == NULL) || (psMBSlaveDev->xDevOnTimeout == TRUE)) //是否处于延时阶段
    {
        return;
    }
    psMBCmd = &psMBSlaveDev->psDevCurData->sMBDevCmdTable;  //从设备命令列表

    if(psMBCmd == NULL)
    {
         return;
    }
    
    /****************************测试设备**********************************/
    psMBMasterInfo->xMBRunInTestMode = TRUE;  //接口处于测试从设备状态
    for( n=0; n<MB_TEST_RETRY_TIMES; n++ )
    {
        errorCode = eMBDevCmdTest(psMBMasterInfo, psMBSlaveDev, psMBCmd, psMBSlaveDev->ucDevAddr);			
     
        if(errorCode == MB_MRE_NO_ERR) //证明从设备有反应
        {
            pcPDUCur = psMBMasterInfo->pucMasterPDUCur + MB_PDU_DATA_OFF;  //当前帧的数据域
        
            usAddr = ( (USHORT)(*pcPDUCur++) ) << 8;     //地址 
            usAddr |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            usDataVal = ( (USHORT)(*pcPDUCur++) ) << 8;   //数据
            usDataVal |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            psMBSlaveDev->xOnLine        = TRUE;                      //从设备反馈正确，则设备在线
            psMBSlaveDev->ucRetryTimes    = 0;                         //测试次数清零
            
            if(usAddr == psMBCmd->usAddr)  //地址一致
            {   
                if(psMBCmd->xCheckVal)  //测试时比较数值
                {
                     if(usDataVal == psMBCmd->usValue)
                     {
                          psMBSlaveDev->xDataReady = TRUE;   //从设备数据准备好
                     }
                     else                                                           
                     { 
                         psMBSlaveDev->xDataReady = FALSE;  //反馈正确，但测试值不一致
                     }                                    
                }
                else
                {
                     psMBSlaveDev->xDataReady = TRUE; 
                }
            }
            else
            {
                psMBSlaveDev->xDataReady = FALSE;
            }
            break;
        }			
    }
    if(errorCode != MB_MRE_NO_ERR)  //多次测试仍返回错误
    {
        psMBSlaveDev->xDataReady = FALSE;
        
        if(psMBSlaveDev->ucRetryTimes == MB_TEST_RETRY_TIMES)  //前两周期测试都报故障
        {
            psMBSlaveDev->xOnLine           = FALSE;                   //从设备掉线
            psMBSlaveDev->ucDevCurTestAddr   = 0;                       //从设备当前测试通讯地址置零
        }
        else
        {
            psMBSlaveDev->ucRetryTimes++;
            vMBMastersDevOfflineTmrEnable(psMBSlaveDev);
        }            
    }
    psMBMasterInfo->xMBRunInTestMode = FALSE;  //退出测试从设备状态    
}




