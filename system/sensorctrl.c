#include "system.h"
#include "systemctrl.h"

/*系统CO2浓度变化*/
void vSystem_CO2PPM(System* pt)
{
    uint8_t  n, ucCO2Num; 
    uint16_t usTotalCO2PPM, usCO2PPM;   
    
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    CO2Sensor*   pCO2Sensor   = NULL;
    
    for(n=0, ucCO2Num=0, usTotalCO2PPM=0, usCO2PPM=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)pThis->psCO2SenList[n];
        if(pCO2Sensor->xCO2SenErr == FALSE)
        {
            usTotalCO2PPM = usTotalCO2PPM + pCO2Sensor->usAvgCO2PPM;
            ucCO2Num++;
        }  
    }
    if(ucCO2Num != 0)
    {
        usCO2PPM = usTotalCO2PPM / ucCO2Num;  //CO2平均浓度
    }
    if(pThis->usCO2PPM != usCO2PPM && pThis->xCO2SenErr == FALSE)
    {
        pThis->usCO2PPM = usCO2PPM;
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->usCO2PPM = pThis->usCO2PPM;
        }
        //(2)当室内CO2浓度大于【CO2报警浓度指标值】（默认3000PPM），声光报警
        if( pThis->usCO2PPM >= pThis->usCO2PPMAlarm)  
        {
            vSystem_SetAlarm(pThis);
        }
        else
        {
            vSystem_DelAlarmRequst(pThis); //否则申请消除声光报警
        }     
#if DEBUG_ENABLE > 0
   // debug("vSystem_CO2PPM  pThis->usCO2PPM %d ucCO2Num %d\n", pThis->usCO2PPM, ucCO2Num);
#endif   
        vSystem_ExAirFanCtrl(pThis);  
    } 
}

/*系统CO2传感器故障*/
void vSystem_CO2SensorErr(System* pt)
{
    BOOL xCO2SenErr;
    uint8_t  n, ucCO2Num;  
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    CO2Sensor*   pCO2Sensor   = NULL;
    
    for(n=0, ucCO2Num=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)pThis->psCO2SenList[n];
        if(pCO2Sensor->xCO2SenErr == FALSE)
        {
            ucCO2Num++;
        }  
    }
    //所有二氧化碳传感器均故障，下发一个总故障标志给空调机组
    xCO2SenErr= (ucCO2Num == 0) ? TRUE:FALSE;
    if(pThis->xCO2SenErr != xCO2SenErr)
    {
        pThis->xCO2SenErr = xCO2SenErr;
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->xCO2SenErr = pThis->xCO2SenErr;
        }
        //(4)同类全部传感器通讯故障,声光报警
        if(pThis->xCO2SenErr == TRUE)
        {
            vSystem_SetAlarm(pThis);
        }
        else
        {
            vSystem_DelAlarmRequst(pThis); //否则申请消除声光报警
        }              
    }
#if DEBUG_ENABLE > 0
    //debug("vSystem_CO2SensorErr ucTempNum %d  xCO2SenErr %d \n", ucCO2Num, pThis->xCO2SenErr);
#endif     
    vSystem_CO2PPM(pThis);    
}

/*系统室外温湿度变化*/
void vSystem_TempHumiOut(System* pt)
{
    uint8_t  n, ucTempNum, ucHumiNum; 
    int16_t  sTotalTemp, sAmbientOut_T;
    uint16_t usTotalHumi, usAmbientOut_H;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    ucTempNum = 0;
    ucHumiNum = 0;
    
	if(pThis->xTempSenOutErr == TRUE)
	{
		return;
	}
    for(n=0,sTotalTemp=0,usTotalHumi=0, sAmbientOut_T=0, usAmbientOut_H=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenOutList[n];
        if(pTempHumiSensor->xTempSenErr == FALSE)
        {
            sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiSenErr == FALSE)
        {
            usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;
            ucHumiNum++;
        }           
    }
    if(ucTempNum != 0)
    {
        sAmbientOut_T  = sTotalTemp / ucTempNum;  //室外平均环境温度
    }
    if(ucHumiNum != 0)
    {
        usAmbientOut_H = usTotalHumi / ucHumiNum;  //室外平均环境湿度
    }

    if(pThis->sAmbientOut_T != sAmbientOut_T || pThis->usAmbientOut_H != usAmbientOut_H)
    {
        if(pThis->xTempSenOutErr == FALSE)
        {
            pThis->sAmbientOut_T  = sAmbientOut_T;
        }
        if(pThis->xHumiSenOutErr == FALSE)
        {
            pThis->usAmbientOut_H = usAmbientOut_H;
        } 
#if DEBUG_ENABLE > 0
    //debug("vSystem_TempHumiOut  sAmbientOut_T %d  usAmbientOut_H %d ucTempNum %d  ucHumiNum %d\n", 
    //         pThis->sAmbientOut_T,  pThis->usAmbientOut_H, ucTempNum, ucHumiNum);
#endif         
//        vSystem_ChangeUnitRunningMode(pThis);  //模式切换逻辑        
    }  
}

