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

#define MODBUS_TIMER_INTR_USED		(TIMER0_IRQn)
#define MODBUS_SLAVE_TIMER 	        (LPC_TIM0)
#define TMR_TICK_PER_SECOND      OS_CFG_TMR_TASK_RATE_HZ

/* ----------------------- static functions ---------------------------------*/
static void slave_timeout_ind(void * p_tmr, void * p_arg);

/* ----------------------- Variables ----------------------------------------*/
OS_TMR SlavePortTimer;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	OS_ERR err = OS_ERR_NONE;
	ULONG i = 0;
	i= (usTim1Timerout50us * 80 ) /( 1000000 / TMR_TICK_PER_SECOND ); 
	OSTmrCreate(&SlavePortTimer,
			"SlavePortTimer",
		    i,   //50us太快，改为80us 
			0,
			OS_OPT_TMR_ONE_SHOT,
			slave_timeout_ind,
			0,
			&err);
	if( err == OS_ERR_NONE )
	{
		 return TRUE;
	}
    return FALSE;
}

void
vMBPortTimersEnable( void )
{	
	OS_ERR err = OS_ERR_NONE;
	OS_STATE S;
	S=OSTmrStateGet(&SlavePortTimer, &err);
    (void)OSTmrStart(&SlavePortTimer, &err);
}

void
vMBPortTimersDisable( void )
{
	OS_ERR err = OS_ERR_NONE;
    (void)OSTmrStop(&SlavePortTimer, OS_OPT_TMR_NONE, NULL, &err);
}

static void slave_timeout_ind(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
   (void)pxMBPortCBTimerExpired();
}

#endif
