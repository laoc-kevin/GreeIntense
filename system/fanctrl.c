#include "system.h"
#include "systemctrl.h"
#include "md_timer.h"

uint32_t ulExAirFanRequestTime = 0;

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
    (void)xTmrStop(&pThis->sExAirFanRequestTimeTmr);   //停止运行需求时间定时器
    (void)xTmrStop(&pThis->sExAirFanCtrlTmr);          //停止周期轮询
}

/*系统当前正在运行的时间最长定频排风机*/
ExAirFan* psSystem_LongestExAirFan(System* pt)
{
    uint8_t   i, n, ucConstantNum;
    System*   pThis = (System*)pt;
    ExAirFan* pExAirFan = NULL;
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM] = {NULL};
    
    for(n=0, ucConstantNum=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        if( pExAirFan->eFanFreqType == CONSTANT_FREQ && pExAirFan->Device.eRunningState == STATE_RUN )  //所有运行的定频排风机
        {
            pFanByRunTimeList[ucConstantNum] = pExAirFan;   
            ucConstantNum++;
        }       
    }
    if(ucConstantNum >= 1)
    {
        pExAirFan = pFanByRunTimeList[0];
        for(n=0; n < ucConstantNum; n++)  
        {          
            if(pExAirFan->Device.usRunTime_H < pFanByRunTimeList[n]->Device.usRunTime_H)
            {
                pExAirFan = pFanByRunTimeList[n];
            }
        }
    }
    else
    {
        return NULL;
    }        
#if DEBUG_ENABLE > 0
    if(pExAirFan != NULL)
    {
        myprintf("psSystem_LongestExAirFan  ucConstantNum %d  ucDevIndex %d\n", ucConstantNum, pExAirFan->Device.ucDevIndex);
    }
#endif
    return pExAirFan;
}

/*系统未运行且无故障的时间最短定频排风机*/
ExAirFan* psSystem_ShortestExAirFan(System* pt)
{
    uint8_t   i, n, ucConstantNum;
    System*   pThis = (System*)pt; 
    ExAirFan* pExAirFan = NULL; 
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM] = {NULL};
    
    for(n=0, ucConstantNum=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        
        //所有未运行且无故障且远程的定频排风机
        if( pExAirFan->eFanFreqType == CONSTANT_FREQ && pExAirFan->Device.eRunningState == STATE_STOP && 
            pExAirFan->xExAirFanErr == FALSE && pExAirFan->xExAirFanRemote == TRUE)  
        {
            pFanByRunTimeList[ucConstantNum] = pExAirFan;   
            ucConstantNum++;
        }      
    }
    if(ucConstantNum >= 1)
    {
        pExAirFan = pFanByRunTimeList[0];
        for(n=0; n < ucConstantNum; n++)     //所有未运行的定频排风机最短已运行时间的风机
        {          
            if(pExAirFan->Device.usRunTime_H > pFanByRunTimeList[n]->Device.usRunTime_H)
            {
                pExAirFan = pFanByRunTimeList[n];
            }
        }
    }
    else
    {
        return NULL;
    }
#if DEBUG_ENABLE > 0
    if(pExAirFan != NULL)
    {
        myprintf("psSystem_ShortestExAirFan ucDevIndex %d\n",  pExAirFan->Device.ucDevIndex);
    }
#endif         
    return pExAirFan;
}

