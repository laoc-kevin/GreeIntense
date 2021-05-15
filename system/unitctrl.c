#include "bms.h"
#include "system.h"
#include "systemctrl.h"

#define MODULAR_POLL_TIME_OUT_S   30

/*系统开启所有机组*/
void vSystem_OpenUnits(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->IDevSwitch.switchOpen(SUPER_PTR(pModularRoof, IDevSwitch)); //开启机组
    }
}

/*系统关闭所有机组*/
void vSystem_CloseUnits(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->IDevSwitch.switchClose(SUPER_PTR(pModularRoof, IDevSwitch)); //关闭机组
    }
}

/*机组启停命令变化*/
void vSystem_UnitSwitchCmd(System* pt, ModularRoof* pModularRoof)
{
    pModularRoof->xSwitchCmdChanged = TRUE;
    pModularRoof->eSwitchState = pModularRoof->eSwitchCmd;
}

/*机组启停状态变化*/
void vSystem_UnitSwitchState(System* pt, ModularRoof* pModularRoof)
{
    if(pModularRoof->eSwitchCmd != pModularRoof->eSwitchState && pModularRoof->xSwitchCmdChanged == FALSE)
	{
		pModularRoof->eSwitchCmd = pModularRoof->eSwitchState;
	}
}

/*机组运行状态变化*/
void vSystem_UnitRunningState(System* pt, ModularRoof* pmModularRoof, BOOL xRunningState)
{
    uint8_t  n   = 0;
    OS_ERR   err = OS_ERR_NONE;
    
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    ExAirFan*    pExAirFan    = NULL;
    BMS*         psBMS        = BMS_Core();
    
#if DEBUG_ENABLE > 0
    debug("vSystem_DeviceRunningState  eSystemState %d  \n", pThis->eSystemState);
#endif  
    
	if(pmModularRoof != NULL && xRunningState)
	{
		if(pmModularRoof->Device.eRunningState == STATE_RUN)
	    {
			pmModularRoof->eSwitchCmd = CMD_OPEN;
            pmModularRoof->eSwitchState = CMD_OPEN;				
	    }
	    else
        {
			pmModularRoof->eSwitchCmd = CMD_CLOSE;
		    pmModularRoof->eSwitchState = CMD_CLOSE;				
		}			
	}
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->Device.eRunningState == STATE_RUN)  //机组运行
        {
            switch(pModularRoof->eRunningMode)
            {
                case RUN_MODE_COOL:
                    pThis->eSystemState = STATE_COOL;
				    pThis->eRunningMode = RUN_MODE_COOL;
                break;
                case RUN_MODE_HEAT:
                    pThis->eSystemState = STATE_HEAT;
				    pThis->eRunningMode = RUN_MODE_HEAT;
                break;
                case RUN_MODE_FAN:
                    pThis->eSystemState = STATE_FAN;
				    pThis->eRunningMode = RUN_MODE_FAN;
                break;
                case RUN_MODE_WET:
                    pThis->eSystemState = STATE_WET;
				    pThis->eRunningMode = RUN_MODE_WET;
                break;
                default:break;
            }
            if(pThis->eSystemMode == MODE_CLOSE)
            {
                pThis->eRunningMode = pModularRoof->eRunningMode;
                psBMS->eRunningMode = pModularRoof->eRunningMode;
                pThis->eSystemMode  = MODE_MANUAL;
                psBMS->eSystemMode  = MODE_MANUAL;
            } 
            return;
        }
    }
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        if(pExAirFan->Device.eRunningState == STATE_RUN)
        {
            if(pThis->eSystemMode == MODE_CLOSE)
            {
                pThis->eSystemMode = MODE_MANUAL;
                psBMS->eSystemMode = MODE_MANUAL;
            } 
            pThis->eSystemState = STATE_EX_FAN;
            return;
        }	
    }
    pThis->eSystemState = STATE_CLOSED;    
}


