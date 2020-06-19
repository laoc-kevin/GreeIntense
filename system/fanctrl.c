#include "system.h"
#include "systemctrl.h"
#include "md_timer.h"

ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM] = {NULL};

/*系统关闭所有排风机*/
void vSystem_CloseExAirFans(System* pt)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ExAirFan* pExAirFan = NULL;
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
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        
        if( (pExAirFan->eFanFreqType != VARIABLE_FREQ) && (pExAirFan->Device.eRunningState == STATE_RUN) )  //所有运行的定频排风机
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

/*系统未运行且无故障的时间最短定频排风机*/
ExAirFan* psSystem_ShortestExAirFan(System* pt)
{
    uint8_t   i, n, m;
    System*   pThis = (System*)pt;
    
    ExAirFan* pExAirFan = NULL;   
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        
        //所有未运行且无故障的定频排风机
        if( pExAirFan->eFanFreqType == CONSTANT_FREQ && pExAirFan->Device.eRunningState == STATE_STOP && pExAirFan->xExAirFanErr == FALSE)  
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
    ExAirFan* pExAirFan = NULL;
    
    if(pThis->eSystemMode != MODE_MANUAL && pThis->eSystemMode != MODE_EMERGENCY)
    {
        return;
    }   
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanRequestTimeTmrCallback \n");
#endif       
    
    if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE )   //变频+定频关闭变频风机
    {
        pExAirFan = pThis->pExAirFanVariate;
    }
    if(pThis->eExAirFanType == TYPE_CONSTANT)            //定频关闭运行时间最长的定频风机
    {
        pExAirFan = psSystem_LongestExAirFan(pThis);
    }
    if(pExAirFan != NULL)
    {
       pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch)); 
    }    
}

/*系统排风机控制周期定时器*/
void vSystem_ExAirFanCtrlTmrCallback(void* p_tmr, void* p_arg)
{
    System*   pThis = (System*)p_arg;
    ExAirFan* pExAirFan = NULL;
    
    uint8_t  ucConstantFanRequestNum = pThis->ucConstantFanRequestNum;
    uint32_t ulExAirRequest_Vol      = pThis->ulExAirRequest_Vol;
    uint32_t ulExAirFanRated_Vol     = pThis->ulExAirFanRated_Vol;
    uint16_t usExAirFanMinFreq       = pThis->usExAirFanMinFreq;
    uint16_t usExAirFanCtrlPeriod    = pThis->usExAirFanCtrlPeriod;
   
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanCtrlTmrCallback %d\n", pThis->ulExAirFanRequestTime);
#endif         
    /********若【排风机类型】为变频+定频，且变频风机无故障*********/
    if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate != NULL)
    {
        pExAirFan = pThis->pExAirFanVariate;
        //需求时间t=（系统排风需求量-【排风机额定风量】*【风机开启需求数】）*50/(【排风机最小频率】*【排风机额定风量】)*【排风机控制周期】；
        pThis->ulExAirFanRequestTime = (uint32_t)( (ulExAirRequest_Vol - ulExAirFanRated_Vol*ucConstantFanRequestNum) * 500L / 
                                                    usExAirFanMinFreq * usExAirFanCtrlPeriod / ulExAirFanRated_Vol);
        
        //若t>=【排风机最小运行时间】（默认300s）则最小频率开启变频排风机；否则不开启。
        if(pExAirFan == NULL)
        {
            return;
        }
        if(pThis->ulExAirFanRequestTime >= pThis->usExAirFanRunTimeLeast)
        {
            pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
            vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMinFreq);        //最小频率

            //开启排风机运行定时器
            (void)xTimerRegist(&pThis->sExAirFanRequestTimeTmr, pThis->ulExAirFanRequestTime, 
                               0, OS_OPT_TMR_ONE_SHOT, vSystem_ExAirFanRequestTimeTmrCallback, pThis, FALSE);
        }
        else   //否则不开启
        {
            pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch));
        }
    }
    /******若【排风机类型】为全定频，或者变频风机故障******/
    if( pThis->eExAirFanType == TYPE_CONSTANT || (pThis->pExAirFanVariate->xExAirFanErr == TRUE && pThis->pExAirFanVariate != NULL) )
    {
        //需求时间t=（（系统排风需求量-【排风机额定风量】*【风机开启需求数】）/【排风机额定风量】）*【排风机控制周期】；
        pThis->ulExAirFanRequestTime =  (ulExAirRequest_Vol - ulExAirFanRated_Vol*ucConstantFanRequestNum) * usExAirFanCtrlPeriod / ulExAirFanRated_Vol ;
        
        //若t>=【排风机最小运行时间】则开启1台定频排风机；否则不开启
        if(pThis->ulExAirFanRequestTime >= pThis->usExAirFanRunTimeLeast)
        {
            pExAirFan = psSystem_ShortestExAirFan(pThis);
            if(pExAirFan == NULL)
            {
                return;
            }
            pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
            
            //开启排风机运行定时器
            (void)xTimerRegist(&pThis->sExAirFanRequestTimeTmr, pThis->ulExAirFanRequestTime, 0, 
                                OS_OPT_TMR_ONE_SHOT, vSystem_ExAirFanRequestTimeTmrCallback, pThis, FALSE);
        }
    }
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanCtrlTmrCallback %d\n", pThis->ulExAirFanRequestTime);
#endif      
}