/*系统排风风机运行需求时间定时器*/
void vSystem_ExAirFanRequestTimeTmrCallback(void* p_tmr, void* p_arg)
{
    System*   pThis = (System*)p_arg;
    ExAirFan* pExAirFan = NULL;
     
    if(pThis->eSystemMode == MODE_AUTO || pThis->eSystemMode == MODE_EMERGENCY)
    {
        /********若【排风机类型】为变频+定频，且变频风机无故障且为远程*********/
        if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL && 
           pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate->xExAirFanRemote == TRUE)  
        {
            pThis->xVariableFanRequest = FALSE;
            pExAirFan = pThis->pExAirFanVariate;
        }
        /******若【排风机类型】为全定频，或者变频风机故障或本地******/
        if( pThis->eExAirFanType == TYPE_CONSTANT || 
           (pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL &&
           (pThis->pExAirFanVariate->xExAirFanErr == TRUE || pThis->pExAirFanVariate->xExAirFanRemote == FALSE)) )   
        {
            pExAirFan = psSystem_LongestExAirFan(pThis);  //关闭运行时间最长的定频风机
        }
        if(pExAirFan != NULL)
        {
            pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch));
#if DEBUG_ENABLE > 0
            myprintf("vSystem_ExAirFanRequestTimeTmrCallback %d \n", pThis->eSystemMode);
#endif             
        } 
    }
}

/*系统启停定频排风风机*/
void vSystem_ExAirFanConstantSwitch(System* pt)
{
    uint8_t   i, n, ucConstantNum, ucRunningNum;
    OS_ERR    err   = OS_ERR_NONE;
    
    System*   pThis     = (System*)pt;
    ExAirFan* pExAirFan = NULL;   
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM] = {NULL};
    
    if(pThis->eSystemMode == MODE_MANUAL || pThis->eSystemMode ==MODE_CLOSE)
    {
        return;
    }
    for(n=0, ucConstantNum=0, ucRunningNum=0; n < EX_AIR_FAN_NUM; n++)  //所有无故障且远程定频排风机
    {
        pExAirFan = pThis->psExAirFanList[n];
        if(pExAirFan->eFanFreqType == CONSTANT_FREQ && pExAirFan->xExAirFanErr == FALSE && pExAirFan->xExAirFanRemote == TRUE)
        {
            pFanByRunTimeList[ucConstantNum] = pExAirFan;   
            ucConstantNum++;
            if(pExAirFan->Device.eRunningState == STATE_RUN)
            {
                ucRunningNum++;    //当前运行的定频排风机个数   
            }       
        }        
    }
    if(ucConstantNum >= 1)
    { 
        for(n=0; n < ucConstantNum-1; n++)   //无故障定频排风机已运行时间排序
        {          
            for(i=0; i< ucConstantNum-n-1; i++)
            {
                if(pFanByRunTimeList[i]->Device.usRunTime_H > pFanByRunTimeList[i+1]->Device.usRunTime_H)
                {
                               pExAirFan   = pFanByRunTimeList[i];
                    pFanByRunTimeList[i]   = pFanByRunTimeList[i+1]; 
                    pFanByRunTimeList[i+1] = pExAirFan;
                }
            }
        }
    }
    //系统需求排风机个数 > 无故障定频排风机，则最大频率开启变频风机 
    if(pThis->ucConstantFanRequestNum > ucConstantNum && pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && 
       pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate != NULL && pThis->pExAirFanVariate->xExAirFanRemote == TRUE)
    {
        pExAirFan = pThis->pExAirFanVariate;
        pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启变频排风机
        vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMaxFreq);         //最大频率
    }
    if(pThis->ucConstantFanRequestNum <= ucConstantNum && pThis->xVariableFanRequest == FALSE && pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && 
       pThis->pExAirFanVariate != NULL && pThis->pExAirFanVariate->Device.eRunningState == STATE_RUN)
    {
        pExAirFan = pThis->pExAirFanVariate;
        pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch));  //关闭变频排风机
    }
    if(pThis->ucConstantFanRequestNum < ucRunningNum)  //系统需求排风机个数 < 运行的定频排风机个数  
    {
        for(i=0, n=ucRunningNum - pThis->ucConstantFanRequestNum; n>0 && i<ucConstantNum; i++)
        {
            pExAirFan = pFanByRunTimeList[ucConstantNum-i-1];   //运行时间最长的排风机
            if(pExAirFan != NULL && pExAirFan->Device.eRunningState == STATE_RUN)  //是否在运行
            {
                pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch)); //关闭运行时间最长的排风机
                n--;
            }
        }
    }
    if(pThis->ucConstantFanRequestNum > ucRunningNum)   //系统需求排风机个数 > 运行的定频排风机个数
    {
        for(i=0, n = pThis->ucConstantFanRequestNum-ucRunningNum; n>0 && i<ucConstantNum; i++)
        {
            pExAirFan = pFanByRunTimeList[i];   //运行时间最短的排风机
            if(pExAirFan != NULL && pExAirFan->Device.eRunningState == STATE_STOP && pExAirFan->xExAirFanErr == FALSE)  //是否未运行且无故障
            {
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
                n--;
            }
        }
    }
