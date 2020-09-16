#include "sensor.h"
#include "md_timer.h"

#define TMR_TICK_PER_SECOND                 OS_CFG_TMR_TASK_RATE_HZ
#define SENSOR_TIME_OUT_S                   2
#define SENSOR_TIME_OUT_DELAY_S             60

#define SENSOR_CO2_PROTOCOL_TYPE_ID         0
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

void vSensor_Init(Sensor* pt, sMBMasterInfo* psMBMasterInfo, eSensorType eSensorType, UCHAR ucDevAddr, uint8_t ucDevIndex)
{
    OS_ERR err = OS_ERR_NONE;
    
    Sensor*  pThis   = (Sensor*)pt;
    IDevCom* pDevCom = SUPER_PTR(pThis, IDevCom);
    
    pThis->psMBMasterInfo        = psMBMasterInfo; //所属通讯主栈
    pThis->sMBSlaveDev.ucDevAddr = ucDevAddr;
    pThis->eSensorType           = eSensorType; 
    pThis->Device.ucDevIndex     = ucDevIndex;
    
    pThis->registMonitor(pThis);            //注册监控数据
    
    pDevCom->initDevCommData(pDevCom);      //通讯数据初始化
    vSensor_RegistDev(pThis);               //向通讯主栈中注册设备

    //传感器1s周期定时器,延时启动
    (void)xTimerRegist(&pThis->sSensorTmr, SENSOR_TIME_OUT_DELAY_S, SENSOR_TIME_OUT_S, 
                        OS_OPT_TMR_PERIODIC, pThis->timeoutInd, pThis, FALSE);   
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
            if(eDataType == RegInputData)
            {
                switch(usAddr)
                {
                    case 0   :  i = 0;  break; 
//                    case 528 :  i = 1;  break;					
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
    Sensor*        pThis   = SUB_PTR(pt, IDevCom, Sensor);
    CO2Sensor*     pCO2Sen = SUB_PTR(pThis, Sensor, CO2Sensor);
    sMBTestDevCmd* psMBCmd = &pThis->sDevCommData.sMBDevCmdTable;
    
MASTER_PBUF_INDEX_ALLOC()
MASTER_TEST_CMD_INIT(psMBCmd, 528, READ_REG_HOLD, pCO2Sen->usDataOffset, TRUE)  
    
    /******************************输入寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(&pCO2Sen->sSensor_RegInBuf, &pThis->sDevCommData.sMBRegInTable;)
    
    MASTER_REG_IN_DATA(0,   uint16, 0, 65535, RO, 1, (void*)&pCO2Sen->usCO2PPM)
//	MASTER_REG_IN_DATA(528, uint16, 0, 65535, RO, 1, (void*)&pCO2Sen->usDataOffset)
//      
MASTER_END_DATA_BUF(0, 1)
     
    pCO2Sen->usMaxPPM = MAX_CO2_PPM;
    pCO2Sen->usMinPPM = MIN_CO2_PPM;
    
    pThis->sDevCommData.ucProtocolID = SENSOR_CO2_PROTOCOL_TYPE_ID;
    pThis->sDevCommData.pxDevDataMapIndex = xCO2Sensor_DevDataMapIndex;    //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData); 
}

void vCO2Sensor_TimeoutInd(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
    Sensor*    pThis    = (Sensor*)p_arg;
    CO2Sensor* psCO2Sen = SUB_PTR(pThis, Sensor, CO2Sensor);
    
    uint8_t* pcSampleIndex = &pThis->ucSampleIndex;
    
    //判断传感器是否故障
    if( (psCO2Sen->usAvgCO2PPM < psCO2Sen->usMinPPM) || (psCO2Sen->usAvgCO2PPM > psCO2Sen->usMaxPPM) ||
        (psCO2Sen->Sensor.sMBSlaveDev.xOnLine == FALSE) )
    {
        psCO2Sen->xCO2SenErr = TRUE;
		psCO2Sen->usCO2PPM = 0;
		psCO2Sen->usAvgCO2PPM = 0;
    }
    else
    {
        psCO2Sen->xCO2SenErr = FALSE;
    }
    
    //对传感器参数进行采样
    if(*pcSampleIndex < SENSOR_SAMPLE_NUM 
		&& psCO2Sen->usCO2PPM >  psCO2Sen->usMinPPM && psCO2Sen->usCO2PPM < psCO2Sen->usMaxPPM)
    {
        psCO2Sen->ulTotalCO2PPM = psCO2Sen->ulTotalCO2PPM - psCO2Sen->usSampleCO2PPM[*pcSampleIndex];
        psCO2Sen->usSampleCO2PPM[*pcSampleIndex]  = psCO2Sen->usCO2PPM;
        psCO2Sen->ulTotalCO2PPM  = psCO2Sen->ulTotalCO2PPM + psCO2Sen->usCO2PPM;
     
        (*pcSampleIndex)++;
    }
    else if(*pcSampleIndex == SENSOR_SAMPLE_NUM)
    {
        psCO2Sen->usAvgCO2PPM  = psCO2Sen->ulTotalCO2PPM  / SENSOR_SAMPLE_NUM;   //取平均值
        *pcSampleIndex = 0;
    } 
       
#if DEBUG_ENABLE > 0
//    myprintf("vCO2Sensor_TimeoutInd ucDevAddr %d  xOnLine %d  usCO2PPM %d usAvgCO2PPM %d\n", 
//	psCO2Sen->Sensor.sMBSlaveDev.ucDevAddr ,psCO2Sen->Sensor.sMBSlaveDev.xOnLine , psCO2Sen->usCO2PPM, psCO2Sen->usAvgCO2PPM);
#endif 
}

/* CO2传感器数据监控*/
void vCO2Sensor_RegistMonitor(Sensor* pt)
{
    OS_ERR err = OS_ERR_NONE;
    CO2Sensor* pThis = SUB_PTR(pThis, Sensor, CO2Sensor);
    
    OSSemCreate( &(pThis->Sensor.sValChange), "sValChange", 0, &err );  //事件消息量初始化

    MONITOR(&pThis->usAvgCO2PPM, uint16, &pThis->Sensor.sValChange)
    MONITOR(&pThis->xCO2SenErr,   uint8, &pThis->Sensor.sValChange)
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
        case SENSOR_TEMP_HUMI_PROTOCOL_TYPE_ID:
            if(eDataType == RegHoldData)
            {
                switch(usAddr)
                {
                    case 0	  :  i = 0 ;  break;                   
                    case 1	  :  i = 1 ;  break; 
					case 2	  :  i = 2 ;  break; 
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
    Sensor*         pThis        = SUB_PTR(pt, IDevCom, Sensor);
    TempHumiSensor* pTempHumiSen = SUB_PTR(pThis, Sensor, TempHumiSensor);  
    sMBTestDevCmd*  psMBCmd      = &pThis->sDevCommData.sMBDevCmdTable;
  
MASTER_PBUF_INDEX_ALLOC()
MASTER_TEST_CMD_INIT(psMBCmd, 0x30, READ_REG_HOLD, pThis->sMBSlaveDev.ucDevAddr, TRUE)  
    
    /******************************保持寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(&pTempHumiSen->sSensor_RegHoldBuf, &pThis->sDevCommData.sMBRegHoldTable) 
    
    MASTER_REG_HOLD_DATA(0, int16,  -32768, 32767, 0, RO,  1, (void*)&pTempHumiSen->sTemp)
    MASTER_REG_HOLD_DATA(1, uint16,     0, 65535, 0, RO, 10, (void*)&pTempHumiSen->usHumi) 
  
MASTER_END_DATA_BUF(0, 1)
    
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
    
    uint8_t* pcSampleIndex   = &pThis->ucSampleIndex;
    uint8_t* pcSampleIndex_2 = &pThis->ucSampleIndex_2;
    
    //判断传感器是否故障
    if( (pTempHumiSen->sAvgTemp < pTempHumiSen->sMinTemp) || (pTempHumiSen->sAvgTemp > pTempHumiSen->sMaxTemp) ||
        (pTempHumiSen->Sensor.sMBSlaveDev.xOnLine == FALSE) ) 
    {
        pTempHumiSen->xTempSenErr = TRUE;
		pTempHumiSen->sTemp = 0;
		pTempHumiSen->sAvgTemp = 0;
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
		pTempHumiSen->usHumi = 0;
		pTempHumiSen->usAvgHumi = 0;
    }
    else
    {
        pTempHumiSen->xHumiSenErr = FALSE;
    }
    
    //对传感器参数进行采样
    if(*pcSampleIndex< SENSOR_SAMPLE_NUM && 
		pTempHumiSen->sTemp < pTempHumiSen->sMaxTemp && pTempHumiSen->sTemp > pTempHumiSen->sMinTemp)
    {
        pTempHumiSen->lTotalTemp  = pTempHumiSen->lTotalTemp - pTempHumiSen->sSampleTemp[*pcSampleIndex];
        pTempHumiSen->sSampleTemp[*pcSampleIndex]  = pTempHumiSen->sTemp;
        pTempHumiSen->lTotalTemp  = pTempHumiSen->lTotalTemp + pTempHumiSen->sTemp;
        (*pcSampleIndex)++;
    }
    else if(*pcSampleIndex == SENSOR_SAMPLE_NUM)
    {
        pTempHumiSen->sAvgTemp = pTempHumiSen->lTotalTemp / SENSOR_SAMPLE_NUM;  //取平均值
        *pcSampleIndex = 0;
    }
    
    if(*pcSampleIndex_2 < SENSOR_SAMPLE_NUM)
    {
        pTempHumiSen->ulTotalHumi = pTempHumiSen->ulTotalHumi - pTempHumiSen->usSampleHumi[*pcSampleIndex_2];
        pTempHumiSen->usSampleHumi[*pcSampleIndex_2] = pTempHumiSen->usHumi;
        pTempHumiSen->ulTotalHumi = pTempHumiSen->ulTotalHumi + pTempHumiSen->usHumi;
        (*pcSampleIndex_2)++;
    }
    else if(*pcSampleIndex_2 == SENSOR_SAMPLE_NUM)
    {
        pTempHumiSen->usAvgHumi = pTempHumiSen->ulTotalHumi / SENSOR_SAMPLE_NUM;
        *pcSampleIndex_2 = 0;
    }   
    
#if DEBUG_ENABLE > 0
//    if(pTempHumiSen->Sensor.sMBSlaveDev.ucDevAddr == 7)
//    {
//        myprintf("vTempHumiSensor_TimeoutInd sTemp %d  usHumi %d\n", pTempHumiSen->sTemp, pTempHumiSen->usHumi);
//    } 
#endif     
}

/*温湿度传感器数据监控*/
void vTempHumiSensor_RegistMonitor(Sensor* pt)
{
    OS_ERR err = OS_ERR_NONE;
    TempHumiSensor* pThis = SUB_PTR(pThis, Sensor, TempHumiSensor);

    OSSemCreate( &(pThis->Sensor.sValChange), "sValChange", 0, &err );  //事件消息量初始化
    
    MONITOR(&pThis->sAvgTemp,    int16, &pThis->Sensor.sValChange)
    MONITOR(&pThis->xTempSenErr, uint8, &pThis->Sensor.sValChange)
    
    MONITOR(&pThis->usAvgHumi,  uint16, &pThis->Sensor.sValChange)
    MONITOR(&pThis->xHumiSenErr, uint8, &pThis->Sensor.sValChange)
}

CTOR(TempHumiSensor)   //温湿度传感器构造函数
    SUPER_CTOR(Sensor);

    FUNCTION_SETTING(Sensor.registMonitor,           vTempHumiSensor_RegistMonitor);
    FUNCTION_SETTING(Sensor.IDevCom.initDevCommData, vTempHumiSensor_InitDevCommData);
    FUNCTION_SETTING(Sensor.timeoutInd,              vTempHumiSensor_TimeoutInd);
END_CTOR



