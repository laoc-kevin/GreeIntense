#include "bms.h"
#include "system.h"
#include "md_modbus.h"
#include "mbmap.h"

/*************************************************************
*                         BMS接口                            *
**************************************************************/
static BMS* psBMS = NULL;

/*通讯映射函数*/
BOOL xBMS_DevDataMapIndex(eDataType eDataType, USHORT usAddr, USHORT* psIndex)
{
    USHORT i = 0;
   
    switch(usAddr)
    {
        case 12	:  i = 0 ;  break;
        case 59	:  i = 1 ;  break;
        case 60	:  i = 2 ;  break;
        case 61	:  i = 3 ;  break;
        case 62	:  i = 4 ;  break;
        case 63	:  i = 5 ;  break;
           
        default:
    		return FALSE;
    	break;
    }
    *psIndex = i;
    return TRUE;  
}

/*BMS通讯数据表初始化*/
void vBMS_InitBMSCommData(BMS* pt)
{
    BMS*    pThis   = (BMS*)pt;
    System* pSystem = (System*)System_Core;
    
    
SLAVE_PBUF_INDEX_ALLOC()    
    
SLAVE_BEGIN_DATA_BUF(pThis->sBMS_RegHoldBuf, &pThis->sBMSCommData.sMBRegHoldTable) 
    
    SLAVE_REG_HOLD_DATA(0,  uint16,  0,  65535, 0, RO,  (void*)&pSystem->usFreAirSet_Vol)
    SLAVE_REG_HOLD_DATA(2,  uint16,  0,  65535, 0, RO,  (void*)&pSystem->usFreAirSet_Vol)
    SLAVE_REG_HOLD_DATA(10, uint16,  0,  65535, 0, RO,  (void*)&pSystem->usFreAirSet_Vol)
        
SLAVE_END_DATA_BUF(0, 10)    
    
    pThis->sBMSCommData.pxSlaveDataMapIndex = xBMS_DevDataMapIndex;         //绑定映射函数
    pThis->psBMSInfo->sMBCommInfo.psSlaveCurData = &pThis->sBMSCommData;
}

/*BMS数据监控*/
void vBMS_MonitorRegist(BMS* pt)
{
    BMS* pThis = (BMS*)pt;

    MONITOR(&pThis->System.eSystemMode,  &pThis->sValChange)
    MONITOR(&pThis->System.eRunningMode, &pThis->sValChange)
    
    MONITOR(&pThis->System.sTempSet,        &pThis->sValChange)
    MONITOR(&pThis->System.usFreAirSet_Vol, &pThis->sValChange)
    
    MONITOR(&pThis->System.usHumidityMin, &pThis->sValChange)
    MONITOR(&pThis->System.usHumidityMax, &pThis->sValChange)
    
    MONITOR(&pThis->System.usCO2AdjustThr_V,  &pThis->sValChange)
    MONITOR(&pThis->System.usCO2AdjustDeviat, &pThis->sValChange)
    
    MONITOR(&pThis->System.usExAirFanMinFreq, &pThis->sValChange) 
    MONITOR(&pThis->System.usExAirFanMaxFreq, &pThis->sValChange)
    
    MONITOR(&pThis->System.eExAirFanType, &pThis->sValChange)
}

void vBMS_Init(BMS* pt)
{
    BMS* pThis = (BMS*)pt;   
    pThis->psBMSInfo = psMBGetSlaveInfo();
    
    vBMS_InitBMSCommData(pThis);
    vBMS_MonitorRegist(pThis); 
}

CTOR(BMS)   //BMS构造函数
    FUNCTION_SETTING(init, vBMS_Init);
END_CTOR

BMS* BMS_Core()
{
    if(psBMS == NULL)
    {
        psBMS = (BMS*)BMS_new();
        psBMS->init(psBMS);
    }
    return psBMS;
}

