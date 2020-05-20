#include "system.h"
#include "systemctrl.h"

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

void vSystem_ExAirFanPreventTmrCallback(void* p_arg)
{
    System*   pThis = (System*)p_arg;
    pThis->sExAirFanPreventTmr = 0;
}

/*系统排风风机控制*/
void vSystem_CtrlExAirFan(System* pt)
{
    uint8_t   i, n, m, ucRunningNum;
    System*   pThis = (System*)pt;
    
    ExAirFan* pExAirFan        = NULL;   
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM - 1] = {NULL,};  
    
    //手动模式或制热工况下，手动开关风机
    if( (pThis->eSystemMode == MODE_MANUAL) || (pThis->eRunningMode == RUN_MODE_HEAT) )
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
         //不同工况切换至少满足【排风机防频繁调节时间】（默认1800s）
        if(pThis->sExAirFanPreventTmr !=0)
        {
            return;
        }
        m  = 0;
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
                if(pFanByRunTimeList[i]->Device.usRunTime > pFanByRunTimeList[m+1]->Device.usRunTime)
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
            //开启排风机防频繁调节定时器
            pThis->sExAirFanPreventTmr = sTimerRegist(TIMER_ONE_SHOT, pThis->usExAirFanTestTime, 
                                                      vSystem_ExAirFanPreventTmrCallback, pThis);
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
            //开启排风机防频繁调节定时器
            pThis->sExAirFanPreventTmr = sTimerRegist(TIMER_ONE_SHOT, pThis->usExAirFanTestTime, 
                                                      vSystem_ExAirFanPreventTmrCallback, pThis);
        }
    }  
}

/*系统风量检测稳定时间*/
void vSystem_ExAirFanTestTmrCallback(void* p_arg)
{
    System*   pThis = (System*)p_arg;
    uint8_t   ucExAirFanRequstNum = 0;
    
    pThis->sExAirFanTestTmr = 0;
    
    //（1）当系统排风需求量<=【排风机额定风量】（默认36000 m³/h）
    if(pThis->usExAirSet_Vol <= pThis->ulExAirFanRated_Vol)
    {
        ucExAirFanRequstNum = 0;
    }
    //当【排风机额定风量】<系统排风需求量<=【排风机额定风量】*2
    if( (pThis->ulExAirFanRated_Vol > pThis->usExAirSet_Vol) && (pThis->usExAirSet_Vol<= pThis->ulExAirFanRated_Vol*2) )
    {
        ucExAirFanRequstNum = 1;
    }
    //当【排风机额定风量】*2<系统排风需求量<=【排风机额定风量】*3
    if( (pThis->ulExAirFanRated_Vol*2 < pThis->usExAirSet_Vol) && (pThis->usExAirSet_Vol<= pThis->ulExAirFanRated_Vol*3) )
    {
        ucExAirFanRequstNum = 2;
    }
    //当【排风机额定风量】*3<系统排风需求量
    if(pThis->ulExAirFanRated_Vol*3 < pThis->usExAirSet_Vol) 
    {
        ucExAirFanRequstNum = 3;
    }
    //工况不一致
    if(pThis->ucExAirFanRequstNum != ucExAirFanRequstNum)
    {
        pThis->ucExAirFanRequstNum = ucExAirFanRequstNum; 
        //重启风量检测稳定时间定时器
        pThis->sExAirFanTestTmr = sTimerRegist(TIMER_ONE_SHOT, pThis->usExAirFanTestTime, 
                                               vSystem_ExAirFanTestTmrCallback, pThis);
    }
    //工况一致
    if(pThis->ucExAirFanRequstNum == ucExAirFanRequstNum)
    {
        vSystem_CtrlExAirFan(pThis);  //开始风机台数控制
    }
}

/*系统排风风机频率调节*/
void vSystem_AdjustExAirFanFreq(void* p_arg)
{
    uint8_t   n      = 0;
    uint16_t  usFreq = 0;
    
    System*   pThis = (System*)p_arg;
    ExAirFan* pExAirFanVariate = pThis->pExAirFanVariate;    //变频风机
    
    //手动模式
    if(pThis->eSystemMode == MODE_MANUAL)
    {
        usFreq = pExAirFanVariate->usRunningFreq;
        pExAirFanVariate->IDevFreq.setFreq(SUPER_PTR(pExAirFanVariate, IDevFreq), usFreq);  //设置频率 
    }
    
    //非手动模式
    if(pThis->eSystemMode != MODE_MANUAL)
    {
        pThis->sExAirFanFreqAdjustTmr = 0;
    
        usFreq = ( (pThis->usExAirSet_Vol)-(pThis->ulExAirFanRated_Vol)*(pThis->ucExAirFanRequstNum) ) / 
                   (pThis->ulExAirFanRated_Vol)*50;
        
        if(pExAirFanVariate->Device.eRunningState == STATE_STOP) //如果变频风机未开启，则先开启
        {
            pExAirFanVariate->IDevSwitch.switchOpen(SUPER_PTR(pExAirFanVariate, IDevSwitch));
        }
        pExAirFanVariate->IDevFreq.setFreq(SUPER_PTR(pExAirFanVariate, IDevFreq), usFreq);  //设置频率 
    }
}

