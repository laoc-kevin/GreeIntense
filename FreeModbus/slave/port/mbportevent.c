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
 * File: $Id: portevent.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0 || MB_SLAVE_CPN_ENABLED > 0

#define TIME_TICK_OUT_MS    500

/* ----------------------- Start implementation -----------------------------*/

/**********************************************************************
 * @brief  modbus协议栈事件初始化函数
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL xMBSlavePortEventInit(sMBSlavePort* psMBPort)
{
    OS_ERR err = OS_ERR_NONE;
    
    psMBPort->xEventInQueue = FALSE;
    OSSemCreate(&psMBPort->sMBEventSem, "sMBEventSem", 0, &err);      //从栈事件消息量
    
    return (err == OS_ERR_NONE);
}

/**********************************************************************
 * @brief  modbus从栈事件发送
 * @param  eEvent  当前事件
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL xMBSlavePortEventPost(sMBSlavePort* psMBPort, eMBSlaveEventType eEvent)
{
	OS_ERR err = OS_ERR_NONE;
    psMBPort->xEventInQueue = TRUE;
    psMBPort->eQueuedEvent = eEvent;
	
    (void)OSSemPost(&psMBPort->sMBEventSem, OS_OPT_POST_ALL, &err);
    
    return (err == OS_ERR_NONE);
}

/**********************************************************************
 * @brief  获取modbus从栈最新的事件
 * @param  eEvent  当前事件
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL xMBSlavePortEventGet(sMBSlavePort* psMBPort, eMBSlaveEventType * eEvent)
{
    BOOL xEventHappened = FALSE;
	CPU_TS ts = 0;
    OS_ERR err = OS_ERR_NONE;
	
	OS_TICK i = (OS_TICK)( TIME_TICK_OUT_MS * TMR_TICK_PER_SECOND / 1000 );  //等待响应时间
    
	(void)OSSemPend(&psMBPort->sMBEventSem, i, OS_OPT_PEND_BLOCKING, &ts, &err);    
	(void)OSSemSet(&psMBPort->sMBEventSem, 0, &err);
  
    if( psMBPort->xEventInQueue )
    {
        *eEvent = psMBPort->eQueuedEvent;
        psMBPort->xEventInQueue = FALSE;
        xEventHappened = TRUE;	
    }
    return xEventHappened;
}

#endif
