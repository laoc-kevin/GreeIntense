#include <bsp_int.h>
#include <lib_def.h>
#include <os.h>

#include "lpc_gpio.h"
#include "lpc_clkpwr.h"

#include "app_val.h"
#include "app_led.h"
#include "app_dtu.h"
#include "app_modbus.h"
#include "app_output.h"
#include "app_input.h"

#include "mb.h"
#include "mb_m.h"
#include "mbrtu_m.h"
#include "mbfunc_m.h"
#include "user_mb_dict_m.h"
#include "user_mb_scan_m.h"

#include "port.h"
#include "my_rtt_printf.h"

#define MB_MASTER_SCAN_TASK_STK_SIZE  128

/**********************************************************************
*变量声明
************************************************************************/

UART_Def sMBSlaveUart= { &Uart1Rx,&Uart1Tx,&Uart1DE,&Uart1Inv,UART_1,                    /* 从栈串口设置 */
					    {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}         /* 默认串口配置 9600 8n1 */
			          };
	
UART_Def sMBMasterUart = { &Uart0Rx,&Uart0Tx,&Uart0DE,&Uart0Inv,UART_0,                  /* 主栈串口设置 */
						  {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}  
					    };

static void AppMbDevTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev, UCHAR iSlaveAddr);
static void AppMbDevCurStateTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev);
                        
static void AppMbScanSlaveDevice(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev);                    
static void AppMbScanReadSlave(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr);
static void AppMbScanWriteSlave(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr, UCHAR bCheckPreValue); 
                        
static eMBMasterReqErrCode AppMbDevCmdTest(sMBMasterInfo* psMBMasterInfo, const sMBSlaveDevInfo* psMBSlaveDev, 
                                           const sMBTestDevCmd* psMBDevCmd);                        

/**********************************************************************
 * @brief   创建主栈轮询从设备任务
 * @param   *p_arg    
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void AppMbScanSlaveTaskCreate(sMBMasterInfo* psMBMasterInfo, OS_PRIO prio)
{   
    OS_ERR               err = OS_ERR_NONE;
    CPU_STK_SIZE    stk_size = MB_MASTER_SCAN_TASK_STK_SIZE; 
    
    OS_TCB*            p_tcb = (OS_TCB*)calloc(stk_size, sizeof(OS_TCB));    
    CPU_STK*      p_stk_base = (CPU_STK*) calloc(stk_size, sizeof(CPU_STK));
    
    OSTaskCreate( p_tcb,
                  "AppMbScanSlaveTask",
                  AppMbScanSlaveTask,
                  (void*)psMBMasterInfo,
                  prio,
                  p_stk_base ,
                  stk_size / 10u,
                  stk_size,
                  0u,
                  0u,
                  0u,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
                  &err);
}


/**********************************************************************
 * @brief   主栈轮询从设备任务
 * @param   *p_arg    
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void AppMbScanSlaveTask(void *p_arg)
{
	UCHAR iSlaveAddr;
	UCHAR n, iIndex;
	OS_ERR err = OS_ERR_NONE;
	CPU_TS ts = 0;
  
	USHORT msReadInterval = MB_SCAN_READ_SLAVE_INTERVAL_MS;

    eMBMasterReqErrCode       errorCode = MB_MRE_NO_ERR;
    sMBSlaveDevInfo*       psMBSlaveDev = NULL;
    
	sMBMasterInfo*       psMBMasterInfo = (sMBMasterInfo*)p_arg;
    sMBMasterDevsInfo*     psMBDevsInfo = psMBMasterInfo->psMBDevsInfo;       //从设备状态信息
    
    UCHAR ucMaxAddr = psMBDevsInfo->ucSlaveDevMaxAddr;
    UCHAR ucMinAddr = psMBDevsInfo->ucSlaveDevMinAddr;
    
    UCHAR ucAddrSub = ucMaxAddr - ucMinAddr;  //设备地址差
    
    UCHAR* pcDevsScanInv = calloc(ucAddrSub, sizeof(UCHAR));
	UCHAR* pcDevAddrOccupy = calloc(ucAddrSub, sizeof(UCHAR));                      //被占用的从设备通讯地址
    
#ifdef MB_MASTER_DTU_ENABLED     //GPRS模块功能支持	
    AppMbDTUInit(psMBMasterInfo);   //初始化DTU
#endif

    /*************************上电后先对从设备进行在线测试，主要收集各从设备通讯地址和在线状态**********************/

    for(iSlaveAddr = ucMinAddr; iSlaveAddr <= ucMaxAddr; iSlaveAddr++)  
	{
        pcDevsScanInv[iSlaveAddr - ucMinAddr] = ucAddrSub;  //首次上电所以都要测试
    }

	while (DEF_TRUE)
	{
		(void)OSTimeDlyHMSM(0, 0, 0, msReadInterval, OS_OPT_TIME_HMSM_STRICT, &err);
        
#ifdef MB_MASTER_DTU_ENABLED     //GPRS模块功能支持，特殊处理
		AppMbScanDevDTU(psMBMasterInfo);  //轮询DTU模块
#endif
        
		/*********************************轮询从设备***********************************/
		for(psMBSlaveDev = psMBDevsInfo->psMBSlaveDevsList; psMBSlaveDev != NULL; psMBSlaveDev = psMBSlaveDev->pNext)
        {
            if(psMBSlaveDev->ucOnLine == FALSE)
            {
                for(iSlaveAddr = ucMinAddr; iSlaveAddr <= ucMaxAddr; iSlaveAddr++)
                {    
                    if(pcDevAddrOccupy[iSlaveAddr-ucMinAddr] == FALSE)
                    {
                        AppMbDevTest(psMBMasterInfo, psMBSlaveDev, iSlaveAddr);  //确定从设备参数类型测试和设备通讯地址
                        if(psMBSlaveDev->ucOnLine == TRUE)
                        {
                            pcDevAddrOccupy[iSlaveAddr-ucMinAddr] = TRUE;  //从设备通讯地址占用
                            break;
                        }                            
                    }
                }
            }
        }
        for(psMBSlaveDev = psMBDevsInfo->psMBSlaveDevsList; psMBSlaveDev != NULL; psMBSlaveDev = psMBSlaveDev->pNext)
        {
            if(psMBSlaveDev->ucOnLine == TRUE && psMBSlaveDev->ucDevAddr <= ucMaxAddr && psMBSlaveDev->ucDevAddr >= ucMinAddr )
            {
                AppMbDevCurStateTest(psMBMasterInfo, psMBSlaveDev);  //防止从设备可能掉线
                if( (psMBSlaveDev->ucOnLine == TRUE) && (psMBSlaveDev->ucRetryTimes == 0) ) //在线且不处于延时阶段
                {
                    AppMbScanSlaveDevice(psMBMasterInfo, psMBSlaveDev);
                }
                else if(psMBSlaveDev->ucOnLine == FALSE)
                {
                    pcDevAddrOccupy[psMBSlaveDev->ucDevAddr-ucMinAddr] = FALSE;
                }                    
            }
        }     
	}
}       

