#include <bsp_int.h>
#include <lib_def.h>
#include <os.h>

#include "lpc_gpio.h"
#include "lpc_clkpwr.h"

#include "app_val.h"

#include "md_led.h"

#include "md_modbus.h"
#include "md_output.h"
#include "md_input.h"

#include "mb.h"
#include "mb_m.h"

#include "mbrtu_m.h"
#include "mbfunc_m.h"
#include "mbtest_m.h"
#include "mbdict_m.h"
#include "mbscan_m.h"

#include "port.h"
#include "my_rtt_printf.h"

#define MB_DEFAULT_SLAVE_POLL_TASK_PRIO    9

#define MB_DEFAULT_MASTER_POLL_TASK_PRIO   10
#define MB_DEFAULT_MASTER_SCAN_TASK_PRIO   11

#define MB_MASTER_LED_SEND_DELAY_MS      30
#define MB_MASTER_LED_RECEIVE_DELAY_MS   50
/**********************************************************************
*变量声明
************************************************************************/
sMBMasterInfo     MBMasterInfo;         //主栈接口
sMBSlaveInfo      MBSlaveInfo;          //从栈接口

BOOL MBMasterLedState = 0;
BOOL MBSlaveLedState  = 0;

sUART_Def MBSlaveUart= { &Uart1Rx, &Uart1Tx, &Uart1DE, &Uart1Inv, UART_1,                /* 从栈串口设置 */
                         {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}        /* 默认串口配置 9600 8n1 */
		               };
	
sUART_Def MBMasterUart = { &Uart0Rx, &Uart0Tx, &Uart0DE, &Uart0Inv, UART_0,              /* 主栈串口设置 */
                          {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}  
					     };

sMBMasterNodeInfo MBMasterNode = { MB_RTU, &MBMasterUart, "UART0",                        /* 主栈配置信息 */
                                   MB_MASTER_MIN_DEV_ADDR, MB_MASTER_MAX_DEV_ADDR,
                                   MB_DEFAULT_MASTER_POLL_TASK_PRIO, MB_DEFAULT_MASTER_SCAN_TASK_PRIO, 
                                   TRUE
                                 };

sMBSlaveNodeInfo  MBSlaveNode = {MB_RTU, &MBSlaveUart, "UART1", NULL, MB_DEFAULT_SLAVE_POLL_TASK_PRIO}; /* 从栈配置信息 */
                                                                   
/******************************************************************
*@brief 主栈数据接收回调								
******************************************************************/
void  vModbusMasterReceiveCallback(void* p_arg)
{
    OS_ERR err = OS_ERR_NONE;
    
    vLedStateChange(&LedModbus1, MBMasterLedState);
    MBMasterLedState = !MBMasterLedState;
} 

/******************************************************************
*@brief 主栈数据发送回调								
******************************************************************/
void  vModbusMasterSendCallback(void* p_arg)  
{
    OS_ERR err = OS_ERR_NONE;

    vLedStateChange(&LedModbus1, MBMasterLedState);
    MBMasterLedState = !MBMasterLedState;
} 

/******************************************************************
*@brief 从栈数据接收回调								
******************************************************************/
void  vModbusSlaveReceiveCallback(void* p_arg)
{
    vLedOn(&LedModbus2);
} 

/******************************************************************
*@brief 从栈数据发送回调								
******************************************************************/
void  vModbusSlaveSendCallback(void* p_arg)  
{
    vLedOff(&LedModbus2);
} 

/**********************************************************************
 * @brief  MODBUS主栈初始化
 *********************************************************************/
void vModbusMasterInit(OS_PRIO ucPollPrio, OS_PRIO ucScanPrio)
{
    MBMasterNode.ucMasterPollPrio = ucPollPrio;
    MBMasterNode.ucMasterScanPrio = ucScanPrio;
    
    (void)xMBMasterRegistNode(&MBMasterInfo, &MBMasterNode);
    
    //modbus回调函数
    pvMBMasterReceiveCallback = vModbusMasterReceiveCallback;
    pvMBMasterSendCallback    = vModbusMasterSendCallback;
}
  
/**********************************************************************
 * @brief  MODBUS从栈初始化
 *********************************************************************/
void vModbusSlaveInit(OS_PRIO prio)
{
    MBSlaveNode.pcSlaveAddr = pcGetControllerID();
    MBSlaveNode.ucSlavePollPrio = prio;

    (void)xMBSlaveRegistNode(&MBSlaveInfo, &MBSlaveNode);

    pvMBSlaveReceiveCallback  = vModbusSlaveReceiveCallback;
    pvMBSlaveSendCallback     = vModbusSlaveSendCallback;  	    
}

/******************************************************************
*@brief 获取主栈地址								
******************************************************************/
sMBMasterInfo*  psMBGetMasterInfo(void)
{
    return &MBMasterInfo;
}

/******************************************************************
*@brief 获取从栈栈地址								
******************************************************************/
sMBSlaveInfo*  psMBGetSlaveInfo(void)
{
    return &MBSlaveInfo;
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