//#if DEBUG_ENABLE > 0
//    myprintf("vSystem_ExAirFanConstantSwitch  ucConstantFanRequestNum %d  ucRunningNum %d \n", pThis->ucConstantFanRequestNum, ucRunningNum);
//#endif    
}

/*系统排风需求量计算*/
void vSystem_ExAirRequest_Vol(System* pt)
{
    System*   pThis = (System*)pt;
    
    uint32_t ulExAirRequest_Vol   = pThis->ulExAirRequest_Vol; 
    uint16_t usExAirFanMinFreq    = pThis->usExAirFanMinFreq;
    uint32_t ulExAirFanRated_Vol  = pThis->ulExAirFanRated_Vol;
    uint16_t usExAirFanCtrlPeriod = pThis->usExAirFanCtrlPeriod;
    
    if(pThis->eRunningMode == RUN_MODE_HEAT)    //（2）制热模式；
    {
        //系统排风需求量=当天目标新风量*【制热排百分比】（默认90）/100
        pThis->ulExAirRequest_Vol = pThis->ulFreAirSet_Vol*pThis->ucExAirHeatRatio/100;
    }
    else      //（1）送风模式、湿帘模式、制冷模式；
    {
        //系统排风需求量=当天目标新风量*【制冷排风百分比】（默认90）/100
        pThis->ulExAirRequest_Vol = pThis->ulFreAirSet_Vol*pThis->ucExAirCoolRatio/100;
    }
    
     //（1）当系统排风需求量<【排风机额定风量】（默认36000 m³/h）
    if(pThis->ulExAirRequest_Vol < pThis->ulExAirFanRated_Vol)
    {
        pThis->ucConstantFanOpenNum = 0;
    }
    //（2）当【排风机额定风量】<=系统排风需求量<【排风机额定风量】*2
    if( (pThis->ulExAirFanRated_Vol <= pThis->ulExAirRequest_Vol) && (pThis->ulExAirRequest_Vol< pThis->ulExAirFanRated_Vol*2) )
    {
        pThis->ucConstantFanOpenNum = 1;
    }
    //（3）当【排风机额定风量】*2<=系统排风需求量<【排风机额定风量】*3
    if( (pThis->ulExAirFanRated_Vol*2 <= pThis->ulExAirRequest_Vol) && (pThis->ulExAirRequest_Vol< pThis->ulExAirFanRated_Vol*3) )
    {
        pThis->ucConstantFanOpenNum = 2;
    }
    //（4）当【排风机额定风量】*3<=系统排风需求量
    if(pThis->ulExAirFanRated_Vol*3 <= pThis->ulExAirRequest_Vol) 
    {
        pThis->ucConstantFanOpenNum = 3;
    }
    
    /********若【排风机类型】为变频+定频，且变频风机无故障且为远程*********/
    if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL && 
       pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate->xExAirFanRemote == TRUE)  
    {
        pThis->ucConstantFanRequestNum = pThis->ucConstantFanOpenNum;
        if(ulExAirFanRated_Vol != 0)
        {
            //需求时间t=（（系统排风需求量-【排风机额定风量】*【风机常开数】）*50）/（【排风机最小频率】*【排风机额定风量】）*【排风机控制周期】；
            pThis->ulExAirFanRequestTime = (uint32_t)( (pThis->ulExAirRequest_Vol - ulExAirFanRated_Vol*pThis->ucConstantFanOpenNum) * 500L / 
                                                        usExAirFanMinFreq * usExAirFanCtrlPeriod / ulExAirFanRated_Vol);
        } 
    }
    /******若【排风机类型】为全定频，或者变频风机故障或本地******/
    if( pThis->eExAirFanType == TYPE_CONSTANT || 
       (pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL &&
       (pThis->pExAirFanVariate->xExAirFanErr == TRUE || pThis->pExAirFanVariate->xExAirFanRemote == FALSE)) )   
    {
        pThis->ucConstantFanRequestNum = pThis->ucConstantFanOpenNum;
        //需求时间t =（系统排风需求量-【排风机额定风量】*【风机常开数】)/【排风机额定风量】*【排风机控制周期】
        if(ulExAirFanRated_Vol != 0)
        {
            pThis->ulExAirFanRequestTime = (uint32_t)( (pThis->ulExAirRequest_Vol - ulExAirFanRated_Vol*pThis->ucConstantFanOpenNum)* 
                                                        usExAirFanCtrlPeriod / ulExAirFanRated_Vol);
        }
        //若t>=【排风机最小运行时间】则开启1台定频排风机；否则不开启
        if(pThis->ulExAirFanRequestTime >= pThis->usExAirFanRunTimeLeast)
        {
             pThis->ucConstantFanRequestNum = pThis->ucConstantFanOpenNum+1;
        }
    }
    if(pThis->ulExAirRequest_Vol != ulExAirRequest_Vol)
    {
#if DEBUG_ENABLE > 0
        myprintf("ulSystem_ExAirRequest_Vol ulExAirRequest_Vol %ld ulExAirFanRequestTime %ld  ucConstantFanRequestNum %d  ucConstantFanOpenNum %d \n", 
                  pThis->ulExAirRequest_Vol, pThis->ulExAirFanRequestTime, pThis->ucConstantFanRequestNum, pThis->ucConstantFanOpenNum);
#endif  
    }
}