/**********************************************************************
 * @brief  主栈对从设备未知状态进行测试
 * @param  psMBMasterInfo  主栈信息块
 * @param  psMBSlaveDev    某从设备状态
 * @param  iSlaveAddr      从设备地址
 * @return sMBSlaveDevInfo
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void AppMbDevTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev, UCHAR iSlaveAddr)
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
        
    	errorCode = AppMbDevCmdTest(psMBMasterInfo, psMBSlaveDev, psMBCmd);	
        
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
void AppMbDevCurStateTest(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev)
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
        errorCode = AppMbDevCmdTest(psMBMasterInfo, psMBSlaveDev, psMBCmd);			
     
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
eMBMasterReqErrCode AppMbDevCmdTest(sMBMasterInfo* psMBMasterInfo, const sMBSlaveDevInfo* psMBSlaveDev, 
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

/**********************************************************************
 * @brief   主栈轮询某个从设备
 * @param   psMBMasterInfo  主栈信息块 
 * @param   psMBSlaveDev    从设备
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void AppMbScanSlaveDevice(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev)
{
    eMBMasterReqErrCode errorCode    = MB_MRE_NO_ERR;
    sMBMasterDevsInfo*  psMBDevsInfo = psMBMasterInfo->psMBDevsInfo;      //从设备列表
    UCHAR               iSlaveAddr   = psMBSlaveDev->ucDevAddr;                 //通讯地址
    
    psMBDevsInfo->psMBSlaveDevCur = psMBSlaveDev;   //当前从设备
    
    if( (psMBSlaveDev != NULL) && (psMBSlaveDev->ucOnLine == TRUE) )   //如果设备在线则进行轮询
    {
        if( psMBSlaveDev->ucDataReady == TRUE)   //从栈数据准备好了才同步上来
        {	 	    
            if(psMBSlaveDev->ucSynchronized == FALSE) //重新上线的话，同步所有数据，先读后写
            {
                AppMbScanReadSlave(psMBMasterInfo, iSlaveAddr);			 //读从栈数据		
                AppMbScanWriteSlave(psMBMasterInfo, iSlaveAddr, FALSE);  //同步从栈数据
                psMBSlaveDev->ucSynchronized = TRUE;                     //同步完成
            }
            else   //同步完成后，先写后读
            {
                AppMbScanWriteSlave(psMBMasterInfo, iSlaveAddr, TRUE);  //写有变化数据	
                AppMbScanReadSlave(psMBMasterInfo, iSlaveAddr);			 //读从栈数据										
            }
        }
        else  //从栈数据未好，则只进行写不读
        {
            if(psMBSlaveDev->ucSynchronized == FALSE) 
            {
                AppMbScanWriteSlave(psMBMasterInfo, iSlaveAddr, FALSE);  //同步从栈数据
                psMBSlaveDev->ucSynchronized = TRUE;  //同步完成
            }
            else    
            {
                AppMbScanReadSlave(psMBMasterInfo, iSlaveAddr);			 //读从栈数据
            }
        }
        myprintf("iSlaveAddr %d CHWSwState %d   \n",iSlaveAddr, CHWSwState);
        myprintf("******************** iSlaveAddr %d ***********************\n", iSlaveAddr);
    }		
}

/**********************************************************************
 * @brief   主栈轮询读某个从设备
 * @param   psMBMasterInfo  主栈信息块
 * @param   iSlaveAddr      从设备地址
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void AppMbScanReadSlave(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr)
{
     eMBMasterReqErrCode errorCode    = MB_MRE_NO_ERR;
    
#if MB_FUNC_READ_HOLDING_ENABLED > 0 			
    errorCode = eMBMasterScanReadHoldingRegister(psMBMasterInfo, iSlaveAddr); //读保持寄存器 							
#endif
					
#if MB_FUNC_READ_COILS_ENABLED > 0
    errorCode = eMBMasterScanReadCoils(psMBMasterInfo, iSlaveAddr);           //读线圈
#endif
					
#if MB_FUNC_READ_INPUT_ENABLED > 0				
    errorCode = eMBMasterScanReadInputRegister(psMBMasterInfo, iSlaveAddr);	  //读输入寄存器					
#endif	
				
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    errorCode = eMBMasterScanReadDiscreteInputs(psMBMasterInfo, iSlaveAddr);   //读离散量
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
void AppMbScanWriteSlave(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr, UCHAR bCheckPreValue)
{
    eMBMasterReqErrCode errorCode    = MB_MRE_NO_ERR;
    
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 			
    errorCode = eMBMasterScanWriteHoldingRegister(psMBMasterInfo, iSlaveAddr, bCheckPreValue);	//写保持寄存器 									
#endif
					
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    errorCode = eMBMasterScanWriteCoils(psMBMasterInfo, iSlaveAddr, bCheckPreValue);            //写线圈 
#endif   
}

/**********************************************************************
 * @brief   UART1中断响应函数
 * @return	none
 *********************************************************************/
