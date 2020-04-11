/*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
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
 * File: $Id: portserial.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "lpc_mbdriver.h"
#include <bsp_int.h>
#include <lib_def.h>
#include <os.h>
#include "lpc_gpio.h"
#include "my_rtt_printf.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0 || MB_SLAVE_CPN_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
static UART_Def *ModbusSlaveUart; 

/* ----------------------- static functions ---------------------------------*/


/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	UART_FIFOReset(ModbusSlaveUart->ID, ( UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV2));
	if(xRxEnable)
	{
         UART_IntConfig(ModbusSlaveUart->ID, UART_INTCFG_RBR, ENABLE); 		//开启接收中断
		 ModbusSendOrRecive(ModbusSlaveUart, UART_RX_EN);
	}
	else
	{
		 UART_IntConfig(ModbusSlaveUart->ID, UART_INTCFG_RBR, DISABLE);    //关闭接收中断
		 ModbusSendOrRecive(ModbusSlaveUart, UART_TX_EN);
		
	}

	if(xTxEnable)
	{
		UART_IntConfig(ModbusSlaveUart->ID, UART_INTCFG_THRE, ENABLE); 	   //开启发送中断
		ModbusSendOrRecive(ModbusSlaveUart, UART_TX_EN);
		UART_TxCmd(ModbusSlaveUart->ID, ENABLE);                           //UART中断
	}
	else
	{
		UART_IntConfig(ModbusSlaveUart->ID, UART_INTCFG_THRE, DISABLE); 	//关闭接收中断
		ModbusSendOrRecive(ModbusSlaveUart, UART_RX_EN);
		UART_TxCmd(ModbusSlaveUart->ID, DISABLE);                           
	}
	UART_FIFOReset(ModbusSlaveUart->ID, ( UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV2));
	
}

void
vMBPortClose(void)
{
	UART_IntConfig(ModbusSlaveUart->ID, UART_INTCFG_THRE|UART_INTCFG_RBR, DISABLE);
	UART_TxCmd(ModbusSlaveUart->ID, DISABLE);
}

BOOL
xMBPortSerialInit( UART_Def *Uart )
{	
	BOOL bInitialized = TRUE;
	ModbusSlaveUart = Uart;  
	ModbusUartInit(ModbusSlaveUart);
    return bInitialized;
}

BOOL
xMBPortSerialPutByte(CHAR ucByte )
{
	UCHAR h;
	UCHAR l;
	
	h=ucByte >> 4 ;
	l=ucByte % 16 ;	
	h= (h<10)? h+48: h+87;
	l= (l<10)? l+48: l+87;	
	
//    myprintf("TX:%c%c\n", h,l);
	
	UART_SendByte(ModbusSlaveUart->ID, ucByte);
    return TRUE;
}

BOOL
xMBPortSerialGetByte(CHAR* pucByte)
{
	UCHAR h;
	UCHAR l;
	
	*pucByte = UART_ReceiveByte(ModbusSlaveUart->ID);
	
	h=(* pucByte )>> 4 ;
	l=(* pucByte ) % 16 ;	
	h= (h<10)? h+48: h+87;
	l= (l<10)? l+48: l+87;	
	
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
void prvvUARTTxReadyISR(void)
{
    (void)pxMBFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
    (void)pxMBFrameCBByteReceived();
}




#endif