/*系统排风机控制周期定时器*/
void vSystem_ExAirFanCtrlTmrCallback(void* p_tmr, void* p_arg)
{
    OS_ERR    err   = OS_ERR_NONE;
    System*   pThis = (System*)p_arg;
    ExAirFan* pExAirFan = NULL;
   
    if(pThis->eSystemMode == MODE_MANUAL || pThis->eSystemMode ==MODE_CLOSE)
    {
        return;
    }
    (void)vSystem_ExAirRequest_Vol(pThis);  //计算排风需求量
    
    /********若【排风机类型】为变频+定频，且变频风机无故障且为远程*********/
    if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL && 
       pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate->xExAirFanRemote == TRUE) 
    {
        pExAirFan = pThis->pExAirFanVariate;
        if(pExAirFan != NULL)
        {
            //若t>=【排风机最小运行时间】（默认300s）则最小频率开启变频排风机；否则不开启。
            if(pThis->ulExAirFanRequestTime >= pThis->usExAirFanRunTimeLeast)   
            {
                pThis->xVariableFanRequest = TRUE;
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
                vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMinFreq);        //最小频率
            
               //开启排风机运行定时器
                if( OSTmrStateGet(&pThis->sExAirFanRequestTimeTmr, &err) != OS_TMR_STATE_RUNNING || 
                    ulExAirFanRequestTime != pThis->ulExAirFanRequestTime )
                {
                    ulExAirFanRequestTime = pThis->ulExAirFanRequestTime;
                    xTimerRegist(&pThis->sExAirFanRequestTimeTmr, pThis->ulExAirFanRequestTime, 0, 
                                 OS_OPT_TMR_ONE_SHOT, vSystem_ExAirFanRequestTimeTmrCallback, pThis, FALSE);
#if DEBUG_ENABLE > 0
                    myprintf("xTimerRegist  vSystem_ExAirFanRequestTimeTmrCallback %ld\n", pThis->ulExAirFanRequestTime);
#endif
                }
            }
            else   //否则不开启
            {
                pThis->xVariableFanRequest = FALSE;
                pExAirFan->IDevSwitch.switchClose(SUPER_PTR(pExAirFan, IDevSwitch));
            }
        }
    }
    /******若【排风机类型】为全定频，或者变频风机故障或本地******/
    if( pThis->eExAirFanType == TYPE_CONSTANT || 
       (pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL &&
       (pThis->pExAirFanVariate->xExAirFanErr == TRUE || pThis->pExAirFanVariate->xExAirFanRemote == FALSE)) ) 
    {
        //若t>=【排风机最小运行时间】则开启1台定频排风机；否则不开启
        if(pThis->ulExAirFanRequestTime >= pThis->usExAirFanRunTimeLeast)
        {
            //开启排风机运行定时器
            if( OSTmrStateGet(&pThis->sExAirFanRequestTimeTmr, &err) != OS_TMR_STATE_RUNNING || 
                ulExAirFanRequestTime != pThis->ulExAirFanRequestTime )
            {
                ulExAirFanRequestTime = pThis->ulExAirFanRequestTime;
                vSystem_ExAirFanConstantSwitch(pThis);
                xTimerRegist(&pThis->sExAirFanRequestTimeTmr, pThis->ulExAirFanRequestTime, 0, 
                             OS_OPT_TMR_ONE_SHOT, vSystem_ExAirFanRequestTimeTmrCallback, pThis, FALSE);
#if DEBUG_ENABLE > 0
                myprintf("xTimerRegist  vSystem_ExAirFanRequestTimeTmrCallback %ld\n", pThis->ulExAirFanRequestTime);
#endif
            }
        }
    }
