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
 * File: $Id: porttimer.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "lpc_timer.h"
#include "lpc_mbdriver.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0 || MB_SLAVE_CPN_ENABLED > 0

#define TMR_TICK_PER_SECOND      OS_CFG_TMR_TASK_RATE_HZ

/* ----------------------- static functions ---------------------------------*/
static void vSlaveTimeoutInd(void * p_tmr, void * p_arg);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBSlavePortTimersInit(sMBSlavePort* psMBPort, USHORT usTim1Timerout50us)
{
	OS_ERR err = OS_ERR_NONE;
	ULONG    i = (usTim1Timerout50us * 80 ) / (1000000 / TMR_TICK_PER_SECOND);

    OSTmrCreate(&psMBPort->sSlavePortTmr,
			"sSlavePortTmr",
		    i,   //50us太快，改为80us 
			0,
			OS_OPT_TMR_ONE_SHOT,
			vSlaveTimeoutInd,
			(void*)psMBPort,
			&err);
	if( err == OS_ERR_NONE )
	{
		 return TRUE;
	}
    return FALSE;
}

void vMBSlavePortTimersEnable(sMBSlavePort* psMBPort)
{	
	OS_ERR err = OS_ERR_NONE;
    
	(void)OSTmrStateGet(&psMBPort->sSlavePortTmr, &err);;
    (void)OSTmrStart(&psMBPort->sSlavePortTmr, &err);
}

void vMBSlavePortTimersDisable(sMBSlavePort* psMBPort)
{
	OS_ERR err = OS_ERR_NONE;
    (void)OSTmrStop(&psMBPort->sSlavePortTmr, OS_OPT_TMR_NONE, NULL, &err);
}

static void TIMERExpiredISR(void * p_arg)    //定时器中断服务函数
{
	sMBSlavePort*      psMBPort = (sMBSlavePort*)p_arg;
	sMBSlaveInfo* psMBSlaveInfo = psMBSlaveFindNodeByPort(psMBPort->pcMBPortName);
	
	if( psMBSlaveInfo != NULL )
	{
		pxMBSlaveFrameCBTimerExpiredCur(psMBSlaveInfo);
	} 
}

static void vSlaveTimeoutInd(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
     TIMERExpiredISR(p_arg);
}

#endif
