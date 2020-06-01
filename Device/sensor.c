#include "sensor.h"
#include "md_timer.h"

#define TMR_TICK_PER_SECOND             OS_CFG_TMR_TASK_RATE_HZ
#define SENSOR_TIME_OUT_S               1

#define SENSOR_CO2_PROTOCOL_TYPE_ID     0
#define SENSOR_TEMP_HUMI_PROTOCOL_TYPE_ID   0

/*************************************************************
*                         传感器                             *
**************************************************************/

/*向通讯主栈中注册设备*/
void vSensor_RegistDev(Sensor* pt)
{
    Sensor* pThis = (Sensor*)pt;
    (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBSlaveDev);
}

void vSensor_Init(Sensor* pt, sMBMasterInfo* psMBMasterInfo, eSensorType eSensorType)
{
    OS_ERR err = OS_ERR_NONE;
    
    Sensor*  pThis   = (Sensor*)pt;
    IDevCom* pDevCom = SUPER_PTR(pThis, IDevCom);
    
    pThis->psMBMasterInfo = psMBMasterInfo; //所属通讯主栈
    
    pThis->registMonitor(pThis);            //注册监控数据
    pDevCom->initDevCommData(pDevCom);      //通讯数据初始化
    vSensor_RegistDev(pThis);               //向通讯主栈中注册设备
    
    pThis->eSensorType = eSensorType;
    
    //传感器1s周期定时器
    (void)xTimerRegist(&pThis->sSensorTmr, 0, SENSOR_TIME_OUT_S, OS_OPT_TMR_PERIODIC, pThis->timeoutInd, pThis); 
}

ABS_CTOR(Sensor)  //传感器抽象类构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init, vSensor_Init);
END_CTOR

/*************************************************************
*                         CO2传感器                          *
**************************************************************/

/*通讯映射函数*/
BOOL xCO2Sensor_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID, USHORT usAddr, USHORT* psIndex)
{
    USHORT i = 0;
    switch(ucProtocolID)
	{
        case SENSOR_CO2_PROTOCOL_TYPE_ID:
            if(eDataType == RegHoldData)
            {
                switch(usAddr)
                {
                    case 1	  :  i = 0 ;  break;
                    case 0X30 :  i = 1 ;  break;
                       
                    default:
                		return FALSE;
                	break;
                }
            }
        break;
        default: break;
	}
    *psIndex = i;
    return TRUE;
}

