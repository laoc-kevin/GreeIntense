#include "fan.h"
#include "md_timer.h"

#define EX_AIR_FAN_TIME_OUT_S       2
#define EX_AIR_FAN_ERROR_DELAY_S    10

/*************************************************************
*                         室外风机                             *
**************************************************************/
CTOR(AmbientOutFan)    //室外风机构造函数
    SUPER_CTOR(Device);
END_CTOR


/*************************************************************
*                         送风机                             *
**************************************************************/
CTOR(SupAirFan)    //送风机构造函数
    SUPER_CTOR(Device);
END_CTOR

/*************************************************************
*                         排风机                             *
**************************************************************/
/*注册风机数字量接口*/
void vExAirFan_RegistDigitalIO(ExAirFan* pt, uint8_t ucSwitch_DO, uint8_t ucRunState_DI, uint8_t ucErr_DI)
{
    ExAirFan* pThis    = (ExAirFan*)pt;  
    pThis->sSwitch_DO.ucChannel = ucSwitch_DO;
    pThis->sRunState_DI.ucChannel = ucRunState_DI;
    pThis->sErr_DI.ucChannel = ucErr_DI;
    
    vDigitalInputRegist(ucRunState_DI, (void*)&pThis->Device.eRunningState);
    vDigitalInputRegist(ucErr_DI, (void*)&pThis->xExAirFanErr);
    
}

/*注册风机模拟量接口*/
void vExAirFan_RegistAnalogIO(ExAirFan* pt, uint8_t ucFreq_AO, uint8_t ucFreq_AI, uint16_t usMinFreq, uint16_t usMaxFreq)
{
    ExAirFan* pThis = (ExAirFan*)pt;

    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
        pThis->usMaxFreq = usMaxFreq;
        pThis->usMinFreq = usMinFreq;
        
        //频率输出
        pThis->sFreq_AO.ucChannel = ucFreq_AO;
        pThis->sFreq_AO.lMax = (int32_t)usMaxFreq;
        pThis->sFreq_AO.lMin = (int32_t)usMinFreq;
        vAnalogOutputRegist(ucFreq_AO, usMinFreq, usMaxFreq);
        
        //频率输入
        pThis->sFreq_AI.ucChannel = ucFreq_AI;
        pThis->sFreq_AI.lMax = (int32_t)usMaxFreq;
        pThis->sFreq_AI.lMin = (int32_t)usMinFreq;
        
        vAnalogInputRegist(ucFreq_AI, pThis->sFreq_AI.lMin, pThis->sFreq_AI.lMax, (void*)&pThis->usRunningFreq); 
        vAnalogOutputRegist(ucFreq_AO, pThis->sFreq_AO.lMin, pThis->sFreq_AO.lMax);         
    }   
}

/*开启风机*/
void vExAirFan_SwitchOpen(IDevSwitch* pt)    
{
    OS_ERR err = OS_ERR_NONE;
    ExAirFan* pThis = SUB_PTR(pt, IDevSwitch, ExAirFan);
    
    if(pThis->xExAirFanErr == FALSE)  //无故障
    {
        vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, ON);  //输出开启,继电器闭合
        pThis->eSwitchCmd = ON;
#if DEBUG_ENABLE > 0 
        if(pThis->Device.eRunningState == STATE_STOP)
        {
//            pThis->Device.eRunningState = STATE_RUN;
            myprintf("vExAirFan_SwitchOpen  eRunningState %d ucDevIndex %d\n", pThis->Device.eRunningState, pThis->Device.ucDevIndex);
        }            
#endif
    } 
}

/*关闭风机*/
void vExAirFan_SwitchClose(IDevSwitch* pt)   
{
    ExAirFan* pThis = SUB_PTR(pt, IDevSwitch, ExAirFan);
    
    vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, OFF); //输出关闭，继电器断开
    pThis->eSwitchCmd = OFF;
    
#if DEBUG_ENABLE > 0 
    if(pThis->Device.eRunningState == STATE_RUN)
    {
//        pThis->Device.eRunningState = STATE_STOP;
        myprintf("vExAirFan_SwitchClose  eRunningState %d ucDevIndex %d \n", pThis->Device.eRunningState, pThis->Device.ucDevIndex); 
    }            
#endif
}

/*设置频率*/
void vExFan_SetFreq(IDevFreq* pt, uint16_t usFreq)   
{
    ExAirFan* pThis = SUB_PTR(pt, IDevFreq, ExAirFan);
    
    if(pThis->eFanFreqType == VARIABLE_FREQ && pThis->xExAirFanErr == FALSE)
    {
        if(usFreq < pThis->usMinFreq)
        {
            usFreq = pThis->usMinFreq;
        }
        if(usFreq > pThis->usMaxFreq)
        {
            usFreq = pThis->usMaxFreq;
        }
        vAnalogOutputSetRealVal(pThis->sFreq_AO.ucChannel, usFreq);
    }
#if DEBUG_ENABLE > 0 
    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
//         pThis->usRunningFreq = usFreq;
         myprintf("vExFan_SetFreq usRunningFreq %d  usFreq %d\n", pThis->usRunningFreq, usFreq);  
    } 
#endif    
}

