/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "mbconfig.h"
#include "mbport_m.h"
#include "bsp.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit( sMBMasterPort* psMBPort )     //初始化
{
    /**
     * set 485 mode receive and transmit control IO
     * @note MODBUS_MASTER_RT_CONTROL_PIN_INDEX need be defined by user
     */
	BOOL bInitialized = TRUE;
    
    const sUART_Def* psMBMasterUart = psMBPort->psMBMasterUart;
	MB_UartInit(psMBMasterUart);
    
    return bInitialized;
}

void vMBMasterPortSerialEnable( sMBMasterPort* psMBPort, BOOL xRxEnable, BOOL xTxEnable)      
{
    const sUART_Def* psMBMasterUart = psMBPort->psMBMasterUart;
	sMBMasterInfo*   psMBMasterInfo = psMBPort->psMBMasterInfo;
	
//	myprintf("vMBMasterPortSerialEnable***ucMBDestAddr %d  \n",psMBMasterInfo->ucMBDestAddr);
//	
	if(psMBMasterInfo->ucMBDestAddr ==1 || psMBMasterInfo->ucMBDestAddr ==2)
	{
		UART_FIFOReset(psMBMasterUart->ID, ( UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV2));
	}
	
    if(xRxEnable)
	{
         UART_IntConfig(psMBMasterUart->ID, UART_INTCFG_RBR, ENABLE); 		//开启接收中断
		 MB_SendOrRecive(psMBMasterUart, UART_RX_EN);
	}
	else
	{
		 UART_IntConfig(psMBMasterUart->ID, UART_INTCFG_RBR, DISABLE);    //开启关闭接收中断
		 MB_SendOrRecive(psMBMasterUart, UART_TX_EN);
	}

	if(xTxEnable)
	{
		UART_IntConfig(psMBMasterUart->ID, UART_INTCFG_THRE, ENABLE); 		//开启发送中断
		MB_SendOrRecive(psMBMasterUart, UART_TX_EN);
		UART_TxCmd(psMBMasterUart->ID, ENABLE);                           //UART中断
	}
	else
	{
		UART_IntConfig(psMBMasterUart->ID, UART_INTCFG_THRE, DISABLE); 		//关闭接收中断
		MB_SendOrRecive(psMBMasterUart, UART_RX_EN);
		UART_TxCmd(psMBMasterUart->ID, ENABLE);                           //UART中断
	}
	if(psMBMasterInfo->ucMBDestAddr == 1 || psMBMasterInfo->ucMBDestAddr == 2 )
	{
		UART_FIFOReset(psMBMasterUart->ID, ( UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV2));
	}
}

void vMBMasterPortClose(sMBMasterPort* psMBPort)   //关闭串口
{
    const sUART_Def* psMBMasterUart = psMBPort->psMBMasterUart;
    UART_IntConfig(psMBMasterUart->ID, UART_INTCFG_THRE|UART_INTCFG_RBR, DISABLE);
	UART_TxCmd(psMBMasterUart->ID, DISABLE);
}

BOOL xMBMasterPortSerialPutByte(sMBMasterPort* psMBPort, CHAR ucByte)   //发送一个字节
{
//	UCHAR h;
//	UCHAR l;
//	
//	h=ucByte >> 4 ;
//	l=ucByte % 16 ;	
//	h= (h<10)? h+48: h+87;
//	l= (l<10)? l+48: l+87;	
	
//    myprintf("TX:%c%c\n", h,l);
	const sUART_Def* psMBMasterUart = psMBPort->psMBMasterUart;
    UART_SendByte(psMBMasterUart->ID, ucByte);
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(const sMBMasterPort* psMBPort, CHAR * pucByte)  //接收一个字节
{
//	UCHAR h;
//	UCHAR l;
	const sUART_Def* psMBMasterUart = psMBPort->psMBMasterUart;
    *pucByte = UART_ReceiveByte(psMBMasterUart->ID);
//	
//	h=(* pucByte )>> 4 ;
//	l=(* pucByte ) % 16 ;	
//	h= (h<10)? h+48: h+87;
//	l= (l<10)? l+48: l+87;	
//	
//    myprintf("RX:%c%c\n", h,l);

    return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvMasterUARTTxReadyISR(const sMBMasterPort* psMBPort)   //串口服务函数
{
	sMBMasterInfo*   psMBMasterInfo = psMBPort->psMBMasterInfo;
	if( psMBMasterInfo != NULL )
	{
		(void)pxMBMasterFrameCBTransmitterEmptyCur(psMBMasterInfo);
	} 
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvMasterUARTRxISR(const sMBMasterPort* psMBPort)
{
	sMBMasterInfo*   psMBMasterInfo = psMBPort->psMBMasterInfo;
	if(psMBMasterInfo != NULL)
	{
		(void)pxMBMasterFrameCBByteReceivedCur(psMBMasterInfo);
	} 
}

#endif