//#if DEBUG_ENABLE > 0
//    myprintf("vSystem_ExAirFanCtrlTmrCallback %d\n", pThis->ulExAirFanRequestTime);
//#endif    
}

/*系统全定频排风风机控制*/
void vSystem_ExAirFanConstantCtrl(System* pt)
{
    System*   pThis = (System*)pt;
    
    if(pThis->eSystemMode == MODE_MANUAL || pThis->eSystemMode ==MODE_CLOSE)
    {
        return;
    }
    /******若【排风机类型】为全定频，或者变频风机故障或本地******/
    if( pThis->eExAirFanType == TYPE_CONSTANT || 
       (pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL &&
       (pThis->pExAirFanVariate->xExAirFanErr == TRUE || pThis->pExAirFanVariate->xExAirFanRemote == FALSE)) ) 
    {
#if DEBUG_ENABLE > 0
        myprintf("vSystem_ExAirFanConstantCtrl \n");
#endif  
        vSystem_ExAirFanConstantSwitch(pThis);
        
        //每个【排风机控制周期】（默认1800s）周期执行一次以下A、B、C逻辑
        (void)xTimerRegist(&pThis->sExAirFanCtrlTmr, 0, pThis->usExAirFanCtrlPeriod,  
                           OS_OPT_TMR_PERIODIC, vSystem_ExAirFanCtrlTmrCallback, pThis, TRUE);
    }
}

