#include "system.h"
#include "systemctrl.h"
#include "md_timer.h"

/*系统关闭所有排风机*/
void vSystem_CloseExAirFans(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ExAirFan*    pExAirFan    = NULL;
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch)); //关闭所有排风机
    }
}

/*系统当前正在运行的时间最长定频排风机*/
ExAirFan* psSystem_LongestExAirFan(System* pt)
{
    uint8_t   i, n, m;
    System*   pThis = (System*)pt;
    
    ExAirFan* pExAirFan = NULL;   
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM] = {NULL,};  
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        
        if( (pExAirFan->eFanFreqType != VARIABLE_FREQ) && (pExAirFan->Device.eRunningState == STATE_RUN))  //所有运行的定频排风机
        {
            pFanByRunTimeList[m] = pThis->psExAirFanList[n];   
            m++;
        }       
    }
    //所有运行的定频排风机已运行时间排序
    for(n=0; n < m; n++)  
    {          
        for(i=0; i< m-n; i++)
        {
            if(pFanByRunTimeList[i]->Device.ulRunTime_S > pFanByRunTimeList[i+1]->Device.ulRunTime_S)
            {
                           pExAirFan   = pFanByRunTimeList[i];
                pFanByRunTimeList[i]   = pFanByRunTimeList[i+1]; 
                pFanByRunTimeList[i+1] = pExAirFan;
            }
        }
    }
    return  pFanByRunTimeList[m-1];
}

/*系统未运行的时间最短定频排风机*/
ExAirFan* psSystem_ShortestExAirFan(System* pt)
{
    uint8_t   i, n, m;
    System*   pThis = (System*)pt;
    
    ExAirFan* pExAirFan = NULL;   
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM] = {NULL,};  
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        
        if( (pExAirFan->eFanFreqType != VARIABLE_FREQ) && (pExAirFan->Device.eRunningState == STATE_STOP))  //所有未运行的定频排风机
        {
            pFanByRunTimeList[m] = pThis->psExAirFanList[n];   
            m++;
        }      
    }
    //所有未运行的定频排风机已运行时间排序
    for(n=0; n < m; n++)  
    {          
        for(i=0; i< m-n; i++)
        {
            if(pFanByRunTimeList[i]->Device.ulRunTime_S > pFanByRunTimeList[i+1]->Device.ulRunTime_S)
            {
                           pExAirFan   = pFanByRunTimeList[i];
                pFanByRunTimeList[i]   = pFanByRunTimeList[i+1]; 
                pFanByRunTimeList[i+1] = pExAirFan;
            }
        }
    }
    return pFanByRunTimeList[0];
}

/*系统排风风机运行需求时间定时器*/
void vSystem_ExAirFanRequestTimeTmrCallback(void* p_tmr, void* p_arg)
{
    System*   pThis = (System*)p_arg;
    
    ExAirFan* pExAirFan = psSystem_LongestExAirFan(pThis);
    pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch));
}

