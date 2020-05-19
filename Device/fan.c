#include "fan.h"


/*************************************************************
*                         送风机                             *
**************************************************************/
CTOR(SupAirFan)    //送风机构造函数

END_CTOR



/*************************************************************
*                         排风机                             *
**************************************************************/
/*注册风机启停接口*/
void vExAirFan_RegistSwitchIO(ExAirFan* pt, uint8_t ucSwitch_DO)
{
    ExAirFan* pThis    = (ExAirFan*)pt;  
    pThis->sSwitch_DO.ucChannel = ucSwitch_DO;
}

/*注册风机频率接口*/
void vExAirFan_RegistFreqIO(ExAirFan* pt, uint8_t ucFreq_AO, uint8_t ucFreq_AI, uint16_t usMinFreq, uint16_t usMaxFreq)
{
    ExAirFan* pThis = (ExAirFan*)pt;

    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
        pThis->usMaxFreq = usMaxFreq;
        pThis->usMinFreq = usMinFreq;
        
        //频率输出
        pThis->sFreq_AO.ucChannel = ucFreq_AO;
        pThis->sFreq_AO.lMax = usMaxFreq;
        pThis->sFreq_AO.lMin = usMinFreq;
        vAnalogOutputRegister(ucFreq_AO, usMinFreq, usMaxFreq);
        
        //频率输入
        pThis->sFreq_AI.ucChannel = ucFreq_AI;
        pThis->sFreq_AI.lMax = usMaxFreq;
        pThis->sFreq_AI.lMin = usMinFreq;
        
        vAnalogInputRegister(ucFreq_AI, usMinFreq, usMaxFreq, &(pThis->usRunningFreq));  
    }   
}

/*开启风机*/
void vExAirFan_SwitchOpen(IDevSwitch* pt)    
{
    ExAirFan* pThis = SUB_PTR(pt, IDevSwitch, ExAirFan);
    
    if(pThis->Device.eRunningState == RUN_STATE_STOP)
    {
        vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, ON);  //输出开启,继电器闭合
    } 
}

/*关闭风机*/
void vExAirFan_SwitchClose(IDevSwitch* pt)   
{
    ExAirFan* pThis = SUB_PTR(pt, IDevSwitch, ExAirFan);
    
    if(pThis->Device.eRunningState == RUN_STATE_RUN)
    {
        vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, OFF); //输出关闭，继电器断开
    }
}


/*设置频率*/
void vExFan_SetFreq(IDevFreq* pt, uint16_t usFreq)   
{
    ExAirFan* pThis = SUB_PTR(pt, IDevFreq, ExAirFan);
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
        if(usFreq < pThis->usMinFreq)
        {
            usFreq = pThis->usMinFreq;
        }
        if(usFreq > pThis->usMaxFreq)
        {
            usFreq = pThis->usMaxFreq;
        }
        pThis->usRunningFreq = usFreq;
        vAnalogOutputSetRealVal(pThis->sFreq_AO.ucChannel, usFreq);
    }
}

/*设置频率上下限*/
void vExFan_SetFreqRange(IDevFreq* pt, uint16_t usMinFreq, uint16_t usMaxFreq) 
{
    ExAirFan* pThis = SUB_PTR(pt, IDevFreq, ExAirFan);
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)
    {
        pThis->usMaxFreq = usMaxFreq;
        pThis->usMinFreq = usMinFreq;
    
        pThis->sFreq_AO.lMax = usMaxFreq;
        pThis->sFreq_AO.lMin = usMinFreq;
        
        pThis->sFreq_AI.lMax = usMaxFreq;
        pThis->sFreq_AI.lMin = usMinFreq;
        
        vAnalogOutputSetRange(pThis->sFreq_AO.ucChannel, usMinFreq, usMaxFreq);
    }
}

/*排风机初始化*/
void vExAirFan_Init(ExAirFan* pt, sFanInfo* psFan)
{
    ExAirFan* pThis     = (ExAirFan*)pt;
    pThis->eFanFreqType = psFan->eFanFreqType;  
    
    vExAirFan_RegistSwitchIO(pThis, psFan->ucSwitch_DO);  //启停接口
    
    if(pThis->eFanFreqType == VARIABLE_FREQ)  //变频接口
    {
        vExAirFan_RegistFreqIO(pThis, psFan->ucFreq_AO, psFan->ucFreq_AI, psFan->usMinFreq, psFan->usMaxFreq);
    } 
}


CTOR(ExAirFan)     //排风机构造函数
    SUPER_CTOR(Device);
   
    FUNCTION_SETTING(init, vExAirFan_Init);

    FUNCTION_SETTING(IDevFreq.setFreq,       vExFan_SetFreq);
    FUNCTION_SETTING(IDevFreq.setFreqRange,  vExFan_SetFreqRange);

    FUNCTION_SETTING(IDevSwitch.switchOpen,  vExAirFan_SwitchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose, vExAirFan_SwitchClose);
END_CTOR


//void vExFan_setRunning (void* pt, uint8_t ucFreq)
//{
//    ExAirFan* f = ExAirFan_new();
//    uint16_t freq = 10;
//    
//   
////    f->Fan.Device.ucAlarmFlag;
//}

