#include "dtu.h"
#include "mbfunc_m.h"
#include "mbscan_m.h"

#ifdef MB_MASTER_DTU_ENABLED     //GPRS模块功能支持

#define DTU_PROTOCOL_VERSIPON                   0x0B    //DTU通讯协议版本号 V1.1

#define DTU_TIMEOUT_S                           30
#define TMR_TICK_PER_SECOND                     OS_CFG_TMR_TASK_RATE_HZ

#define DTU247_INIT_START_REG_HOLD_ADDR         0x00
#define INIT_DTU247_REG_HOLD_VALUE              0xF0
#define INITED_DTU247_REG_HOLD_VALUE            0x0F

#define DTU200_SLAVE_ADDR                       200
#define DTU247_SLAVE_ADDR                       247

#define TEST_DTU200_PROTOCOL_REG_HOLD_ADDR      0
#define TEST_DTU200_PROTOCOL_REG_HOLD_VALUE     0x5601

#define TEST_DTU247_PROTOCOL_REG_HOLD_ADDR      1
#define TEST_DTU247_PROTOCOL_REG_HOLD_VALUE     0x0F

#define TEST_DTU247_PROTOCOL_REG_IN_ADDR        12
#define TEST_DTU247_PROTOCOL_REG_IN_VALUE       0xFF

static USHORT usDTUInitCmd[2]  ={DTU_PROTOCOL_VERSIPON, INIT_DTU247_REG_HOLD_VALUE};
static USHORT usDTUInitedCmd[2]={DTU_PROTOCOL_VERSIPON, INITED_DTU247_REG_HOLD_VALUE};	


/*DTU模块定时器中断*/
void vDTU_TimeoutInd(void * p_tmr, void * p_arg)
{
    DTU* pThis = (DTU*)p_arg;
    pThis->ucDTUInited = TRUE;
}

/*DTU模块初始化定时器*/
BOOL xDTU_TmrTimeoutInit(DTU* pt, USHORT usTimerout)
{
	OS_ERR err = OS_ERR_NONE;

	DTU* pThis = (DTU*)pt;
    
	ULONG i = usTimerout  * TMR_TICK_PER_SECOND; 
    ULONG n = 2 * TMR_TICK_PER_SECOND;
	
	OSTmrCreate(&pThis->DTUTimerTimeout,       //主定时器
			    "DTUTimerTimeout",
			    i,      
			    0,
			    OS_OPT_TMR_ONE_SHOT,
			    vDTU_TimeoutInd,
			    (void*)pThis,
			    &err);
	return (err == OS_ERR_NONE);
}

/*DTU模块定时器使能*/
void vDTU_TmrTimeoutEnable(DTU* pt)
{
	OS_ERR err = OS_ERR_NONE;
    DTU* pThis = (DTU*)pt;
    
	pThis->ucDTUInited = FALSE;
    (void)OSTmrStart(&pThis->DTUTimerTimeout, &err);
}	


/*DTU测试*/
void vDTU_DevTest(DTU* pt)
{
    UCHAR n, ucSlaveDevMaxAddr, ucSlaveDevMinAddr;
    
    DTU* pThis = (DTU*)pt;
    
	OS_ERR                      err     = OS_ERR_NONE;
    eMBMasterReqErrCode   errorCode     = MB_MRE_EILLSTATE;
    sMBMasterInfo*       psMBMasterInfo = pThis->psMBMasterInfo;
	sMBMasterDevsInfo*    psMBDevsInfo  = &psMBMasterInfo->sMBDevsInfo;   //从设备状态表

    if( (psMBMasterInfo->bDTUEnable == FALSE) || (!pThis->ucDTUInited) )
    {
        return;
    }
    /***********************开始测试DTU的状态*************************************/
    ucSlaveDevMaxAddr = psMBDevsInfo->ucSlaveDevMaxAddr;  //切换最大，最小地址
    ucSlaveDevMinAddr = psMBDevsInfo->ucSlaveDevMinAddr;
    
    psMBDevsInfo->ucSlaveDevMaxAddr = DTU247_SLAVE_ADDR;
    psMBDevsInfo->ucSlaveDevMinAddr = DTU200_SLAVE_ADDR;
    
    psMBMasterInfo->xMBRunInTestMode = TRUE;  //处于测试从设备状态
    for(n = 0; n < 5; n++)
    {	
#if  MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 
        
        //查看GPRS模块是否已完成初始化		
        errorCode = eMBMasterReqWriteMultipleHoldingRegister(psMBMasterInfo, DTU247_SLAVE_ADDR, 
                                                             DTU247_INIT_START_REG_HOLD_ADDR, 
            								                 2, pThis->psDTUInitedCmd, 
                                                             MB_MASTER_WAITING_DELAY);                                 
#endif       
   	    if(errorCode == MB_MRE_NO_ERR)
   	    {
   	    	break;
   	    }
        else if(errorCode == MB_MRE_ILL_ARG)
        {
            continue;
        }
        (void)OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    
    if (errorCode != MB_MRE_NO_ERR)  //未完成初始化
    {
	
#if  MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0    
      	errorCode = eMBMasterReqWriteMultipleHoldingRegister(psMBMasterInfo, DTU247_SLAVE_ADDR, 
                                                             DTU247_INIT_START_REG_HOLD_ADDR, 
                                                             2, pThis->psDTUInitCmd, 
                                                             MB_MASTER_WAITING_DELAY);	//进行初始化
#endif
        pThis->sDevDTU247.ucOnLine = FALSE;
        pThis->sDevDTU200.ucOnLine = FALSE;
                      
        pThis->sDevDTU247.ucDataReady = FALSE;
        pThis->sDevDTU200.ucDataReady = FALSE;
        
        vDTU_TmrTimeoutEnable(pThis);
    }
    else   //GPRS模块已经初始化
    {
        pThis->sDevDTU247.ucOnLine = TRUE;  
        pThis->sDevDTU200.ucOnLine = TRUE;  
  
#if MB_FUNC_READ_INPUT_ENABLED > 0                          
		errorCode = eMBMasterReqReadInputRegister(psMBMasterInfo, DTU247_SLAVE_ADDR, 
                                                  TEST_DTU247_PROTOCOL_REG_IN_ADDR, 
                                                  1, MB_MASTER_WAITING_DELAY);     //查看GPRS模块参数是否改变	
#endif               
        if (errorCode == MB_MRE_NO_ERR)  //GPRS模块参数改变完毕
        {   
             pThis->sDevDTU247.ucDataReady = TRUE;  					
        }    
    }
    psMBMasterInfo->xMBRunInTestMode = FALSE;  //退出测试从设备状态 
    
    psMBDevsInfo->ucSlaveDevMaxAddr = ucSlaveDevMaxAddr;
    psMBDevsInfo->ucSlaveDevMinAddr = ucSlaveDevMinAddr;   
}

/*通讯映射函数*/
USHORT usDTU247_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID,  USHORT usAddr)
{

    
    
    
}