/*系统排风机控制周期定时器*/
void vSystem_ExAirFanCtrlTmrCallback(void* p_tmr, void* p_arg)
{
    System*   pThis = (System*)p_arg;
    ExAirFan* pExAirFan = NULL;
    
    uint8_t   ucExAirFanRequstNum = pThis->ucExAirFanRequstNum;
    uint16_t  usExAirRequest_Vol      = pThis->usExAirRequest_Vol;
    uint16_t  ulExAirFanRated_Vol = pThis->ulExAirFanRated_Vol;
    uint16_t  usExAirFanMinFreq   = pThis->usExAirFanMinFreq;
    uint16_t  usExAirFanCtrlPeriod  = pThis->usExAirFanCtrlPeriod;
    
    /********若【排风机类型】为变频+定频，且变频风机无故障*********/
    if(pThis->eExAirFanType == Type_CONSTANT_VARIABLE && pThis->pExAirFanVariate->xExAirFanErr == FALSE)
    {
        //需求时间t=（（系统排风需求量-【排风机额定风量】*【风机开启需求数】）/【排风机额定风量】）*【排风机控制周期】；
        pThis->usExAirFanRequestTime =  (usExAirRequest_Vol - ulExAirFanRated_Vol*ucExAirFanRequstNum) * 500 / 
                                        (usExAirFanMinFreq*ulExAirFanRated_Vol) * usExAirFanCtrlPeriod;
        
        //若t>=【排风机最小运行时间】（默认300s）则最小频率开启变频排风机；否则不开启。
        if(pThis->usExAirFanRequestTime >= pThis->usExAirFanRunTimeLeast)
        {
            pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
            vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMinFreq);    //最小频率
            
            //开启排风机运行定时器
            (void)xTimerRegist(&pThis->sExAirFanRequestTimeTmr, pThis->usExAirFanRequestTime, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_ExAirFanRequestTimeTmrCallback, pThis);
        }
    }
    
    /******若【排风机类型】为全定频，或者变频风机故障******/
    if(pThis->eExAirFanType == Type_CONSTANT || pThis->pExAirFanVariate->xExAirFanErr == TRUE)
    {
        //需求时间t=（（系统排风需求量-【排风机额定风量】*【风机开启需求数】）/【排风机额定风量】）*【排风机控制周期】；
        pThis->usExAirFanRequestTime =  (usExAirRequest_Vol - ulExAirFanRated_Vol*ucExAirFanRequstNum) / 
                                         ulExAirFanRated_Vol * usExAirFanCtrlPeriod;
        
        //若t>=【排风机最小运行时间】则开启1台定频排风机；否则不开启
        if(pThis->usExAirFanRequestTime >= pThis->usExAirFanRunTimeLeast)
        {
            pExAirFan = psSystem_ShortestExAirFan(pThis);
            pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
            
            //开启排风机运行定时器
            (void)xTimerRegist(&pThis->sExAirFanRequestTimeTmr, pThis->usExAirFanRequestTime, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_ExAirFanRequestTimeTmrCallback, pThis);
        }
    }
}

/*系统启停定频排风风机*/
void vSystem_ExAirFanConstant(System* pt)
{
    uint8_t   i, n, m, ucRunningNum;
    System*   pThis = (System*)pt;

    ExAirFan* pExAirFan        = NULL;   
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM] = {NULL,};  
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)  //所有定频排风机
    {
        pExAirFan = pThis->psExAirFanList[n];
        if(pExAirFan->eFanFreqType != VARIABLE_FREQ)
        {
            pFanByRunTimeList[m] = pThis->psExAirFanList[n];   
            m++;
        }
        if(pExAirFan->Device.eRunningState == STATE_RUN)
        {
            ucRunningNum++;    //当前运行的定频排风机个数   
        }                
    }
    //定频排风机已运行时间排序
    for(n=0; n < m; n++)  
    {          
        for(i=0; i< m-n; i++)
        {
            if(pFanByRunTimeList[i]->Device.ulRunTime_S > pFanByRunTimeList[i+1]->Device.ulRunTime_S)
            {
                           pExAirFan   = pFanByRunTimeList[i];
                pFanByRunTimeList[i]   = pFanByRunTimeList[i+1]; 
                pFanByRunTimeList[i+1] = pExAirFan;
            }
        }
    }
    //系统需求排风机个数 < 运行的定频排风机个数  
    if(pThis->ucExAirFanRequstNum < ucRunningNum)
    {
        for(i=0, n = ucRunningNum-pThis->ucExAirFanRequstNum; n!=0 && i<m; i++)
        {
            pExAirFan = pFanByRunTimeList[m-i-1];   //运行时间最长的排风机
            if(pExAirFan->Device.eRunningState == STATE_RUN)  //是否在运行
            {
                pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch)); //关闭运行时间最长的排风机
                n--;
            }
        }
    }
    //系统需求排风机个数 > 运行的定频排风机个数
    if(pThis->ucExAirFanRequstNum > ucRunningNum)
    {
        for(i=0, n = pThis->ucExAirFanRequstNum-ucRunningNum; n!=0 && i<m; i++)
        {
            pExAirFan = pFanByRunTimeList[i];   //运行时间最短的排风机
            if(pExAirFan->Device.eRunningState == STATE_STOP)  //是否未运行
            {
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
                n--;
            }
        }
    }
}

/*系统全定频排风风机控制*/
void vSystem_ExAirFanConstantCtrl(System* pt)
{
    System*   pThis = (System*)pt;
    
    if(pThis->eExAirFanType != Type_CONSTANT)
    {
        return;
    }
    vSystem_ExAirFanConstant(pThis);
    
    //每个【排风机控制周期】（默认1800s）周期执行一次以下A、B、C逻辑
    (void)xTimerRegist(&pThis->sExAirFanCtrlTmr, 0, pThis->usExAirFanCtrlPeriod,  
                       OS_OPT_TMR_PERIODIC, vSystem_ExAirFanCtrlTmrCallback, pThis);
}

