#include "fan.h"
#include "md_timer.h"

#define EX_AIR_FAN_TIME_OUT_S               2
#define EX_AIR_FAN_TIME_DELAY_S             5
#define EX_AIR_FAN_CTRL_ERR_TIME_DELAY_S    10

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
void vExAirFan_RegistDigitalIO(ExAirFan* pt, uint8_t ucSwitch_DO, uint8_t ucRunState_DI, uint8_t ucErr_DI, uint8_t ucRemote_DI)
{
    ExAirFan* pThis    = (ExAirFan*)pt;  
    
    pThis->sSwitch_DO.ucChannel   = ucSwitch_DO;
    pThis->sRunState_DI.ucChannel = ucRunState_DI;
    pThis->sRemote_DI.ucChannel   = ucRemote_DI;
    pThis->sErr_DI.ucChannel      = ucErr_DI;
    
    vDigitalInputRegist(ucRunState_DI, (void*)&pThis->Device.eRunningState);
    vDigitalInputRegist(ucErr_DI,      (void*)&pThis->xExAirFanErr);
    vDigitalInputRegist(ucRemote_DI,   (void*)&pThis->xExAirFanRemote);
    
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
        
        //频率输入
        pThis->sFreq_AI.ucChannel = ucFreq_AI;
        pThis->sFreq_AI.lMax = (int32_t)usMaxFreq;
        pThis->sFreq_AI.lMin = (int32_t)usMinFreq;
        
        vAnalogInputRegist(ucFreq_AI, pThis->sFreq_AI.lMin, pThis->sFreq_AI.lMax, (void*)&pThis->usRunningFreq); 
        vAnalogOutputRegist(ucFreq_AO, pThis->sFreq_AO.lMin, pThis->sFreq_AO.lMax);         
    }   
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
        pThis->usSetFreq = usFreq;
        
        if(pThis->eCtrlCmd == ON && pThis->usRunningFreq != usFreq)
        {
            vAnalogOutputSetRealVal(pThis->sFreq_AO.ucChannel, usFreq);
#if DEBUG_ENABLE > 0 
        myprintf("vExFan_SetFreq usRunningFreq %d  usSetFreq %d\n", pThis->usRunningFreq, pThis->usSetFreq);  
#endif 
        }

    }
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
        
//        vAnalogOutputSetRange(pThis->sFreq_AO.ucChannel, pThis->sFreq_AO.lMin, pThis->sFreq_AO.lMax);
//        vAnalogInputSetRange(pThis->sFreq_AI.ucChannel, pThis->sFreq_AI.lMin, pThis->sFreq_AI.lMax);
        
        if( pThis->usSetFreq < pThis->usMinFreq)
        {
            usFreq = pThis->usMinFreq;
            vExFan_SetFreq(SUPER_PTR(pThis, IDevFreq), usFreq);
        }
        if(pThis->usSetFreq > pThis->usMaxFreq)
        {
            usFreq = pThis->usMaxFreq;
            vExFan_SetFreq(SUPER_PTR(pThis, IDevFreq), usFreq);
        }    
#if DEBUG_ENABLE > 0 
//        myprintf("vExFan_SetFreqRange sFreq_AO.lMin %ld; usMinFreq %d usMaxFreq %d usSetFreq %d \n", 
//		pThis->sFreq_AO.lMin, pThis->usMinFreq, pThis->usMaxFreq, pThis->usSetFreq);
#endif 
    }   
}

/*开启风机*/
void vExAirFan_SwitchOpen(IDevSwitch* pt)    
{
    OS_ERR err = OS_ERR_NONE;
    ExAirFan* pThis = SUB_PTR(pt, IDevSwitch, ExAirFan);
    
    if(pThis->xExAirFanErr == FALSE && pThis->xExAirFanRemote == TRUE)  //无故障且为远程
    {
        vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, ON);  //输出开启,继电器闭合
        pThis->eCtrlCmd = ON;
 
        if(pThis->Device.eRunningState == STATE_STOP)
        {
            if(pThis->eFanFreqType == VARIABLE_FREQ)
            {
                vExFan_SetFreq(SUPER_PTR(pThis, IDevFreq), pThis->usSetFreq); 
            }
        } 
#if DEBUG_ENABLE > 0
//        myprintf("vExAirFan_SwitchOpen  eRunningState %d ucDevIndex %d usSetFreq %d\n", 
//        pThis->Device.eRunningState, pThis->Device.ucDevIndex, pThis->usSetFreq);
#endif        
    } 
}

/*关闭风机*/
void vExAirFan_SwitchClose(IDevSwitch* pt)   
{
    ExAirFan* pThis = SUB_PTR(pt, IDevSwitch, ExAirFan);
    
    vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, OFF); //输出关闭，继电器断开
    pThis->eCtrlCmd = OFF;
    
#if DEBUG_ENABLE > 0 
//    if(pThis->Device.eRunningState == STATE_RUN)
//    {
//        myprintf("vExAirFan_SwitchClose  eRunningState %d ucDevIndex %d \n", pThis->Device.eRunningState, pThis->Device.ucDevIndex); 
//    }            
#endif
}

