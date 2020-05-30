#include "bms.h"
#include "system.h"
#include "sensor.h"
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
    
    SLAVE_REG_HOLD_DATA(0,  uint16,    0,  65535, RO, 1, (void*)&pSystem->usUnitID)
    SLAVE_REG_HOLD_DATA(1,  uint16,    0,  65535, RO, 1, (void*)&pSystem->usProtocolVer)
    SLAVE_REG_HOLD_DATA(2,  uint16,    0,      3, RW, 1, (void*)&pThis->eSystemMode)
    SLAVE_REG_HOLD_DATA(3,  uint16,    0,      4, RW, 1, (void*)&pThis->eRunningMode)
    SLAVE_REG_HOLD_DATA(4,  uint16,    0,      7, RW, 1, (void*)&pSystem->eSystemState) 
        
    SLAVE_REG_HOLD_DATA(11,  int16,   -2,     56, RW, 1, (void*)&pSystem->sChickenGrowDays)  
    SLAVE_REG_HOLD_DATA(12,  int16, -300,    450, RW, 1, (void*)&pThis->sTempSet)   
    SLAVE_REG_HOLD_DATA(13, uint16,    0,      1, RW, 1, (void*)&pThis->usFreAirSet_Vol_H)
    SLAVE_REG_HOLD_DATA(14, uint16,    0,  64464, RW, 1, (void*)&pThis->usFreAirSet_Vol_L)
    SLAVE_REG_HOLD_DATA(15, uint16,    0,    450, RW, 1, (void*)&pSystem->usEnergyTemp)
        
    SLAVE_REG_HOLD_DATA(17, uint16,    0,    550, RW, 1, (void*)&pSystem->usSupAirMax_T)     
    SLAVE_REG_HOLD_DATA(18, uint16,    0,    100, RW, 1, (void*)&pSystem->usHumidityMax)   
    SLAVE_REG_HOLD_DATA(19, uint16,    0,    100, RW, 1, (void*)&pSystem->usHumidityMin)
    SLAVE_REG_HOLD_DATA(20, uint16,    0,    100, RW, 1, (void*)&pSystem->usTempDeviat)     
    SLAVE_REG_HOLD_DATA(29, uint16,    0,   3500, RW, 1, (void*)&pThis->usCO2AdjustThr_V) 
        
    SLAVE_REG_HOLD_DATA(30, uint16,    0,   3500, RW, 1, (void*)&pThis->usCO2AdjustDeviat)    
    SLAVE_REG_HOLD_DATA(31, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2ExAirDeviat_1) 
    SLAVE_REG_HOLD_DATA(32, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2ExAirDeviat_2)
    SLAVE_REG_HOLD_DATA(33, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2PPMAlarm)
    SLAVE_REG_HOLD_DATA(37, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanFreq) 
        
    SLAVE_REG_HOLD_DATA(38, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanMinFreq)     
    SLAVE_REG_HOLD_DATA(39, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanMaxFreq)  
    SLAVE_REG_HOLD_DATA(40, uint16,    0,      1, RW, 1, (void*)&pThis->usExAirFanRated_Vol_H)    
    SLAVE_REG_HOLD_DATA(41, uint16,    0,  34464, RW, 1, (void*)&pThis->usExAirFanRated_Vol_L)  
    SLAVE_REG_HOLD_DATA(42, uint16,    0,   7200, RW, 1, (void*)&pSystem->usExAirFanCtrlPeriod)
        
    SLAVE_REG_HOLD_DATA(43, uint16,    0,   7200, RW, 1, (void*)&pSystem->usExAirFanRunTimeLeast)
    SLAVE_REG_HOLD_DATA(44, uint16,    0,      1, RW, 1, (void*)&pThis->eExAirFanType) 
    SLAVE_REG_HOLD_DATA(45,  uint8,    0,    100, RW, 1, (void*)&pThis->ucExAirCoolRatio)    
    SLAVE_REG_HOLD_DATA(46,  uint8,    0,    100, RW, 1, (void*)&pThis->ucExAirHeatRatio)    
    SLAVE_REG_HOLD_DATA(52, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_1)
        
    SLAVE_REG_HOLD_DATA(53, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_2)
    SLAVE_REG_HOLD_DATA(54, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_3)                                                                      
    SLAVE_REG_HOLD_DATA(55, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_4)
    SLAVE_REG_HOLD_DATA(56, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_5) 
    SLAVE_REG_HOLD_DATA(57, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_6)
        
    SLAVE_REG_HOLD_DATA(58, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_7) 
    SLAVE_REG_HOLD_DATA(59, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangeTime_8)
    SLAVE_REG_HOLD_DATA(64, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangePeriod_1)   
    SLAVE_REG_HOLD_DATA(65, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangePeriod_2)
    SLAVE_REG_HOLD_DATA(66, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangePeriod_3)  
        
    SLAVE_REG_HOLD_DATA(67, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangePeriod_4)
    SLAVE_REG_HOLD_DATA(68, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangePeriod_5) 
    SLAVE_REG_HOLD_DATA(69, uint16,    0,   7200, RW, 1, (void*)&pSystem->usModeChangePeriod_6)
    SLAVE_REG_HOLD_DATA(76, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_1)    
    SLAVE_REG_HOLD_DATA(77, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_2)
        
    SLAVE_REG_HOLD_DATA(78, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_3)                                                                      
    SLAVE_REG_HOLD_DATA(79, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_4)
    SLAVE_REG_HOLD_DATA(80, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_5) 
    SLAVE_REG_HOLD_DATA(81, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_6)
    SLAVE_REG_HOLD_DATA(93, uint16,    0,      1, RO, 1, (void*)&pThis->usTotalFreAir_Vol_H) 
        
    SLAVE_REG_HOLD_DATA(94, uint16,  0,            64464,        RO, 1, (void*)&pThis->usTotalFreAir_Vol_L)   
    SLAVE_REG_HOLD_DATA(95, uint16,  MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->usCO2PPM) 
    SLAVE_REG_HOLD_DATA(96,  int16,  MIN_IN_TEMP,  MAX_IN_TEMP,  RO, 1, (void*)&pSystem->sAmbientIn_T) 
    SLAVE_REG_HOLD_DATA(97,  int16,  MIN_OUT_TEMP, MAX_OUT_TEMP, RO, 1, (void*)&pSystem->sAmbientOut_T)
    SLAVE_REG_HOLD_DATA(98, uint16,  MIN_HUMI,     MAX_HUMI,     RO, 1, (void*)&pSystem->usAmbientIn_H) 
        
    SLAVE_REG_HOLD_DATA(99,  uint16,    0,    100, RO, 1, (void*)&pSystem->usAmbientOut_H)  
    SLAVE_REG_HOLD_DATA(113, uint16,    0,  65535, RW, 1, (void*)&pSystem->psModularRoofList[0]->eSwitchCmd) 
    SLAVE_REG_HOLD_DATA(114, uint16,    0,  65535, RO, 1, (void*)&pSystem->psModularRoofList[0]->eRunningMode) 
    SLAVE_REG_HOLD_DATA(115, uint16,    0,      3, RO, 1, (void*)&pSystem->psModularRoofList[0]->eFuncMode)
    SLAVE_REG_HOLD_DATA(116, uint16,    0,   5000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usCO2PPMSelf)
        
    SLAVE_REG_HOLD_DATA(117,  int16, -200,   1400, RO, 1, (void*)&pSystem->psModularRoofList[0]->sRetAir_T)   
    SLAVE_REG_HOLD_DATA(118,  int16, -200,   1400, RO, 1, (void*)&pSystem->psModularRoofList[0]->sSupAir_T)
    SLAVE_REG_HOLD_DATA(119, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usRetAir_Vol)    
    SLAVE_REG_HOLD_DATA(120, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usSupAir_Vol)   
    SLAVE_REG_HOLD_DATA(121, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usFreAir_Vol) 
        
    SLAVE_REG_HOLD_DATA(122, uint16,    0,  65000, RW, 1, (void*)&pSystem->psModularRoofList[0]->usFreAirSet_Vol)
    SLAVE_REG_HOLD_DATA(124, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[0]->usFreAirDamper_Ang)
    SLAVE_REG_HOLD_DATA(125, uint16,  160,    350, RO, 1, (void*)&pSystem->psModularRoofList[0]->usCoolTempSet) 
    SLAVE_REG_HOLD_DATA(126, uint16,  160,    350, RO, 1, (void*)&pSystem->psModularRoofList[0]->usHeatTempSet) 
    SLAVE_REG_HOLD_DATA(127,  int16, -400,    700, RO, 1, (void*)&pSystem->psModularRoofList[0]->sAmbientInSelf_T)
        
    SLAVE_REG_HOLD_DATA(128,  int16, -400,    700, RO, 1, (void*)&pSystem->psModularRoofList[0]->sAmbientOutSelf_T)        
    SLAVE_REG_HOLD_DATA(129, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[0]->usAmbientInSelf_H) 
    SLAVE_REG_HOLD_DATA(130, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[0]->usAmbientOutSelf_H) 
    SLAVE_REG_HOLD_DATA(131,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->ucModularState)
    SLAVE_REG_HOLD_DATA(132,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->ucModularState)
        
    SLAVE_REG_HOLD_DATA(133,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[0]->psModularList[2]->ucModularState)
    SLAVE_REG_HOLD_DATA(134,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[0]->psModularList[3]->ucModularState)
    SLAVE_REG_HOLD_DATA(135,  uint8,    0,  65535, RO, 1, (void*)&pSystem->psModularRoofList[0]->Device.usRunTime_H)   
    SLAVE_REG_HOLD_DATA(145, uint16,    0,  65535, RW, 1, (void*)&pSystem->psModularRoofList[1]->eSwitchCmd) 
    SLAVE_REG_HOLD_DATA(146, uint16,    0,  65535, RO, 1, (void*)&pSystem->psModularRoofList[1]->eRunningMode) 
        
    SLAVE_REG_HOLD_DATA(147, uint16,    0,     3, RO, 1, (void*)&pSystem->psModularRoofList[1]->eFuncMode)
    SLAVE_REG_HOLD_DATA(148, uint16,    0,  5000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usCO2PPMSelf)                                                                                      
    SLAVE_REG_HOLD_DATA(149,  int16, -200,  1400, RO, 1, (void*)&pSystem->psModularRoofList[1]->sRetAir_T)   
    SLAVE_REG_HOLD_DATA(150,  int16, -200,  1400, RO, 1, (void*)&pSystem->psModularRoofList[1]->sSupAir_T)
    SLAVE_REG_HOLD_DATA(151, uint16,    0, 65000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usRetAir_Vol) 
        
    SLAVE_REG_HOLD_DATA(152, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usSupAir_Vol)   
    SLAVE_REG_HOLD_DATA(153, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usFreAir_Vol)                                                                                        
    SLAVE_REG_HOLD_DATA(154, uint16,    0,  65000, RW, 1, (void*)&pSystem->psModularRoofList[1]->usFreAirSet_Vol)
    SLAVE_REG_HOLD_DATA(156, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[1]->usFreAirDamper_Ang)
    SLAVE_REG_HOLD_DATA(157, uint16,  160,    350, RO, 1, (void*)&pSystem->psModularRoofList[1]->usCoolTempSet) 
        
    SLAVE_REG_HOLD_DATA(158, uint16,  160,    350, RO, 1, (void*)&pSystem->psModularRoofList[1]->usHeatTempSet) 
    SLAVE_REG_HOLD_DATA(159,  int16, -400,    700, RO, 1, (void*)&pSystem->psModularRoofList[1]->sAmbientInSelf_T)                                                                                      
    SLAVE_REG_HOLD_DATA(160,  int16, -400,    700, RO, 1, (void*)&pSystem->psModularRoofList[1]->sAmbientOutSelf_T)        
    SLAVE_REG_HOLD_DATA(161, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[1]->usAmbientInSelf_H) 
    SLAVE_REG_HOLD_DATA(162, uint16,    0,    100, RO, 1, (void*)&pSystem->psModularRoofList[1]->usAmbientOutSelf_H) 
        
    SLAVE_REG_HOLD_DATA(163,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[1]->psModularList[0]->ucModularState)
    SLAVE_REG_HOLD_DATA(164,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[1]->psModularList[1]->ucModularState)                                                                                        
    SLAVE_REG_HOLD_DATA(165,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[1]->psModularList[2]->ucModularState)
    SLAVE_REG_HOLD_DATA(166,  uint8,    0,      5, RO, 1, (void*)&pSystem->psModularRoofList[1]->psModularList[3]->ucModularState)
    SLAVE_REG_HOLD_DATA(167,  uint8,    0,  65535, RO, 1, (void*)&pSystem->psModularRoofList[1]->Device.usRunTime_H)   

    SLAVE_REG_HOLD_DATA(179, uint16,    0,    500, RO, 1, (void*)&pSystem->pExAirFanVariate->usRunningFreq); 
    SLAVE_REG_HOLD_DATA(181, uint16,    0,  65535, RO, 1, (void*)&pSystem->psExAirFanList[0]->Device.usRunTime_H);
    SLAVE_REG_HOLD_DATA(191, uint16,    0,  65535, RO, 1, (void*)&pSystem->psExAirFanList[1]->Device.usRunTime_H);
    SLAVE_REG_HOLD_DATA(201, uint16,    0,  65535, RO, 1, (void*)&pSystem->psExAirFanList[2]->Device.usRunTime_H);
    SLAVE_REG_HOLD_DATA(211, uint16,    0,  65535, RO, 1, (void*)&pSystem->psExAirFanList[3]->Device.usRunTime_H);

    SLAVE_REG_HOLD_DATA(235, uint16, MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->psCO2SenList[0]->usAvgCO2PPM) 
    SLAVE_REG_HOLD_DATA(236, uint16, MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->psCO2SenList[1]->usAvgCO2PPM)  
    SLAVE_REG_HOLD_DATA(237, uint16, MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->psCO2SenList[2]->usAvgCO2PPM)
    SLAVE_REG_HOLD_DATA(241,  int16, MIN_OUT_TEMP, MAX_OUT_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenOutList[0]->sAvgTemp) 
    SLAVE_REG_HOLD_DATA(242, uint16, MIN_HUMI,     MAX_HUMI,     RO, 1, (void*)&pSystem->psTempHumiSenOutList[0]->usAvgHumi)     
    
    SLAVE_REG_HOLD_DATA(250,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[0]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(251,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[1]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(252,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[2]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(253,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[3]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(254,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[4]->sAvgTemp)    
    
    SLAVE_REG_HOLD_DATA(255,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[5]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(256,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[6]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(257,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[7]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(258,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[8]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(259,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[9]->sAvgTemp)
    
    SLAVE_REG_HOLD_DATA(260,   int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[10]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(261,   int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[11]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(268,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[0]->usAvgHumi)    
    SLAVE_REG_HOLD_DATA(269,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[1]->usAvgHumi) 
    SLAVE_REG_HOLD_DATA(270,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[2]->usAvgHumi)
                              
    SLAVE_REG_HOLD_DATA(271,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[3]->usAvgHumi)    
    SLAVE_REG_HOLD_DATA(272,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[4]->usAvgHumi) 
    SLAVE_REG_HOLD_DATA(273,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[5]->usAvgHumi)
    SLAVE_REG_HOLD_DATA(274,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[6]->usAvgHumi)    
    SLAVE_REG_HOLD_DATA(275,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[7]->usAvgHumi)
                              
    SLAVE_REG_HOLD_DATA(276,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[8]->usAvgHumi)
    SLAVE_REG_HOLD_DATA(277,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[9]->usAvgHumi)    
    SLAVE_REG_HOLD_DATA(278,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[10]->usAvgHumi) 
    SLAVE_REG_HOLD_DATA(279,  uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenInList[11]->usAvgHumi)
         
SLAVE_END_DATA_BUF(0, 279)    
    
    /******************************线圈数据域*************************/ 
SLAVE_BEGIN_DATA_BUF(pThis->sBMS_BitCoilBuf,  &pThis->sBMSCommData.sMBCoilTable)
    
    SLAVE_COIL_BIT_DATA(4,  RW, (void*)&pSystem->xAlarmEnable) 
    SLAVE_COIL_BIT_DATA(16, RO, (void*)&pSystem->xTempSenInErr) 
    SLAVE_COIL_BIT_DATA(17, RO, (void*)&pSystem->xHumiSenInErr) 
    SLAVE_COIL_BIT_DATA(18, RO, (void*)&pSystem->xCO2SenErr) 
    SLAVE_COIL_BIT_DATA(24, RW, (void*)&pSystem->psModularRoofList[0]->xErrClean)
    
    SLAVE_COIL_BIT_DATA(25, RO, (void*)&pSystem->psModularRoofList[0]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(26, RO, (void*)&pSystem->psModularRoofList[0]->xStopErrFlag)
    SLAVE_COIL_BIT_DATA(27, RO, (void*)&pSystem->psModularRoofList[0]->Device.xErrFlag) 
    SLAVE_COIL_BIT_DATA(28, RO, (void*)&pSystem->psModularRoofList[0]->Device.xAlarmFlag)
    SLAVE_COIL_BIT_DATA(29, RO, (void*)&pSystem->psModularRoofList[0]->sMBSlaveDev.xOnLine)
    
    SLAVE_COIL_BIT_DATA(30, RO, (void*)&pSystem->psModularRoofList[0]->psSupAirFan->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(31, RO, (void*)&pSystem->psModularRoofList[0]->xSupAirDamperState)    
    SLAVE_COIL_BIT_DATA(32, RO, (void*)&pSystem->psModularRoofList[0]->xWetMode)
    SLAVE_COIL_BIT_DATA(34, RO, (void*)&pSystem->psModularRoofList[0]->xRetAirSenErr)
    SLAVE_COIL_BIT_DATA(35, RO, (void*)&pSystem->psModularRoofList[0]->xFreAirSenErr)  

    SLAVE_COIL_BIT_DATA(36, RO, (void*)&pSystem->psModularRoofList[0]->xCO2SenSelfErr_1)
    SLAVE_COIL_BIT_DATA(37, RO, (void*)&pSystem->psModularRoofList[0]->xCO2SenSelfErr_2) 
    SLAVE_COIL_BIT_DATA(38, RO, (void*)&pSystem->psModularRoofList[0]->xTempSenInSelfErr)
    SLAVE_COIL_BIT_DATA(39, RO, (void*)&pSystem->psModularRoofList[0]->xHumiSenInSelfErr) 
    SLAVE_COIL_BIT_DATA(48, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->psCompList[0]->Device.eRunningState)
    
    SLAVE_COIL_BIT_DATA(49, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->psCompList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(50, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->psAmbientOutFanList[0]->Device.eRunningState)    
    SLAVE_COIL_BIT_DATA(51, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->psAmbientOutFanList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(52, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->xTempSenOutErr)    
    SLAVE_COIL_BIT_DATA(53, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[0]->xHumiSenOutErr)    
    
    SLAVE_COIL_BIT_DATA(56, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->psCompList[0]->Device.eRunningState)  
    SLAVE_COIL_BIT_DATA(57, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->psCompList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(58, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->psAmbientOutFanList[0]->Device.eRunningState)    
    SLAVE_COIL_BIT_DATA(59, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->psAmbientOutFanList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(60, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->xTempSenOutErr) 
                                                                                   
    SLAVE_COIL_BIT_DATA(61, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[1]->xHumiSenOutErr) 
    SLAVE_COIL_BIT_DATA(64, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[2]->psCompList[0]->Device.eRunningState)  
    SLAVE_COIL_BIT_DATA(65, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[2]->psCompList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(66, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[2]->psAmbientOutFanList[0]->Device.eRunningState)    
    SLAVE_COIL_BIT_DATA(67, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[2]->psAmbientOutFanList[1]->Device.eRunningState)
                                                                                     
    SLAVE_COIL_BIT_DATA(68, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[2]->xTempSenOutErr)                                                                              
    SLAVE_COIL_BIT_DATA(69, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[2]->xHumiSenOutErr) 
    SLAVE_COIL_BIT_DATA(72, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[3]->psCompList[0]->Device.eRunningState)  
    SLAVE_COIL_BIT_DATA(73, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[3]->psCompList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(74, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[3]->psAmbientOutFanList[0]->Device.eRunningState)
        
    SLAVE_COIL_BIT_DATA(75, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[3]->psAmbientOutFanList[1]->Device.eRunningState)                                                                               
    SLAVE_COIL_BIT_DATA(76, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[3]->xTempSenOutErr)                                                                              
    SLAVE_COIL_BIT_DATA(77, RO, (void*)&pSystem->psModularRoofList[0]->psModularList[3]->xHumiSenOutErr)    
    SLAVE_COIL_BIT_DATA(88, RW, (void*)&pSystem->psModularRoofList[1]->xErrClean)                                                              
    SLAVE_COIL_BIT_DATA(89, RO, (void*)&pSystem->psModularRoofList[1]->Device.eRunningState)
        
    SLAVE_COIL_BIT_DATA(90, RO, (void*)&pSystem->psModularRoofList[1]->xStopErrFlag)
    SLAVE_COIL_BIT_DATA(91, RO, (void*)&pSystem->psModularRoofList[1]->Device.xErrFlag) 
    SLAVE_COIL_BIT_DATA(92, RO, (void*)&pSystem->psModularRoofList[1]->Device.xAlarmFlag)
    SLAVE_COIL_BIT_DATA(93, RO, (void*)&pSystem->psModularRoofList[1]->sMBSlaveDev.xOnLine)                                                            
    SLAVE_COIL_BIT_DATA(94, RO, (void*)&pSystem->psModularRoofList[1]->psSupAirFan->Device.eRunningState)
        
    SLAVE_COIL_BIT_DATA(95, RO, (void*)&pSystem->psModularRoofList[1]->xSupAirDamperState)    
    SLAVE_COIL_BIT_DATA(96, RO, (void*)&pSystem->psModularRoofList[1]->xWetMode)
    SLAVE_COIL_BIT_DATA(98, RO, (void*)&pSystem->psModularRoofList[1]->xRetAirSenErr)
    SLAVE_COIL_BIT_DATA(99, RO, (void*)&pSystem->psModularRoofList[1]->xFreAirSenErr)                                                                
    SLAVE_COIL_BIT_DATA(100, RO, (void*)&pSystem->psModularRoofList[1]->xCO2SenSelfErr_1)
        
    SLAVE_COIL_BIT_DATA(101, RO, (void*)&pSystem->psModularRoofList[1]->xCO2SenSelfErr_2) 
    SLAVE_COIL_BIT_DATA(102, RO, (void*)&pSystem->psModularRoofList[1]->xTempSenInSelfErr)
    SLAVE_COIL_BIT_DATA(103, RO, (void*)&pSystem->psModularRoofList[1]->xHumiSenInSelfErr) 
    SLAVE_COIL_BIT_DATA(112, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[0]->psCompList[0]->Device.eRunningState)                                                               
    SLAVE_COIL_BIT_DATA(113, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[0]->psCompList[1]->Device.eRunningState)
        
    SLAVE_COIL_BIT_DATA(114, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[0]->psAmbientOutFanList[0]->Device.eRunningState)    
    SLAVE_COIL_BIT_DATA(115, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[0]->psAmbientOutFanList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(116, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[0]->xTempSenOutErr)    
    SLAVE_COIL_BIT_DATA(117, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[0]->xHumiSenOutErr)                                                                  
    SLAVE_COIL_BIT_DATA(120, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[1]->psCompList[0]->Device.eRunningState) 
        
    SLAVE_COIL_BIT_DATA(121, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[1]->psCompList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(122, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[1]->psAmbientOutFanList[0]->Device.eRunningState)    
    SLAVE_COIL_BIT_DATA(123, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[1]->psAmbientOutFanList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(124, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[1]->xTempSenOutErr)                                                                          
    SLAVE_COIL_BIT_DATA(125, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[1]->xHumiSenOutErr) 
        
    SLAVE_COIL_BIT_DATA(128, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[2]->psCompList[0]->Device.eRunningState) 
    SLAVE_COIL_BIT_DATA(129, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[2]->psCompList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(130, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[2]->psAmbientOutFanList[0]->Device.eRunningState)    
    SLAVE_COIL_BIT_DATA(131, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[2]->psAmbientOutFanList[1]->Device.eRunningState)                                                                              
    SLAVE_COIL_BIT_DATA(132, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[2]->xTempSenOutErr)    
        
    SLAVE_COIL_BIT_DATA(133, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[2]->xHumiSenOutErr) 
    SLAVE_COIL_BIT_DATA(136, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[3]->psCompList[0]->Device.eRunningState)  
    SLAVE_COIL_BIT_DATA(137, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[3]->psCompList[1]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(138, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[3]->psAmbientOutFanList[0]->Device.eRunningState)                                                               
    SLAVE_COIL_BIT_DATA(139, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[3]->psAmbientOutFanList[1]->Device.eRunningState) 
        
    SLAVE_COIL_BIT_DATA(140, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[3]->xTempSenOutErr)          
    SLAVE_COIL_BIT_DATA(141, RO, (void*)&pSystem->psModularRoofList[1]->psModularList[3]->xHumiSenOutErr) 
    
SLAVE_END_DATA_BUF(0, 279)    
    
    
    
    
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
    
         
    MONITOR(&pThis->usExAirFanFreq,    &pThis->sValChange) 
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