/*通讯数据表初始化*/
void vCO2Sensor_InitDevCommData(IDevCom* pt)
{
    Sensor*      pThis = SUB_PTR(pt, IDevCom, Sensor);
    CO2Sensor* pCO2Sen = SUB_PTR(pThis, Sensor, CO2Sensor);
   
    sMBDevDataTable* psMBRegHoldTable = &pThis->sDevCommData.sMBRegHoldTable; 
    sMBTestDevCmd*            psMBCmd = &pThis->sDevCommData.sMBDevCmdTable;
    
    
MASTER_PBUF_INDEX_ALLOC()
MASTER_TEST_CMD_INIT(psMBCmd, 0x30, READ_REG_HOLD, pThis->sMBSlaveDev.ucDevAddr, FALSE)  
    
    /******************************保持寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(pThis->sSensor_RegHoldBuf, psMBRegHoldTable)
    
    MASTER_REG_HOLD_DATA(1,   uint16, MIN_CO2_PPM, MAX_CO2_PPM, 0, RO, 0, (void*)&pCO2Sen->usCO2PPM)
    MASTER_REG_HOLD_DATA(0x30, uint8, 1, 255, 0, RW, pThis->sMBSlaveDev.ucDevAddr, (void*)&pThis->sMBSlaveDev.ucDevAddr)
        
MASTER_END_DATA_BUF(1, 0x30)
    
    pCO2Sen->usMaxPPM = MAX_CO2_PPM;
    pCO2Sen->usMinPPM = MIN_CO2_PPM;
    
    pThis->sDevCommData.ucProtocolID = SENSOR_CO2_PROTOCOL_TYPE_ID;
    pThis->sDevCommData.pxDevDataMapIndex = xCO2Sensor_DevDataMapIndex;    //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}


void vCO2Sensor_TimeoutInd(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
    Sensor*       pThis = (Sensor*)p_arg;
    CO2Sensor* psCO2Sen = SUB_PTR(pThis, Sensor, CO2Sensor);
    
    uint8_t*        pcSampleIndex = &pThis->ucSampleIndex;
    
    //对传感器参数进行采样
    if(*pcSampleIndex< SENSOR_SAMPLE_NUM)
    {
        psCO2Sen->usTotalCO2PPM = psCO2Sen->usTotalCO2PPM - psCO2Sen->usSampleCO2PPM[*pcSampleIndex];
        psCO2Sen->usSampleCO2PPM[*pcSampleIndex]  = psCO2Sen->usCO2PPM;
        psCO2Sen->usTotalCO2PPM  = psCO2Sen->usTotalCO2PPM + psCO2Sen->usCO2PPM;
     
        (*pcSampleIndex)++;
    }
    else if(*pcSampleIndex == SENSOR_SAMPLE_NUM)
    {
        *pcSampleIndex = 0;
    }
    psCO2Sen->usAvgCO2PPM  = psCO2Sen->usTotalCO2PPM  / SENSOR_SAMPLE_NUM;   //取平均值
    
    //判断传感器是否故障
    if( (psCO2Sen->usAvgCO2PPM < psCO2Sen->usMinPPM) || (psCO2Sen->usAvgCO2PPM > psCO2Sen->usMaxPPM) ||
        (psCO2Sen->Sensor.sMBSlaveDev.xOnLine == FALSE) )
    {
        psCO2Sen->xCO2SenErr = TRUE;
    }
    else
    {
        psCO2Sen->xCO2SenErr = FALSE;
    }
}

/* CO2传感器数据监控*/
void vCO2Sensor_RegistMonitor(Sensor* pt)
{
    OS_ERR err = OS_ERR_NONE;
    CO2Sensor* pThis = SUB_PTR(pThis, Sensor, CO2Sensor);
    
    OSSemCreate( &(pThis->Sensor.sValChange), "sValChange", 0, &err );  //事件消息量初始化

    MONITOR(&pThis->usAvgCO2PPM, &pThis->Sensor.sValChange)
    MONITOR(&pThis->xCO2SenErr,   &pThis->Sensor.sValChange)
}

CTOR(CO2Sensor)   //CO2传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.registMonitor, vCO2Sensor_RegistMonitor);
    FUNCTION_SETTING(Sensor.IDevCom.initDevCommData, vCO2Sensor_InitDevCommData);
    FUNCTION_SETTING(Sensor.timeoutInd, vCO2Sensor_TimeoutInd);
END_CTOR


/*************************************************************
*                         温湿度传感器                       *
**************************************************************/

/*通讯映射函数*/
BOOL xTempHumiSensor_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID, USHORT usAddr, USHORT* psIndex)
{
    USHORT i = 0;
    switch(ucProtocolID)
	{
        case 0:
            if(eDataType == RegHoldData)
            {
                switch(usAddr)
                {
                    case 1	  :  i = 0 ;  break;
                    case 2	  :  i = 1 ;  break;
                    case 0X30 :  i = 2 ;  break;
                       
                    default:
                		return FALSE;
                	break;
                }
            }
        break;
        default: break;
	}
    *psIndex = i;
    return TRUE; 
}