/*系统所有压缩机是否关闭*/
BOOL xSystem_UnitCompsClosed(System* pt)
{
    uint8_t  n, m, i;
    
    System*      pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;

    for(n=0; n< MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->xCompRunning == TRUE && pModularRoof->xStopErrFlag == FALSE && pModularRoof->xCommErr == FALSE)
        {
            if(pThis->xCompFirstRun == TRUE)   //压缩机首次开启
            {
                pThis->xCompFirstRun = FALSE;
            }
            return FALSE;
        }
    }
    return TRUE;
}

/*切换机组节能温度*/
uint16_t usSystem_ChangeEnergyTemp(System* pt)
{
    uint8_t  n = 0;
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
    
    uint16_t usTempSet     = 0;
    uint16_t usEnergyTemp  = pThis->usEnergyTemp;
    uint16_t usTempDeviat  = pThis->usTempDeviat;
    
     /*注：1、当室内目标温度>=【节能温度】（默认25℃），
                    机组制冷目标温度=室内目标温度；
                    T=室内目标温度；
        
             2、当室内目标温度<节能温度（默认25℃），机组制冷目标温度=【节能温度】；
                    T=【节能温度】-【温度偏差】（默认0.5℃）；*/
	if(pThis->eSystemMode != MODE_AUTO)
	{
		return usTempSet;
	}
	
    if(pThis->eRunningMode == STATE_COOL || pThis->eRunningMode == STATE_WET)  //制冷模式或湿膜
    {
        if(pThis->usTempSet < pThis->usEnergyTemp)
        {
            usTempSet = usEnergyTemp - usTempDeviat;
        }
        else
        {
            usTempSet = pThis->usTempSet;
        }
        for(n=0; n < MODULAR_ROOF_NUM; n++)  //调整制冷温度
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->usCoolTempSet = usTempSet;
            pModularRoof->usHeatTempSet = usTempSet;
        }
#if DEBUG_ENABLE > 0
//        debug("vSystem_ChangeEnergyTemp %d\n", usTempSet); 
#endif
    }
    return usTempSet;    
}

/*设定机组运行模式*/
void vSystem_SetUnitRunningMode(System* pt, eRunningMode eRunMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;

    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    
    pThis->eRunningMode = eRunMode;
    pThis->psBMS->eRunningMode = eRunMode;  
    
#if DEBUG_ENABLE > 0
    debug("vSystem_SetUnitRunningMode %d\n", eRunMode); 
#endif        
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];    
        pModularRoof->setRunningMode(pModularRoof, eRunMode);
        
        if(pModularRoof->Device.eRunningState == STATE_RUN)  //机组运行
        {
            pModularRoof->usCoolTempSet = pThis->usTempSet;
            pModularRoof->usHeatTempSet = pThis->usTempSet;
            
            switch(pModularRoof->eRunningMode)
            {
                case RUN_MODE_COOL:
                    pThis->eSystemState = STATE_COOL;
                break;
                case RUN_MODE_HEAT:
                    pThis->eSystemState = STATE_HEAT;
                break;
                case RUN_MODE_FAN:
                    pThis->eSystemState = STATE_FAN;
                break;
                case RUN_MODE_WET:
                    pThis->eSystemState = STATE_WET;
                break;
                default:break;
            }
        }    
    }
    if(pThis->eSystemMode == MODE_AUTO)
    {
        vSystem_OpenUnits(pThis);
    }
    if((pThis->eRunningMode == STATE_COOL || pThis->eRunningMode == STATE_WET) && (pThis->eSystemMode == MODE_AUTO))  //制冷模式或湿膜
    {
        (void)usSystem_ChangeEnergyTemp(pThis);
    }
    (void)vSystem_ExAirRequest_Vol(pThis);  //计算排风需求量
}

void vSystem_ModeChangePeriodTimeCallback(void* p_tmr, void* p_arg)
{
    System* pThis = (System*)p_arg;

#if DEBUG_ENABLE > 0
    debug("vSystem_ModeChangePeriodTimeCallback %d\n", pThis->sAmbientIn_T);
#endif         
}

