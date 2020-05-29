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
    
    ModularRoof* pModularRoof_1 = (ModularRoof*)(pSystem->psModularRoofList[0]);
    ModularRoof* pModularRoof_2 = (ModularRoof*)(pSystem->psModularRoofList[1]);
    
SLAVE_PBUF_INDEX_ALLOC()    
    
SLAVE_BEGIN_DATA_BUF(pThis->sBMS_RegHoldBuf, &pThis->sBMSCommData.sMBRegHoldTable) 
    
    SLAVE_REG_HOLD_DATA(0,  uint16,    0,  65535, RO, 1, (void*)&pSystem->usUnitID)
    SLAVE_REG_HOLD_DATA(1,  uint16,    0,  65535, RO, 1, (void*)&pSystem->usProtocolVer)
    SLAVE_REG_HOLD_DATA(2,  uint16,    0,      3, RW, 1, (void*)&pThis->eSystemMode)
    SLAVE_REG_HOLD_DATA(3,  uint16,    0,      4, RW, 1, (void*)&pThis->eRunningMode)
    SLAVE_REG_HOLD_DATA(6,   int16,   -2,     56, RW, 1, (void*)&pSystem->sChickenGrowDays)
        
    SLAVE_REG_HOLD_DATA(7,   int16, -300,    450, RW, 1, (void*)&pThis->sTempSet)   
    SLAVE_REG_HOLD_DATA(8,  uint16,    0,      1, RW, 1, (void*)&pThis->usFreAirSet_Vol_H)
    SLAVE_REG_HOLD_DATA(9,  uint16,    0,  64464, RW, 1, (void*)&pThis->usFreAirSet_Vol_L)
    SLAVE_REG_HOLD_DATA(10, uint16,    0,    450, RW, 1, (void*)&pSystem->usEnergyTemp)
        
    SLAVE_REG_HOLD_DATA(12, uint16,    0,    550, RW, 1, (void*)&pSystem->usSupAirMax_T)     
    SLAVE_REG_HOLD_DATA(13, uint16,    0,    100, RW, 1, (void*)&pSystem->usHumidityMax)   
    SLAVE_REG_HOLD_DATA(14, uint16,    0,    100, RW, 1, (void*)&pSystem->usHumidityMin)
    SLAVE_REG_HOLD_DATA(20, uint16,    0,   3500, RW, 1, (void*)&pThis->usCO2AdjustThr_V)    
    SLAVE_REG_HOLD_DATA(21, uint16,    0,   3500, RW, 1, (void*)&pThis->usCO2AdjustDeviat) 
        
    SLAVE_REG_HOLD_DATA(22, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2ExAirDeviat_1) 
    SLAVE_REG_HOLD_DATA(23, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2ExAirDeviat_2)
    SLAVE_REG_HOLD_DATA(24, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2PPMAlarm)         
    SLAVE_REG_HOLD_DATA(29, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanMinFreq)   
    SLAVE_REG_HOLD_DATA(30, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanMaxFreq)
        
    SLAVE_REG_HOLD_DATA(31, uint16,    0,      1, RW, 1, (void*)&pThis->usExAirFanRated_Vol_H)    
    SLAVE_REG_HOLD_DATA(32, uint16,    0,  34464, RW, 1, (void*)&pThis->usExAirFanRated_Vol_L)
    SLAVE_REG_HOLD_DATA(33, uint16,    0,   7200, RW, 1, (void*)&pSystem->usExAirFanCtrlPeriod)
    SLAVE_REG_HOLD_DATA(34, uint16,    0,   7200, RW, 1, (void*)&pSystem->usExAirFanRunTimeLeast)
    SLAVE_REG_HOLD_DATA(35, uint16,    0,      1, RW, 1, (void*)&pThis->eExAirFanType)
        
    SLAVE_REG_HOLD_DATA(36,  uint8,    0,    100, RW, 1, (void*)&pThis->ucExAirCoolRatio)    
    SLAVE_REG_HOLD_DATA(37,  uint8,    0,    100, RW, 1, (void*)&pThis->ucExAirHeatRatio)
    SLAVE_REG_HOLD_DATA(43, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_1)
    SLAVE_REG_HOLD_DATA(44, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_2)
    SLAVE_REG_HOLD_DATA(45, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_3) 
                                                                        
    SLAVE_REG_HOLD_DATA(46, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_4)
    SLAVE_REG_HOLD_DATA(47, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_5) 
    SLAVE_REG_HOLD_DATA(48, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_6)
    SLAVE_REG_HOLD_DATA(53, uint16,    0,      1, RO, 1, (void*)&pThis->usTotalFreAir_Vol_H) 
    SLAVE_REG_HOLD_DATA(54, uint16,    0,  64464, RO, 1, (void*)&pThis->usTotalFreAir_Vol_L)
        
    SLAVE_REG_HOLD_DATA(55, uint16,    0,   5000, RO, 1, (void*)&pSystem->usCO2PPM) 
    SLAVE_REG_HOLD_DATA(56, int16,  -400,   1200, RO, 1, (void*)&pSystem->sAmbientIn_T) 
    SLAVE_REG_HOLD_DATA(57, int16,  -200,    600, RO, 1, (void*)&pSystem->sAmbientOut_T)
    SLAVE_REG_HOLD_DATA(58, uint16,    0,    100, RO, 1, (void*)&pSystem->usAmbientIn_H) 
    SLAVE_REG_HOLD_DATA(59, uint16,    0,    100, RO, 1, (void*)&pSystem->usAmbientOut_H) 
        
    SLAVE_REG_HOLD_DATA(73, uint16,    0,  65535, RW, 1, (void*)&pSystem->psModularRoofList[0]->eSwitchCmd) 
    SLAVE_REG_HOLD_DATA(74, uint16,    0,  65535, RO, 1, (void*)&pSystem->psModularRoofList[0]->eRunningMode) 
    SLAVE_REG_HOLD_DATA(75, uint16,    0,      3, RO, 1, (void*)&pSystem->psModularRoofList[0]->eFuncMode)
    SLAVE_REG_HOLD_DATA(76, uint16,    0,   5000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usCO2PPMSelf)
    SLAVE_REG_HOLD_DATA(77,  int16, -200,   1400, RO, 1, (void*)&pSystem->psModularRoofList[0]->sRetAir_T)
        
    SLAVE_REG_HOLD_DATA(78,  int16, -200,   1400, RO, 1, (void*)&pSystem->psModularRoofList[0]->sSupAir_T)
    SLAVE_REG_HOLD_DATA(79, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usRetAir_Vol)    
    SLAVE_REG_HOLD_DATA(80, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usSupAir_Vol)   
    SLAVE_REG_HOLD_DATA(81, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usFreAir_Vol)  
    SLAVE_REG_HOLD_DATA(82, uint16,    0,  65000, RW, 1, (void*)&pSystem->psModularRoofList[0]->ulFreAirSet_Vol)

    SLAVE_REG_HOLD_DATA(84, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[0]->usFreAirDamper_Ang)
    SLAVE_REG_HOLD_DATA(85, uint16,  160,    350, RO, 1, (void*)&pSystem->psModularRoofList[0]->usCoolTempSet) 
    SLAVE_REG_HOLD_DATA(86, uint16,  160,    350, RO, 1, (void*)&pSystem->psModularRoofList[0]->usHeatTempSet) 
    SLAVE_REG_HOLD_DATA(87, int16,  -400,    700, RO, 1, (void*)&pSystem->psModularRoofList[0]->sAmbientInSelf_T)
    SLAVE_REG_HOLD_DATA(88, int16,  -400,    700, RO, 1, (void*)&pSystem->psModularRoofList[0]->sAmbientOutSelf_T)        
    
    SLAVE_REG_HOLD_DATA(89, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[0]->usAmbientInSelf_H) 
    SLAVE_REG_HOLD_DATA(90, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[0]->usAmbientOutSelf_H) 
    SLAVE_REG_HOLD_DATA(91, uint16,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->eSystemState)
    SLAVE_REG_HOLD_DATA(92, uint16,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->eSystemState)
    SLAVE_REG_HOLD_DATA(93, uint16,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->eSystemState)
        
