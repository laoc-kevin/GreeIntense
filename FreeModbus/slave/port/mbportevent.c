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

#define TIME_TICK_OUT 0


/* ----------------------- Start implementation -----------------------------*/

/**********************************************************************
 * @brief  modbus协议栈事件初始化函数
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL xMBSlavePortEventInit(sMBSlavePort* psMBPort)
{
    psMBPort->xEventInQueue = FALSE;
    return TRUE;
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
	
//	 (void)OSTaskSemPost(&AppMbSlavePollTaskTCB, OS_OPT_POST_NONE, &err);
    return TRUE;
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
	
	(void)OSSemPend(&psMBPort->sMBEventSem, TIME_TICK_OUT, OS_OPT_PEND_BLOCKING, &ts, &err);
//	(void)OSSemSet(&SlaveEventSem, 0, &err);
    if( psMBPort->xEventInQueue )
    {
        *eEvent = psMBPort->eQueuedEvent;
        psMBPort->xEventInQueue = FALSE;
        xEventHappened = TRUE;	
    }
    return xEventHappened;
}

#endif