/*调整机组运行模式*/
void vSystem_AdjustUnitRunningMode(System* pt)
{
    uint8_t  n = 0;
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
    
    uint16_t usTempSet     = 0;
    int16_t  sAmbientIn_T  = pThis->sAmbientIn_T;
    uint16_t usEnergyTemp  = pThis->usEnergyTemp;
    uint16_t usTempDeviat  = pThis->usTempDeviat;
    
    if(pThis->eSystemMode != MODE_AUTO || pThis->eSystemState == STATE_CLOSED)   //6. 自动模式且已经运行
    {
        return;
    }  
#if DEBUG_ENABLE > 0
    debug("vSystem_AdjustUnitRunningMode %d\n", pThis->eRunningMode);
#endif      
    //（1）系统送风模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_FAN)
    {
        //室内温度>室内目标温度+ T1（默认1.5℃），持续满足t1(默认5min)时间，
        //且满足【模式切换间隔时间1】（默认10min）则机组切换为湿膜模式；
        if( (sAmbientIn_T > pThis->usTempSet + pThis->usModeAdjustTemp_1) && 
            (usGetTmrState(&pThis->sModeChangeTmr_1) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_1) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_3, pThis->usModeChangePeriod_3*60, 0, 
                               OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_4, pThis->usModeChangePeriod_4*60, 0, 
                   OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            
            debug("xTimerRegist sModeChangePeriodTmr_3 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            debug("xTimerRegist sModeChangePeriodTmr_4 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            
            return;            
        }
        //室内温度<室内目标温度- T2（默认1.5℃），持续满足t2(默认5min)时间，
        //且满足【模式切换间隔时间2】（默认10min）则机组切换为制热模式；
        if( (sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_2) && 
            (usGetTmrState(&pThis->sModeChangeTmr_2) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_2) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_HEAT);
            if(xSystem_UnitCompsClosed(pThis) == TRUE)
            {
                pThis->xCompFirstRun = TRUE;
            }
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_6, pThis->usModeChangePeriod_6*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangePeriodTmr_6 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
    }
    //（2）系统湿膜模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_WET)
    {
		usTempSet = usSystem_ChangeEnergyTemp(pThis);
		
        //室内温度>T+ T3（默认1.5℃），持续满足t3(默认5min)时间，
        //且满足【模式切换间隔时间3】（默认10min）则机组切换为制冷模式；
        if( (sAmbientIn_T > usTempSet + pThis->usModeAdjustTemp_3) && 
            (usGetTmrState(&pThis->sModeChangeTmr_3) != OS_TMR_STATE_RUNNING) && 
            (usGetTmrState(&pThis->sModeChangePeriodTmr_3) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_COOL);
            if(xSystem_UnitCompsClosed(pThis) == TRUE)        //首次切入制冷模式
            {
                pThis->xCompFirstRun = TRUE;  //首次开启
            }
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_5, pThis->usModeChangePeriod_5*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangePeriodTmr_5 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);

            return;
        }
        //室内温度<室内目标温度- T4（默认1.5℃），持续满足t4(默认5min)时间，
        //且满足【模式切换间隔时间4】（默认10min）则机组切换为送风模式；
        if( (sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_4) && 
            (usGetTmrState(&pThis->sModeChangeTmr_4) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_4) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_1, pThis->usModeChangePeriod_1*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangePeriodTmr_1 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);

            return;
        }
    }
    //（3）系统制冷模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_COOL)
    {
		usTempSet = usSystem_ChangeEnergyTemp(pThis);
		
        //室内温度<T- T5（默认1.5℃）持续满足t5(默认5min)时间
        //且满足【模式切换间隔时间5】（默认10min）机组切换为湿膜模式。
        if( (sAmbientIn_T < usTempSet - pThis->usModeAdjustTemp_5) && 
            (usGetTmrState(&pThis->sModeChangeTmr_5) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_5) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_3, pThis->usModeChangePeriod_3*60, 0, 
                               OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_4, pThis->usModeChangePeriod_4*60, 0, 
                   OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            
            debug("xTimerRegist sModeChangePeriodTmr_3 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            debug("xTimerRegist sModeChangePeriodTmr_4 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
           
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        //且满足【模式切换间隔时间5】（默认10min）机组切换为湿膜模式。
        if((pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClosed(pThis) == TRUE) )
        {
            if( (usGetTmrState(&pThis->sModeChangeTmr_7) != OS_TMR_STATE_RUNNING) && 
                (usGetTmrState(&pThis->sModeChangePeriodTmr_5) != OS_TMR_STATE_RUNNING) )
            {
                vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
                (void)xTimerRegist(&pThis->sModeChangePeriodTmr_3, pThis->usModeChangePeriod_3*60, 0, 
                                   OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
                (void)xTimerRegist(&pThis->sModeChangePeriodTmr_4, pThis->usModeChangePeriod_4*60, 0, 
                       OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
                
                debug("xTimerRegist sModeChangePeriodTmr_3 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
                debug("xTimerRegist sModeChangePeriodTmr_4 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
                
                return;
            }
        }
    }  
    //（4）系统制热模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_HEAT)
    {
        //室内温度>室内目标温度+ T6（默认1.5℃）持续满足t6(默认5min)时间
        //且满足【模式切换间隔时间6】（默认10min）则机组切换为送风模式；
        if( (sAmbientIn_T > pThis->usTempSet + pThis->usModeAdjustTemp_6) && 
            (usGetTmrState(&pThis->sModeChangeTmr_6) != OS_TMR_STATE_RUNNING) &&
             (usGetTmrState(&pThis->sModeChangePeriodTmr_6) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_1, pThis->usModeChangePeriod_1*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangePeriodTmr_1 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t8(默认5min)
        //且满足【模式切换间隔时间6】（默认10min）则机组切换为送风模式；
        if( (pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClosed(pThis) == TRUE) )
        {
            if( (usGetTmrState(&pThis->sModeChangeTmr_8) != OS_TMR_STATE_RUNNING) && 
                (usGetTmrState(&pThis->sModeChangePeriodTmr_6) != OS_TMR_STATE_RUNNING) )
            {
                vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
                (void)xTimerRegist(&pThis->sModeChangePeriodTmr_1, pThis->usModeChangePeriod_1*60, 0, 
                                    OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
                debug("xTimerRegist sModeChangePeriodTmr_1 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
                return;
            }
        }
    } 
}

void vSystem_ModeChangeTimeCallback(void* p_tmr, void* p_arg)
{
    System* pThis = (System*)p_arg;
    
#if DEBUG_ENABLE > 0
    debug("vSystem_ModeChangeTimeCallback \n");
#endif         
    vSystem_AdjustUnitRunningMode(pThis);
}

/*切换机组运行模式*/
void vSystem_ChangeUnitRunningMode(System* pt)
{
    System* pThis = (System*)pt; 
    
    uint16_t usTempSet     = 0;
    int16_t  sAmbientIn_T  = pThis->sAmbientIn_T;
    uint16_t usEnergyTemp  = pThis->usEnergyTemp;
    uint16_t usTempDeviat  = pThis->usTempDeviat;
    
    //6. 自动模式
    if(pThis->eSystemMode != MODE_AUTO || pThis->eSystemState == STATE_CLOSED)
    {
        return;
    }
#if DEBUG_ENABLE > 0
    debug("vSystem_ChangeUnitRunningMode %d\n", pThis->eRunningMode);
#endif           
    //（1）系统送风模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_FAN)
    {
        //室内温度>室内目标温度+ T1（默认1.5℃），持续满足t1(默认5min)时间
        if( (sAmbientIn_T > pThis->usTempSet + pThis->usModeAdjustTemp_1) && 
            (usGetTmrState(&pThis->sModeChangeTmr_1) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_1, pThis->usModeChangeTime_1*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangeTmr_1 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
            
        }
        //室内温度<室内目标温度- T2（默认1.5℃），持续满足t2(默认5min)时间
        if( (sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_2) && 
            (usGetTmrState(&pThis->sModeChangeTmr_2) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_2, pThis->usModeChangeTime_2*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
             debug("xTimerRegist sModeChangeTmr_2 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
             return;
        }
    }
    //（2）系统湿膜模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_WET)
    {
		usTempSet = usSystem_ChangeEnergyTemp(pThis);
		
        //室内温度>T+ T3（默认1.5℃），持续满足t3(默认5min)时间
        if( (sAmbientIn_T > usTempSet + pThis->usModeAdjustTemp_3) && 
            (usGetTmrState(&pThis->sModeChangeTmr_3) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_3, pThis->usModeChangeTime_3*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangeTmr_3 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
        //室内温度<室内目标温度- T4（默认1.5℃），持续满足t2(默认5min)时间
        if( (pThis->sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_4) && 
            (usGetTmrState(&pThis->sModeChangeTmr_4) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_4, pThis->usModeChangeTime_4*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangeTmr_4 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);            
            return;
        }
    }
    //（3）系统制冷模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_COOL)
    {
		 usTempSet = usSystem_ChangeEnergyTemp(pThis);
		
        //室内温度<T- T5（默认1.5℃）持续满足t5(默认5min)时间
        if( (sAmbientIn_T < usTempSet - pThis->usModeAdjustTemp_5) && 
            (usGetTmrState(&pThis->sModeChangeTmr_5) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_5, pThis->usModeChangeTime_5*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangeTmr_5 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        if((pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClosed(pThis) == TRUE) )
        {
            if(usGetTmrState(&pThis->sModeChangeTmr_7) != OS_TMR_STATE_RUNNING)
            {
                (void)xTimerRegist(&pThis->sModeChangeTmr_7, pThis->usModeChangeTime_7*60, 0, 
                                    OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
                debug("xTimerRegist sModeChangeTmr_7 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
                return;
            }
        }
    }  
    //（4）系统制热模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_HEAT)
    {
        pThis->xCompFirstRun = TRUE;
        //室内温度>室内目标温度+ T6（默认1.5℃）持续满足t6(默认5min)时间
        if( (sAmbientIn_T > pThis->usTempSet + pThis->usModeAdjustTemp_6) && 
            (usGetTmrState(&pThis->sModeChangeTmr_6) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_6, pThis->usModeChangeTime_6*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            debug("xTimerRegist sModeChangeTmr_6 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        if( (pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClosed(pThis) == TRUE) )
        {
            if(usGetTmrState(&pThis->sModeChangeTmr_8) != OS_TMR_STATE_RUNNING)
            {
                (void)xTimerRegist(&pThis->sModeChangeTmr_8, pThis->usModeChangeTime_8*60, 0, 
                                    OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
                debug("xTimerRegist sModeChangeTmr_8 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);                
                return;
            }
        }
    }
}

/*机组送风温度变化*/
void vSystem_UnitSupAirTemp(System* pt, ModularRoof* pModularRoof)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    //(3)当送风温度大于【送风温度最大值】（默认45℃）,声光报警
    if(pModularRoof->sSupAir_T > pThis->usSupAirMax_T)
    {
        vSystem_SetAlarm(pThis);           
    }
    else
    {
        vSystem_DelAlarmRequst(pThis); //机组送风温度恢复正常，申请消除声光报警
    }         
}

/*机组新风量变化*/
void vSystem_UnitFreAir(System* pt)
{
    uint8_t  n = 0; 

    System*      pThis         = (System*)pt;
    BMS*         psBMS         = BMS_Core();
    ModularRoof* pModularRoof  = NULL;
    uint32_t ulTotalFreAir_Vol = 0;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->xStopErrFlag == FALSE && pModularRoof->xCommErr == FALSE) //机组无故障
        {    
            ulTotalFreAir_Vol +=  pModularRoof->usFreAir_Vol;
        }
    }
    pThis->ulTotalFreAir_Vol = ulTotalFreAir_Vol;
    psBMS->usTotalFreAir_Vol_H = pThis->ulTotalFreAir_Vol / 65535;
    psBMS->usTotalFreAir_Vol_L = pThis->ulTotalFreAir_Vol % 65535;
}

/*机组CO2浓度变化*/
void vSystem_UnitCO2PPM(System* pt)
{ 
    uint8_t  n, ucNum; 
    uint16_t usTotalCO2PPM, usCO2PPM;   
    
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    CO2Sensor*   pCO2Sensor   = NULL;
    
    if(pThis->xCO2SenErr == TRUE)   //系统传感器故障，采用机组参数
    {
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            if( pModularRoof->xCommErr == FALSE)  //机组在线
            {
                usTotalCO2PPM += pModularRoof->usCO2PPMSelf;
                ucNum++;
            }
        }
        if(ucNum != 0) 
        {
            usCO2PPM  = usTotalCO2PPM / ucNum;    //机组CO2平均浓度
        }
        if(pThis->usCO2PPM != usCO2PPM)
        {
            pThis->usCO2PPM = usCO2PPM;
             //(2)当室内CO2浓度大于【CO2报警浓度指标值】（默认3000PPM），声光报警
            if( pThis->usCO2PPM >= pThis->usCO2PPMAlarm)  
            {
                vSystem_SetAlarm(pThis);
            }
            else
            {
                vSystem_DelAlarmRequst(pThis); //否则申请消除声光报警
            }
            vSystem_ExAirFanCtrl(pThis);
        }
    }
}

/*机组室外温湿度变化*/
void vSystem_UnitTempHumiOut(System* pt)
{
    uint8_t  n, ucNum;
    int16_t  sTotalTemp = 0; 
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
 
    if( pThis->xTempSenOutErr == TRUE )   //系统传感器故障，采用机组参数
    {
        for(n=0, ucNum=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            if(pModularRoof->xCommErr == FALSE)  //机组在线
            {
                sTotalTemp  += pModularRoof->sAmbientOutSelf_T;
                usTotalHumi += pModularRoof->usAmbientOutSelf_H;
                ucNum++;
            }
        }
        if(ucNum != 0) 
        {
            if( pThis->xTempSenOutErr == TRUE)              
            {
                pThis->sAmbientOut_T  = sTotalTemp / ucNum;    //机组室外平均环境温度
            }
            if( pThis->xHumiSenOutErr == TRUE)      
            {
                pThis->usAmbientOut_H  = usTotalHumi / ucNum;  //机组室外平均环境湿度
            }
        }
		//debug("vSystem_UnitTempHumiOut  sAmbientOut_T %d  usAmbientOut_H %d sTotalTemp %d ucNum %d\n", 
        //          pThis->sAmbientOut_T, pThis->usAmbientOut_H, sTotalTemp, ucNum);
    }
}

/*机组室内温湿度变化*/
void vSystem_UnitTempHumiIn(System* pt)
{
    uint8_t  n, ucNum;
 
    int16_t  sTotalTemp, sAmbientIn_T; 
    uint16_t usTotalHumi, usAmbientIn_H;  
    
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
 
    if(pThis->xTempSenInErr == TRUE)   //系统传感器故障，采用机组参数
    {
        for(n=0, ucNum=0, sTotalTemp=0, usTotalHumi=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            if(pModularRoof->xCommErr == FALSE)  //机组在线
            {
                sTotalTemp  +=  pModularRoof->sAmbientInSelf_T;
                usTotalHumi += pModularRoof->usAmbientInSelf_H;
                ucNum++;;
            }   
        }
        if(ucNum != 0) 
        {
            if( pThis->xTempSenInErr == TRUE)              
            {
                sAmbientIn_T  = sTotalTemp / ucNum;     //机组室内平均环境温度
            }
            if( pThis->xHumiSenInErr == TRUE)      
            {
                usAmbientIn_H  = usTotalHumi / ucNum;  //机组室内平均环境湿度
            }
        }
		for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->sAmbientIn_T  = sAmbientIn_T;
            pModularRoof->usAmbientIn_H = usAmbientIn_H;
        } 
		
        if(pThis->sAmbientIn_T != sAmbientIn_T || pThis->usAmbientIn_H != usAmbientIn_H)
        {
            pThis->sAmbientIn_T = sAmbientIn_T;
            pThis->usAmbientIn_H = usAmbientIn_H;
            vSystem_ChangeUnitRunningMode(pThis);  //模式切换逻辑
        }
#if DEBUG_ENABLE > 0
        debug("vSystem_UnitTempHumiIn  sAmbientIn_T %d  usAmbientIn_H %d  sTotalTemp %d ucNum\n", 
                  pThis->sAmbientIn_T, pThis->usAmbientIn_H, sTotalTemp, ucNum);
#endif 		
    }
}

/*机组故障处理*/
void vSystem_UnitErr(System* pt)
{
    uint8_t  n, ucUnitNum; 
    
    uint16_t usFreAirSet_Vol = 0;
    System* pThis = (System*)pt;
    BMS*    psBMS = BMS_Core();
    
    ModularRoof* pModularRoof = NULL;
    ModularRoof* psUnit = NULL;
     
    for(n=0, ucUnitNum=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        if(pModularRoof->xStopErrFlag == FALSE && pModularRoof->xCommErr == FALSE) 
        {
            ucUnitNum++;
            psUnit = pModularRoof;
            if(pThis->eSystemMode == MODE_AUTO || pThis->eSystemMode == MODE_EMERGENCY)
            {
                pModularRoof->IDevSwitch.switchOpen(SUPER_PTR(pModularRoof, IDevSwitch)); //开启机组
            }                
        }            
    }
    if(ucUnitNum > 0 && ucUnitNum < MODULAR_ROOF_NUM ) //其中一台机组可用
    {
        if(pThis->ulFreAirSet_Vol > MODULAR_MAX_FRE_AIR_VOL)    //保证新风量
        {
            psUnit->usFreAirSet_Vol = MODULAR_MAX_FRE_AIR_VOL;
            pThis->ulFreAirSet_Vol  = MODULAR_MAX_FRE_AIR_VOL;
            
            psBMS->usFreAirSet_Vol_L = pThis->ulFreAirSet_Vol % 65535;
            psBMS->usFreAirSet_Vol_H = pThis->ulFreAirSet_Vol / 65535;
        }
        else
        {
            psUnit->usFreAirSet_Vol = (uint16_t)pThis->ulFreAirSet_Vol;
        }
    }
    else if(ucUnitNum == MODULAR_ROOF_NUM)  //两台
    {
        usFreAirSet_Vol = (uint16_t)(pThis->ulFreAirSet_Vol / MODULAR_ROOF_NUM);
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->usFreAirSet_Vol = usFreAirSet_Vol;
        }  
    }
    if(ucUnitNum < MODULAR_ROOF_NUM)   
    {
        vSystem_SetAlarm(pThis);
    }
    else    //所机组无故障申请消除声光报警
    {
        vSystem_DelAlarmRequst(pThis);
    }
    vSystem_UnitFreAir(pThis);
    vSystem_UnitCO2PPM(pThis);
    vSystem_UnitTempHumiOut(pThis);
    vSystem_UnitTempHumiIn(pThis);
    
    if(ucUnitNum == 0) 
    {
        pThis->xUnitErrFlag = TRUE;
        if(pThis->eSystemMode == MODE_AUTO || pThis->eSystemMode == MODE_EMERGENCY)  //如果为自动或紧急送风切回手动
        {
            pThis->eSystemMode = MODE_MANUAL;
            psBMS->eSystemMode = MODE_MANUAL;
        } 
    }
    else
    {
        pThis->xUnitErrFlag = FALSE;
    }        
#if DEBUG_ENABLE > 0
    debug("vSystem_UnitErr %d\n", pThis->xUnitErrFlag);
#endif  
}

