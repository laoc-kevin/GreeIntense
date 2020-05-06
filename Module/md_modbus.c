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


/**********************************************************************
*变量声明
************************************************************************/

sUART_Def sMBSlaveUart= { &Uart1Rx,&Uart1Tx,&Uart1DE,&Uart1Inv,UART_1,                    /* 从栈串口设置 */
					    {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}         /* 默认串口配置 9600 8n1 */
			          };
	
sUART_Def sMBMasterUart = { &Uart0Rx,&Uart0Tx,&Uart0DE,&Uart0Inv,UART_0,                  /* 主栈串口设置 */
						  {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}  
					    };

                        
                        
                        
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