/*系统排风风机频率调节*/
void vSystem_AdjustExAirFanFreq(System* pt, uint16_t usFreq)
{
    System*   pThis = (System*)pt;
    ExAirFan* pExAirFanVariate = pThis->psExAirFanList[0];    //变频风机

    if(pThis->eExAirFanType != TYPE_CONSTANT_VARIABLE || pExAirFanVariate == NULL)
    {
        return;
    }
    if(usFreq < pThis->usExAirFanMinFreq)
    {
        usFreq = pThis->usExAirFanMinFreq;
    }
    if(usFreq > pThis->usExAirFanMaxFreq)
    {
        usFreq = pThis->usExAirFanMaxFreq;
    }
#if DEBUG_ENABLE > 0          
    myprintf("vSystem_AdjustExAirFanFreq  usExAirFanFreq  %d usRunningFreq %d\n", usFreq, pExAirFanVariate->usRunningFreq);
#endif    

    pExAirFanVariate->IDevFreq.setFreq(SUPER_PTR(pExAirFanVariate, IDevFreq), usFreq);  //设置频率
    pThis->usExAirFanFreq = usFreq;
    pThis->psBMS->usExAirFanFreq = usFreq;        
}

/*系统定频+变频排风风机控制*/
void vSystem_ExAirFanBothCtrl(System* pt)
{
    uint16_t  usFreq = 0;
    System*   pThis = (System*)pt;
    ExAirFan* pExAirFan = pThis->pExAirFanVariate;
    
    uint8_t   ucConstantFanOpenNum  = pThis->ucConstantFanOpenNum;
    uint32_t  ulExAirRequest_Vol    = pThis->ulExAirRequest_Vol;
    uint16_t  ulExAirFanRated_Vol   = pThis->ulExAirFanRated_Vol;
    uint16_t  usExAirFanMinFreq     = pThis->usExAirFanMinFreq;
    uint16_t  usExAirFanCtrlPeriod  = pThis->usExAirFanCtrlPeriod;
   
    if(pThis->eExAirFanType != TYPE_CONSTANT_VARIABLE)
    {
        return;
    }   
    (void)vSystem_ExAirFanConstantSwitch(pThis);
    
    if( pExAirFan == NULL || ulExAirFanRated_Vol == 0)
    {
        return;
    }
    /*当系统排风需求量-【排风机额定风量】*【风机常开数】>=【排风机额定风量】*【排风机最小频率】/50，
      只开启变频排风机*/
    if( (ulExAirRequest_Vol - ulExAirFanRated_Vol*ucConstantFanOpenNum) >= (usExAirFanMinFreq*ulExAirFanRated_Vol/500) )
    { 
        //排风机频率 = 系统排风需求量-【排风机额定风量】*【风机常开数】*50 /【排风机额定风量】
        if(ulExAirFanRated_Vol == 0)
        {
            return;
        }
        usFreq = (ulExAirRequest_Vol - ulExAirFanRated_Vol*ucConstantFanOpenNum) * 500 / ulExAirFanRated_Vol;
        if(usFreq >= pThis->usExAirFanMinFreq)    //大于最小频率则开启  
        {
            pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启变频排风机
            pThis->xVariableFanRequest = TRUE;
        }
        if(usFreq >= pThis->usExAirFanMinFreq && usFreq <= pThis->usExAirFanMaxFreq)         
        {
            vSystem_AdjustExAirFanFreq(pThis, usFreq); //设定频率
        }
        if(usFreq > pThis->usExAirFanMaxFreq)   
        {
            vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMaxFreq);  //最大频率
        }
        (void)xTmrStop(&pThis->sExAirFanRequestTimeTmr);   //停止运行需求时间定时器
        (void)xTmrStop(&pThis->sExAirFanCtrlTmr);          //停止周期轮询
    }
    else
    {
        //否则，每个【排风机控制周期】（默认1800s）周期执行一次以下A、B、C逻辑：
        (void)xTimerRegist(&pThis->sExAirFanCtrlTmr, 0, usExAirFanCtrlPeriod,  
                           OS_OPT_TMR_PERIODIC, vSystem_ExAirFanCtrlTmrCallback, pThis, TRUE);
    }
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanBothCtrl  ulExAirRequest_Vol %ld  ucConstantFanOpenNum %d \n", ulExAirRequest_Vol, ucConstantFanOpenNum);
#endif    
}

