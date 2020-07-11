#include "bms.h"
#include "system.h"
#include "systemctrl.h"
#include "md_event.h"
#include "md_modbus.h"
#include "md_timer.h"

#define  SYSTEM_POLL_INTERVAL_S   10

#define  MAX_EX_FAN_RATED_VOL     100000l
#define  MAX_FRE_AIR_SET_VOL      130000l

/*系统开启*/
void vSystem_SwitchOpen(System* pt)
{
    System* pThis = (System*)pt;

    vSystem_OpenUnits(pThis);   
}

/*系统关闭*/
void vSystem_SwitchClose(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    
    vSystem_CloseExAirFans(pThis);
    vSystem_CloseUnits(pThis);
}

/*切换系统模式*/
void vSystem_ChangeSystemMode(System* pt, eSystemMode eSystemMode)
{
    uint8_t  n   = 0;
    OS_ERR   err = OS_ERR_NONE;
    
    System*      pThis        = (System*)pt;
    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    BMS*         psBMS        = BMS_Core();
    
    if(pThis->eSystemMode == eSystemMode)
    {
        return;
    }
    if(eSystemMode == MODE_AUTO)    //自动模式
    {
        if(pThis->xUnitErrFlag == TRUE || pThis->xExFanErrFlag == TRUE)
        {
            psBMS->eSystemMode = pThis->eSystemMode;
            return;
        }
        pThis->eSystemMode = eSystemMode;
        
        //若室内温度>室内目标温度+ T0（默认1.5℃），机组送风模式开启；否则，机组制热模式开启；
        if(pThis->sAmbientIn_T > pThis->usTempSet + pThis->usModeAdjustTemp_0)
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN);
        }
        else
        {
            vSystem_SetUnitRunningMode(pThis, RUN_MODE_HEAT);
        }
        vSystem_SwitchOpen(pThis);  //开启系统
    }
    if(eSystemMode == MODE_CLOSE)    //关闭模式
    {
        pThis->eSystemMode = eSystemMode;
        vSystem_SwitchClose(pThis);
    }
    if(eSystemMode == MODE_MANUAL)    //手动模式
    {
        pThis->eSystemMode = eSystemMode;
    }
    if(eSystemMode == MODE_EMERGENCY) //紧急模式
    {
        if(pThis->xExFanErrFlag == TRUE)
        {
            psBMS->eSystemMode = pThis->eSystemMode;
            return;
        }
        pThis->eSystemMode = eSystemMode;
        vSystem_SwitchOpen(pThis);    //开启系统
        vSystem_SetUnitRunningMode(pThis, RUN_MODE_FAN); //开启送风模式
    }
#if DEBUG_ENABLE > 0  
        myprintf("vSystem_ChangeSystemMode %d %d\n", pThis->eSystemMode, psBMS->eSystemMode);
#endif 
}

/*设定系统目标温度值*/
void vSystem_SetTemp(System* pt, uint16_t usTempSet)
{
    uint8_t  n = 0;
    
    System*      pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;

    if( pThis->usTempSet != usTempSet)
    {
        pThis->usTempSet = usTempSet;

#if DEBUG_ENABLE > 0
        myprintf("vSystem_SetTemp %d\n", pThis->usTempSet);
#endif
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->usCoolTempSet = usTempSet;
            pModularRoof->usHeatTempSet = usTempSet;
        }
        vSystem_ChangeUnitRunningMode(pThis); 
    }
}

