#include "bms.h"
#include "system.h"
#include "md_event.h"
#include "md_modbus.h"
#include "md_timer.h"

/*************************************************************
*                         系统                               *
**************************************************************/
#define SYSTEM_POLL_TASK_PRIO    9

static System* psSystem = NULL;

/*系统排风机配置信息*/
sFanInfo ExAirFanSet[EX_AIR_FAN_NUM] = { {VARIABLE_FREQ, 25, 50, 1, 1, 1},
                                         {CONSTANT_FREQ, 0,   0, 0, 0, 2},
                                         {CONSTANT_FREQ, 0,   0, 0, 0, 3},
                                         {CONSTANT_FREQ, 0,   0, 0, 0, 4},
                                       };

/*系统开启*/
void vSystem_SwitchOpen(System* pt)
{
    System* pThis = (System*)pt;   
    if(pThis->eSystemMode)
    {
        
    }
    
    
    pThis->ucSwitchCmd = ON; 
}

/*系统关闭*/
void vSystem_SwitchClose(System* pt)
{
    System* pThis = (System*)pt;
    pThis->ucSwitchCmd = OFF; 
}

/*设定系统模式*/
void vSystem_SetRunningMode(System* pt, eRunningMode eRunMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    
    if(pThis->eRunningMode != eRunMode)
    {
        pThis->eRunningMode = eRunMode;  
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n]; 
            pModularRoof->eRunningMode = pThis->eRunningMode;
        }
    }
}

/*/调整系统模式*/
void vSystem_AdjustRunningMode(void* p_arg)
{
    System* pThis = (System*)p_arg;
    
    //若t2(默认5分钟)内温度没有达到目标温度t(ng1)±1.5℃
    if(pThis->sAmbientIn_T > pThis->sTargetTemp + pThis->ucAmbientInDeviat_T)  
    {
        if(pThis->eRunningMode == RUN_MODE_FAN)
        {
            vSystem_SetRunningMode(pThis, RUN_MODE_WET);    //切换为湿膜降温模式
        }
        else if(pThis->eRunningMode == RUN_MODE_WET)
        {
            vSystem_SetRunningMode(pThis, RUN_MODE_COOL);    //切换为降温模式
        }
    }
}

/*切换系统模式*/
void vSystem_ChangeRunningMode(System* pt)
{
    System* pThis = (System*)pt;
    
    /*注：室外干球温度: t(wg)   室外湿球温度: t(ws)      室内目标干球温度: t(ng1)   室内实际干球温度: t(ng2)
          养殖鸡数量：n         运行当天目标新风量:G
    */

     //制冷工况:  室内干球温度t(ng2)＞舍内温度目标要求温度t(ng1)，开启制冷工况
    if(pThis->sAmbientIn_T > pThis->sTargetTemp)  
    {
        //A. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTargetTemp >= pThis->sGrowUpTemp) 
        {
            //(1)室外干球温度t(wg)≤模式调节温度（默认23℃）
            if(pThis->sAmbientOut_T <= pThis->sAdjustModeTemp)  
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_FAN);    //开启送风模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_1 * 60, vSystem_AdjustRunningMode, pThis);
            }
            
            //(2)室外干球温度t(wg)＞模式调节温度（默认23℃） 且t（ws）+3 <= t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->sAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) <= pThis->sTargetTemp-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_WET);    //开启湿膜降温模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_2 * 60, vSystem_AdjustRunningMode, pThis);
            }
            
            //(3)室外干球温度t(wg)＞模式调节温度（默认23℃），且t（ws）+3＞t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->sAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) > pThis->sTargetTemp-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_COOL);    //开启降温模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_3 * 60, vSystem_AdjustRunningMode, pThis);
            }
        } 
        //B. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTargetTemp < pThis->sGrowUpTemp) 
        {
            
            
        }  
    }
    
    //制热工况:  室内干球温度t(ng2) <= 舍内温度目标要求温度t(ng1)，开启制热工况
    if(pThis->sAmbientIn_T <= pThis->sTargetTemp)     
    {
        pThis->eRunningMode = RUN_MODE_HEAT;    
    }  
}