/*通讯数据表初始化*/
void vTempHumiSensor_InitDevCommData(IDevCom* pt)
{
    Sensor*                pThis = SUB_PTR(pt, IDevCom, Sensor);
    TempHumiSensor* pTempHumiSen = SUB_PTR(pThis, Sensor, TempHumiSensor);
    
    sMBDevDataTable* psMBRegHoldTable = &pThis->sDevCommData.sMBRegHoldTable; 
    sMBTestDevCmd*            psMBCmd = &pThis->sDevCommData.sMBDevCmdTable;
  
    
MASTER_PBUF_INDEX_ALLOC()
MASTER_TEST_CMD_INIT(psMBCmd, 0x30, READ_REG_HOLD, pThis->sMBSlaveDev.ucDevAddr, FALSE)  
    
    /******************************保持寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(pThis->sSensor_RegHoldBuf, psMBRegHoldTable) 
    
    if(pThis->eSensorType == TYPE_TEMP_HUMI_IN)  
    {
        MASTER_REG_HOLD_DATA(1, int16,  MIN_IN_TEMP, MAX_IN_TEMP, 0, RO, 0, (void*)&pTempHumiSen->sTemp)
        MASTER_REG_HOLD_DATA(2, uint16, MIN_HUMI,    MAX_HUMI, 0, RO, 0, (void*)&pTempHumiSen->usHumi)
    }        
    if(pThis->eSensorType == TYPE_TEMP_HUMI_OUT)  
    {
        MASTER_REG_HOLD_DATA(1, int16,  MIN_OUT_TEMP, MAX_OUT_TEMP, 0, RO, 0, (void*)&pTempHumiSen->sTemp)
        MASTER_REG_HOLD_DATA(2, uint16, MIN_HUMI,    MAX_HUMI, 0, RO, 0, (void*)&pTempHumiSen->usHumi)
    }    
    MASTER_REG_HOLD_DATA(0x30, uint8, 1, 255, 0, RW, pThis->sMBSlaveDev.ucDevAddr, (void*)&pThis->sMBSlaveDev.ucDevAddr)
        
MASTER_END_DATA_BUF(1, 0x30)
    
    if(pThis->eSensorType == TYPE_TEMP_HUMI_IN)  
    {
         pTempHumiSen->sMaxTemp = MAX_IN_TEMP;
         pTempHumiSen->sMinTemp = MIN_IN_TEMP; 
    }        
    if(pThis->eSensorType == TYPE_TEMP_HUMI_OUT)  
    {
        pTempHumiSen->sMaxTemp = MAX_OUT_TEMP;
        pTempHumiSen->sMinTemp = MIN_OUT_TEMP; 
    }   
    pTempHumiSen->usMaxHumi = MAX_HUMI;
    pTempHumiSen->usMinHumi = MIN_HUMI;
    
    pThis->sDevCommData.ucProtocolID = SENSOR_TEMP_HUMI_PROTOCOL_TYPE_ID;
    pThis->sDevCommData.pxDevDataMapIndex = xTempHumiSensor_DevDataMapIndex;    //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}

void vTempHumiSensor_TimeoutInd(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
    Sensor*                pThis = (Sensor*)p_arg;
    TempHumiSensor* pTempHumiSen = SUB_PTR(pThis, Sensor, TempHumiSensor);
    
    uint8_t*        pcSampleIndex = &pThis->ucSampleIndex;
    
    //对传感器参数进行采样
    if(*pcSampleIndex< SENSOR_SAMPLE_NUM)
    {
        pTempHumiSen->sTotalTemp  = pTempHumiSen->sTotalTemp - pTempHumiSen->sSampleTemp[*pcSampleIndex];
        pTempHumiSen->usTotalHumi = pTempHumiSen->usTotalHumi - pTempHumiSen->usSampleHumi[*pcSampleIndex];
        
        pTempHumiSen->sSampleTemp[*pcSampleIndex]  = pTempHumiSen->sTemp;
        pTempHumiSen->usSampleHumi[*pcSampleIndex] = pTempHumiSen->usHumi;
        
        pTempHumiSen->sTotalTemp  = pTempHumiSen->sTotalTemp + pTempHumiSen->sTemp;
        pTempHumiSen->usTotalHumi = pTempHumiSen->usTotalHumi + pTempHumiSen->usHumi;
        (*pcSampleIndex)++;
    }
    else if(*pcSampleIndex == SENSOR_SAMPLE_NUM)
    {
        *pcSampleIndex = 0;
    }
    pTempHumiSen->sAvgTemp  = pTempHumiSen->sTotalTemp  / SENSOR_SAMPLE_NUM;  //取平均值
    pTempHumiSen->usAvgHumi = pTempHumiSen->usTotalHumi / SENSOR_SAMPLE_NUM;
    
    //判断传感器是否故障
    if( (pTempHumiSen->sAvgTemp < pTempHumiSen->sMinTemp) || (pTempHumiSen->sAvgTemp > pTempHumiSen->sMaxTemp) ||
        (pTempHumiSen->Sensor.sMBSlaveDev.xOnLine == FALSE) ) 
    {
        pTempHumiSen->xTempSenErr = TRUE;
    }
    else
    {
        pTempHumiSen->xTempSenErr = FALSE;
    }
    
     //判断传感器是否故障
    if( (pTempHumiSen->usAvgHumi < pTempHumiSen->usMinHumi) || (pTempHumiSen->usAvgHumi > pTempHumiSen->usMaxHumi) ||
        (pTempHumiSen->Sensor.sMBSlaveDev.xOnLine == FALSE) )
    {
        pTempHumiSen->xHumiSenErr = TRUE;
    }
    else
    {
        pTempHumiSen->xHumiSenErr = FALSE;
    }
}

/*温湿度传感器数据监控*/
void vTempHumiSensor_MonitorRegist(Sensor* pt)
{
    OS_ERR err = OS_ERR_NONE;
    TempHumiSensor* pThis = SUB_PTR(pThis, Sensor, TempHumiSensor);

    OSSemCreate( &(pThis->Sensor.sValChange), "sValChange", 0, &err );  //事件消息量初始化
    
    MONITOR(&pThis->sAvgTemp,   &pThis->Sensor.sValChange)
    MONITOR(&pThis->xTempSenErr, &pThis->Sensor.sValChange)
    
    MONITOR(&pThis->usAvgHumi,  &pThis->Sensor.sValChange)
    MONITOR(&pThis->xHumiSenErr, &pThis->Sensor.sValChange)
}

CTOR(TempHumiSensor)   //温湿度传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.registMonitor,           vTempHumiSensor_MonitorRegist);
    FUNCTION_SETTING(Sensor.IDevCom.initDevCommData, vTempHumiSensor_InitDevCommData);
    FUNCTION_SETTING(Sensor.timeoutInd,              vTempHumiSensor_TimeoutInd);
END_CTOR