/*设定系统目标新风量*/
void vSystem_SetFreAir(System* pt, uint16_t usFreAirSet_Vol_H, uint16_t usFreAirSet_Vol_L)
{
    uint8_t  n, ucUnitNum; 
    System* pThis = (System*)pt;
    BMS*    psBMS = BMS_Core();
    
    ModularRoof* pModularRoof = NULL;
    ModularRoof* pUnit        = NULL;
    
    uint16_t usFreAirSet_Vol = 0;
    uint32_t ulFreAirSet_Vol = (uint32_t)usFreAirSet_Vol_H*65535 + (uint32_t)usFreAirSet_Vol_L;

    if(ulFreAirSet_Vol > MAX_FRE_AIR_SET_VOL)
    {
        psBMS->usFreAirSet_Vol_L = pThis->ulFreAirSet_Vol % 65535;
        psBMS->usFreAirSet_Vol_H = pThis->ulFreAirSet_Vol / 65535;
        return;
    }
    if(pThis->ulFreAirSet_Vol == ulFreAirSet_Vol)
    {
       return;
    }
    for(n=0, ucUnitNum=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->xCommErr == FALSE && pModularRoof->xStopErrFlag == FALSE)  //机组在线且无故障
        {
            pUnit = pModularRoof;
            ucUnitNum++;           
        }
    }
    if(ucUnitNum == 0) //无机组可用
    {
        return;
    }
       
    pThis->ulFreAirSet_Vol = ulFreAirSet_Vol;
    if(ucUnitNum > 0 && ucUnitNum < MODULAR_ROOF_NUM ) //只有一台机组可用
    {
        if(pThis->ulFreAirSet_Vol > MODULAR_MAX_FRE_AIR_VOL)    //保证新风量
        {
            pUnit->usFreAirSet_Vol = MODULAR_MAX_FRE_AIR_VOL;
            pThis->ulFreAirSet_Vol = MODULAR_MAX_FRE_AIR_VOL;
            
            psBMS->usFreAirSet_Vol_L = pThis->ulFreAirSet_Vol % 65535;
            psBMS->usFreAirSet_Vol_H = pThis->ulFreAirSet_Vol / 65535;
        }
        else
        {
            pUnit->usFreAirSet_Vol = (uint16_t)pThis->ulFreAirSet_Vol;
        }
    }
    else if(ucUnitNum == MODULAR_ROOF_NUM)  //两台
    {
        usFreAirSet_Vol = (uint16_t)(ulFreAirSet_Vol / MODULAR_ROOF_NUM);
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->usFreAirSet_Vol = usFreAirSet_Vol;
        }  
    }
#if DEBUG_ENABLE > 0
    myprintf("vSystem_SetFreAir  ulFreAirSet_Vol %ld \n", pThis->ulFreAirSet_Vol);
#endif    
    vSystem_ExAirSet_Vol(pThis); //系统排风需求量变化
}

/*设定系统排风机额定风量*/
void vSystem_SetExAirFanRated(System* pt, uint16_t usExAirFanRated_Vol_H, uint16_t usExAirFanRated_Vol_L)  
{
    System* pThis = (System*)pt;
    BMS*    psBMS = BMS_Core();
    
    uint32_t ulExAirFanRated_Vol = (uint32_t)usExAirFanRated_Vol_H*65535 + (uint32_t)usExAirFanRated_Vol_L;
    
    if(ulExAirFanRated_Vol > MAX_EX_FAN_RATED_VOL)
    {
        psBMS->usExAirFanRated_Vol_H = pThis->ulExAirFanRated_Vol % 65535;
        psBMS->usExAirFanRated_Vol_L = pThis->ulExAirFanRated_Vol / 65535;
        return;
    }
    pThis->ulExAirFanRated_Vol = ulExAirFanRated_Vol;
    
#if DEBUG_ENABLE > 0
        myprintf("vSystem_SetExAirFanRated  usExAirFanRated_Vol_L %d  usExAirFanRated_Vol_H %d\n", usExAirFanRated_Vol_L, usExAirFanRated_Vol_H);
#endif 
    vSystem_ExAirFanCtrl(pThis);    
}

