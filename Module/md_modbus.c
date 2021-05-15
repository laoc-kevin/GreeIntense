#include <os.h>

#include "md_led.h"
#include "md_modbus.h"
#include "md_output.h"
#include "md_input.h"

#include "mb.h"
#include "mb_m.h"
#include "mbtcp.h"

#define MB_MASTER_MIN_DEV_ADDR    1    //主栈从设备最小通讯地址(1~255)
#define MB_MASTER_MAX_DEV_ADDR    18   //主栈从设备最大通讯地址(1~255)

#define MB_DEFAULT_SLAVE_POLL_TASK_PRIO          9

#define MB_DEFAULT_MASTER_HEART_BEAT_TASK_PRIO   10
#define MB_DEFAULT_MASTER_POLL_TASK_PRIO         11
#define MB_DEFAULT_MASTER_SCAN_TASK_PRIO         12

/**********************************************************************
*变量声明
************************************************************************/
BOOL MBMasterLedState = 0;
BOOL MBSlaveLedState  = 1;

#if MB_MASTER_RTU_EN && MB_MASTER_RTU_ENABLED
sMBMasterInfo     MBMasterInfo;         //主栈接口

sUART_Def MBMasterUart = { &Uart0Rx, &Uart0Tx, &Uart0DE, &Uart0Inv, UART_0,              /* 主栈串口设置 */
                          {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}  
					     };
sMBMasterNodeInfo MBMasterNode = { MB_RTU, &MBMasterUart, "UART0",                        /* 主栈配置信息 */
                                   MB_MASTER_MIN_DEV_ADDR, MB_MASTER_MAX_DEV_ADDR, FALSE,
                                   MB_DEFAULT_MASTER_POLL_TASK_PRIO, MB_DEFAULT_MASTER_SCAN_TASK_PRIO, 
                                 };
#endif
                                 
#if MB_SLAVE_RTU_EN && MB_SLAVE_RTU_ENABLED 
sMBSlaveInfo MBSlaveInfo;          //从栈接口
                                 
sUART_Def MBSlaveUart= { &Uart1Rx, &Uart1Tx, &Uart1DE, &Uart1Inv, UART_1,                /* 从栈串口设置 */
                         {9600, UART_PARITY_NONE, UART_DATABIT_8, UART_STOPBIT_1}        /* 默认串口配置 9600 8n1 */
		               };
sMBSlaveNodeInfo  MBSlaveNode = {MB_RTU, "UART1", 1, &MBSlaveUart, MB_DEFAULT_SLAVE_POLL_TASK_PRIO}; /* 从栈配置信息 */
#endif

#if MB_SLAVE_TCP_EN && MB_SLAVE_TCP_ENABLED
    sMBSlaveTcpInfo   m_SlaveTcpInfo;
#endif

#if MB_MASTER_TCP_EN && MB_MASTER_TCP_ENABLED
sMBMasterInfo   m_MasterTcpInfo;
sMBMasterNodeInfo m_MBMasterTCPNode = {MB_TCP, NULL, "MB_MASTER_TCP", MB_MASTER_MIN_DEV_ADDR, MB_MASTER_MAX_DEV_ADDR, FALSE,
                                   MB_DEFAULT_MASTER_POLL_TASK_PRIO, MB_DEFAULT_MASTER_SCAN_TASK_PRIO};
#endif

/******************************************************************
*@brief 从栈数据接收回调								
******************************************************************/
void  vModbusSlaveReceiveCallback(void* p_arg)
{
    vLedStateChange(&LedModbus2, MBSlaveLedState);
    MBSlaveLedState = !MBSlaveLedState;
} 

/******************************************************************
*@brief 从栈数据发送回调								
******************************************************************/
void  vModbusSlaveSendCallback(void* p_arg)  
{
    (void)vMBTimeDly(0, 5);
    vLedStateChange(&LedModbus2, MBSlaveLedState);
    MBSlaveLedState = !MBSlaveLedState;
}                                    
                                   
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

#if MB_MASTER_RTU_EN && MB_MASTER_RTU_ENABLED
/**********************************************************************
 * @brief  MODBUS主栈初始化
 *********************************************************************/
void vModbusMasterInit(OS_PRIO ucPollPrio, OS_PRIO ucScanReadPrio, OS_PRIO ucScanWritePrio)
{
    MBMasterNode.ucMasterPollPrio = ucPollPrio;
    MBMasterNode.ucMasterScanReadPrio = ucScanReadPrio;
    MBMasterNode.ucMasterScanWritePrio = ucScanWritePrio;
     
    if(xMBMasterRegistNode(&MBMasterInfo, &MBMasterNode))
    {
         //modbus回调函数
        MBMasterInfo.pvMBMasterReceiveCallback = vModbusMasterReceiveCallback;
        MBMasterInfo.pvMBMasterSendCallback    = vModbusMasterSendCallback;
    }
}