/*设定系统目标温度值*/
void vSystem_SetTemp(System* pt, int16_t sTargetTemp)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->sTargetTemp = sTargetTemp;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->sTargetTemp = pThis->sTargetTemp;
    }
}

/*设定系统目标新风量*/
void vSystem_SetFreAir(System* pt, uint16_t usFreAirSet_Vol)
{
    uint8_t  n = 0; 
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
        pModularRoof->usCO2PPMSet = pThis->usCO2PPMSet;
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

/*系统CO2浓度变化*/
void vSystem_CO2PPM(System* pt)
{
    uint8_t  n = 0; 
    uint16_t usTotalCO2PPM = 0;   
    
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    CO2Sensor*   pCO2Sensor   = NULL;
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)pThis->psCO2SenList[n];     
        usTotalCO2PPM = usTotalCO2PPM + pCO2Sensor->usAvgCO2PPM; 
    }
    if( (usTotalCO2PPM / CO2_SEN_NUM) != pThis->usCO2PPM)
    {
        pThis->usCO2PPM = usTotalCO2PPM / CO2_SEN_NUM;  //CO2平均浓度
    }
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->usCO2PPM = pThis->usCO2PPM;
    }
}

/*系统室外温湿度变化*/
void vSystem_TempHumiOut(System* pt)
{
    uint8_t  n = 0; 
    int16_t  sTotalTemp = 0;
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenOutList[n];     
        sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
        usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;      
    }
    if( (sTotalTemp / TEMP_HUMI_SEN_OUT_NUM) != pThis->sAmbientOut_T)
    {
        pThis->sAmbientOut_T = sTotalTemp / TEMP_HUMI_SEN_OUT_NUM;  //室外平均环境温度
    }
     if( (usTotalHumi / TEMP_HUMI_SEN_OUT_NUM) != pThis->usAmbientOut_H)
    {
        pThis->usAmbientOut_H = usTotalHumi / TEMP_HUMI_SEN_OUT_NUM;  //室外平均环境湿度
    }
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->sAmbientOut_T  = pThis->sAmbientOut_T;
        pModularRoof->usAmbientOut_H = pThis->usAmbientOut_H;
    }
    vSystem_ChangeRunningMode(pThis);  //模式切换逻辑
}

/*系统室内温湿度变化*/
void vSystem_TempHumiIn(System* pt)
{
    uint8_t  n = 0; 
    int16_t  sTotalTemp = 0;
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenInList[n];     
        sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
        usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;      
    }
    if( (sTotalTemp / TEMP_HUMI_SEN_IN_NUM) != pThis->sAmbientIn_T)
    {
        pThis->sAmbientIn_T = sTotalTemp / TEMP_HUMI_SEN_OUT_NUM;  //室内平均环境温度
    }
     if( (usTotalHumi / TEMP_HUMI_SEN_OUT_NUM) != pThis->usAmbientIn_H)
    {
        pThis->usAmbientIn_H = usTotalHumi / TEMP_HUMI_SEN_OUT_NUM;  //室内平均环境湿度
    }
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->sAmbientIn_T  = pThis->sAmbientIn_T;
        pModularRoof->usAmbientIn_H = pThis->usAmbientIn_H;
    }
    
    vSystem_ChangeRunningMode(pThis);  //模式切换逻辑
}

void vSystem_ExAirFanPreventTmrCallback(void* p_arg)
{
    System*   pThis = (System*)p_arg;
    pThis->sExAirFanPreventTmr = 0;
}

