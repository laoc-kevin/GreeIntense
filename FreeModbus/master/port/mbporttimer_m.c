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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_m.h"
#include "mbrtu_m.h"
#include "mbconfig.h"
#include "mbport_m.h"
#include "lpc_timer.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

#define TMR_TICK_PER_SECOND             OS_CFG_TMR_TASK_RATE_HZ

/* ----------------------- static functions ---------------------------------*/
static void TIMERExpiredISR(void * p_arg);
static void vMasterTimeoutInd(void * p_tmr, void * p_arg);

/* ----------------------- Variables ----------------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBsMasterPortTmrsInit(sMBMasterPortInfo* psMBPortInfo, USHORT usTim1Timerout50us)
{
	OS_ERR err = OS_ERR_NONE;
	ULONG i = (ULONG)( (usTim1Timerout50us*80) / (1000000/TMR_TICK_PER_SECOND) );
	
	OSTmrCreate( &(psMBPortInfo->sMasterPortTmr),       //主定时器
			      "sMasterPortTmr",
			      i,      
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      vMasterTimeoutInd,
			      (void*)psMBPortInfo,
			      &err);
	if( err != OS_ERR_NONE )
	{
        return FALSE;
	}
    
	i = MB_MASTER_DELAY_MS_CONVERT * TMR_TICK_PER_SECOND / 1000  ; 
	OSTmrCreate( &(psMBPortInfo->sConvertDelayTmr),
			      "sConvertDelayTmr",
			      i ,
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      vMasterTimeoutInd,
			      (void*)psMBPortInfo,
			      &err);
	if( err != OS_ERR_NONE )
	{
        return FALSE;
	}
	
	i = MB_MASTER_TIMEOUT_MS_RESPOND * TMR_TICK_PER_SECOND / 1000  ;    //主栈等待从栈响应定时器
	OSTmrCreate( &(psMBPortInfo->sRespondTimeoutTmr),
			      "sRespondTimeoutTmr",
			      i,
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      vMasterTimeoutInd,
			      (void*)psMBPortInfo,
			      &err);
   
	if( err != OS_ERR_NONE )
	{
		 return FALSE;
	}
	return TRUE;
}

void vMBsMasterPortTmrsEnable(sMBMasterPortInfo* psMBPortInfo)
{
	OS_ERR err = OS_ERR_NONE;
    vMBMasterSetCurTimerMode(psMBPortInfo, MB_TMODE_T35);
	
    (void)OSTmrStart( &(psMBPortInfo->sMasterPortTmr), &err);
	
	if( OSTmrStateGet( &(psMBPortInfo->sConvertDelayTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->sConvertDelayTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->sRespondTimeoutTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->sRespondTimeoutTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBsMasterPortTmrsConvertDelayEnable(sMBMasterPortInfo* psMBPortInfo)
{
	OS_ERR err = OS_ERR_NONE;
	 vMBMasterSetCurTimerMode(psMBPortInfo, MB_TMODE_CONVERT_DELAY);
	
	(void)OSTmrStart( &(psMBPortInfo->sConvertDelayTmr), &err);
	
	if( OSTmrStateGet( &(psMBPortInfo->sMasterPortTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->sMasterPortTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->sRespondTimeoutTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->sRespondTimeoutTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBsMasterPortTmrsRespondTimeoutEnable(sMBMasterPortInfo* psMBPortInfo)	
{
	OS_ERR err = OS_ERR_NONE;
	 vMBMasterSetCurTimerMode(psMBPortInfo, MB_TMODE_RESPOND_TIMEOUT);
 
	(void)OSTmrStart( &(psMBPortInfo->sRespondTimeoutTmr), &err);
	
	if( OSTmrStateGet( &(psMBPortInfo->sMasterPortTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->sMasterPortTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->sConvertDelayTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->sConvertDelayTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBsMasterPortTmrsDisable(sMBMasterPortInfo* psMBPortInfo)
{
	OS_ERR err = OS_ERR_NONE;
    if( OSTmrStateGet( &(psMBPortInfo->sMasterPortTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->sMasterPortTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->sConvertDelayTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->sConvertDelayTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
	
	if( OSTmrStateGet( &(psMBPortInfo->sRespondTimeoutTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->sRespondTimeoutTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode( sMBMasterPortInfo* psMBPortInfo, eMBMasterTimerMode eMBTimerMode )
{
	psMBPortInfo->eCurTimerMode = eMBTimerMode;
}

static void TIMERExpiredISR(void * p_arg)    //定时器中断服务函数
{
	sMBMasterPortInfo*   psMBPortInfo = (sMBMasterPortInfo*)p_arg;
	sMBMasterInfo*     psMBMasterInfo = psMBMasterFindNodeByPort(psMBPortInfo->pcMBPortName);
	
	if( psMBMasterInfo != NULL )
	{
		pxMBMasterFrameCBTimerExpiredCur(psMBMasterInfo);
	} 
}

static void vMasterTimeoutInd(void * p_tmr, void * p_arg)
{
    TIMERExpiredISR(p_arg);
}

#endif