/*系统排风风机频率调节*/
void vSystem_AdjustExAirFanFreq(System* pt, uint16_t usFreq)
{
    System*   pThis = (System*)pt;

    ExAirFan* pExAirFanVariate = pThis->pExAirFanVariate;    //变频风机
    if(pExAirFanVariate !=NULL)
    {
        pExAirFanVariate->IDevFreq.setFreq(SUPER_PTR(pExAirFanVariate, IDevFreq), usFreq);  //设置频率 
    } 
}

/*系统定频+变频排风风机控制*/
void vSystem_ExAirFanBothCtrl(System* pt)
{
    uint16_t  usFreq = 0;
    System*   pThis = (System*)pt;
    ExAirFan* pExAirFan = pThis->pExAirFanVariate;
    
    uint8_t   ucExAirFanRequstNum = pThis->ucExAirFanRequstNum;
    uint16_t  usExAirRequest_Vol      = pThis->usExAirRequest_Vol;
    uint16_t  ulExAirFanRated_Vol = pThis->ulExAirFanRated_Vol;
    uint16_t  usExAirFanMinFreq   = pThis->usExAirFanMinFreq;
    uint16_t  usExAirFanCtrlPeriod  = pThis->usExAirFanCtrlPeriod;
    
    if(pThis->eExAirFanType != Type_CONSTANT_VARIABLE)
    {
        return;
    }
    vSystem_ExAirFanConstant(pThis);
    
    /*当系统排风需求量-【排风机额定风量】*【风机开启需求数】>=【排风机额定风量】*【排风机最小频率】/50，
      只开启变频排风机*/
    if( (usExAirRequest_Vol - ulExAirFanRated_Vol*ucExAirFanRequstNum) >= (usExAirFanMinFreq*ulExAirFanRated_Vol/500) )
    { 
        pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启变频排风机
        
        //排风机频率=（（系统排风需求量-【排风机额定风量】*【风机开启需求数】）/【排风机额定风量】)*50；
        usFreq = ( usExAirRequest_Vol - ulExAirFanRated_Vol*ucExAirFanRequstNum ) / ulExAirFanRated_Vol * 500;
        vSystem_AdjustExAirFanFreq(pThis, usFreq); //设定频率
    }
    else
    {
        //否则，每个【排风机控制周期】（默认1800s）周期执行一次以下A、B、C逻辑：
        (void)xTimerRegist(&pThis->sExAirFanCtrlTmr, 0, pThis->usExAirFanCtrlPeriod,  
                           OS_OPT_TMR_PERIODIC, vSystem_ExAirFanCtrlTmrCallback, pThis);
    }
}