/*系统排风风机控制*/
void vSystem_ExAirFanCtrl(System* pt)
{
    uint8_t   n = 0;
    uint32_t  ulExAirRequest_Vol = 0;
    
    System*   pThis     = (System*)pt;
    ExAirFan* pExAirFan = NULL;   
   
    //手动模式下，手动开关风机
    if(pThis->eSystemMode == MODE_MANUAL)
    {
        for(n=0; n < EX_AIR_FAN_NUM; n++)  
        {
            pExAirFan = pThis->psExAirFanList[n];
            if(pExAirFan->eCtrlCmd == ON && pExAirFan->xExAirFanErr == FALSE && pExAirFan->xExAirFanRemote == TRUE)
            {
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启排风机
            }
            if(pExAirFan->eCtrlCmd == OFF)
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
        if(pThis->usCO2PPM > pThis->usCO2AdjustThr_V + pThis->usCO2AdjustDeviat)
        {
            /********若【排风机类型】为变频+定频，且变频风机无故障且为远程*********/
            if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL && 
               pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate->xExAirFanRemote == TRUE) 
            {
                pThis->ucConstantFanRequestNum = 3;
                (void)vSystem_ExAirFanConstantSwitch(pThis);
                
                pExAirFan = pThis->pExAirFanVariate;
                pThis->xVariableFanRequest = TRUE;
                
                pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));  //开启变频排风机
                vSystem_AdjustExAirFanFreq(pThis, pThis->usExAirFanMaxFreq);         //最大频率
            }
            /******若【排风机类型】为全定频，或者变频风机故障或本地******/
            if( pThis->eExAirFanType == TYPE_CONSTANT || 
               (pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL &&
               (pThis->pExAirFanVariate->xExAirFanErr == TRUE || pThis->pExAirFanVariate->xExAirFanRemote == FALSE)) ) 
            {
                pThis->ucConstantFanRequestNum = 4;
                (void)vSystem_ExAirFanConstantSwitch(pThis);
            }
            (void)xTmrStop(&pThis->sExAirFanRequestTimeTmr);   //停止运行需求时间定时器
            (void)xTmrStop(&pThis->sExAirFanCtrlTmr);   //停止周期轮询
        }
        /*当室内CO2浓度<【CO2浓度调节阈值】（默认2700PPM）-【CO2浓度排风控制偏差值2】（默认270PPM），
        排风机按照系统排风需求量调节*/
        if(pThis->usCO2PPM < pThis->usCO2AdjustThr_V - pThis->usCO2AdjustDeviat)
        {
            (void)vSystem_ExAirRequest_Vol(pThis);  //计算排风需求量

            /********若【排风机类型】为变频+定频，且变频风机无故障且为远程*********/
            if(pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL && 
               pThis->pExAirFanVariate->xExAirFanErr == FALSE && pThis->pExAirFanVariate->xExAirFanRemote == TRUE) 
            {
                vSystem_ExAirFanBothCtrl(pThis);
            }
            /******若【排风机类型】为全定频，或者变频风机故障或本地******/
            if( pThis->eExAirFanType == TYPE_CONSTANT || 
               (pThis->eExAirFanType == TYPE_CONSTANT_VARIABLE && pThis->pExAirFanVariate != NULL &&
               (pThis->pExAirFanVariate->xExAirFanErr == TRUE || pThis->pExAirFanVariate->xExAirFanRemote == FALSE)) ) 
            {
                vSystem_ExAirFanConstantCtrl(pThis);
            }
        }
#if DEBUG_ENABLE > 0
        myprintf("vSystem_ExAirFanCtrl  ulExAirRequest_Vol %ld  ulExAirFanRequestTime %ld  ucConstantFanRequestNum %d  ucConstantFanOpenNum %d \n", 
        pThis->ulExAirRequest_Vol, pThis->ulExAirFanRequestTime, pThis->ucConstantFanRequestNum, pThis->ucConstantFanOpenNum);