/*设定系统排风机调节周期*/
void vSystem_SetExAirFanCtrlPeriod(System* pt, uint16_t usExAirFanCtrlPeriod)  
{
    System* pThis = (System*)pt;
    
    if(pThis->usExAirFanCtrlPeriod != usExAirFanCtrlPeriod)
    {
        pThis->usExAirFanCtrlPeriod = usExAirFanCtrlPeriod;
#if DEBUG_ENABLE > 0
        myprintf("vSystem_SetExAirFanCtrlPeriod  usExAirFanCtrlPeriod %d  \n", pThis->usExAirFanCtrlPeriod);
#endif 
        vSystem_ExAirFanCtrl(pThis);   
    }
}

/*设定系统湿度阈值*/
void vSystem_SetHumidity(System* pt, uint16_t usHumidityMin, uint16_t usHumidityMax)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usHumidityMin = usHumidityMin;
    pThis->usHumidityMax = usHumidityMax;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->usHumidityMin = usHumidityMin;
        pModularRoof->usHumidityMax = usHumidityMax;
    }
#if DEBUG_ENABLE > 0
    myprintf("vSystem_SetHumidity  usHumidityMin %d  usHumidityMax %d\n", pThis->usHumidityMin, pThis->usHumidityMax);
#endif        
}

/*设定系统目标CO2浓度值*/
void vSystem_SetCO2AdjustThr_V(System* pt, uint16_t usCO2AdjustThr_V)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    
    if(pThis->usCO2AdjustThr_V != usCO2AdjustThr_V)
    {
        pThis->usCO2AdjustThr_V = usCO2AdjustThr_V;
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n]; 
            pModularRoof->usCO2AdjustThr_V = usCO2AdjustThr_V;
        }
#if DEBUG_ENABLE > 0
        myprintf("vSystem_SetCO2AdjustThr_V  usCO2AdjustThr_V %d  \n", pThis->usCO2AdjustThr_V);
#endif  
        vSystem_ExAirFanCtrl(pThis);    
    }
}

/*设定系统CO2浓度偏差*/
void vSystem_SetCO2AdjustDeviat(System* pt, uint16_t usCO2AdjustDeviat)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usCO2AdjustDeviat = usCO2AdjustDeviat;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->usCO2AdjustDeviat = usCO2AdjustDeviat;
    }
#if DEBUG_ENABLE > 0
    myprintf("vSystem_SetCO2AdjustDeviat  usCO2AdjustDeviat %d  \n", pThis->usCO2AdjustDeviat);
#endif   
}

/*系统设备运行状态变化*/
void vSystem_DeviceRunningState(System* pt)
{
    uint8_t  n   = 0;
    OS_ERR   err = OS_ERR_NONE;
    
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    ExAirFan*    pExAirFan    = NULL;
    BMS*         psBMS        = BMS_Core();
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_DeviceRunningState  eSystemState %d  \n", pThis->eSystemState);
#endif  
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->Device.eRunningState == STATE_RUN)  //机组运行
        {  
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

/*注册声光报警启停接口*/
void vSystem_RegistAlarmIO(System* pt, uint8_t ucSwitch_DO)
{
    System* pThis = (System*)pt;  
    pThis->sAlarm_DO.ucChannel = ucSwitch_DO;
}

/*声光报警*/
void vSystem_SetAlarm(System* pt)
{
    System* pThis = (System*)pt; 
    
    if(pThis->xAlarmEnable)  //声光报警使能     
    {
        vDigitalOutputCtrl(pThis->sAlarm_DO.ucChannel, ON); //输出开启,继电器闭合        
#if DEBUG_ENABLE > 0
    myprintf("vSystem_SetAlarm  ucChannel %d xAlarmEnable %d  \n",pThis->sAlarm_DO.ucChannel,  pThis->xAlarmEnable);
#endif   
    }
}

/*消除声光报警*/
void vSystem_DelAlarm(System* pt)
{
    System* pThis = (System*)pt; 
    
    vDigitalOutputCtrl(pThis->sAlarm_DO.ucChannel, OFF);   //输出关闭，继电器断开
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_DelAlarm ucChannel %d \n", pThis->sAlarm_DO.ucChannel);
#endif  
}

/*清除声光报警请求*/
void vSystem_DelAlarmRequst(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    ExAirFan*    pExAirFan    = NULL;
    
    if(pThis->xAlarmEnable == FALSE)
    {
        vSystem_DelAlarm(pThis); //【声光报警使能】关闭。
    }
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 

       //(1)群控控制器与空调机组通讯故障,  (8)空调机组停机保护。声光报警        
        if( (pModularRoof->sMBSlaveDev.xOnLine == FALSE) || (pModularRoof->xStopErrFlag) ) 
        {
            return;
        } 
        //(3)当送风温度大于【送风温度最大值】（默认45℃）,声光报警
        if(pModularRoof->sSupAir_T > pThis->usSupAirMax_T)
        {
            return;            
        }         
    }
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n]; 
        if(pExAirFan->xExAirFanErr == TRUE)
        {
            return;
        }
    }     
    //(2)当室内CO2浓度大于【CO2报警浓度指标值】（默认3000PPM），声光报警
    if( pThis->usCO2PPM >= pThis->usCO2PPMAlarm)  
    {
        return;
    } 
    //(4)同类全部传感器通讯故障,声光报警
    if( (pThis->xCO2SenErr == TRUE) || (pThis->xTempHumiSenOutErr == TRUE) || (pThis->xTempHumiSenInErr == TRUE) ) 
    {
        return;
    }   
    vSystem_DelAlarm(pThis); //不满足报警条件，清除声光报警
}

