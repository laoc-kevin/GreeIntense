#include "bms.h"
#include "system.h"
#include "systemctrl.h"

#define MODULAR_POLL_TIME_OUT_S   30

int16_t   LastAmbientIn_T = 0;

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
    if(pThis->eRunningMode != STATE_WET)  //湿膜模式
    {
        return 0;
    }
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
    return usTempSet;
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ChangeEnergyTemp %d\n", pThis->usEnergyTemp); 
#endif       
}

void vSystem_PollTimeCallBack(void* p_tmr, void* p_arg)    
{
    OS_ERR err = OS_ERR_NONE;
    System* pThis = (System*)p_arg;
    
    //防止温度长时间不变化而导致无法切换模式
    if(pThis->eSystemMode != MODE_AUTO || pThis->eSystemState == STATE_CLOSED || LastAmbientIn_T != pThis->sAmbientIn_T)
    {
        return;
    }
    myprintf("********************vSystem_PollTask***********************\n");
    vSystem_ChangeUnitRunningMode(pThis);           
}

/*设定机组运行模式*/
void vSystem_SetUnitRunningMode(System* pt, eRunningMode eRunMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;

    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    
    if(pThis->eRunningMode == eRunMode)
    {
        return;
    }
    pThis->eRunningMode = eRunMode;
    pThis->psBMS->eRunningMode = eRunMode;  
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_SetUnitRunningMode %d\n", eRunMode); 
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
        LastAmbientIn_T = pThis->sAmbientIn_T;
        vSystem_OpenUnits(pThis);
        (void)xTimerRegist(&pThis->sSystemPollTmr, 0, MODULAR_POLL_TIME_OUT_S, 
                            OS_OPT_TMR_PERIODIC, vSystem_PollTimeCallBack, pThis, FALSE); 
    }
    if(pThis->eRunningMode == STATE_WET)  //湿膜模式
    {
        (void)usSystem_ChangeEnergyTemp(pThis);
    }
    vSystem_ExAirFanCtrl(pThis);   //排风机控制
}