/*通讯映射函数*/
USHORT usDTU200_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID,  USHORT usAddr)
{

    
    
    
}

/*DTU注册通讯数据表*/
void vDTU247_InitCommData(DTU* pt)
{
    DTU* pThis = (DTU*)pt;
    
    
    
    
    pThis->sDevDataDTU247.psMBDevDataMapIndex = usDTU247_DevDataMapIndex;    //绑定映射函数
    pThis->sDevDTU247.psDevDataInfo = &(pThis->sDevDataDTU247);
    
}


/*DTU注册通讯数据表*/
void vDTU200_InitCommData(DTU* pt)
{
    DTU* pThis = (DTU*)pt;
    
    
    
   
    pThis->sDevDataDTU200.psMBDevDataMapIndex = usDTU200_DevDataMapIndex;    //绑定映射函数
    pThis->sDevDTU200.psDevDataInfo = &(pThis->sDevDataDTU200);
}


/*DTU轮询*/
void vDTU_ScanDev(void* pt)
{
    UCHAR ucSlaveDevMaxAddr, ucSlaveDevMinAddr;
    
    DTU* pThis = (DTU*)pt;
    
    eMBMasterReqErrCode  errorCode     = MB_MRE_EILLSTATE;
    
    sMBMasterInfo*      psMBMasterInfo = pThis->psMBMasterInfo;
	sMBMasterDevsInfo*  psMBDevsInfo   = &psMBMasterInfo->sMBDevsInfo;   //从设备状态表
                 
    if( (psMBMasterInfo->bDTUEnable == FALSE) || (!pThis->ucDTUInited) )
    {
        return;
    }
    ucSlaveDevMaxAddr = psMBDevsInfo->ucSlaveDevMaxAddr;  //切换最大，最小地址
    ucSlaveDevMinAddr = psMBDevsInfo->ucSlaveDevMinAddr;
    
    psMBDevsInfo->ucSlaveDevMaxAddr = DTU247_SLAVE_ADDR;
    psMBDevsInfo->ucSlaveDevMinAddr = DTU200_SLAVE_ADDR;
    
    vDTU_DevTest(pThis);
    
    /***********************开始轮询DTU*************************************/
	if( pThis->sDevDTU200.ucOnLine == TRUE)   //轮询DTU模块数据表
	{
        psMBDevsInfo->psMBSlaveDevCur = &pThis->sDevDTU200;
        if(psMBDevsInfo->psMBSlaveDevCur->psDevCurData == NULL)               //数据表为空则不进行轮询
        {
            return;
        }
#if  MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 		
		errorCode =eMBMasterScanWriteHoldingRegister(psMBMasterInfo, DTU200_SLAVE_ADDR, FALSE);
#endif
	}
    
    if( (pThis->sDevDTU247.ucOnLine == TRUE) && (pThis->sDevDTU247.ucDataReady == TRUE) )   //
    {
        psMBDevsInfo->psMBSlaveDevCur = &pThis->sDevDTU247;
        if(psMBDevsInfo->psMBSlaveDevCur->psDevCurData == NULL)               //数据表为空则不进行轮询
        {
            return;
        }       
#if MB_FUNC_READ_INPUT_ENABLED > 0				
        errorCode = eMBMasterScanReadInputRegister(psMBMasterInfo, DTU247_SLAVE_ADDR);						
#endif   
	}
    psMBDevsInfo->ucSlaveDevMaxAddr = ucSlaveDevMaxAddr;
    psMBDevsInfo->ucSlaveDevMinAddr = ucSlaveDevMinAddr;
}

/*DTU初始化*/
BOOL xDTU_Init(DTU* pt)
{
    DTU* pThis = (DTU*)pt;
   
    pThis->psDTUInitCmd   = usDTUInitCmd;
    pThis->psDTUInitedCmd = usDTUInitedCmd;
     
    pThis->sDevDTU247.ucDevAddr = DTU247_SLAVE_ADDR;      //DTU247通讯地址
    pThis->sDevDTU200.ucDevAddr = DTU200_SLAVE_ADDR;      //DTU200通讯地址
    
    vDTU247_InitCommData(pThis);
    vDTU200_InitCommData(pThis);
    
    
    
    
    
    
    
    
    
    
    
    return xDTU_TmrTimeoutInit(pThis, DTU_TIMEOUT_S);    
}

#endif