void vExAirFan_TimeoutInd(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
    ExAirFan* pThis = (ExAirFan*)p_arg;
    
    if(pThis->eCtrlCmd == ON && pThis->Device.eRunningState == STATE_STOP && pThis->xExAirFanErr == FALSE && pThis->xExAirFanRemote == TRUE)
    {
        pThis->xExAirFanCtrl = TRUE;
        vExAirFan_SwitchOpen(SUPER_PTR(pThis, IDevSwitch));  //开启排风机
    }
    if(pThis->eCtrlCmd == OFF && pThis->Device.eRunningState == STATE_RUN)
    {
        pThis->xExAirFanCtrl = TRUE;
        vExAirFan_SwitchClose(SUPER_PTR(pThis, IDevSwitch));  //关闭排风机
    }
    if(pThis->xExAirFanCtrl == TRUE && pThis->xExAirFanRemote == TRUE)  
    {
        if( (pThis->eCtrlCmd == OFF && pThis->Device.eRunningState == STATE_RUN) ||   //风机状态与控制命令不一致且处于远程状态则计时
            (pThis->eCtrlCmd == ON && pThis->Device.eRunningState == STATE_STOP) )
        {
            pThis->ucTimeCount = pThis->ucTimeCount + EX_AIR_FAN_TIME_OUT_S;
        }
        if((pThis->eCtrlCmd == OFF && pThis->Device.eRunningState == STATE_STOP) ||   //风机状态与控制命令一致则停止计时
           (pThis->eCtrlCmd == ON && pThis->Device.eRunningState == STATE_RUN) )
        {
            pThis->xExAirFanCtrl = FALSE;
            pThis->ucTimeCount = 0;
        }
    }
    if(pThis->ucTimeCount >= EX_AIR_FAN_CTRL_ERR_TIME_DELAY_S && pThis->xExAirFanCtrl == TRUE && pThis->xExAirFanRemote == TRUE)  //控制故障
    {
        pThis->ucTimeCount      = 0;
        pThis->xExAirFanCtrlErr = TRUE;
        pThis->xExAirFanCtrl    = FALSE;
    }
    
    if(pThis->xExAirFanErr == TRUE || pThis->xExAirFanRemote == FALSE || pThis->xExAirFanCtrlErr == TRUE)  //故障、本地、控制故障
    {
        pThis->eCtrlCmd = OFF;
        pThis->xExAirFanCtrl = FALSE;
        if(pThis->eCtrlCmd == OFF && pThis->Device.eRunningState == STATE_RUN)
        {
            vExAirFan_SwitchClose(SUPER_PTR(pThis, IDevSwitch));  //关闭排风机
        }
    }
    if(pThis->eFanFreqType == VARIABLE_FREQ && pThis->xExAirFanErr == FALSE && pThis->eCtrlCmd == ON && pThis->Device.eRunningState == STATE_RUN) 
    {
        if( (pThis->usSetFreq > pThis->usRunningFreq && pThis->usSetFreq-pThis->usRunningFreq > 10) || 
            (pThis->usSetFreq < pThis->usRunningFreq && pThis->usRunningFreq - pThis->usSetFreq > 10) )
        {
            vExFan_SetFreq(SUPER_PTR(pThis, IDevFreq), pThis->usSetFreq);   //频率控制
        }
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
//    OS_ERR err = OS_ERR_NONE;
//    ExAirFan* pThis = (ExAirFan*)pt;

//    OSSemCreate( &(pThis->sValChange), "sValChange", 0, &err );  //事件消息量初始化
//    
//    MONITOR(&pThis->xExAirFanErr,         uint8, &pThis->sValChange)
//    MONITOR(&pThis->xExAirFanRemote,      uint8, &pThis->sValChange)
//    MONITOR(&pThis->Device.eRunningState, uint8, &pThis->sValChange)
}

/*排风机EEPROM数据注册*/
void vExAirFan_RegistEEPROMData(ExAirFan* pt)
{
    ExAirFan* pThis = (ExAirFan*)pt;
    EEPROM_DATA(TYPE_RUNTIME, pThis->Device.ulRunTime_S)
//    EEPROM_DATA(TYPE_RUNTIME, pThis->Device.usRunTime_H)
}

/*排风机初始化*/
void vExAirFan_Init(ExAirFan* pt, const sFanInfo* psFan, uint8_t ucDevIndex)
{
    ExAirFan* pThis = (ExAirFan*)pt;
    
    pThis->eFanFreqType      = psFan->eFanFreqType;  
    pThis->Device.ucDevIndex = ucDevIndex;
    
    vExAirFan_RegistDigitalIO(pThis, psFan->ucSwitch_DO, psFan->ucRunState_DI, psFan->ucErr_DI, psFan->ucRemote_DI);  //数字接口
    vExAirFan_RegistEEPROMData(pThis);        //EEPROM数据注册
    vExAirFan_RegistMonitor(pThis);           //注册监控数据
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)  //变频接口
    {
        vExAirFan_RegistAnalogIO(pThis, psFan->ucFreq_AO, psFan->ucFreq_AI, psFan->usMinFreq, psFan->usMaxFreq);
    } 
    //排风机周期定时器
    (void)xTimerRegist(&pThis->sExAirFanTmr, EX_AIR_FAN_TIME_DELAY_S, EX_AIR_FAN_TIME_OUT_S, OS_OPT_TMR_PERIODIC, vExAirFan_TimeoutInd, pThis, FALSE); 
    
#if DEBUG_ENABLE > 0 
//    myprintf("vExAirFan_Init  ucDevIndex %d \n", pThis->Device.ucDevIndex);
#endif  
}

/*排风机类型切换*/
void vExAirFan_ChangeFreqType(ExAirFan* pt, const sFanInfo* psFan)
{
    ExAirFan* pThis     = (ExAirFan*)pt;
    pThis->eFanFreqType = psFan->eFanFreqType;

    vExAirFan_RegistDigitalIO(pThis, psFan->ucSwitch_DO, psFan->ucRunState_DI, psFan->ucErr_DI, psFan->ucRemote_DI);  //数字接口
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)  //变频接口
    {
        vExAirFan_RegistAnalogIO(pThis, psFan->ucFreq_AO, psFan->ucFreq_AI, psFan->usMinFreq, psFan->usMaxFreq);
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