void vSystem_ModeChangePeriodTimeCallback(void* p_tmr, void* p_arg)
{
    System* pThis = (System*)p_arg;
    LastAmbientIn_T = pThis->sAmbientIn_T;
    
//    (void)xTimerRegist(&pThis->sSystemPollTmr, 0, MODULAR_POLL_TIME_OUT_S, 
//                        OS_OPT_TMR_PERIODIC, vSystem_PollTimeCallBack, pThis, FALSE); 
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ModeChangePeriodTimeCallback %d %d\n", LastAmbientIn_T, pThis->sAmbientIn_T);
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
    LastAmbientIn_T = pThis->sAmbientIn_T;
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_AdjustUnitRunningMode %d\n", pThis->eRunningMode);
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
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_1, pThis->usModeChangePeriod_1*60, 0, 
                               OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            
            myprintf("xTimerRegist sModeChangePeriodTmr_1 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
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
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_2, pThis->usModeChangePeriod_2*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            myprintf("xTimerRegist sModeChangePeriodTmr_2 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
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
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_3, pThis->usModeChangePeriod_3*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            myprintf("xTimerRegist sModeChangePeriodTmr_3 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
        //室内温度<室内目标温度- T4（默认1.5℃），持续满足t4(默认5min)时间，
        //且满足【模式切换间隔时间4】（默认10min）则机组切换为送风模式；
        if( (sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_4) && 
            (usGetTmrState(&pThis->sModeChangeTmr_4) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_4) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_4, pThis->usModeChangePeriod_4*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            myprintf("xTimerRegist sModeChangePeriodTmr_4 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
    }
    //（3）系统制冷模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_COOL)
    {
        //室内温度<室内目标温度- T5（默认1.5℃）持续满足t5(默认5min)时间
        //且满足【模式切换间隔时间5】（默认10min）机组切换为湿膜模式。
        if( (sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_5) && 
            (usGetTmrState(&pThis->sModeChangeTmr_5) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_5) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_5, pThis->usModeChangePeriod_5*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            myprintf("xTimerRegist sModeChangePeriodTmr_5 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
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
                (void)xTimerRegist(&pThis->sModeChangePeriodTmr_5, pThis->usModeChangePeriod_5*60, 0, 
                                    OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
                myprintf("xTimerRegist sModeChangePeriodTmr_5 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
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
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_6, pThis->usModeChangePeriod_6*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
            myprintf("xTimerRegist sModeChangePeriodTmr_6 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        //且满足【模式切换间隔时间6】（默认10min）则机组切换为送风模式；
        if( (pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClosed(pThis) == TRUE) )
        {
            if( (usGetTmrState(&pThis->sModeChangeTmr_8) != OS_TMR_STATE_RUNNING) && 
                (usGetTmrState(&pThis->sModeChangePeriodTmr_6) != OS_TMR_STATE_RUNNING) )
            {
                vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
                (void)xTimerRegist(&pThis->sModeChangePeriodTmr_6, pThis->usModeChangePeriod_6*60, 0, 
                                    OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangePeriodTimeCallback, pThis, FALSE);
                myprintf("xTimerRegist sModeChangePeriodTmr_6 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
                return;
            }
        }
    }
#if DEBUG_ENABLE > 0
    myprintf("vSystem_AdjustUnitRunningMode %d\n", pThis->eRunningMode);
#endif  
}

void vSystem_ModeChangeTimeCallback(void* p_tmr, void* p_arg)
{
    System* pThis = (System*)p_arg;
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ModeChangeTimeCallback \n");
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
    LastAmbientIn_T = pThis->sAmbientIn_T;
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ChangeUnitRunningMode %d\n", pThis->eRunningMode);
#endif           
    //（1）系统送风模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_FAN)
    {
        //室内温度>室内目标温度+ T1（默认1.5℃），持续满足t1(默认5min)时间
        if( (sAmbientIn_T > pThis->usTempSet + pThis->usModeAdjustTemp_1) && 
            (usGetTmrState(&pThis->sModeChangeTmr_1) != OS_TMR_STATE_RUNNING) )
        {
            myprintf("xTimerRegist sModeChangeTmr_1 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            (void)xTimerRegist(&pThis->sModeChangeTmr_1, pThis->usModeChangeTime_1*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            
        }
        //室内温度<室内目标温度- T2（默认1.5℃），持续满足t2(默认5min)时间
        if( (sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_2) && 
            (usGetTmrState(&pThis->sModeChangeTmr_2) != OS_TMR_STATE_RUNNING) )
        {
            myprintf("xTimerRegist sModeChangeTmr_2 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            (void)xTimerRegist(&pThis->sModeChangeTmr_2, pThis->usModeChangeTime_2*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
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
            myprintf("xTimerRegist sModeChangeTmr_3 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            (void)xTimerRegist(&pThis->sModeChangeTmr_3, pThis->usModeChangeTime_3*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            return;
        }
        //室内温度<室内目标温度- T4（默认1.5℃），持续满足t2(默认5min)时间
        if( (pThis->sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_4) && 
            (usGetTmrState(&pThis->sModeChangeTmr_4) != OS_TMR_STATE_RUNNING) )
        {
            myprintf("xTimerRegist sModeChangeTmr_4 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            (void)xTimerRegist(&pThis->sModeChangeTmr_4, pThis->usModeChangeTime_4*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);            
            return;
        }
    }
    //（3）系统制冷模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_COOL)
    {
        //室内温度<室内目标温度- T5（默认1.5℃）持续满足t5(默认5min)时间
        if( (sAmbientIn_T < pThis->usTempSet - pThis->usModeAdjustTemp_5) && 
            (usGetTmrState(&pThis->sModeChangeTmr_5) != OS_TMR_STATE_RUNNING) )
        {
            myprintf("xTimerRegist sModeChangeTmr_5 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            (void)xTimerRegist(&pThis->sModeChangeTmr_5, pThis->usModeChangeTime_5*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            return;
        }
        
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        if((pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClosed(pThis) == TRUE) )
        {
            if(usGetTmrState(&pThis->sModeChangeTmr_7) != OS_TMR_STATE_RUNNING)
            {
                myprintf("xTimerRegist sModeChangeTmr_7 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
                (void)xTimerRegist(&pThis->sModeChangeTmr_7, pThis->usModeChangeTime_7*60, 0, 
                                    OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
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
            myprintf("xTimerRegist sModeChangeTmr_6 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
            (void)xTimerRegist(&pThis->sModeChangeTmr_6, pThis->usModeChangeTime_6*60, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        if( (pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClosed(pThis) == TRUE) )
        {
            if(usGetTmrState(&pThis->sModeChangeTmr_8) != OS_TMR_STATE_RUNNING)
            {
                myprintf("xTimerRegist sModeChangeTmr_8 %d sAmbientIn_T %d\n", pThis->eRunningMode, sAmbientIn_T);
                (void)xTimerRegist(&pThis->sModeChangeTmr_8, pThis->usModeChangeTime_8*60, 0, 
                                    OS_OPT_TMR_ONE_SHOT, vSystem_ModeChangeTimeCallback, pThis, FALSE);               
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

    System*      pThis        = (System*)pt;
    BMS*         psBMS        = BMS_Core();
    ModularRoof* pModularRoof = NULL;
    
    pThis->ulTotalFreAir_Vol = 0;
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->xStopErrFlag == FALSE) //机组无故障
        {    
            pThis->ulTotalFreAir_Vol +=  pModularRoof->usFreAir_Vol;
        }
    }
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
            if( pModularRoof->sMBSlaveDev.xOnLine == TRUE )  //机组在线
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
 
    if( (pThis->xTempSenOutErr == TRUE) || (pThis->xHumiSenOutErr == TRUE) )   //系统传感器故障，采用机组参数
    {
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            if( pModularRoof->sMBSlaveDev.xOnLine == TRUE )  //机组在线
            {
                sTotalTemp  +=  pModularRoof->sAmbientOutSelf_T;
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
 
    if( (pThis->xTempSenInErr == TRUE) || (pThis->xHumiSenInErr == TRUE) )   //系统传感器故障，采用机组参数
    {
        for(n=0, sTotalTemp=0, usTotalHumi=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            if( pModularRoof->sMBSlaveDev.xOnLine == TRUE )  //机组在线
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
        if(pThis->sAmbientIn_T != sAmbientIn_T || pThis->usAmbientIn_H != usAmbientIn_H)
        {
            pThis->sAmbientIn_T = sAmbientIn_T;
            pThis->usAmbientIn_H = usAmbientIn_H;
            vSystem_ChangeUnitRunningMode(pThis);  //模式切换逻辑
        }   
    }
}

/*机组故障处理*/
void vSystem_UnitErr(System* pt)
{
    uint8_t  n, ucStopErrNum, ucOnLineNum; 
    
    System* pThis = (System*)pt;
    BMS*    psBMS = BMS_Core();
    
    ModularRoof* pModularRoof = NULL;
    ModularRoof* pOnlineUnit = NULL;
     
    for(n=0, ucStopErrNum=0, ucOnLineNum=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        if(pModularRoof->xStopErrFlag) //这个标志位不包含可恢复的故障。群控收到这个标志位就要下发关机命令
        {
            ucStopErrNum++; 
        }
        //(1)空调机组在线    
        if(pModularRoof->xCommErr == FALSE) 
        {
            ucOnLineNum++;
            pOnlineUnit = pModularRoof;          
        }         
    }
    if(ucOnLineNum > 0 && ucOnLineNum < MODULAR_ROOF_NUM ) //其中一台机组通讯故障
    {
        if(pOnlineUnit->xStopErrFlag == FALSE)  //且无故障
        {
            if(pThis->ulFreAirSet_Vol > 65000)    //保证新风量
            {
                pOnlineUnit->usFreAirSet_Vol = 65000;
            }
            else
            {
                pOnlineUnit->usFreAirSet_Vol = pThis->ulFreAirSet_Vol;
            }
        }
    }
    if(ucStopErrNum != 0 || ucOnLineNum != MODULAR_ROOF_NUM)   
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
    
    if(ucStopErrNum == MODULAR_ROOF_NUM || ucOnLineNum == 0) 
    {
        pThis->xUnitErrFlag = TRUE;
        if(pThis->eSystemMode == MODE_AUTO)  //如果为自动切回手动
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
    myprintf("vSystem_UnitErr %d\n", pThis->xUnitErrFlag);
#endif  
}

