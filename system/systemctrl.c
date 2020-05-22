#include "bms.h"
#include "system.h"
#include "systemctrl.h"
#include "md_event.h"
#include "md_modbus.h"
#include "md_timer.h"

/*系统开启*/
void vSystem_SwitchOpen(System* pt)
{
    System* pThis = (System*)pt;

    vSystem_OpenUnits(pThis);
    
    //开启排风机频率调节时间定时器,频率每【排风机频率调节时间】（默认30s）更新一次  
   (void)xTimerRegist(&pThis->sExAirFanFreqAdjustTmr, 0, pThis->usExAirFanFreqAdjustTime, 
                      OS_OPT_TMR_PERIODIC, vSystem_AdjustExAirFanFreq, pThis);    //风机频率调节
   
    pThis->eSwitchCmd = ON; 
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
    
    pThis->eSwitchCmd = OFF; 
}

/*切换系统模式*/
void vSystem_ChangeSystemMode(System* pt, eSystemMode eSystemMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ExAirFan*    pExAirFan    = NULL;
    ModularRoof* pModularRoof = NULL;
    
    if(pThis->eSystemMode == eSystemMode)
    {
        return;
    }
    pThis->eSystemMode = eSystemMode;
    
    //手动模式
    if(pThis->eSystemMode == MODE_MANUAL)
    {
        
    }
    //自动模式
    if(pThis->eSystemMode == MODE_AUTO)
    {
        vSystem_SwitchOpen(pThis);  //开启系统
    }
    //关闭模式
    if(pThis->eSystemMode == MODE_CLOSE)
    {
        vSystem_SwitchClose(pThis);
    }
    //紧急模式
    if(pThis->eSystemMode == MODE_EMERGENCY)
    {
        vSystem_SwitchOpen(pThis);  //开启系统
        vSystem_SetRunningMode(pThis, RUN_MODE_FAN); //开启送风模式
    }
}

/*设定系统目标温度值*/
void vSystem_SetTemp(System* pt, int16_t sTempSet)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    
    pThis->sTempSet = sTempSet;
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->usCoolTempSet = pThis->sTempSet;
        pModularRoof->usHeatTempSet = pThis->sTempSet;
    }
}

/*设定系统目标新风量*/
void vSystem_SetFreAir(System* pt, uint16_t usFreAirSet_Vol)
{
    uint8_t  n, ucUnitNum; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usFreAirSet_Vol = usFreAirSet_Vol;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];        
        pModularRoof->usFreAirSet_Vol = pThis->usFreAirSet_Vol / MODULAR_ROOF_NUM;
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
        pModularRoof->usHumidityMin = pThis->usHumidityMin;
        pModularRoof->usHumidityMax = pThis->usHumidityMax;
    }
}

/*设定系统目标CO2浓度值*/
void vSystem_SetCO2PPM(System* pt, uint16_t usCO2PPMSet)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usCO2PPMSet = usCO2PPMSet;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->usCO2AdjustThr_V = pThis->usCO2PPMSet;
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
        pModularRoof->usCO2AdjustDeviat = pThis->usCO2AdjustDeviat;
    }
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
    }
}

/*清除声光报警*/
void vSystem_DelAlarm(System* pt)
{
    System* pThis = (System*)pt; 
    vDigitalOutputCtrl(pThis->sAlarm_DO.ucChannel, OFF);   //输出关闭，继电器断开
}

/*清除声光报警请求*/
void vSystem_DelAlarmRequst(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    
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