/******************************************************************
*@brief 获取主栈地址								
******************************************************************/
sMBMasterInfo*  psMBGetMasterInfo(void)
{
    return &MBMasterInfo;
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
		     prvvMasterUARTRxISR(&MBMasterInfo.sMBPort);	      //Modbus Master Uart ISR
		break;
		case UART_IIR_INTID_THRE:
	         prvvMasterUARTTxReadyISR(&MBMasterInfo.sMBPort);  //Modbus Master Uart ISR
		break;
		default:break ;
	}
}
#endif

#if MB_SLAVE_RTU_EN && MB_SLAVE_RTU_ENABLED
/**********************************************************************
 * @brief  MODBUS从栈初始化
 *********************************************************************/
void vModbusSlaveInit(OS_PRIO prio)
{
    MBSlaveNode.ucSlaveAddr = ucGetControllerID();
    MBSlaveNode.ucSlavePollPrio = prio;

    if(xMBSlaveRegistNode(&MBSlaveInfo, &MBSlaveNode))
    {
        MBSlaveInfo.pvMBSlaveReceiveCallback = vModbusSlaveReceiveCallback;
        MBSlaveInfo.pvMBSlaveSendCallback    = vModbusSlaveSendCallback;  	    
    }
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
		    prvvSlaveUARTRxISR(&MBSlaveInfo.sMBPort);	   //Modbus Uart ISR
		break;
		case UART_IIR_INTID_THRE:
	         prvvSlaveUARTTxReadyISR(&MBSlaveInfo.sMBPort);  //Modbus Uart ISR
		break;	
		default:break ;
	}	
}	
#endif 

#if MB_SLAVE_TCP_EN && MB_SLAVE_TCP_ENABLED
BOOL  vModbusSlaveTCPInit(OS_PRIO prio, OS_PRIO pollprio)
{
    m_SlaveTcpInfo.ucSlaveAddr = ucGetControllerID();
    m_SlaveTcpInfo.ucSlaveTcpPollPrio = pollprio;     
    m_SlaveTcpInfo.ucSlaveTcpServerPrio = prio; 
    
    m_SlaveTcpInfo.pvMBSlaveReceiveCallback = vModbusSlaveReceiveCallback;
    m_SlaveTcpInfo.pvMBSlaveSendCallback    = vModbusSlaveSendCallback;  	      
    return xMBSlaveTCPServerInit(&m_SlaveTcpInfo);
}

void vRegistTcpCommData(sMBSlaveCommData *psCommData)
{
    uint8_t i = 0;
    sMBSlaveInfo *pMBSlaveInfo = NULL;
    if(psCommData)
    {
        for(i = 0; i < MB_SLAVE_MAX_TCP_CLIENT; i++)
        {
            pMBSlaveInfo = &m_SlaveTcpInfo.sMBSlaveTcpClients[i];
            pMBSlaveInfo->sMBCommInfo.psSlaveCurData = psCommData;
        }
    }
}

sMBSlaveInfo*  psMBGetTCPSlaveInfo(void)
{
    return &m_SlaveTcpInfo.sMBSlaveTcpClients[0];
}
#endif

#if MB_MASTER_TCP_EN && MB_MASTER_TCP_ENABLED
void  vModbusMasterTCPInit(OS_PRIO ucPollPrio, OS_PRIO ucScanReadPrio, OS_PRIO ucScanWritePrio)
{
    m_MBMasterTCPNode.ucMasterPollPrio = ucPollPrio;
    m_MBMasterTCPNode.ucMasterScanReadPrio = ucScanReadPrio;
    m_MBMasterTCPNode.ucMasterScanWritePrio = ucScanWritePrio;

    if(xMBMasterRegistNode(&m_MasterTcpInfo, &m_MBMasterTCPNode))
    {
        m_MasterTcpInfo.pvMBMasterReceiveCallback = vModbusMasterReceiveCallback;
        m_MasterTcpInfo.pvMBMasterSendCallback    = vModbusMasterSendCallback;
    }
}
sMBMasterInfo*  psMBGetTCPMasterInfo(void)
{
    return &m_MasterTcpInfo;
}
#endif

BOOL xMasterRegistSlaveDev(sMBMasterInfo* psMBMasterInfo, sMBSlaveDev* psMBNewDev)
{
    return xMBMasterRegistDev(psMBMasterInfo, psMBNewDev);
}

void vRegistSlaveCommData(sMBSlaveInfo* psMBSlaveInfo, sMBSlaveCommData *psCommData)
{
    if(psCommData)
    {
        psMBSlaveInfo->sMBCommInfo.psSlaveCurData = psCommData;
    }
}

