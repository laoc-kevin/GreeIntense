#include "mbtest_m.h"
#include "mbfunc_m.h"

static eMBMasterReqErrCode vMBDevCmdTest(sMBMasterInfo* psMBMasterInfo, const sMBSlaveDevInfo* psMBSlaveDev, 
                                         const sMBTestDevCmd* psMBDevCmd);   


/**********************************************************************
 * @brief  主栈对从设备未知状态进行测试
 * @param  psMBMasterInfo  主栈信息块
 * @param  psMBSlaveDev    某从设备状态
 * @param  iSlaveAddr      从设备地址
 * @return sMBSlaveDevInfo
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBDevTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev, UCHAR iSlaveAddr)
{
    UCHAR   n, iIndex, nSlaveTypes;
    USHORT  usAddr, usDataVal;
    
    eMBMasterReqErrCode  errorCode       = MB_MRE_EILLSTATE;

    UCHAR*               pcPDUCur        = NULL;
    sMBSlaveDevDataInfo* psMBDevData     = NULL;       //某从设备数据域
    const sMBTestDevCmd* psMBCmd         = NULL;       //某从设备测试命令表
    
    psMBMasterInfo->xMBRunInTestMode = TRUE;  //接口处于测试从设备状态
    
    for(psMBDevData = psMBSlaveDev->psDevDataInfo;  psMBDevData != NULL; psMBDevData = psMBDevData->pNext)  
    {
        psMBCmd = psMBDevData->psMBDevCmdTable;
        
    	errorCode = vMBDevCmdTest(psMBMasterInfo, psMBSlaveDev, psMBCmd);	
        
        if( errorCode == MB_MRE_NO_ERR ) //证明从设备有反应
        {
            pcPDUCur = psMBMasterInfo->pucMasterPDUCur + MB_PDU_DATA_OFF;  //当前帧的数据域
        
            usAddr = ( (USHORT)(*pcPDUCur++) ) << 8;     //地址 
            usAddr |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            usDataVal = ( (USHORT)(*pcPDUCur++) ) << 8;   //数据
            usDataVal |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            psMBSlaveDev->ucDevAddr       = iSlaveAddr;                 //从设备通讯地址
            psMBSlaveDev->ucOnLine        = TRUE;                       //从设备反馈正确，则设备在线
            psMBSlaveDev->psDevCurData    = psMBDevData;                //从设备当前数据域
            psMBSlaveDev->ucProtocolID    = psMBDevData->ucProtocolID;  //从设备协议ID
            
            if( (usAddr == psMBCmd->ucAddr) && (usDataVal == psMBCmd->usValue) )  //测试值一致
            {   
                psMBSlaveDev->ucDataReady     = TRUE;                       //从设备数据准备好
            }
            else                                                            //反馈正确，但测试值不一致
            { 
                psMBSlaveDev->ucDataReady = FALSE;
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
void vMBDevCurStateTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev)
{
    UCHAR   n, iIndex, nSlaveTypes;
    USHORT  usAddr, usDataVal;
    
    eMBMasterReqErrCode      errorCode = MB_MRE_EILLSTATE;
    
    UCHAR*                    pcPDUCur = NULL;
    const sMBTestDevCmd*       psMBCmd = NULL;
    sMBMasterDevsInfo*    psMBDevsInfo = NULL;
    
    if(psMBSlaveDev == NULL)
    {
        return;
    }
    if(psMBSlaveDev->ucDevOnTimeout == TRUE) //是否处于延时阶段
    {
        return;
    } 
    psMBCmd = psMBSlaveDev->psDevCurData->psMBDevCmdTable;  //从设备命令列表

    /****************************测试设备**********************************/
    psMBMasterInfo->xMBRunInTestMode = TRUE;  //接口处于测试从设备状态
    for( n=0; n<2; n++ )
    {
        errorCode = vMBDevCmdTest(psMBMasterInfo, psMBSlaveDev, psMBCmd);			
     
        if( errorCode == MB_MRE_NO_ERR ) //证明从设备有反应
        {
            pcPDUCur = psMBMasterInfo->pucMasterPDUCur + MB_PDU_DATA_OFF;  //当前帧的数据域
        
            usAddr = ( (USHORT)(*pcPDUCur++) ) << 8;     //地址 
            usAddr |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            usDataVal = ( (USHORT)(*pcPDUCur++) ) << 8;   //数据
            usDataVal |=( (USHORT)(*pcPDUCur++) ) & 0xFF;
            
            psMBSlaveDev->ucOnLine        = TRUE;                      //从设备反馈正确，则设备在线
            psMBSlaveDev->ucRetryTimes    = 0;                         //测试次数清零
            
            if( (usAddr == psMBCmd->ucAddr) && (usDataVal == psMBCmd->usValue) )
            {   
                psMBSlaveDev->ucDataReady     = TRUE;                      //从设备数据准备完毕  
            }
            else 
            {
                psMBSlaveDev->ucDataReady = FALSE; 
            }
            break;
        }			
    }
    if(errorCode != MB_MRE_NO_ERR)  //多次测试仍返回错误
    {
        psMBSlaveDev->ucDataReady = FALSE;
        
        if(psMBSlaveDev->ucRetryTimes == 2)  //前两次测试都报故障
        {
            psMBSlaveDev->ucOnLine    = FALSE;                   //从设备掉线
        }
        else
        {
            psMBSlaveDev->ucRetryTimes++;
            vMBMastersDevOfflineTmrEnable(psMBSlaveDev);
        }            
    }
    psMBMasterInfo->xMBRunInTestMode = FALSE;  //退出测试从设备状态    
}

/**********************************************************************
 * @brief   主栈对从设备发送命令
 * @param   psMBMasterInfo  主栈信息块
 * @param   psMBSlaveDev    从设备
 * @return	eMBMasterReqErrCode
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
eMBMasterReqErrCode vMBDevCmdTest(sMBMasterInfo* psMBMasterInfo, const sMBSlaveDevInfo* psMBSlaveDev, 
                                    const sMBTestDevCmd* psMBDevCmd)
{
    eMBMasterReqErrCode errorCode   = MB_MRE_EILLSTATE;
   
    if( psMBDevCmd->eCmdMode == READ_REG_HOLD )
    {
#if MB_FUNC_READ_HOLDING_ENABLED > 0 
        errorCode = eMBMasterReqReadHoldingRegister(psMBMasterInfo, psMBSlaveDev->ucDevAddr, psMBDevCmd->ucAddr, 
                                                    1, MB_MASTER_WAITING_DELAY);   //测试从设备
#endif						
    }
    else if(psMBDevCmd->eCmdMode == READ_REG_IN)
    {				
#if MB_FUNC_READ_INPUT_ENABLED > 0						
        errorCode = eMBMasterReqReadInputRegister(psMBMasterInfo, psMBSlaveDev->ucDevAddr, psMBDevCmd->ucAddr, 
                                                  1, MB_MASTER_WAITING_DELAY);     //测试从设备
#endif						
    }
    return errorCode;
}