#endif  
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
   
    uint32_t  ulExAirRequest_Vol = pThis->ulExAirRequest_Vol; 
    (void)vSystem_ExAirRequest_Vol(pThis);
    
#if DEBUG_ENABLE > 0
        myprintf("vSystem_ExAirSet_Vol usExAirRequest_Vol %d ucConstantFanOpenNum %d \n", pThis->ulExAirRequest_Vol, pThis->ucConstantFanOpenNum);
#endif    
    if(pThis->ulExAirRequest_Vol != ulExAirRequest_Vol)
    {
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
    
    if(pThis->usExAirFanFreq < pThis->usExAirFanMinFreq)
    {
        pThis->usExAirFanFreq = pThis->usExAirFanMinFreq;
        pThis->psBMS->usExAirFanFreq =pThis->usExAirFanMinFreq;
    }
    if(pThis->usExAirFanFreq > pThis->usExAirFanMaxFreq)
    {
        pThis->usExAirFanFreq = pThis->usExAirFanMaxFreq;
        pThis->psBMS->usExAirFanFreq =pThis->usExAirFanMaxFreq;
    }
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
void vSystem_ExAirFanErr(System* pt)
{
    uint8_t   n, nFanNum; 
    System*   pThis = (System*)pt;
    BMS*      psBMS = BMS_Core();
    
    ExAirFan* pExAirFan = NULL;
    for(n=0, nFanNum=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n];       
        if(pExAirFan->xExAirFanErr == FALSE)  
        {
            nFanNum++;    
        }
    }
    if(nFanNum == 0)  //无一台风机可用
    {
        pThis->xExFanErrFlag = TRUE;
        if(pThis->eSystemMode == MODE_AUTO)   //如果为自动切回手动
        {
            pThis->eSystemMode = MODE_MANUAL;
            psBMS->eSystemMode = MODE_MANUAL;
        }  
    }
    else
    {
        pThis->xExFanErrFlag = FALSE;
    }
    if(nFanNum<EX_AIR_FAN_NUM)   //有一台风机故障
    {
        vSystem_SetAlarm(pThis);
    }
    else
    {
        vSystem_DelAlarmRequst(pThis);
    }     
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanErr  xExFanErrFlag %d\n", pThis->xExFanErrFlag);
#endif    
    vSystem_ExAirFanCtrl(pThis); 
}

/*风机故障清除*/
void vSystem_ExAirFanErrClean(System* pt)
{
    uint8_t   n, nFanNum; 
    System*   pThis = (System*)pt;
    BMS*      psBMS = BMS_Core();
    
    ExAirFan* pExAirFan = NULL;
    if(psBMS->xExAirFanErrClean == FALSE)
    {
        return;
    }
    for(n=0, nFanNum=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n];       
        pExAirFan->xExAirFanErr     = FALSE;
        pExAirFan->xExAirFanCtrlErr = FALSE;
    }
    psBMS->xExAirFanErrClean = FALSE;
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanErrClean \n");
#endif    
}

/*风机远程本地切换*/
void vSystem_ExAirFanRemoteChange(System* pt)
{
    uint8_t   n, nFanNum; 
    System*   pThis = (System*)pt;
    BMS*      psBMS = BMS_Core();
    
    ExAirFan* pExAirFan = NULL;
    for(n=0, nFanNum=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n];       
        if(pExAirFan->xExAirFanRemote == TRUE)  
        {
            nFanNum++;    
        }
    }
    if(nFanNum == 0)  //无一台风机可用
    {
        if(pThis->eSystemMode == MODE_AUTO)   //如果为自动切回手动
        {
            pThis->eSystemMode = MODE_MANUAL;
            psBMS->eSystemMode = MODE_MANUAL;
        }  
    }
    vSystem_ExAirFanCtrl(pThis); 
    
#if DEBUG_ENABLE > 0
    myprintf("vSystem_ExAirFanRemoteChange \n");
#endif    
}