/*系统排风风机台数控制*/
void vSystem_CtrlExAirFan(System* pt)
{
    uint8_t   n, m  = 0;
    System*   pThis = (System*)pt;
    
    ExAirFan* pExAirFan        = NULL;   
    ExAirFan* pFanByRunTimeList[EX_AIR_FAN_NUM - 1] = {NULL,};  
    
    //不同工况切换至少满足【排风机防频繁调节时间】（默认1800s）
    if(pThis->sExAirFanPreventTmr !=0)
    {
        return;
    }
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        if(pThis->psExAirFanList[n]->eFanFreqType != VARIABLE_FREQ)
        {
            pFanByRunTimeList[m] = pThis->psExAirFanList[n];   //所有定频风机
            m++;
        }
        
    }
    //定频风机排风机运行时间排序
    for(n=0; n < EX_AIR_FAN_NUM-1; n++)  
    {          
        for(m=0; m< EX_AIR_FAN_NUM-n-1; m++)
        {
            if(pFanByRunTimeList[m]->Device.usRunTime > pFanByRunTimeList[m+1]->Device.usRunTime)
            {
                           pExAirFan   = pFanByRunTimeList[m];
                pFanByRunTimeList[m]   = pFanByRunTimeList[m+1]; 
                pFanByRunTimeList[m+1] = pExAirFan;
            }
        }
    }
    for(n=0; n < pThis->ucExAirFanRequstNum; n++)
    {
        pExAirFan = pFanByRunTimeList[n];
        pExAirFan->IDevSwitch.switchOpen(SUPER_PTR(pExAirFan, IDevSwitch));
    }
    
    pThis->sExAirFanPreventTmr = sTimerRegist(TIMER_ONE_SHOT, pThis->usExAirFanTestTime, 
                                  vSystem_ExAirFanPreventTmrCallback, pThis);
    
}

/*系统排风需求量变化*/
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
    
    pThis->sExAirFanFreqAdjustTmr = 0;
    
    usFreq = ( (pThis->usExAirSet_Vol)-(pThis->ulExAirFanRated_Vol)*(pThis->ucExAirFanRequstNum) ) / 
               (pThis->ulExAirFanRated_Vol)*50;

    if(pExAirFanVariate->Device.eRunningState == RUN_STATE_STOP) //如果变频风机未开启，则先开启
    {
        pExAirFanVariate->IDevSwitch.switchOpen(SUPER_PTR(pExAirFanVariate, IDevSwitch));
    }
    pExAirFanVariate->IDevFreq.setFreq(SUPER_PTR(pExAirFanVariate, IDevFreq), usFreq);  //设置频率 
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
    uint16_t usTotalFreAir_Vol = 0; 
    BOOL     xCommErr          = 0;         
    
    System* pThis = (System*)pt;
    ModularRoof*       pModularRoof = NULL;
 
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        usTotalFreAir_Vol +=  pModularRoof->usFreAir_Vol;
        
        if(pThis->psModularRoofList[n]->sMBSlaveDev.ucOnLine != TRUE) //机组不在线
        {
            xCommErr = TRUE;    //机组通讯故障
        }
    }
    
    //【排风机控制模式】为实时新风量时
    if(pThis->eExAirFanCtrlMode == MODE_REAL_TIME)
    {
        if(xCommErr == FALSE)    //通讯正常
        {
            //系统排风需求量=（机组一新风量+机组二新风量）*【排风百分比】（默认90）/100
            pThis->usExAirSet_Vol = usTotalFreAir_Vol * pThis->ucExAirRatio_1 / 100; 
        }
        if(xCommErr == TRUE)    //通讯故障
        {
            //系统排风需求量=当天目标新风量*【排风百分比1】（默认90）/100
            pThis->usExAirSet_Vol = pThis->usTargetFreAir_Vol * pThis->ucExAirRatio_1 / 100;
        } 
    }
    
    //【排风机控制模式】为目标新风量时
    if(pThis->eExAirFanCtrlMode == MODE_REAL_TIME)
    {
         //系统排风需求量=当天目标新风量*【排风百分比1】（默认90）/100
         pThis->usExAirSet_Vol = pThis->usTargetFreAir_Vol * pThis->ucExAirRatio_1 / 100;
    }    
}


/*设置变频风机频率范围*/
void vSystem_SetExAirFanFreqRange(System* pt, uint16_t usMinFreq, uint16_t usMaxFreq)
{
    uint8_t  n = 0; 
    
    System*   pThis     = (System*)pt;
    ExAirFan* pExAirFan = NULL;
    IDevFreq* pDevFreq  = NULL;
    
    pThis->usExAirFanMinFreq = usMinFreq;
    pThis->usExAirFanMaxFreq = usMaxFreq;
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = pThis->psExAirFanList[n];       //实例化对象 
        if(pExAirFan->eFanFreqType == VARIABLE_FREQ)  
        {
            pDevFreq = SUPER_PTR(pExAirFan, IDevFreq);  //向上转型
            pExAirFan->IDevFreq.setFreqRange(pDevFreq, usMinFreq, usMaxFreq);   
        } 
    }
}