/*系统排风需求量变化*/
void vSystem_ExAirSet_Vol(System* pt)
{
    System*   pThis = (System*)pt;
    uint8_t   ucExAirFanRequstNum = 0;
    
    //开启排风机频率调节时间定时器
    if(pThis->sExAirFanFreqAdjustTmr == 0)
    {
        pThis->sExAirFanFreqAdjustTmr = sTimerRegist(TIMER_ONE_SHOT, pThis->usExAirFanTestTime,
                                                     vSystem_AdjustExAirFanFreq, pThis);    //风机频率调节 
    }
    //首次检测
    if( pThis->sExAirFanTestTmr == 0)
    {
        //（1）当系统排风需求量<=【排风机额定风量】（默认36000 m³/h）
        if(pThis->usExAirSet_Vol <= pThis->ulExAirFanRated_Vol)
        {
            pThis->ucExAirFanRequstNum = 0;
        }
        //当【排风机额定风量】<系统排风需求量<=【排风机额定风量】*2
        if( (pThis->ulExAirFanRated_Vol > pThis->usExAirSet_Vol) && (pThis->usExAirSet_Vol<= pThis->ulExAirFanRated_Vol*2) )
        {
            pThis->ucExAirFanRequstNum = 1;
        }
        //当【排风机额定风量】*2<系统排风需求量<=【排风机额定风量】*3
        if( (pThis->ulExAirFanRated_Vol*2 < pThis->usExAirSet_Vol) && (pThis->usExAirSet_Vol<= pThis->ulExAirFanRated_Vol*3) )
        {
            pThis->ucExAirFanRequstNum = 2;
        }
        //当【排风机额定风量】*3<系统排风需求量
        if(pThis->ulExAirFanRated_Vol*3 < pThis->usExAirSet_Vol) 
        {
            pThis->ucExAirFanRequstNum = 3;
        }
        //开启风量检测稳定时间定时器
        pThis->sExAirFanTestTmr = sTimerRegist(TIMER_ONE_SHOT, pThis->usExAirFanTestTime,
                                                vSystem_ExAirFanTestTmrCallback, pThis); 
    }      
}


/*系统新风量变化*/
void vSystem_FreAir(System* pt)
{
    uint8_t  n = 0; 
    BOOL     xCommErr          = 0;  
    uint16_t usTotalFreAir_Vol = 0; 
           
    System* pThis = (System*)pt;
    ModularRoof* pModularRoof = NULL;
 
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pThis->psModularRoofList[n]->sMBSlaveDev.xOnLine != TRUE) //机组不在线
        {
            xCommErr = TRUE;    //机组通讯故障
            break;
        }
        usTotalFreAir_Vol +=  pModularRoof->usFreAir_Vol;
    }
    
    //【排风机控制模式】为实时新风量时
    if(pThis->eExAirFanCtrlMode == MODE_REAL_TIME)
    {
        if(xCommErr == FALSE)    //机组均通讯正常
        {
            //系统排风需求量=（机组一新风量+机组二新风量）*【排风百分比】（默认90）/100
            pThis->usExAirSet_Vol = usTotalFreAir_Vol * pThis->ucExAirRatio_1 / 100; 
        }
        if(xCommErr == TRUE)    //通讯故障
        {
            //系统排风需求量=当天目标新风量*【排风百分比1】（默认90）/100
            pThis->usExAirSet_Vol = pThis->usFreAirSet_Vol * pThis->ucExAirRatio_1 / 100;
        } 
    }
    //【排风机控制模式】为目标新风量时
    if(pThis->eExAirFanCtrlMode == MODE_REAL_TIME)
    {
         //系统排风需求量=当天目标新风量*【排风百分比1】（默认90）/100
         pThis->usExAirSet_Vol = pThis->usFreAirSet_Vol * pThis->ucExAirRatio_1 / 100;
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
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n];       //实例化对象 
        if(pExAirFan->eFanFreqType == VARIABLE_FREQ)  
        {
            pExAirFan->IDevFreq.setFreqRange(SUPER_PTR(pExAirFan, IDevFreq), usMinFreq, usMaxFreq);   
        } 
    }
}