/*清除声光报警*/
void vSystem_CleanAlarm(System* pt, BOOL* pxAlarmClean)
{
    System* pThis = (System*)pt;

    if(*pxAlarmClean == TRUE)
    {
        vDigitalOutputCtrl(pThis->sAlarm_DO.ucChannel, OFF);   //输出关闭，继电器断开
        *pxAlarmClean = FALSE;
        
#if DEBUG_ENABLE > 0
    myprintf("vSystem_CleanAlarm   pxAlarmClean %d  \n",  *pxAlarmClean);
#endif   
    }
}

/*声光报警使能*/
void vSystem_AlarmEnable(System* pt, BOOL xAlarmEnable)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    ExAirFan*    pExAirFan    = NULL;
    
    pThis->xAlarmEnable = xAlarmEnable;
    if(xAlarmEnable == TRUE)
    {
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n]; 
        
           //(1)群控控制器与空调机组通讯故障,  (8)空调机组停机保护。声光报警        
            if( (pModularRoof->sMBSlaveDev.xOnLine == FALSE) || (pModularRoof->xStopErrFlag) ) 
            {
                vSystem_SetAlarm(pThis);
                return;
            } 
            //(3)当送风温度大于【送风温度最大值】（默认45℃）,声光报警
            if(pModularRoof->sSupAir_T > pThis->usSupAirMax_T)
            {
                vSystem_SetAlarm(pThis);
                return;            
            }         
        }
        for(n=0; n < EX_AIR_FAN_NUM; n++)
        {
            pExAirFan = pThis->psExAirFanList[n]; 
            if(pExAirFan->xExAirFanErr == TRUE)
            {
                vSystem_SetAlarm(pThis);
                return;
            }
        }     
        //(2)当室内CO2浓度大于【CO2报警浓度指标值】（默认3000PPM），声光报警
        if( pThis->usCO2PPM >= pThis->usCO2PPMAlarm)  
        {
            vSystem_SetAlarm(pThis);
            return;
        } 
        //(4)同类全部传感器通讯故障,声光报警
        if( (pThis->xCO2SenErr == TRUE) || (pThis->xTempHumiSenOutErr == TRUE) || (pThis->xTempHumiSenInErr == TRUE) ) 
        {
            vSystem_SetAlarm(pThis);
            return;
        }   
    }
    else
    {
        vSystem_DelAlarm(pThis);
    }
}
