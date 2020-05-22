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

/*设定机组运行模式*/
void vSystem_SetRunningMode(System* pt, eRunningMode eRunMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];    
        pModularRoof->IDevRunning.setRunningMode(SUPER_PTR(pModularRoof, IDevRunning), eRunMode);
    }
    
    if(pThis->eRunningMode == eRunMode) //防止制热工况下反复关风机
    {
        return;
    }
    pThis->eRunningMode = eRunMode; 
    if(pThis->eRunningMode == RUN_MODE_HEAT)    //制热工况下，排风机不开启
    {
        vSystem_CloseExAirFans(pThis);
    }
}

/*/调整机组运行模式*/
void vSystem_AdjustRunningMode(void* p_tmr, void* p_arg)
{
    System* pThis = (System*)p_arg;
    
    //6. 自动模式
    if(pThis->eSystemMode != MODE_AUTO)
    {
        return;
    }
    //若规定时间内温度没有达到目标温度t(ng1)±1.5℃
    if(pThis->sAmbientIn_T > pThis->sTempSet + pThis->ucAmbientInDeviat_T)  
    {
        if(pThis->eRunningMode == RUN_MODE_FAN)
        {
            vSystem_SetRunningMode(pThis, RUN_MODE_WET);    //切换为湿膜降温模式
        }
        else if(pThis->eRunningMode == RUN_MODE_WET)
        {
            vSystem_SetRunningMode(pThis, RUN_MODE_COOL);   //切换为降温模式
        }
    }
}

/*切换机组运行模式*/
void vSystem_ChangeRunningMode(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    
    /*注：室外干球温度: t(wg)   室外湿球温度: t(ws)      室内目标干球温度: t(ng1)   室内实际干球温度: t(ng2)
          养殖鸡数量：n         运行当天目标新风量:G
    */
    //6. 自动模式
    if(pThis->eSystemMode != MODE_AUTO)
    {
        return;
    }
     //制冷工况:  室内干球温度t(ng2)＞舍内温度目标要求温度t(ng1)，开启制冷工况
    if(pThis->sAmbientIn_T > pThis->sTempSet)  
    {
        //A. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTempSet >= pThis->usGrowUpTemp) 
        {
            //(1)室外干球温度t(wg)≤模式调节温度（默认23℃）
            if(pThis->sAmbientOut_T <= pThis->usAdjustModeTemp)  
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_FAN);    //开启送风模式 
                (void)xTimerRegist(&pThis->sModeChangeTmr_1, pThis->ucModeChangeTime_1 * 60, 0, 
                                   OS_OPT_TMR_ONE_SHOT, vSystem_AdjustRunningMode, pThis);
            }
            
            //(2)室外干球温度t(wg)＞模式调节温度（默认23℃） 且t（ws）+3 <= t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->usAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) <= pThis->sTempSet-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_WET);    //开启湿膜降温模式
                (void)xTimerRegist(&pThis->sModeChangeTmr_2, pThis->ucModeChangeTime_2 * 60, 0, 
                                   OS_OPT_TMR_ONE_SHOT, vSystem_AdjustRunningMode, pThis);
            }
            
            //(3)室外干球温度t(wg)＞模式调节温度（默认23℃），且t（ws）+3＞t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->usAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) > pThis->sTempSet-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_COOL);    //开启降温模式
                (void)xTimerRegist(&pThis->sModeChangeTmr_3, pThis->ucModeChangeTime_3 * 60, 0, 
                                   OS_OPT_TMR_ONE_SHOT, vSystem_AdjustRunningMode, pThis);
            }
        } 
        //B. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTempSet < pThis->usGrowUpTemp) 
        {
              
            
            
            
            
        }  
    }
    //制热工况:  室内干球温度t(ng2) <= 舍内温度目标要求温度t(ng1)，开启制热工况
    if(pThis->sAmbientIn_T <= pThis->sTempSet)     
    {
        vSystem_SetRunningMode(pThis, RUN_MODE_HEAT);    
    } 
}

/*机组送风温度变化*/
void vSystem_SupAirTemp(System* pt)
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
void vSystem_FreAir(System* pt)
{
    uint8_t  n = 0; 
    BOOL     xCommErr          = 0;  
    uint16_t usTotalFreAir_Vol = 0; 
           
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
 
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pThis->psModularRoofList[n]->sMBSlaveDev.xOnLine != TRUE) //机组不在线
        {
            xCommErr = TRUE;    //机组通讯故障
            break;
        }
        usTotalFreAir_Vol +=  pModularRoof->usFreAir_Vol;
    }
    
    //【排风机控制模式】为实时新风量时
    if(pThis->eExAirFanCtrlMode == MODE_REAL_TIME)
    {
        if(xCommErr == FALSE)    //机组均通讯正常
        {
            //系统排风需求量=（机组一新风量+机组二新风量）*【排风百分比】（默认90）/100
            pThis->usExAirSet_Vol = usTotalFreAir_Vol * pThis->ucExAirRatio_1 / 100; 
        }
        if(xCommErr == TRUE)    //通讯故障
        {
            //系统排风需求量=当天目标新风量*【排风百分比1】（默认90）/100
            pThis->usExAirSet_Vol = pThis->usFreAirSet_Vol * pThis->ucExAirRatio_1 / 100;
        } 
    }
    //【排风机控制模式】为目标新风量时
    if(pThis->eExAirFanCtrlMode == MODE_REAL_TIME)
    {
         //系统排风需求量=当天目标新风量*【排风百分比1】（默认90）/100
         pThis->usExAirSet_Vol = pThis->usFreAirSet_Vol * pThis->ucExAirRatio_1 / 100;
    }  
    vSystem_ExAirSet_Vol(pThis); //系统排风需求量变化   
}

/*机组故障处理*/
void vSystem_UnitErr(System* pt)
{
    uint8_t  n, m; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 

        if(pModularRoof->xStopErrFlag) //这个标志位不包含可恢复的故障。群控收到这个标志位就要下发关机命令
        {
            pModularRoof->IDevSwitch.switchClose(SUPER_PTR(pModularRoof, IDevSwitch)); 
        }

        //(1)群控控制器与空调机组通讯故障,  (8)空调机组停机保护。声光报警        
        if( (pModularRoof->sMBSlaveDev.xOnLine == FALSE) || (pModularRoof->xStopErrFlag) ) 
        {
            vSystem_SetAlarm(pThis);
            m++;            
        }
        else
        {
            //机组故障恢复，在非手动和关闭模式下，需重开机组
            if( (pThis->eSystemMode != MODE_MANUAL) && (pThis->eSystemMode != MODE_CLOSE) )
            {
                pModularRoof->IDevSwitch.switchOpen(SUPER_PTR(pModularRoof, IDevSwitch)); 
            }
        }            
    }
    if(m==0){vSystem_DelAlarmRequst(pThis);}//所有机组无故障申请消除声光报警
}