/*设置频率上下限*/
void vExFan_SetFreqRange(IDevFreq* pt, uint16_t usMinFreq, uint16_t usMaxFreq) 
{
    uint16_t usFreq = 0;
    ExAirFan* pThis = SUB_PTR(pt, IDevFreq, ExAirFan);
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
        pThis->usMaxFreq = usMaxFreq;
        pThis->usMinFreq = usMinFreq;
    
        pThis->sFreq_AO.lMax = (int32_t)usMaxFreq;
        pThis->sFreq_AO.lMin = (int32_t)usMinFreq;
        
        pThis->sFreq_AI.lMax = (int32_t)usMaxFreq;
        pThis->sFreq_AI.lMin = (int32_t)usMinFreq;
        
        vAnalogOutputSetRange(pThis->sFreq_AO.ucChannel, pThis->sFreq_AO.lMin, pThis->sFreq_AO.lMax);
        vAnalogInputSetRange(pThis->sFreq_AI.ucChannel, pThis->sFreq_AI.lMin, pThis->sFreq_AI.lMax);
        
        if(pThis->usRunningFreq < pThis->usMinFreq)
        {
            usFreq = pThis->usMinFreq;
        }
        if(pThis->usRunningFreq > pThis->usMaxFreq)
        {
            usFreq = pThis->usMaxFreq;
        }
        vExFan_SetFreq(SUPER_PTR(pThis, IDevFreq), usFreq);
    }
#if DEBUG_ENABLE > 0 
    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
        myprintf("vExFan_SetFreqRange  usMinFreq %d usMaxFreq %d \n", pThis->usMinFreq, pThis->usMaxFreq);
    } 
#endif        
}

void vExAirFan_TimeoutInd(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
    ExAirFan* pThis = (ExAirFan*)p_arg;
    
    if(pThis->eSwitchCmd == ON && pThis->Device.eRunningState == STATE_STOP)
    {
        vExAirFan_SwitchOpen(SUPER_PTR(pThis, IDevSwitch));  //开启排风机

    }
    if(pThis->eSwitchCmd == OFF && pThis->Device.eRunningState == STATE_RUN)
    {
        vExAirFan_SwitchClose(SUPER_PTR(pThis, IDevSwitch));  //关闭排风机

    }
    if(pThis->xExAirFanErr)
    {
        pThis->eSwitchCmd = OFF;
        vExAirFan_SwitchClose(SUPER_PTR(pThis, IDevSwitch));  //关闭排风机
    }

#if DEBUG_ENABLE > 0 
    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
//        myprintf("vExAirFan_TimeoutInd ucDevIndex %d usRunningFreq %d usMinFreq %d usMaxFreq %d \n",
//        pThis->Device.ucDevIndex, pThis->usRunningFreq, pThis->usMinFreq, pThis->usMaxFreq); 
    } 
#endif      
}

/*排风机数据监控*/
void vExAirFan_RegistMonitor(ExAirFan* pt)
{
    OS_ERR err = OS_ERR_NONE;
    ExAirFan* pThis = (ExAirFan*)pt;

    OSSemCreate( &(pThis->sValChange), "sValChange", 0, &err );  //事件消息量初始化
    
    MONITOR(&pThis->xExAirFanErr,         uint8, &pThis->sValChange)
    MONITOR(&pThis->Device.eRunningState, uint8, &pThis->sValChange)
}

/*排风机EEPROM数据注册*/
void vExAirFan_RegistEEPROMData(ExAirFan* pt)
{
    ExAirFan* pThis = (ExAirFan*)pt;
    EEPROM_DATA(TYPE_RUNTIME, pThis->Device.ulRunTime_S)
    EEPROM_DATA(TYPE_RUNTIME, pThis->Device.usRunTime_H)
}

/*排风机初始化*/
void vExAirFan_Init(ExAirFan* pt, const sFanInfo* psFan, uint8_t ucDevIndex)
{
    ExAirFan* pThis = (ExAirFan*)pt;
    
    pThis->eFanFreqType      = psFan->eFanFreqType;  
    pThis->Device.ucDevIndex = ucDevIndex;
    
    vExAirFan_RegistDigitalIO(pThis, psFan->ucSwitch_DO, psFan->ucRunState_DI, psFan->ucErr_DI);  //数字接口
    vExAirFan_RegistEEPROMData(pThis);        //EEPROM数据注册
    vExAirFan_RegistMonitor(pThis);           //注册监控数据
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)  //变频接口
    {
        vExAirFan_RegistAnalogIO(pThis, psFan->ucFreq_AO, psFan->ucFreq_AI, psFan->usMinFreq, psFan->usMaxFreq);
    } 
    
    //排风机2s周期定时器
    (void)xTimerRegist(&pThis->sExAirFanTmr, 0, EX_AIR_FAN_TIME_OUT_S, OS_OPT_TMR_PERIODIC, vExAirFan_TimeoutInd, pThis, FALSE); 
}

/*排风机类型切换*/
void vExAirFan_ChangeFreqType(ExAirFan* pt, const sFanInfo* psFan)
{
    ExAirFan* pThis     = (ExAirFan*)pt;
    pThis->eFanFreqType = psFan->eFanFreqType;

    vExAirFan_RegistDigitalIO(pThis, psFan->ucSwitch_DO, psFan->ucRunState_DI, psFan->ucErr_DI);  //数字接口
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)  //变频接口
    {
        vExAirFan_RegistAnalogIO(pThis, psFan->ucFreq_AO, psFan->ucFreq_AI, psFan->usMinFreq, psFan->usMaxFreq);
//        pThis->IDevFreq.setFreq(SUPER_PTR(pThis, IDevFreq), 100);
    }     
}

CTOR(ExAirFan)     //排风机构造函数
    SUPER_CTOR(Device);
   
    FUNCTION_SETTING(init, vExAirFan_Init);
    FUNCTION_SETTING(changeFreqType, vExAirFan_ChangeFreqType);

    FUNCTION_SETTING(IDevFreq.setFreq,       vExFan_SetFreq);
    FUNCTION_SETTING(IDevFreq.setFreqRange,  vExFan_SetFreqRange);

    FUNCTION_SETTING(IDevSwitch.switchOpen,  vExAirFan_SwitchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose, vExAirFan_SwitchClose);
END_CTOR


