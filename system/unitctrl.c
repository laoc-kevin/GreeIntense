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
        pModularRoof->IDevSwitch.switchOpen(SUPER_PTR(pModularRoof, IDevSwitch)); //开启所有机组
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
        pModularRoof->IDevSwitch.switchClose(SUPER_PTR(pModularRoof, IDevSwitch)); //关闭所有机组
    }
}

/*设定机组运行模式*/
void vSystem_SetRunningMode(System* pt, eRunningMode eRunMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    
    pThis->eRunningMode = eRunMode; 
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];               
        pModularRoof->eRunningMode = pThis->eRunningMode;
    }
    
    if(pThis->eRunningMode == RUN_MODE_HEAT)    //制热工况下，排风机不开启
    {
        vSystem_CloseExAirFans(pThis);
    }
}

/*/调整机组运行模式*/
void vSystem_AdjustRunningMode(void* p_arg)
{
    System* pThis = (System*)p_arg;
    
    //6. 自动模式
    if(pThis->eSystemMode != MODE_AUTO)
    {
        return;
    }
    //若t2(默认5分钟)内温度没有达到目标温度t(ng1)±1.5℃
    if(pThis->sAmbientIn_T > pThis->sTargetTemp + pThis->ucAmbientInDeviat_T)  
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
    if(pThis->sAmbientIn_T > pThis->sTargetTemp)  
    {
        //A. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTargetTemp >= pThis->sGrowUpTemp) 
        {
            //(1)室外干球温度t(wg)≤模式调节温度（默认23℃）
            if(pThis->sAmbientOut_T <= pThis->sAdjustModeTemp)  
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_FAN);    //开启送风模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_1 * 60, vSystem_AdjustRunningMode, pThis);
            }
            
            //(2)室外干球温度t(wg)＞模式调节温度（默认23℃） 且t（ws）+3 <= t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->sAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) <= pThis->sTargetTemp-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_WET);    //开启湿膜降温模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_2 * 60, vSystem_AdjustRunningMode, pThis);
            }
            
            //(3)室外干球温度t(wg)＞模式调节温度（默认23℃），且t（ws）+3＞t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->sAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) > pThis->sTargetTemp-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_COOL);    //开启降温模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_3 * 60, vSystem_AdjustRunningMode, pThis);
            }
        } 
        //B. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTargetTemp < pThis->sGrowUpTemp) 
        {
              
        }  
    }
    //制热工况:  室内干球温度t(ng2) <= 舍内温度目标要求温度t(ng1)，开启制热工况
    if(pThis->sAmbientIn_T <= pThis->sTargetTemp)     
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
        if(pModularRoof->sSupAir_T > pThis->sSupAirMax_T)
        {
            vSystem_SetAlarm(pThis);
            return;            
        }         
    }
    vSystem_DelAlarmRequst(pThis); //否则申请消除声光报警
}

/*机组故障处理*/
void vSystem_UnitErr(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 

       //(1)群控控制器与空调机组通讯故障,  (8)空调机组停机保护。声光报警        
        if( (pModularRoof->sMBSlaveDev.xOnLine == FALSE) || (pModularRoof->xStopErrFlag) ) 
        {
            vSystem_SetAlarm(pThis); 
            return;            
        }          
    }
    vSystem_DelAlarmRequst(pThis); //否则申请消除声光报警
}

