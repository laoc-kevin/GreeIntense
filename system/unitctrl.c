#include "bms.h"
#include "system.h"
#include "systemctrl.h"

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

/*机组所有压缩机关闭*/
BOOL xSystem_UnitCompsClose(System* pt)
{
    uint8_t  n, m, i;
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    Modular*     pModular     = NULL;
    Compressor*  pComp        = NULL;
    
    for(n=0; n< MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        for(i=0; i< MODULAR_NUM; i++)
        {
            pModular = pModularRoof->psModularList[i];
            for(m=0; m < COMP_NUM; m++)
            {
                pComp = pModular->psCompList[m];
                if(pComp->Device.eRunningState == STATE_RUN)
                {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

/*切换机组节能温度*/
void vSystem_ChangeEnergyTemp(System* pt)
{
    System* pThis = (System*)pt;
    
    int16_t  sAmbientIn_T  = pThis->sAmbientIn_T;
    uint16_t usEnergyTemp  = pThis->usEnergyTemp;
    uint16_t usTempDeviat  = pThis->usTempDeviat;
    
     /*注：1、当室内目标温度>=【节能温度】（默认25℃），
                    机组制冷目标温度=室内目标温度；
                    T=室内目标温度；
        
             2、当室内目标温度<节能温度（默认25℃），机组制冷目标温度=【节能温度】；
                    T=【节能温度】-【温度偏差】（默认0.5℃）；*/
    if(sAmbientIn_T < pThis->usEnergyTemp)
    {
        pThis->sTempSet = usEnergyTemp - usTempDeviat;
    }
    vSystem_SetTemp(pThis, pThis->sTempSet);
}

/*设定机组运行模式*/
void vSystem_SetUnitRunningMode(System* pt, eRunningMode eRunMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];    
        pModularRoof->setRunningMode(pModularRoof, eRunMode);
    }
    switch(eRunMode)
    {
        case RUN_MODE_COOL:
            pThis->eSystemState = STATE_COOL;
        break;
        case RUN_MODE_HEAT:
            pThis->eSystemState = STATE_HEAT;
        break;
        case RUN_MODE_FAN:
            if(pThis->eSystemMode != MODE_EMERGENCY)
            {
                pThis->eSystemState = STATE_FAN;
            }
        break;
        case RUN_MODE_WET:
            pThis->eSystemState = STATE_WET;
        break;
        default:break;
    }        
    pThis->eRunningMode = eRunMode;
    myprintf("vSystem_SetUnitRunningMode %d\n", pThis->eRunningMode);    
}

/*/调整机组运行模式*/
void vSystem_AdjustUnitRunningMode(void* p_tmr, void* p_arg)
{
    System* pThis = (System*)p_arg;
    int16_t sAmbientIn_T = pThis->sAmbientIn_T;

    if(pThis->eSystemMode != MODE_AUTO)     //6. 自动模式
    {
        return;
    }
    //（1）系统送风模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_FAN)
    {
        //室内温度>室内目标温度+ T1（默认1.5℃），持续满足t1(默认5min)时间，
        //且满足【模式切换间隔时间1】（默认10min）则机组切换为湿膜模式；
        if( (sAmbientIn_T > pThis->sTempSet + pThis->usModeAdjustTemp_1) && 
            (usGetTmrState(&pThis->sModeChangeTmr_1) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_1) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_1, pThis->usModeChangePeriod_1, 
                               0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
            return;            
        }
        //室内温度<室内目标温度- T2（默认1.5℃），持续满足t2(默认5min)时间，
        //且满足【模式切换间隔时间2】（默认10min）则机组切换为制热模式；
        if( (sAmbientIn_T > pThis->sTempSet + pThis->usModeAdjustTemp_2) && 
            (usGetTmrState(&pThis->sModeChangeTmr_2) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_2) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_HEAT);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_2, pThis->usModeChangePeriod_2, 
                               0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
            return;
        }
    }
    //（2）系统湿膜模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_WET)
    {
        vSystem_ChangeEnergyTemp(pThis);

        //室内温度>T+ T3（默认1.5℃），持续满足t3(默认5min)时间，
        //且满足【模式切换间隔时间3】（默认10min）则机组切换为制冷模式；
        if( (sAmbientIn_T > pThis->sTempSet + pThis->usModeAdjustTemp_3) && 
            (usGetTmrState(&pThis->sModeChangeTmr_3) != OS_TMR_STATE_RUNNING) && 
            (usGetTmrState(&pThis->sModeChangePeriodTmr_3) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_COOL);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_3, pThis->usModeChangePeriod_3, 
                               0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
            return;
        }
        //室内温度<室内目标温度- T4（默认1.5℃），持续满足t4(默认5min)时间，
        //且满足【模式切换间隔时间4】（默认10min）则机组切换为送风模式；
        if( (sAmbientIn_T > pThis->sTempSet - pThis->usModeAdjustTemp_4) && 
            (usGetTmrState(&pThis->sModeChangeTmr_4) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_4) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_4, pThis->usModeChangePeriod_4, 
                               0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
            return;
        }
    }
    //（3）系统制冷模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_COOL)
    {
        //室内温度<室内目标温度- T5（默认1.5℃）持续满足t5(默认5min)时间
        //且满足【模式切换间隔时间5】（默认10min）机组切换为湿膜模式。
        if( (sAmbientIn_T > pThis->sTempSet - pThis->usModeAdjustTemp_5) && 
            (usGetTmrState(&pThis->sModeChangeTmr_5) != OS_TMR_STATE_RUNNING) &&
            (usGetTmrState(&pThis->sModeChangePeriodTmr_5) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_5, pThis->usModeChangePeriod_5, 
                               0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        //且满足【模式切换间隔时间5】（默认10min）机组切换为湿膜模式。
        if((pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClose(pThis) == TRUE) )
        {
            if( (usGetTmrState(&pThis->sModeChangeTmr_7) != OS_TMR_STATE_RUNNING) && 
                (usGetTmrState(&pThis->sModeChangePeriodTmr_5) != OS_TMR_STATE_RUNNING) )
            {
                vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
                (void)xTimerRegist(&pThis->sModeChangePeriodTmr_5, pThis->usModeChangePeriod_5, 
                                   0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
                return;
            }
        }
    }  
    //（4）系统制热模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_HEAT)
    {
        //室内温度<室内目标温度- T6（默认1.5℃）持续满足t6(默认5min)时间
        //且满足【模式切换间隔时间6】（默认10min）则机组切换为送风模式；
        if( (sAmbientIn_T > pThis->sTempSet - pThis->usModeAdjustTemp_6) && 
            (usGetTmrState(&pThis->sModeChangeTmr_6) != OS_TMR_STATE_RUNNING) &&
             (usGetTmrState(&pThis->sModeChangePeriodTmr_6) != OS_TMR_STATE_RUNNING) )
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
            (void)xTimerRegist(&pThis->sModeChangePeriodTmr_6, pThis->usModeChangePeriod_6, 
                               0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        //且满足【模式切换间隔时间6】（默认10min）则机组切换为送风模式；
        if( (pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClose(pThis) == TRUE) )
        {
            if( (usGetTmrState(&pThis->sModeChangeTmr_8) != OS_TMR_STATE_RUNNING) && 
                (usGetTmrState(&pThis->sModeChangePeriodTmr_6) != OS_TMR_STATE_RUNNING) )
            {
                vSystem_SetUnitRunningMode(pThis, RUN_MODE_WET);
                (void)xTimerRegist(&pThis->sModeChangePeriodTmr_6, pThis->usModeChangePeriod_5, 
                                   0, OS_OPT_TMR_ONE_SHOT, NULL, pThis);
                return;
            }
        }
    }     
}

/*切换机组运行模式*/
void vSystem_ChangeUnitRunningMode(System* pt)
{
    System* pThis = (System*)pt; 
    int16_t sAmbientIn_T  = pThis->sAmbientIn_T;
    
    //6. 自动模式
    if(pThis->eSystemMode != MODE_AUTO)
    {
        return;
    }
    //（1）系统送风模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_FAN)
    {
        //室内温度>室内目标温度+ T1（默认1.5℃），持续满足t1(默认5min)时间
        if( (sAmbientIn_T > pThis->sTempSet + pThis->usModeAdjustTemp_1) && 
            (usGetTmrState(&pThis->sModeChangeTmr_1) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_1, pThis->usModeChangeTime_1, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
        }
        //室内温度<室内目标温度- T2（默认1.5℃），持续满足t2(默认5min)时间
        if( (sAmbientIn_T > pThis->sTempSet + pThis->usModeAdjustTemp_2) && 
            (usGetTmrState(&pThis->sModeChangeTmr_2) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_2, pThis->usModeChangeTime_2, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
        }
    }
    //（2）系统湿膜模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_WET)
    {
        vSystem_ChangeEnergyTemp(pThis);

        //室内温度>T+ T3（默认1.5℃），持续满足t3(默认5min)时间
        if( (sAmbientIn_T > pThis->sTempSet + pThis->usModeAdjustTemp_3) && 
            (usGetTmrState(&pThis->sModeChangeTmr_3) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_3, pThis->usModeChangeTime_3, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
            return;
        }
        //室内温度<室内目标温度- T4（默认1.5℃），持续满足t2(默认5min)时间
        if( (pThis->sAmbientIn_T > pThis->sTempSet - pThis->usModeAdjustTemp_4) && 
            (usGetTmrState(&pThis->sModeChangeTmr_4) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_4, pThis->usModeChangeTime_4, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
            return;
        }
    }
    //（3）系统制冷模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_COOL)
    {
        //室内温度<室内目标温度- T5（默认1.5℃）持续满足t5(默认5min)时间
        if( (sAmbientIn_T > pThis->sTempSet - pThis->usModeAdjustTemp_5) && 
            (usGetTmrState(&pThis->sModeChangeTmr_5) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_5, pThis->usModeChangeTime_5, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
            return;
        }
        
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        if((pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClose(pThis) == TRUE) )
        {
            if(usGetTmrState(&pThis->sModeChangeTmr_7) != OS_TMR_STATE_RUNNING)
            {
                (void)xTimerRegist(&pThis->sModeChangeTmr_7, pThis->usModeChangeTime_7, 
                                   0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
                return;
            }
        }
    }  
    //（4）系统制热模式运行，按照以下切换
    if(pThis->eRunningMode == RUN_MODE_HEAT)
    {
        //室内温度<室内目标温度- T6（默认1.5℃）持续满足t6(默认5min)时间
        if( (sAmbientIn_T > pThis->sTempSet - pThis->usModeAdjustTemp_6) && 
            (usGetTmrState(&pThis->sModeChangeTmr_6) != OS_TMR_STATE_RUNNING) )
        {
            (void)xTimerRegist(&pThis->sModeChangeTmr_6, pThis->usModeChangeTime_6, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
            return;
        }
        //机组压缩机全部待机（首次开启，压缩机未运行不纳入压机待机情况）持续满足t7(默认5min)
        if( (pThis->xCompFirstRun == FALSE) && (xSystem_UnitCompsClose(pThis) == TRUE) )
        {
            if(usGetTmrState(&pThis->sModeChangeTmr_8) != OS_TMR_STATE_RUNNING)
            {
                (void)xTimerRegist(&pThis->sModeChangeTmr_8, pThis->usModeChangeTime_8, 
                                   0, OS_OPT_TMR_ONE_SHOT, vSystem_AdjustUnitRunningMode, pThis);
                return;
            }
        }
    }    
}

/*机组送风温度变化*/
void vSystem_UnitSupAirTemp(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
   
    ModularRoof* pModularRoof = NULL;
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        
        //(3)当送风温度大于【送风温度最大值】（默认45℃）,声光报警
        if(pModularRoof->sSupAir_T > pThis->usSupAirMax_T)
        {
            vSystem_SetAlarm(pThis);
            return;            
        }         
    }
    vSystem_DelAlarmRequst(pThis); //所有机组送风温度恢复正常，申请消除声光报警
}

/*机组新风量变化*/
void vSystem_UnitFreAir(System* pt)
{
    uint8_t  n = 0; 

    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
    
    BMS* psBMS = BMS_Core();
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pThis->psModularRoofList[n]->sMBSlaveDev.xOnLine == TRUE) //机组在线
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
    uint16_t usTotalCO2PPM = 0;   
    
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
            pThis->usCO2PPM  = usTotalCO2PPM / ucNum;    //机组CO2平均浓度
        }
        
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
 
    int16_t  sTotalTemp = 0; 
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
 
    if( (pThis->xTempSenInErr == TRUE) || (pThis->xHumiSenInErr == TRUE) )   //系统传感器故障，采用机组参数
    {
        for(n=0; n < MODULAR_ROOF_NUM; n++)
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
                pThis->sAmbientIn_T  = sTotalTemp / ucNum;     //机组室内平均环境温度
            }
            if( pThis->xHumiSenInErr == TRUE)      
            {
                pThis->usAmbientOut_H  = usTotalHumi / ucNum;  //机组室内平均环境湿度
            }
        }
        vSystem_ChangeUnitRunningMode(pThis);  //模式切换逻辑
    }
}

/*机组状态变化*/
void vSystem_UnitState(System* pt)
{
    uint8_t  n;
 
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
 
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->Device.eRunningState == STATE_RUN)  //机组运行
        {
            return;
        }   
    }
    if(pThis->eSystemMode == MODE_CLOSE)   //系统关闭模式下
    {
        pThis->eSystemState = STATE_CLOSED;
    }   
}

/*机组故障处理*/
void vSystem_UnitErr(System* pt)
{
    uint8_t  n, ucStopErrNum, ucOnLineNum; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    ModularRoof* pOnlineUnit = NULL;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 

        if(pModularRoof->xStopErrFlag) //这个标志位不包含可恢复的故障。群控收到这个标志位就要下发关机命令
        {
            pModularRoof->IDevSwitch.switchClose(SUPER_PTR(pModularRoof, IDevSwitch)); 
            ucStopErrNum++; 
        }
        //(1)空调机组在线    
        if(pModularRoof->sMBSlaveDev.xOnLine == TRUE) 
        {
            ucOnLineNum++;
            pOnlineUnit = pModularRoof;
            pModularRoof->xCommErr = FALSE;            
        }
        else //(1)群控控制器与空调机组通讯故障    
        {
            pModularRoof->xCommErr = TRUE;
        }            
    }
    if(ucOnLineNum != 0 && ucOnLineNum != MODULAR_ROOF_NUM ) //其中一台机组通讯故障
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
}