/*系统排风风机控制*/
void vSystem_ExAirFanCtrl(System* pt)
{
    uint8_t   i, n, m, ucRunningNum;
    System*   pThis = (System*)pt;
    
    ExAirFan* pExAirFan        = NULL;   
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM - 1] = {NULL,};  
    
    //手动模式下，手动开关风机
    if(pThis->eSystemMode == MODE_MANUAL)
    {
        for(n=0; n < EX_AIR_FAN_NUM; n++)  
        {
            pExAirFan = pThis->psExAirFanList[n];
            if(pExAirFan->eSwitchCmd == ON)
            {
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启排风机
            }
            if(pExAirFan->eSwitchCmd == OFF)
            {
                 pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch)); //关闭排风机
            }                
        }
    }
    //非手动模式
    if(pThis->eSystemMode != MODE_MANUAL)
    {
        /*当室内CO2浓度>【CO2浓度调节阈值】（默认2700PPM）+【CO2浓度排风控制偏差值1】（默认270PPM），
        风机全开，变频风机频率为最大频率。*/
        if(pThis->usCO2PPM > pThis->usCO2AdjustThr_V + pThis->usCO2AdjustDeviat)
        {
            /********若【排风机类型】为变频+定频，且变频风机无故障*********/
            if(pThis->eExAirFanType == Type_CONSTANT_VARIABLE && pThis->pExAirFanVariate->xExAirFanErr == FALSE)
            {
                pThis->ucExAirFanRequstNum = 3;
                vSystem_ExAirFanBothCtrl(pThis);
                
                pExAirFan = pThis->pExAirFanVariate;
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启变频排风机
                vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMaxFreq);         //最大频率
                
            }
            /******若【排风机类型】为全定频，或者变频风机故障******/
            if(pThis->eExAirFanType == Type_CONSTANT || pThis->pExAirFanVariate->xExAirFanErr == TRUE)
            {
                pThis->ucExAirFanRequstNum = 4;
                vSystem_ExAirFanConstantCtrl(pThis);
            }
        }
        
        /*当室内CO2浓度<【CO2浓度调节阈值】（默认2700PPM）-【CO2浓度排风控制偏差值2】（默认270PPM），
        排风机按照系统排风需求量调节*/
        if(pThis->usCO2PPM < pThis->usCO2AdjustThr_V - pThis->usCO2AdjustDeviat)
        {
            /********若【排风机类型】为变频+定频，且变频风机无故障*********/
            if(pThis->eExAirFanType == Type_CONSTANT_VARIABLE && pThis->pExAirFanVariate->xExAirFanErr == FALSE)
            {
                vSystem_ExAirFanBothCtrl(pThis);
            }
             /******若【排风机类型】为全定频，或者变频风机故障******/
            if(pThis->eExAirFanType == Type_CONSTANT || pThis->pExAirFanVariate->xExAirFanErr == TRUE)
            {
                vSystem_ExAirFanConstantCtrl(pThis);
            }
        }
    }  
}

/*系统排风需求量变化*/
void vSystem_ExAirSet_Vol(System* pt)
{
    System*   pThis = (System*)pt;
    uint8_t   ucExAirFanRequstNum = 0;
    
    if(pThis->eRunningMode == RUN_MODE_HEAT)
    {
        //系统排风需求量=当天目标新风量*【制热排百分比】（默认90）/100
        pThis->usExAirRequest_Vol = pThis->ulFreAirSet_Vol*pThis->ucExAirHeatRatio/100;
    }
    else
    {
        //系统排风需求量=当天目标新风量*【制冷排风百分比】（默认90）/100
        pThis->usExAirRequest_Vol = pThis->ulFreAirSet_Vol*pThis->ucExAirCoolRatio/100;
    }
    
     //（1）当系统排风需求量<=【排风机额定风量】（默认36000 m³/h）
    if(pThis->usExAirRequest_Vol <= pThis->ulExAirFanRated_Vol)
    {
        pThis->ucExAirFanRequstNum = 0;
    }
    //（2）当【排风机额定风量】<系统排风需求量<=【排风机额定风量】*2
    if( (pThis->ulExAirFanRated_Vol > pThis->usExAirRequest_Vol) && (pThis->usExAirRequest_Vol<= pThis->ulExAirFanRated_Vol*2) )
    {
        pThis->ucExAirFanRequstNum = 1;
    }
    //（3）当【排风机额定风量】*2<系统排风需求量<=【排风机额定风量】*3
    if( (pThis->ulExAirFanRated_Vol*2 < pThis->usExAirRequest_Vol) && (pThis->usExAirRequest_Vol<= pThis->ulExAirFanRated_Vol*3) )
    {
        pThis->ucExAirFanRequstNum = 2;
    }
    //（4）当【排风机额定风量】*3<系统排风需求量
    if(pThis->ulExAirFanRated_Vol*3 < pThis->usExAirRequest_Vol) 
    {
        pThis->ucExAirFanRequstNum = 3;
    }
    vSystem_ExAirFanCtrl(pThis);
}

/*设置变频风机频率范围*/
void vSystem_SetExAirFanFreqRange(System* pt, uint16_t usMinFreq, uint16_t usMaxFreq)
{
    uint8_t  n = 0; 
    
    System*   pThis     = (System*)pt;
    ExAirFan* pExAirFan = NULL;
    
    pThis->usExAirFanMinFreq = usMinFreq;
    pThis->usExAirFanMaxFreq = usMaxFreq;
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n];       
        if(pExAirFan->eFanFreqType == VARIABLE_FREQ)  
        {
            pExAirFan->IDevFreq.setFreqRange(SUPER_PTR(pExAirFan, IDevFreq), usMinFreq, usMaxFreq);   
        } 
    }
}