/*系统内部消息轮询*/
void vSystem_PollTask(void *p_arg)
{
    CPU_TS            ts = 0;
    OS_MSG_SIZE  msgSize = 0;

    OS_ERR           err = OS_ERR_NONE;
    
    BMS* psBMS = BMS_Core();
    
    while(DEF_TRUE)
	{
        sMsg* psMsg = (sMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);
        if( (USHORT*)psMsg->pvArg == (USHORT*)(&psBMS->System.sAmbientIn_T) )  //查看是哪个变量发生变化
        {
            
            
        }  
    }
}

/*创建系统内部消息轮询任务*/
BOOL xSystem_CreatePollTask(System* pt)
{
    OS_ERR    err = OS_ERR_NONE;
    System* pThis = (System*)pt;
    
    sTaskInfo*  psTaskInfo = &pThis->sTaskInfo;
   
    OS_PRIO             prio = psTaskInfo->ucPrio;
    OS_TCB*            p_tcb = (OS_TCB*)(&psTaskInfo->sTCB);  
    CPU_STK*      p_stk_base = (CPU_STK*)(psTaskInfo->usStk);
    CPU_STK_SIZE    stk_size = MB_DEV_POLL_TASK_STK_SIZE; 
    
    OSTaskCreate(&psTaskInfo->sTCB,
                  "vSystem_PollTask",
                  vSystem_PollTask,
                  (void*)0,
                  prio,
                  p_stk_base ,
                  stk_size / 10u,
                  stk_size,
                  0u,
                  0u,
                  0u,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
                  &err);
    return (err == OS_ERR_NONE);              
}

/*系统初始化*/
void vSystem_Init(System* pt)
{
    uint8_t n;
    System* pThis = (System*)pt;
    
    ModularRoof*    pModularRoof    = NULL;
    ExAirFan*       pExAirFan       = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    CO2Sensor*      pCO2Sensor      = NULL;
    DTU*            psDTU           = NULL;
    
    vModbusInit();
    
    pThis->psMBMasterInfo   = psMBGetMasterInfo();
    pThis->sTaskInfo.ucPrio = SYSTEM_POLL_TASK_PRIO;
  
    psDTU = DTU_new(psDTU);
    psDTU->init(psDTU, pThis->psMBMasterInfo);
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = (ModularRoof*)ModularRoof_new();
        pModularRoof->init(pModularRoof, pThis->psMBMasterInfo); //初始化
        
        pThis->psModularRoofList[n] = pModularRoof;
    }
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = (ExAirFan*)ExAirFan_new();  //实例化对象
        pExAirFan->init(pExAirFan, &ExAirFanSet[n]);
        
        if(pExAirFan->eFanFreqType == VARIABLE_FREQ)  
        {
            pThis->pExAirFanVariate = pExAirFan;
        }
        pThis->psExAirFanList[n] = pExAirFan;        
    }
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)CO2Sensor_new();     //实例化对象
        pCO2Sensor->Sensor.init( SUPER_PTR(pCO2Sensor, Sensor),  pThis->psMBMasterInfo); //向上转型，由子类转为父类
        
        pThis->psCO2SenList[n] = pCO2Sensor;
    }
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo);
        
        pThis->psTempHumiSenOutList[n] = pTempHumiSensor; 
    }
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo);
        
        pThis->psTempHumiSenInList[n] = pTempHumiSensor; 
    }
    
    
    CONNECT( &(BMS_Core()->sBMSValChange), &pThis->sTaskInfo.sTCB);  //绑定BMS变量变化事件
    
    xSystem_CreatePollTask(pThis); 
}

CTOR(System)   //系统构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init, vSystem_Init);
END_CTOR


System* System_Core()
{
    if(psSystem == NULL)
    {
        psSystem = (System*)System_new();
        psSystem->init(psSystem);
    }
    return psSystem;
}