void UART1_IRQHandler(void)
{
	uint32_t intSrc=0, curIntr=0, lineSts=0;

	/* Determine the interrupt source */
	intSrc = UART_GetIntId(UART_1);
	curIntr = intSrc & UART_IIR_INTID_MASK;

	switch(curIntr)
	{
		case UART1_IIR_INTID_MODEM:			
		break;
		
		case UART_IIR_INTID_RLS:		
			lineSts = UART_GetLineStatus(UART_1);     					// Check line status
			lineSts &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
					  | UART_LSR_BI | UART_LSR_RXFE);                 // Mask out the Receive Ready and Transmit Holding empty status
		break;
		
		case UART_IIR_INTID_RDA:
		case UART_IIR_INTID_CTI:
		    prvvSlaveUARTRxISR("UART1");	   //Modbus Uart ISR
		break;
		
		case UART_IIR_INTID_THRE:
	         prvvSlaveUARTTxReadyISR("UART1");  //Modbus Uart ISR
		break;
		
		default:break ;
	}	
}	

/**********************************************************************
 * @brief   UART0中断响应函数
 * @return	none
 *********************************************************************/
void UART0_IRQHandler(void)
{
	uint32_t intSrc=0, curIntr=0, lineSts=0;

	/* Determine the interrupt source */
	intSrc = UART_GetIntId(UART_0);
	curIntr = intSrc & UART_IIR_INTID_MASK;
	switch(curIntr)
	{	
		case UART_IIR_INTID_RLS:		
			lineSts = UART_GetLineStatus(UART_0);     					// Check line status
			lineSts &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
					  | UART_LSR_BI | UART_LSR_RXFE);                   // Mask out the Receive Ready and Transmit Holding empty status
		break;
		
		case UART_IIR_INTID_RDA:
		case UART_IIR_INTID_CTI:
		     prvvMasterUARTRxISR("UART0");	      //Modbus Master Uart ISR
		break;
		
		case UART_IIR_INTID_THRE:
	         prvvMasterUARTTxReadyISR("UART0");  //Modbus Master Uart ISR
		break;
		
		default:break ;
	}	
}

