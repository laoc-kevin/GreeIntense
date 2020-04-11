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
static void MasterTimeoutInd(void * p_tmr, void * p_arg);

/* ----------------------- Variables ----------------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTmrsInit(sMBMasterPortInfo* psMBPortInfo, USHORT usTim1Timerout50us)
{
	OS_ERR err = OS_ERR_NONE;
	ULONG i = (ULONG)( (usTim1Timerout50us*80) / (1000000/TMR_TICK_PER_SECOND) );
	
	OSTmrCreate( &(psMBPortInfo->MasterPortTmr),       //主定时器
			      "MasterPortTmr",
			      i,      
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      MasterTimeoutInd,
			      (void*)psMBPortInfo,
			      &err);
	if( err != OS_ERR_NONE )
	{
        return FALSE;
	}
    
	i = MB_MASTER_DELAY_MS_CONVERT * TMR_TICK_PER_SECOND / 1000  ; 
	OSTmrCreate( &(psMBPortInfo->ConvertDelayTmr),
			      "ConvertDelayTmr",
			      i ,
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      MasterTimeoutInd,
			      (void*)psMBPortInfo,
			      &err);
	if( err != OS_ERR_NONE )
	{
        return FALSE;
	}
	
	i = MB_MASTER_TIMEOUT_MS_RESPOND * TMR_TICK_PER_SECOND / 1000  ;    //主栈等待从栈响应定时器
	OSTmrCreate( &(psMBPortInfo->RespondTimeoutTmr),
			      "RespondTimeoutTmr",
			      i,
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      MasterTimeoutInd,
			      (void*)psMBPortInfo,
			      &err);
   
	if( err != OS_ERR_NONE )
	{
		 return FALSE;
	}
	return TRUE;
}

void vMBMasterPortTmrsEnable(sMBMasterPortInfo* psMBPortInfo)
{
	OS_ERR err = OS_ERR_NONE;
    vMBMasterSetCurTimerMode(psMBPortInfo, MB_TMODE_T35);
	
    (void)OSTmrStart( &(psMBPortInfo->MasterPortTmr), &err);
	
	if( OSTmrStateGet( &(psMBPortInfo->ConvertDelayTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->ConvertDelayTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->RespondTimeoutTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->RespondTimeoutTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBMasterPortTmrsConvertDelayEnable(sMBMasterPortInfo* psMBPortInfo)
{
	OS_ERR err = OS_ERR_NONE;
	 vMBMasterSetCurTimerMode(psMBPortInfo, MB_TMODE_CONVERT_DELAY);
	
	(void)OSTmrStart( &(psMBPortInfo->ConvertDelayTmr), &err);
	
	if( OSTmrStateGet( &(psMBPortInfo->MasterPortTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->MasterPortTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->RespondTimeoutTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->RespondTimeoutTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBMasterPortTmrsRespondTimeoutEnable(sMBMasterPortInfo* psMBPortInfo)	
{
	OS_ERR err = OS_ERR_NONE;
	 vMBMasterSetCurTimerMode(psMBPortInfo, MB_TMODE_RESPOND_TIMEOUT);
 
	(void)OSTmrStart( &(psMBPortInfo->RespondTimeoutTmr), &err);
	
	if( OSTmrStateGet( &(psMBPortInfo->MasterPortTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->MasterPortTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->ConvertDelayTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->ConvertDelayTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBMasterPortTmrsDisable(sMBMasterPortInfo* psMBPortInfo)
{
	OS_ERR err = OS_ERR_NONE;
    if( OSTmrStateGet( &(psMBPortInfo->MasterPortTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop( &(psMBPortInfo->MasterPortTmr), OS_OPT_TMR_NONE, NULL, &err);
	}
	
	if( OSTmrStateGet( &(psMBPortInfo->ConvertDelayTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->ConvertDelayTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
	
	if( OSTmrStateGet( &(psMBPortInfo->RespondTimeoutTmr), &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop( &(psMBPortInfo->RespondTimeoutTmr), OS_OPT_TMR_NONE, NULL, &err);
	} 
}

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode( sMBMasterPortInfo* psMBPortInfo, eMBMasterTimerMode eMBTimerMode )
{
	psMBPortInfo->eMasterCurTimerMode = eMBTimerMode;
}

static void TIMERExpiredISR(void * p_arg)    //定时器中断服务函数
{
	sMBMasterPortInfo* psMBPortInfo = (sMBMasterPortInfo*)p_arg;
	sMBMasterInfo*     psMBMasterInfo = NULL;
	
	psMBMasterInfo = psMBMasterFindNodeByPort(psMBPortInfo->pcMasterPortName);
	
	if( psMBMasterInfo != NULL )
	{
		pxMBMasterFrameCBTimerExpiredCur(psMBMasterInfo);
	} 
}

static void MasterTimeoutInd(void * p_tmr, void * p_arg)
{
    TIMERExpiredISR(p_arg);
}

#endif
