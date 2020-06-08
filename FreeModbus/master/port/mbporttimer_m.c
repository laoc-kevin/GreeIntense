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

/* ----------------------- Start implementation -----------------------------*/
void vMBsMasterPortTmrsEnable(sMBMasterPort* psMBPort)
{
	OS_ERR err = OS_ERR_NONE;
    vMBMasterSetCurTimerMode(psMBPort, MB_TMODE_T35);
	
    (void)OSTmrStart(&psMBPort->sMasterPortTmr, &err);
	
	if( OSTmrStateGet(&psMBPort->sConvertDelayTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop(&psMBPort->sConvertDelayTmr, OS_OPT_TMR_NONE, NULL, &err);
	}
	if( OSTmrStateGet(&psMBPort->sRespondTimeoutTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop(&psMBPort->sRespondTimeoutTmr, OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBsMasterPortTmrsConvertDelayEnable(sMBMasterPort* psMBPort)
{
	OS_ERR err = OS_ERR_NONE;
	 vMBMasterSetCurTimerMode(psMBPort, MB_TMODE_CONVERT_DELAY);
	
	(void)OSTmrStart(&psMBPort->sConvertDelayTmr, &err);
	
	if( OSTmrStateGet(&psMBPort->sMasterPortTmr, &err) == OS_TMR_STATE_RUNNING)
	{
        (void)OSTmrStop(&psMBPort->sMasterPortTmr, OS_OPT_TMR_NONE, NULL, &err);
	}
	if( OSTmrStateGet(&psMBPort->sRespondTimeoutTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop(&psMBPort->sRespondTimeoutTmr, OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBsMasterPortTmrsRespondTimeoutEnable(sMBMasterPort* psMBPort)	
{
	OS_ERR err = OS_ERR_NONE;
	 vMBMasterSetCurTimerMode(psMBPort, MB_TMODE_RESPOND_TIMEOUT);
 
	(void)OSTmrStart(&psMBPort->sRespondTimeoutTmr, &err);
	
	if( OSTmrStateGet(&psMBPort->sMasterPortTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop(&psMBPort->sMasterPortTmr, OS_OPT_TMR_NONE, NULL, &err);
	}
	if( OSTmrStateGet(&psMBPort->sConvertDelayTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop(&psMBPort->sConvertDelayTmr, OS_OPT_TMR_NONE, NULL, &err);
	} 
}

void vMBsMasterPortTmrsDisable(sMBMasterPort* psMBPort)
{
	OS_ERR err = OS_ERR_NONE;
    if( OSTmrStateGet(&psMBPort->sMasterPortTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		 (void)OSTmrStop(&psMBPort->sMasterPortTmr, OS_OPT_TMR_NONE, NULL, &err);
	}
	if( OSTmrStateGet(&psMBPort->sConvertDelayTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop(&psMBPort->sConvertDelayTmr, OS_OPT_TMR_NONE, NULL, &err);
	} 
	if( OSTmrStateGet(&psMBPort->sRespondTimeoutTmr, &err) == OS_TMR_STATE_RUNNING)
	{
		(void)OSTmrStop(&psMBPort->sRespondTimeoutTmr, OS_OPT_TMR_NONE, NULL, &err);
	} 
}

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode(sMBMasterPort* psMBPort, eMBMasterTimerMode eMBTimerMode)
{
	psMBPort->eCurTimerMode = eMBTimerMode;
}

void vMasterTimeoutInd(void * p_tmr, void * p_arg)
{
    sMBMasterPort*   psMBPort = (sMBMasterPort*)p_arg;
	sMBMasterInfo*   psMBMasterInfo = psMBPort->psMBMasterInfo;
	
	if(psMBMasterInfo != NULL)
	{
		pxMBMasterFrameCBTimerExpiredCur(psMBMasterInfo);
	} 
}

BOOL xMBsMasterPortTmrsInit(sMBMasterPort* psMBPort, USHORT usTim1Timerout50us)
{
	OS_ERR err = OS_ERR_NONE;
	OS_TICK i = (OS_TICK)( (usTim1Timerout50us*80) / (1000000/TMR_TICK_PER_SECOND) );
	
	OSTmrCreate(&psMBPort->sMasterPortTmr,       //主定时器
			    "sMasterPortTmr",
			    i,      
			    0,
			    OS_OPT_TMR_ONE_SHOT,
			    vMasterTimeoutInd,
			    (void*)psMBPort,
			    &err);
	if(err != OS_ERR_NONE)
	{
        return FALSE;
	}
    
	i = MB_MASTER_DELAY_MS_CONVERT * TMR_TICK_PER_SECOND / 1000  ; 
	OSTmrCreate( &(psMBPort->sConvertDelayTmr),
			      "sConvertDelayTmr",
			      i ,
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      vMasterTimeoutInd,
			      (void*)psMBPort,
			      &err);
	if( err != OS_ERR_NONE )
	{
        return FALSE;
	}
	
	i = MB_MASTER_TIMEOUT_MS_RESPOND * TMR_TICK_PER_SECOND / 1000  ;    //主栈等待从栈响应定时器
	OSTmrCreate( &(psMBPort->sRespondTimeoutTmr),
			      "sRespondTimeoutTmr",
			      i,
			      0,
			      OS_OPT_TMR_ONE_SHOT,
			      vMasterTimeoutInd,
			      (void*)psMBPort,
			      &err);
   
	if( err != OS_ERR_NONE )
	{
		 return FALSE;
	}
	return TRUE;
}

#endif
