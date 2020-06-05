/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbport.h,v 1.17 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef _MB_PORT_H
#define _MB_PORT_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#include "port.h"
/* ----------------------- Type definitions ---------------------------------*/

typedef enum
{
    EV_READY,                   /*!< Startup finished. */
    EV_FRAME_RECEIVED,          /*!< Frame received. */
    EV_EXECUTE,                 /*!< Execute function. */
    EV_FRAME_SENT               /*!< Frame sent. */
} eMBSlaveEventType;


typedef struct                        /* 从栈接口定义  */
{
	const sUART_Def*     psMBSlaveUart;        //从栈通讯串口结构
	OS_TMR              sSlavePortTmr;         //从栈3.5字符间隔定时器
                        
	eMBSlaveEventType   eQueuedEvent;          //从栈事件
	OS_SEM              sMBEventSem;           //从栈事件消息量                   
    BOOL                xEventInQueue;         //从栈有新事件
                
    const CHAR*         pcMBPortName;          //从栈名称
}sMBSlavePort;


#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0 || MB_SLAVE_CPN_ENABLED > 0

/* -----------------------Slave Serial port functions ----------------------------*/
BOOL xMBSlavePortSerialInit(sMBSlavePort* psMBPort);

void vMBSlavePortClose(sMBSlavePort* psMBPort);

void xMBSlavePortSerialClose(sMBSlavePort* psMBPort);

void vMBSlavePortSerialEnable(sMBSlavePort* psMBPort, BOOL xRxEnable, BOOL xTxEnable);

INLINE BOOL xMBSlavePortSerialGetByte(sMBSlavePort* psMBPort, CHAR* pucByte);

INLINE BOOL xMBSlavePortSerialPutByte(sMBSlavePort* psMBPort, CHAR ucByte);

void prvvSlaveUARTTxReadyISR(const CHAR* pcMBPortName);

void prvvSlaveUARTRxISR(const CHAR* pcMBPortName);

/* -----------------------Master Serial port event functions ----------------------------*/
BOOL xMBSlavePortEventInit(sMBSlavePort* psMBPort);

BOOL xMBSlavePortEventPost(sMBSlavePort* psMBPort, eMBSlaveEventType eEvent);

BOOL xMBSlavePortEventGet(sMBSlavePort* psMBPort, eMBSlaveEventType* eEvent);


/* ----------------------- Timers functions ---------------------------------*/
BOOL xMBSlavePortTimersInit(sMBSlavePort* psMBPort, USHORT usTim1Timerout50us);

void xMBSlavePortTimersClose(sMBSlavePort* psMBPort);

INLINE void vMBSlavePortTimersEnable(sMBSlavePort* psMBPort);

INLINE void vMBSlavePortTimersDisable(sMBSlavePort* psMBPort);

#endif


#if MB_SLAVE_TCP_ENABLED > 0 

/* ----------------------- TCP port functions -------------------------------*/
BOOL xMBSlaveTCPPortInit( USHORT usTCPPort );

void vMBSlaveTCPPortClose( void );

void vMBSlaveTCPPortDisable( void );

BOOL xMBSlaveTCPPortGetRequest( UCHAR **ppucMBTCPFrame, USHORT * usTCPLength );

BOOL xMBSlaveTCPPortSendResponse( const UCHAR *pucMBTCPFrame, USHORT usTCPLength );

#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