/*系统启停定频排风风机*/
void vSystem_ExAirFanConstantSwitch(System* pt)
{
    uint8_t   i, n, m, ucRunningNum;
    
    System*   pThis     = (System*)pt;
    ExAirFan* pExAirFan = NULL;   

#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanConstantSwitch  pThis->ucConstantFanRequestNum %d  ucRunningNum %d\n", pThis->ucConstantFanRequestNum, ucRunningNum);
#endif    
    
    for(n=0, m=0, ucRunningNum=0; n < EX_AIR_FAN_NUM; n++)  //所有定频排风机
    {
        pExAirFan = pThis->psExAirFanList[n];
        if(pExAirFan->eFanFreqType != VARIABLE_FREQ)
        {
            pFanByRunTimeList[m] = pThis->psExAirFanList[n];   
            m++;
           if(pExAirFan->Device.eRunningState == STATE_RUN)
           {
               ucRunningNum++;    //当前运行的定频排风机个数   
           }       
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
    if(pThis->ucConstantFanRequestNum < ucRunningNum)
    {
        for(i=0, n=ucRunningNum - pThis->ucConstantFanRequestNum; n!=0 && i<m; i++)
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
    if(pThis->ucConstantFanRequestNum > ucRunningNum)
    {
        for(i=0, n = pThis->ucConstantFanRequestNum-ucRunningNum; n!=0 && i<m; i++)
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
    
    if(pThis->eExAirFanType != TYPE_CONSTANT)
    {
        return;
    }
    vSystem_ExAirFanConstantSwitch(pThis);
    
    //每个【排风机控制周期】（默认1800s）周期执行一次以下A、B、C逻辑
    (void)xTimerRegist(&pThis->sExAirFanCtrlTmr, 0, pThis->usExAirFanCtrlPeriod,  
                       OS_OPT_TMR_PERIODIC, vSystem_ExAirFanCtrlTmrCallback, pThis, TRUE);
}

/*系统排风风机频率调节*/
void vSystem_AdjustExAirFanFreq(System* pt, uint16_t usFreq)
{
    System*   pThis = (System*)pt;
    ExAirFan* pExAirFanVariate = pThis->psExAirFanList[0];    //变频风机
    
    BMS* psBMS = BMS_Core();  
    
    if(pThis->eExAirFanType != TYPE_CONSTANT_VARIABLE || pExAirFanVariate == NULL)
    {
        return;
    }
#if DEBUG_ENABLE > 0          
    myprintf("vSystem_AdjustExAirFanFreq  usExAirFanFreq  %d usRunningFreq %d\n", usFreq, pExAirFanVariate->usRunningFreq);
#endif    

    pExAirFanVariate->IDevFreq.setFreq(SUPER_PTR(pExAirFanVariate, IDevFreq), usFreq);  //设置频率
    pThis->usExAirFanFreq = usFreq;
    psBMS->usExAirFanFreq = usFreq;        
}

/*系统定频+变频排风风机控制*/
void vSystem_ExAirFanBothCtrl(System* pt)
{
    uint16_t  usFreq = 0;
    System*   pThis = (System*)pt;
    ExAirFan* pExAirFan = pThis->pExAirFanVariate;
    
    uint8_t   ucConstantFanRequestNum  = pThis->ucConstantFanRequestNum;
    uint32_t  ulExAirRequest_Vol       = pThis->ulExAirRequest_Vol;
    uint16_t  ulExAirFanRated_Vol      = pThis->ulExAirFanRated_Vol;
    uint16_t  usExAirFanMinFreq        = pThis->usExAirFanMinFreq;
    uint16_t  usExAirFanCtrlPeriod     = pThis->usExAirFanCtrlPeriod;
   
    if(pThis->eExAirFanType != TYPE_CONSTANT_VARIABLE || pExAirFan == NULL)
    {
        return;
    }
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanBothCtrl  ulExAirRequest_Vol %ld  ucConstantFanRequestNum %d \n", ulExAirRequest_Vol, ucConstantFanRequestNum);
#endif       
    vSystem_ExAirFanConstantSwitch(pThis);
    
    /*当系统排风需求量-【排风机额定风量】*【风机开启需求数】>=【排风机额定风量】*【排风机最小频率】/50，
      只开启变频排风机*/
    if( (ulExAirRequest_Vol - ulExAirFanRated_Vol*ucConstantFanRequestNum) >= (usExAirFanMinFreq*ulExAirFanRated_Vol/500) )
    { 
        pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启变频排风机
        
        //排风机频率=（（系统排风需求量-【排风机额定风量】*【风机开启需求数】）*50 /【排风机额定风量】；
        usFreq = (ulExAirRequest_Vol - ulExAirFanRated_Vol*ucConstantFanRequestNum) * 500 / ulExAirFanRated_Vol;
        vSystem_AdjustExAirFanFreq(pThis, usFreq); //设定频率
    }
    else
    {
        //否则，每个【排风机控制周期】（默认1800s）周期执行一次以下A、B、C逻辑：
        (void)xTimerRegist(&pThis->sExAirFanCtrlTmr, 0, usExAirFanCtrlPeriod,  
                           OS_OPT_TMR_PERIODIC, vSystem_ExAirFanCtrlTmrCallback, pThis, TRUE);
    }
}

/*系统排风需求量计算*/
uint32_t ulSystem_ExAirRequest_Vol(System* pt)
{
    System*   pThis = (System*)pt;
    
    uint32_t  ulExAirRequest_Vol = 0; 
    if(pThis->eRunningMode == RUN_MODE_HEAT)    //（2）制热模式；
    {
        //系统排风需求量=当天目标新风量*【制热排百分比】（默认90）/100
        ulExAirRequest_Vol = pThis->ulFreAirSet_Vol*pThis->ucExAirHeatRatio/100;
    }
    else      //（1）送风模式、湿帘模式、制冷模式；
    {
        //系统排风需求量=当天目标新风量*【制冷排风百分比】（默认90）/100
        ulExAirRequest_Vol = pThis->ulFreAirSet_Vol*pThis->ucExAirCoolRatio/100;
    }
    
     //（1）当系统排风需求量<【排风机额定风量】（默认36000 m³/h）
    if(ulExAirRequest_Vol < pThis->ulExAirFanRated_Vol)
    {
        pThis->ucConstantFanRequestNum = 0;
    }
    //（2）当【排风机额定风量】<=系统排风需求量<【排风机额定风量】*2
    if( (pThis->ulExAirFanRated_Vol <= ulExAirRequest_Vol) && (ulExAirRequest_Vol< pThis->ulExAirFanRated_Vol*2) )
    {
        pThis->ucConstantFanRequestNum = 1;
    }
    //（3）当【排风机额定风量】*2<=系统排风需求量<【排风机额定风量】*3
    if( (pThis->ulExAirFanRated_Vol*2 <= ulExAirRequest_Vol) && (ulExAirRequest_Vol< pThis->ulExAirFanRated_Vol*3) )
    {
        pThis->ucConstantFanRequestNum = 2;
    }
    //（4）当【排风机额定风量】*3<=系统排风需求量
    if(pThis->ulExAirFanRated_Vol*3 <= ulExAirRequest_Vol) 
    {
        pThis->ucConstantFanRequestNum = 3;
    }
    return ulExAirRequest_Vol;
}

/*系统排风风机控制*/
void vSystem_ExAirFanCtrl(System* pt)
{
    uint8_t n = 0;
    System*   pThis     = (System*)pt;
    ExAirFan* pExAirFan = NULL;   
    uint32_t  ulExAirRequest_Vol = 0;
 #if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanCtrl  ucConstantFanRequestNum %d \n", pThis->ucConstantFanRequestNum);
#endif     
    
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
    //自动模式或紧急送风模式
    if(pThis->eSystemMode == MODE_AUTO || pThis->eSystemMode == MODE_EMERGENCY)
    {
        /*当室内CO2浓度>【CO2浓度调节阈值】（默认2700PPM）+【CO2浓度排风控制偏差值1】（默认270PPM），
        风机全开，变频风机频率为最大频率。*/
        
//        myprintf("vSystem_ExAirFanCtrl  usCO2PPM %d usCO2AdjustThr_V %d usCO2AdjustDeviat %d  \n\n", 
//                 pThis->usCO2PPM, pThis->usCO2AdjustThr_V, pThis->usCO2AdjustDeviat);
        
        if(pThis->usCO2PPM > pThis->usCO2AdjustThr_V + pThis->usCO2AdjustDeviat)
        {
            /********若【排风机类型】为变频+定频，且变频风机无故障*********/
            if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate != NULL)
            {
                pThis->ucConstantFanRequestNum = 3;
                vSystem_ExAirFanBothCtrl(pThis);
                
                pExAirFan = pThis->pExAirFanVariate;
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启变频排风机
                vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMaxFreq);         //最大频率
                
            }
            /******若【排风机类型】为全定频，或者变频风机故障******/
            if(pThis->eExAirFanType == TYPE_CONSTANT || pThis->pExAirFanVariate->xExAirFanErr == TRUE)
            {
                pThis->ucConstantFanRequestNum = 4;
                vSystem_ExAirFanConstantCtrl(pThis);
            }
        }
        /*当室内CO2浓度<【CO2浓度调节阈值】（默认2700PPM）-【CO2浓度排风控制偏差值2】（默认270PPM），
        排风机按照系统排风需求量调节*/
        if(pThis->usCO2PPM < pThis->usCO2AdjustThr_V - pThis->usCO2AdjustDeviat)
        {
            ulExAirRequest_Vol = ulSystem_ExAirRequest_Vol(pThis);  //计算排风需求量
            if( pThis->ulExAirRequest_Vol != ulExAirRequest_Vol)
            {
                pThis->ulExAirRequest_Vol = ulExAirRequest_Vol;
                /********若【排风机类型】为变频+定频，且变频风机无故障*********/
                if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate != NULL)
                {
                    vSystem_ExAirFanBothCtrl(pThis);
                }
                /******若【排风机类型】为全定频，或者变频风机故障******/
                if(pThis->eExAirFanType == TYPE_CONSTANT || (pThis->pExAirFanVariate->xExAirFanErr == TRUE && pThis->pExAirFanVariate != NULL))
                {
                    vSystem_ExAirFanConstantCtrl(pThis);
                }
            } 
        }
    }
}

/*系统排风百分比设定*/
void vSystem_ExAirRatio(System* pt, uint8_t ucExAirCoolRatio, uint8_t ucExAirHeatRatio)
{
    System*   pThis = (System*)pt;

    if(pThis->ucExAirCoolRatio != ucExAirCoolRatio || pThis->ucExAirCoolRatio != ucExAirCoolRatio)
    {
        pThis->ucExAirCoolRatio = ucExAirCoolRatio;
        pThis->ucExAirCoolRatio = ucExAirCoolRatio;
        vSystem_ExAirSet_Vol(pThis);
    }
}

/*系统排风需求量设定*/
void vSystem_ExAirSet_Vol(System* pt)
{
    System*   pThis = (System*)pt;
    uint32_t  ulExAirRequest_Vol = ulSystem_ExAirRequest_Vol(pThis); 
 
    if( pThis->ulExAirRequest_Vol != ulExAirRequest_Vol)
    {
#if DEBUG_ENABLE > 0
        myprintf("vSystem_ExAirSet_Vol usExAirRequest_Vol %d ucConstantFanRequestNum %d \n", ulExAirRequest_Vol, pThis->ucConstantFanRequestNum);
#endif
        pThis->ulExAirRequest_Vol = ulExAirRequest_Vol;
        vSystem_ExAirFanCtrl(pThis);
    }
}

/*设置变频风机频率范围*/
void vSystem_SetExAirFanFreqRange(System* pt, uint16_t usMinFreq, uint16_t usMaxFreq)
{
    uint8_t  n = 0; 
    
    System*   pThis     = (System*)pt;
    ExAirFan* pExAirFan = NULL;
    
    pThis->usExAirFanMinFreq = usMinFreq;
    pThis->usExAirFanMaxFreq = usMaxFreq;
  
#if DEBUG_ENABLE > 0
    myprintf("vSystem_SetExAirFanFreqRange  usMinFreq %d  usMaxFreq %d\n", pThis->usExAirFanMinFreq, pThis->usExAirFanMaxFreq);
#endif
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n];       
        if(pExAirFan->eFanFreqType == VARIABLE_FREQ)  
        {
            pExAirFan->IDevFreq.setFreqRange(SUPER_PTR(pExAirFan, IDevFreq), usMinFreq, usMaxFreq);   
        } 
    }
}

/*风机故障处理*/
void vSystem_ExAirFanErr(System* pt, ExAirFan* pExAirFan)
{
    uint8_t  n = 0; 
    System*  pThis = (System*)pt;

    if(pExAirFan->xExAirFanErr == TRUE)  
    {
        pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch)); //关闭排风机
        vSystem_SetAlarm(pThis);
    }
    else
    {
        vSystem_DelAlarmRequst(pThis);
    }
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanErr  ExAirFan %d\n", pExAirFan->Device.ucDevIndex);
#endif    
    vSystem_ExAirFanCtrl(pThis);
    
}
