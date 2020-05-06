#include <bsp_int.h>
#include <lib_def.h>
#include <os.h>

#include "lpc_gpio.h"
#include "lpc_clkpwr.h"

#include "app_val.h"

#include "md_led.h"
#include "md_dtu.h"
#include "md_modbus.h"
#include "md_output.h"
#include "md_input.h"

#include "mb.h"

#include "mbrtu_m.h"
#include "mbfunc_m.h"
#include "mbtest_m.h"
#include "mbdict_m.h"
#include "mbscan_m.h"

#include "port.h"
#include "my_rtt_printf.h"

#define MB_MASTER_SCAN_TASK_STK_SIZE            128

/**********************************************************************
*变量声明
************************************************************************/

sUART_Def sMBSlaveUart= { &Uart1Rx,&Uart1Tx,&Uart1DE,&Uart1Inv,UART_1,                    /* 从栈串口设置 */
					    {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}         /* 默认串口配置 9600 8n1 */
			          };
	
sUART_Def sMBMasterUart = { &Uart0Rx,&Uart0Tx,&Uart0DE,&Uart0Inv,UART_0,                  /* 主栈串口设置 */
						  {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}  
					    };

                        
static void vMBScanSlaveDevTask(void *p_arg);                        

/**********************************************************************
 * @brief   创建主栈轮询从设备任务
 * @param   *p_arg    
 * @return	none
 * @author  laoc
 * @date    2019.01.22
 *********************************************************************/
void vMBScanSlaveDevTaskCreate(sMBMasterInfo* psMBMasterInfo, OS_PRIO prio)
{   
    OS_ERR               err = OS_ERR_NONE;
    CPU_STK_SIZE    stk_size = MB_MASTER_SCAN_TASK_STK_SIZE; 
    
    OS_TCB*            p_tcb = (OS_TCB*)calloc(stk_size, sizeof(OS_TCB));    
    CPU_STK*      p_stk_base = (CPU_STK*) calloc(stk_size, sizeof(CPU_STK));
    
    OSTaskCreate( p_tcb,
                  "vMBScanSlaveDevTask",
                  vMBScanSlaveDevTask,
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
void vMBScanSlaveDevTask(void *p_arg)
{
	UCHAR iSlaveAddr;
	UCHAR n, iIndex;
	OS_ERR err = OS_ERR_NONE;
	CPU_TS ts = 0;
  
	USHORT msReadInterval = MB_SCAN_READ_SLAVE_INTERVAL_MS;

    eMBMasterReqErrCode       errorCode = MB_MRE_NO_ERR;
    sMBSlaveDevInfo*       psMBSlaveDev = NULL;
    
	sMBMasterInfo*       psMBMasterInfo = (sMBMasterInfo*)p_arg;
    sMBMasterDevsInfo*     psMBDevsInfo = &psMBMasterInfo->sMBDevsInfo;       //从设备状态信息
    
    UCHAR ucMaxAddr = psMBDevsInfo->ucSlaveDevMaxAddr;
    UCHAR ucMinAddr = psMBDevsInfo->ucSlaveDevMinAddr;
    
    UCHAR ucAddrSub = ucMaxAddr - ucMinAddr;  //设备地址差
    
    UCHAR* pcDevsScanInv = calloc(ucAddrSub, sizeof(UCHAR));
	UCHAR* pcDevAddrOccupy = calloc(ucAddrSub, sizeof(UCHAR));                      //被占用的从设备通讯地址
    
#ifdef MB_MASTER_DTU_ENABLED     //GPRS模块功能支持	
    vDTUInit(psMBMasterInfo);    //初始化DTU
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
		vDTUScanDev(psMBMasterInfo);  //轮询DTU模块
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
                        vMBDevTest(psMBMasterInfo, psMBSlaveDev, iSlaveAddr);  //确定从设备参数类型测试和设备通讯地址
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
                vMBDevCurStateTest(psMBMasterInfo, psMBSlaveDev);  //防止从设备可能掉线
                if( (psMBSlaveDev->ucOnLine == TRUE) && (psMBSlaveDev->ucRetryTimes == 0) ) //在线且不处于延时阶段
                {
                    vMBScanSlaveDev(psMBMasterInfo, psMBSlaveDev);
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