SLAVE_END_DATA_BUF(0, 10)    
    
    pThis->sBMSCommData.pxSlaveDataMapIndex = xBMS_DevDataMapIndex;         //绑定映射函数
    pThis->psBMSInfo->sMBCommInfo.psSlaveCurData = &pThis->sBMSCommData;
}

/*BMS数据监控*/
void vBMS_MonitorRegist(BMS* pt)
{
    OS_ERR err = OS_ERR_NONE;
    BMS* pThis = (BMS*)pt;

    OSSemCreate( &(pThis->sValChange), "sValChange", 0, &err );  //事件消息量初始化
    
    MONITOR(&pThis->eSystemMode,  &pThis->sValChange)
    MONITOR(&pThis->eRunningMode, &pThis->sValChange)
    
    MONITOR(&pThis->sTempSet,          &pThis->sValChange)
    MONITOR(&pThis->usFreAirSet_Vol_H, &pThis->sValChange)
    MONITOR(&pThis->usFreAirSet_Vol_L, &pThis->sValChange)
    
    MONITOR(&pThis->ucExAirCoolRatio, &pThis->sValChange)
    MONITOR(&pThis->ucExAirHeatRatio, &pThis->sValChange)
    
    MONITOR(&pThis->usHumidityMin, &pThis->sValChange)
    MONITOR(&pThis->usHumidityMax, &pThis->sValChange)
    
    MONITOR(&pThis->usCO2AdjustThr_V,  &pThis->sValChange)
    MONITOR(&pThis->usCO2AdjustDeviat, &pThis->sValChange)
    
    MONITOR(&pThis->usExAirFanMinFreq, &pThis->sValChange) 
    MONITOR(&pThis->usExAirFanMaxFreq, &pThis->sValChange)
    
    MONITOR(&pThis->usExAirFanRated_Vol_H, &pThis->sValChange) 
    MONITOR(&pThis->usExAirFanRated_Vol_L, &pThis->sValChange)
    
    MONITOR(&pThis->eExAirFanType, &pThis->sValChange)
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

