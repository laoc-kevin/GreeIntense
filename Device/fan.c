#include "fan.h"

/*****************************风机*****************************/

void vFan_init(void* pt, eFreqType eFreqType)
{
    Fan* pThis = (Fan*)pt;    
    pThis->eFanFreqType = eFreqType;
}

ABS_CTOR(Fan)  //风机构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init, vFan_init);
END_CTOR


/****************************送风机****************************/
CTOR(SupAirFan)    //送风机构造函数

END_CTOR


/*****************************排风机*****************************/ 

/*注册风机启停接口*/
void vExAirFan_registSwitch_IO(void* pt, uint8_t ucSwitch_DO)
{
    ExAirFan* pThis    = (ExAirFan*)pt;  
    pThis->sSwitch_DO.ucChannel = ucSwitch_DO;
    
}

/*开启风机*/
void vExAirFan_switchOpen(void* pt)    
{
    ExAirFan* pThis = (ExAirFan*)pt;
    vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, ON);  //输出开启,继电器闭合
}

/*关闭风机*/
void vExAirFan_switchClose(void* pt)   
{
    ExAirFan* pThis = (ExAirFan*)pt;
    vDigitalOutputCtrl(pThis->sSwitch_DO.ucChannel, OFF); //输出关闭，继电器断开
}

/*注册风机频率接口*/
void vExAirFan_registFreq_IO(void* pt, uint8_t ucFreq_AO, uint8_t ucFreq_AI, uint16_t usMinFreq, uint16_t usMaxFreq)
{
    ExAirFan* pThis = (ExAirFan*)pt;

    if(pThis->Fan.eFanFreqType == VARIABLE_FREQ)
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

/*设置频率*/
void vExFan_setFreq(void* pt, uint16_t usFreq)   
{
    ExAirFan* pThis = (ExAirFan*)pt;
    
    if(pThis->Fan.eFanFreqType == VARIABLE_FREQ)
    {
        pThis->usRunningFreq = usFreq;
        vAnalogOutputSetRealVal(pThis->sFreq_AO.ucChannel, usFreq);
    }
}

/*设置频率上下限*/
void vExFan_setFreqRange(void* pt, uint16_t usMinFreq, uint16_t usMaxFreq) 
{
    ExAirFan* pThis = (ExAirFan*)pt;
    
    if(pThis->Fan.eFanFreqType == VARIABLE_FREQ)
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

CTOR(ExAirFan)     //排风机构造函数
    SUPER_CTOR(Fan);

    FUNCTION_SETTING(IDevFreq.setFreq,            vExFan_setFreq);
    FUNCTION_SETTING(IDevFreq.registFreq_IO,      vExAirFan_registFreq_IO);
    FUNCTION_SETTING(IDevFreq.setFreqRange,       vExFan_setFreqRange);

    FUNCTION_SETTING(IDevSwitch.registSwitch_IO,  vExAirFan_registSwitch_IO);
    FUNCTION_SETTING(IDevSwitch.switchOpen,       vExAirFan_switchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose,      vExAirFan_switchClose);
END_CTOR


void vExFan_setRunning (void* pt, uint8_t ucFreq)
{
    ExAirFan* f = ExAirFan_new();
    uint16_t freq = 10;
    
   
//    f->Fan.Device.ucAlarmFlag;
}

