#include "system.h"
#include "systemctrl.h"

/*湿球温度*/
float HumiFactor[10] = {-6.569910081f, 0.63758824f, 9.240366999f, -0.00279f,     -5.159058621f,
                         0.579769533f, 0.0000134f,  2.601427878f, -0.234920132f,  0.00256f};


/*系统CO2浓度变化*/
void vSystem_CO2PPM(System* pt)
{
    uint8_t  n, ucCO2Num; 
    uint16_t usTotalCO2PPM = 0;   
    
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    CO2Sensor*   pCO2Sensor   = NULL;
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)pThis->psCO2SenList[n];
        if(pCO2Sensor->xCO2Error == FALSE)
        {
            usTotalCO2PPM = usTotalCO2PPM + pCO2Sensor->usAvgCO2PPM;
            ucCO2Num++;
        }  
    }
    if(ucCO2Num != 0)
    {
        pThis->usCO2PPM = usTotalCO2PPM / ucCO2Num;  //CO2平均浓度
    }
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
}

/*系统CO2传感器故障*/
void vSystem_CO2SensorErr(System* pt)
{
    uint8_t  n, ucCO2Num;  
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    CO2Sensor*   pCO2Sensor   = NULL;
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)pThis->psCO2SenList[n];
        if(pCO2Sensor->xCO2Error == FALSE)
        {
            ucCO2Num++;
        }  
    }
    //所有二氧化碳传感器均故障，下发一个总故障标志给空调机组
    pThis->xCO2SenErr = (ucCO2Num == 0) ? TRUE:FALSE;
    
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

/*系统室外温湿度变化*/
void vSystem_TempHumiOut(System* pt)
{
    uint8_t  n, ucTempNum, ucHumiNum; 
    int16_t  T, sTotalTemp = 0;
    uint16_t H, usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenOutList[n];
        if(pTempHumiSensor->xTempError == FALSE)
        {
            sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiError == FALSE)
        {
            usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;
            ucHumiNum++;
        }           
    }
    if(ucTempNum != 0)
    {
        pThis->sAmbientOut_T  = sTotalTemp / ucTempNum;  //室外平均环境温度
    }
    if(ucHumiNum != 0)
    {
        pThis->usAmbientOut_H = usTotalHumi / ucHumiNum;  //室外平均环境湿度
    }
    
//    for(n=0; n < MODULAR_ROOF_NUM; n++)
//    {
//        pModularRoof = pThis->psModularRoofList[n];
//        pModularRoof->sAmbientOut_T  = pThis->sAmbientOut_T;
//        pModularRoof->usAmbientOut_H = pThis->usAmbientOut_H;
//    }
    
//    T = pThis->sAmbientOut_T;
//    H = pThis->usAmbientOut_H;
//    
//    pThis->sAmbientOut_Ts = HumiFactor[0] + HumiFactor[1]*T + HumiFactor[2]*H + HumiFactor[3]*T*T +
//                            HumiFactor[4]*H*H + HumiFactor[5]*T*H + HumiFactor[6]*T*T*T + HumiFactor[7]*H*H*H +
//                            HumiFactor[8]*T*H*H +  HumiFactor[9]*T*T*H;
    
    vSystem_ChangeRunningMode(pThis);  //模式切换逻辑
}

/*系统室外温湿度传感器故障*/
void vSystem_TempHumiOutErr(System* pt)
{
    uint8_t  n, ucTempNum, ucHumiNum;
    System* pThis = (System*)pt;
    
    TempHumiSensor* pTempHumiSensor = NULL;
    
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenOutList[n];
        if(pTempHumiSensor->xTempError == FALSE)
        {
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiError == FALSE)
        {
            ucHumiNum++;
        }           
    }

    //传感器全部故障
    pThis->xTempSenOutErr = (ucTempNum == 0) ? TRUE:FALSE;
    pThis->xHumiSenOutErr = (ucHumiNum == 0) ? TRUE:FALSE;
    
    if(pThis->xTempSenOutErr == TRUE && pThis->xHumiSenOutErr == FALSE)
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

/*系统室内温湿度变化*/
void vSystem_TempHumiIn(System* pt)
{
    uint8_t  n, ucTempNum, ucHumiNum; 
    int16_t  sTotalTemp = 0;
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;

    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenInList[n]; 
        if(pTempHumiSensor->xTempError == FALSE)
        {
            sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiError == FALSE)
        {
            usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;
            ucHumiNum++;
        }            
    }

    if(ucTempNum != 0)
    {
         pThis->sAmbientIn_T  = sTotalTemp / ucTempNum;  //室内平均环境温度
    }
    if(ucHumiNum != 0)
    {
        pThis->usAmbientIn_H = usTotalHumi / ucHumiNum;  //室内平均环境湿度
    }
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->sAmbientIn_T  = pThis->sAmbientIn_T;
        pModularRoof->usAmbientIn_H = pThis->usAmbientIn_H;
    }
    vSystem_ChangeRunningMode(pThis);  //模式切换逻辑
}

/*系统室内温湿度传感器故障*/
void vSystem_TempHumiInErr(System* pt)
{
    uint8_t  n, ucTempNum, ucHumiNum;   
    
    int16_t  sTotalTemp = 0;
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenInList[n]; 
        if(pTempHumiSensor->xTempError == FALSE)
        {
            ucTempNum++;            
        }
        if(pTempHumiSensor->xHumiError == FALSE)
        {
            ucHumiNum++;
        }            
    }
    
    //传感器全部故障
    pThis->xTempSenInErr = (ucTempNum == 0) ? TRUE:FALSE;
    pThis->xHumiSenInErr = (ucHumiNum == 0) ? TRUE:FALSE;
    
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