/*系统室外温湿度传感器故障*/
void vSystem_TempHumiOutErr(System* pt)
{
    BOOL xTempSenOutErr, xHumiSenOutErr;
    uint8_t  n, ucTempNum, ucHumiNum;
    
    System*         pThis = (System*)pt;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    ucTempNum = 0;
    ucHumiNum = 0;
    
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenOutList[n];
        if(pTempHumiSensor->xTempSenErr == FALSE)
        {
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiSenErr == FALSE)
        {
            ucHumiNum++;
        }           
    }
    //传感器全部故障
    xTempSenOutErr = (ucTempNum == 0) ? TRUE:FALSE;
    xHumiSenOutErr = (ucHumiNum == 0) ? TRUE:FALSE;
    
    if(pThis->xTempSenOutErr != xTempSenOutErr || pThis->xHumiSenOutErr != xHumiSenOutErr)
    {
        pThis->xTempSenOutErr = xTempSenOutErr;
        pThis->xHumiSenOutErr = xHumiSenOutErr;
        
        if(pThis->xTempSenOutErr == TRUE || pThis->xHumiSenOutErr == TRUE)
        {
            pThis->xTempHumiSenOutErr = TRUE;
            vSystem_SetAlarm(pThis);   //同类全部传感器通讯故障,声光报警
        }
        else
        {
            pThis->xTempHumiSenOutErr = FALSE;
            vSystem_DelAlarmRequst(pThis); //否则申请消除声光报警
        }   
    }
#if DEBUG_ENABLE > 0
    //debug("vSystem_TempHumiOutErr ucTempNum %d ucHumiNum %d xTempSenOutErr %d xHumiSenOutErr %d \n", 
    //         ucTempNum, ucHumiNum, pThis->xTempSenOutErr, pThis->xHumiSenOutErr);
#endif     
    vSystem_TempHumiOut(pThis);
}

/*系统室内温湿度变化*/
void vSystem_TempHumiIn(System* pt)
{
    uint8_t  n, ucTempNum, ucHumiNum; 
    int16_t  sTotalTemp, sAmbientIn_T;
    uint16_t usTotalHumi, usAmbientIn_H;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*    pModularRoof    = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;

    ucTempNum = 0;
    ucHumiNum = 0;
    
	if(pThis->xTempSenInErr == TRUE)
	{
		return;
	}
	
    for(n=0, sTotalTemp=0, usTotalHumi=0, sAmbientIn_T=0, usAmbientIn_H=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenInList[n]; 
        if(pTempHumiSensor->xTempSenErr == FALSE)
        {
            sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiSenErr == FALSE)
        {
            usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;
            ucHumiNum++;
        }            
    }
    if(ucTempNum != 0)
    {
         sAmbientIn_T  = sTotalTemp / ucTempNum;  //室内平均环境温度
    }
    if(ucHumiNum != 0)
    {
         usAmbientIn_H = usTotalHumi / ucHumiNum;  //室内平均环境湿度
    }
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->sAmbientIn_T  = sAmbientIn_T;
        pModularRoof->usAmbientIn_H = usAmbientIn_H;
    } 
    if(pThis->sAmbientIn_T != sAmbientIn_T || pThis->usAmbientIn_H != usAmbientIn_H)
    {
        if(pThis->xTempSenInErr == FALSE)
        {
            pThis->sAmbientIn_T  = sAmbientIn_T;
        }
        if(pThis->xHumiSenInErr == FALSE)
        {
            pThis->usAmbientIn_H = usAmbientIn_H;
        } 
#if DEBUG_ENABLE > 0
      //  debug("vSystem_TempHumiIn  sAmbientIn_T %d  usAmbientIn_H %d ucTempNum %d ucHumiNum %d  sTotalTemp %d usTotalHumi %d\n", 
      //            pThis->sAmbientIn_T, pThis->usAmbientIn_H, ucTempNum, ucHumiNum, sTotalTemp, usTotalHumi);
#endif   
        vSystem_ChangeUnitRunningMode(pThis);  //模式切换逻辑   
    } 
}

/*系统室内温湿度传感器故障*/
void vSystem_TempHumiInErr(System* pt)
{
    BOOL xTempSenInErr, xHumiSenInErr;
    uint8_t  n, ucTempNum, ucHumiNum;   
    
    int16_t  sTotalTemp = 0;
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    ucTempNum = 0;
    ucHumiNum = 0;
    
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenInList[n]; 
        if(pTempHumiSensor->xTempSenErr == FALSE)
        {
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiSenErr == FALSE)
        {
            ucHumiNum++;
        }            
    }
    //传感器全部故障
    xTempSenInErr = (ucTempNum == 0) ? TRUE:FALSE;
    xHumiSenInErr = (ucHumiNum == 0) ? TRUE:FALSE;
    
    if(pThis->xTempSenInErr != xTempSenInErr || pThis->xHumiSenInErr != xHumiSenInErr)
    {
        pThis->xTempSenInErr = xTempSenInErr;
        pThis->xHumiSenInErr = xHumiSenInErr;
        
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n];
            pModularRoof->xTempSenInErr = pThis->xTempSenInErr;
            pModularRoof->xHumiSenInErr = pThis->xHumiSenInErr; 
        }
        
        if(pThis->xTempSenInErr == TRUE && pThis->xHumiSenInErr == TRUE)
        {
            pThis->xTempHumiSenInErr = TRUE;
            vSystem_SetAlarm(pThis);   //同类全部传感器通讯故障,声光报警
        }
        else
        {
            pThis->xTempHumiSenInErr = FALSE;
            vSystem_DelAlarmRequst(pThis); //否则申请消除声光报警
        } 
    }
#if DEBUG_ENABLE > 0
    //debug("vSystem_TempHumiInErr ucTempNum %d ucHumiNum %d xTempSenInErr %d xHumiSenInErr %d \n", 
    //         ucTempNum, ucHumiNum, pThis->xTempSenInErr, pThis->xHumiSenInErr);
#endif  
    vSystem_TempHumiIn(pThis);
}
