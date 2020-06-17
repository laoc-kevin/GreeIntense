#include "bms.h"
#include "system.h"
#include "sensor.h"
#include "md_modbus.h"
#include "mbmap.h"


/*************************************************************
*                         BMS接口                            *
**************************************************************/
BMS* psBMS = NULL;
BMS  BMSCore;

/*通讯映射函数*/
BOOL xBMS_DevDataMapIndex(eDataType eDataType, USHORT usAddr, USHORT* psIndex)
{
    USHORT i = 0;
   
    if(eDataType == RegHoldData)
    {
        switch(usAddr)
        {
            case 0	:  i = 0  ;  break;
            case 1	:  i = 1  ;  break;
            case 2	:  i = 2  ;  break;
            case 3	:  i = 3  ;  break;
            case 4	:  i = 4  ;  break;
            case 11	:  i = 5  ;  break;
            case 12	:  i = 6  ;  break;
            case 13	:  i = 7  ;  break;
            case 14	:  i = 8  ;  break;
            case 15	:  i = 9  ;  break;
            case 17	:  i = 10 ;  break;
            case 18	:  i = 11 ;  break;
            case 19	:  i = 12 ;  break;
            case 20	:  i = 13 ;  break;
            case 29	:  i = 14 ;  break;
            case 30	:  i = 15 ;  break;
            case 31	:  i = 16 ;  break;
            case 32	:  i = 17 ;  break;
            case 33	:  i = 18 ;  break;
            case 37	:  i = 19 ;  break;
            case 38	:  i = 20 ;  break;
            case 39	:  i = 21 ;  break;
            case 40	:  i = 22 ;  break;
            case 41	:  i = 23 ;  break;
            case 42	:  i = 24 ;  break;
            case 43	:  i = 25 ;  break;
            case 44	:  i = 26 ;  break;
            case 45	:  i = 27 ;  break;
            case 46	:  i = 28 ;  break;
            case 52	:  i = 29 ;  break;
            case 53	:  i = 30 ;  break;
            case 54	:  i = 31 ;  break;
            case 55	:  i = 32 ;  break;
            case 56	:  i = 33 ;  break;
            case 57	:  i = 34 ;  break;
            case 58	:  i = 35 ;  break;
            case 59	:  i = 36 ;  break;
            case 64	:  i = 37 ;  break;
            case 65	:  i = 38 ;  break;
            case 66	:  i = 39 ;  break;
            case 67	:  i = 40 ;  break;
            case 68	:  i = 41 ;  break;
            case 69	:  i = 42 ;  break;
            case 75	:  i = 43 ;  break;
            case 76	:  i = 44 ;  break;
            case 77	:  i = 45 ;  break;
            case 78	:  i = 46 ;  break;
            case 79	:  i = 47 ;  break;
            case 80	:  i = 48 ;  break;
            case 81	:  i = 49 ;  break;
            case 93	:  i = 50 ;  break;
            case 94	:  i = 51 ;  break;
            case 95	:  i = 52 ;  break;
            case 96	:  i = 53 ;  break;
            case 97	:  i = 54 ;  break;
            case 98	:  i = 55 ;  break;
            case 99	:  i = 56 ;  break;
            case 113:  i = 57 ;  break;
            case 114:  i = 58 ;  break;
            case 115:  i = 59 ;  break;
            case 116:  i = 60 ;  break;
            case 117:  i = 61 ;  break;
            case 118:  i = 62 ;  break;
            case 119:  i = 63 ;  break;
            case 120:  i = 64 ;  break;
            case 121:  i = 65 ;  break;
            case 122:  i = 66 ;  break;
            case 124:  i = 67 ;  break;
            case 125:  i = 68 ;  break;
            case 126:  i = 69 ;  break;
            case 127:  i = 70 ;  break;
            case 128:  i = 71 ;  break;
            case 129:  i = 72 ;  break;
            case 130:  i = 73 ;  break;
            case 131:  i = 74 ;  break;
            case 132:  i = 75 ;  break;
            case 133:  i = 76 ;  break;
            case 134:  i = 77 ;  break;
            case 135:  i = 78 ;  break;
            case 145:  i = 79 ;  break;
            case 146:  i = 80 ;  break;
            case 147:  i = 81 ;  break;
            case 148:  i = 82 ;  break;
            case 149:  i = 83 ;  break;
            case 150:  i = 84 ;  break;
            case 151:  i = 85 ;  break;
            case 152:  i = 86 ;  break;
            case 153:  i = 87 ;  break;
            case 154:  i = 88 ;  break;
            case 156:  i = 89 ;  break;
            case 157:  i = 90 ;  break;
            case 158:  i = 91 ;  break;
            case 159:  i = 92 ;  break;
            case 160:  i = 93 ;  break;
            case 161:  i = 94 ;  break;
            case 162:  i = 95 ;  break;
            case 163:  i = 96 ;  break;
            case 164:  i = 97 ;  break;
            case 165:  i = 98 ;  break;
            case 166:  i = 99 ;  break;
            case 167:  i = 100;  break;
            case 179:  i = 101;  break;
            case 181:  i = 102;  break;
            case 191:  i = 103;  break;
            case 201:  i = 104;  break;
            case 211:  i = 105;  break;
            case 235:  i = 106;  break;
            case 236:  i = 107;  break;
            case 237:  i = 108;  break;
            case 241:  i = 109;  break;
            case 242:  i = 110;  break;
            case 250:  i = 111;  break;
            case 251:  i = 112;  break;
            case 252:  i = 113;  break;
            case 253:  i = 114;  break;
            case 254:  i = 115;  break;
            case 255:  i = 116;  break;
            case 256:  i = 117;  break;
            case 257:  i = 118;  break;
            case 258:  i = 119;  break;
            case 259:  i = 120;  break;
            case 260:  i = 121;  break;
            case 261:  i = 122;  break;
            case 268:  i = 123;  break;
            case 269:  i = 124;  break;
            case 270:  i = 125;  break;
            case 271:  i = 126;  break;
            case 272:  i = 127;  break;
            case 273:  i = 128;  break;
            case 274:  i = 129;  break;
            case 275:  i = 130;  break;
            case 276:  i = 131;  break;
            case 277:  i = 132;  break;
            case 278:  i = 133;  break;
            case 279:  i = 134;  break;
            case 286:  i = 135;  break;
            case 287:  i = 136;  break;
            case 288:  i = 137;  break;
            case 305:  i = 138;  break;
            case 306:  i = 139;  break;
            case 307:  i = 140;  break;

            default:
    	    	return FALSE;
    	    break;
        }
    }  
    else if(eDataType == CoilData)
    {
        switch(usAddr)
        {
            case 4  :  i =  0 ;  break;
            case 16 :  i =  1 ;  break;
            case 17 :  i =  2 ;  break;
            case 18 :  i =  3 ;  break;
            case 24 :  i =  4 ;  break;
            case 25 :  i =  5 ;  break;
            case 26 :  i =  6 ;  break;
            case 27 :  i =  7 ;  break;
            case 28 :  i =  8 ;  break;
            case 29 :  i =  9 ;  break;
            case 30	:  i = 10 ;  break;
            case 31	:  i = 11 ;  break;
            case 32	:  i = 12 ;  break;
            case 34	:  i = 13 ;  break;
            case 35	:  i = 14 ;  break;
            case 36	:  i = 15 ;  break;
            case 37	:  i = 16 ;  break;
            case 38	:  i = 17 ;  break;
            case 39	:  i = 18 ;  break;
            case 48	:  i = 19 ;  break;
            case 49	:  i = 20 ;  break;
            case 50	:  i = 21 ;  break;
            case 51	:  i = 22 ;  break;
            case 52	:  i = 23 ;  break;
            case 53	:  i = 24 ;  break;
            case 56	:  i = 25 ;  break;
            case 57	:  i = 26 ;  break;
            case 58	:  i = 27 ;  break;
            case 59	:  i = 28 ;  break;
            case 60	:  i = 29 ;  break;
            case 61	:  i = 30 ;  break;
            case 64	:  i = 31 ;  break;
            case 65	:  i = 32 ;  break;
            case 66	:  i = 33 ;  break;
            case 67	:  i = 34 ;  break;
            case 68	:  i = 35 ;  break;
            case 69	:  i = 36 ;  break;
            case 72	:  i = 37 ;  break;
            case 73	:  i = 38 ;  break;
            case 74	:  i = 39 ;  break;
            case 75	:  i = 40 ;  break;
            case 76	:  i = 41 ;  break;
            case 77	:  i = 42 ;  break;
            case 88	:  i = 43 ;  break;
            case 89	:  i = 44 ;  break;
            case 90	:  i = 45 ;  break;
            case 91	:  i = 46 ;  break;
            case 92	:  i = 47 ;  break;
            case 93	:  i = 48 ;  break;
            case 94	:  i = 49 ;  break;
            case 95	:  i = 50 ;  break;
            case 96	:  i = 51 ;  break;
            case 98	:  i = 52 ;  break;
            case 99	:  i = 53 ;  break;
            case 100:  i = 54 ;  break;
            case 101:  i = 55 ;  break;
            case 102:  i = 56 ;  break;
            case 103:  i = 57 ;  break;
            case 112:  i = 58 ;  break;
            case 113:  i = 59 ;  break;
            case 114:  i = 60 ;  break;
            case 115:  i = 61 ;  break;
            case 116:  i = 62 ;  break;
            case 117:  i = 63 ;  break;
            case 120:  i = 64 ;  break;
            case 121:  i = 65 ;  break;
            case 122:  i = 66 ;  break;
            case 123:  i = 67 ;  break;
            case 124:  i = 68 ;  break;
            case 125:  i = 69 ;  break;
            case 128:  i = 70 ;  break;
            case 129:  i = 71 ;  break;
            case 130:  i = 72 ;  break;
            case 131:  i = 73 ;  break;
            case 132:  i = 74 ;  break;
            case 133:  i = 75 ;  break;
            case 136:  i = 76 ;  break;
            case 137:  i = 77 ;  break;
            case 138:  i = 78 ;  break;
            case 139:  i = 79 ;  break;
            case 140:  i = 80 ;  break;
            case 141:  i = 81 ;  break;
            case 152:  i = 82 ;  break;
            case 153:  i = 83 ;  break;
            case 160:  i = 84 ;  break;
            case 161:  i = 85 ;  break;
            case 162:  i = 86 ;  break;
            case 168:  i = 87 ;  break;
            case 169:  i = 88 ;  break;
            case 170:  i = 89 ;  break;
            case 176:  i = 90 ;  break;
            case 177:  i = 91 ;  break;
            case 178:  i = 92 ;  break;
            case 184:  i = 93 ;  break;
            case 185:  i = 94 ;  break;
            case 186:  i = 95 ;  break;
            case 192:  i = 96 ;  break;
            case 193:  i = 97 ;  break;
            case 194:  i = 98 ;  break;
            case 200:  i = 99 ;  break;
            case 201:  i = 100;  break;
            case 208:  i = 101;  break;
            case 209:  i = 102;  break;
            case 210:  i = 103;  break;
            case 211:  i = 104;  break;
            case 212:  i = 105;  break;
            case 213:  i = 106;  break;
            case 214:  i = 107;  break;
            case 215:  i = 108;  break;
            case 216:  i = 109;  break;
            case 217:  i = 110;  break;
            case 218:  i = 111;  break;
            case 219:  i = 112;  break;
            case 224:  i = 113;  break;
            case 225:  i = 114;  break;
            case 226:  i = 115;  break;
            case 227:  i = 116;  break;
            case 228:  i = 117;  break;
            case 229:  i = 118;  break;
            case 230:  i = 119;  break;
            case 231:  i = 120;  break;
            case 232:  i = 121;  break;
            case 233:  i = 122;  break;
            case 234:  i = 123;  break;
            case 235:  i = 124;  break;
                    
            default:
    	    	return FALSE;
    	    break;
        }
    } 
    *psIndex = i;
    return TRUE;  
}

/*BMS通讯数据表初始化*/
void vBMS_InitBMSCommData(BMS* pt)
{
    BMS*    pThis   = (BMS*)pt;
    System* pSystem = (System*)System_Core();
    
SLAVE_PBUF_INDEX_ALLOC()  
 
SLAVE_BEGIN_DATA_BUF(&pThis->sBMS_RegHoldBuf, &pThis->sBMSCommData.sMBRegHoldTable) 
    
    SLAVE_REG_HOLD_DATA(0,  uint16,    0,  65535, RO, 1, (void*)&pSystem->usUnitID)
    SLAVE_REG_HOLD_DATA(1,  uint16,    0,  65535, RO, 1, (void*)&pSystem->usProtocolVer)
    SLAVE_REG_HOLD_DATA(2,   uint8,    0,      3, RW, 1, (void*)&pThis->eSystemMode)
    SLAVE_REG_HOLD_DATA(3,   uint8,    1,      4, RW, 1, (void*)&pThis->eRunningMode)
    SLAVE_REG_HOLD_DATA(4,   uint8,    0,      7, RO, 1, (void*)&pSystem->eSystemState) 
        
    SLAVE_REG_HOLD_DATA(11,  int16,   -2,     56, RW, 1, (void*)&pSystem->sChickenGrowDays)  
    SLAVE_REG_HOLD_DATA(12, uint16,  160,    350, RW, 1, (void*)&pThis->usTempSet)   
    SLAVE_REG_HOLD_DATA(13, uint16,    0,  64464, RW, 1, (void*)&pThis->usFreAirSet_Vol_L)
    SLAVE_REG_HOLD_DATA(14, uint16,    0,      1, RW, 1, (void*)&pThis->usFreAirSet_Vol_H)
    SLAVE_REG_HOLD_DATA(15, uint16,    0,    450, RW, 1, (void*)&pSystem->usEnergyTemp)
        
    SLAVE_REG_HOLD_DATA(17, uint16,    0,    550, RW, 1, (void*)&pSystem->usSupAirMax_T)     
    SLAVE_REG_HOLD_DATA(18, uint16,    0,    100, RW, 1, (void*)&pThis->usHumidityMax)   
    SLAVE_REG_HOLD_DATA(19, uint16,    0,    100, RW, 1, (void*)&pThis->usHumidityMin)
    SLAVE_REG_HOLD_DATA(20, uint16,    0,    100, RW, 1, (void*)&pSystem->usTempDeviat)     
    SLAVE_REG_HOLD_DATA(29, uint16,    0,   3500, RW, 1, (void*)&pThis->usCO2AdjustThr_V) 
        
    SLAVE_REG_HOLD_DATA(30, uint16,    0,   3500, RW, 1, (void*)&pThis->usCO2AdjustDeviat)    
    SLAVE_REG_HOLD_DATA(31, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2ExAirDeviat_1) 
    SLAVE_REG_HOLD_DATA(32, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2ExAirDeviat_2)
    SLAVE_REG_HOLD_DATA(33, uint16,    0,   3500, RW, 1, (void*)&pSystem->usCO2PPMAlarm)
    SLAVE_REG_HOLD_DATA(37, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanFreq) 
     
    SLAVE_REG_HOLD_DATA(38, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanMinFreq)     
    SLAVE_REG_HOLD_DATA(39, uint16,    0,    500, RW, 1, (void*)&pThis->usExAirFanMaxFreq)  
    SLAVE_REG_HOLD_DATA(40, uint16,    0,  34464, RW, 1, (void*)&pThis->usExAirFanRated_Vol_L)    
    SLAVE_REG_HOLD_DATA(41, uint16,    0,      1, RW, 1, (void*)&pThis->usExAirFanRated_Vol_H)  
    SLAVE_REG_HOLD_DATA(42, uint16,    0,   7200, RW, 1, (void*)&pSystem->usExAirFanCtrlPeriod)
        
    SLAVE_REG_HOLD_DATA(43, uint16,    0,   7200, RW, 1, (void*)&pSystem->usExAirFanRunTimeLeast)
    SLAVE_REG_HOLD_DATA(44,  uint8,    0,      1, RW, 1, (void*)&pThis->eExAirFanType) 
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
    SLAVE_REG_HOLD_DATA(75, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_0)
    SLAVE_REG_HOLD_DATA(76, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_1)
        
    SLAVE_REG_HOLD_DATA(77, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_2)  
    SLAVE_REG_HOLD_DATA(78, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_3)                                                                      
    SLAVE_REG_HOLD_DATA(79, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_4)
    SLAVE_REG_HOLD_DATA(80, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_5) 
    SLAVE_REG_HOLD_DATA(81, uint16,    0,    100, RW, 1, (void*)&pSystem->usModeAdjustTemp_6)
        
    SLAVE_REG_HOLD_DATA(93, uint16,     0,            64464,        RO, 1, (void*)&pThis->usTotalFreAir_Vol_L)     
    SLAVE_REG_HOLD_DATA(94, uint16,     0,            1,            RO, 1, (void*)&pThis->usTotalFreAir_Vol_H)   
    SLAVE_REG_HOLD_DATA(95, uint16,     MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->usCO2PPM) 
    SLAVE_REG_HOLD_DATA(96,  int16,     MIN_IN_TEMP,  MAX_IN_TEMP,  RO, 1, (void*)&pSystem->sAmbientIn_T) 
    SLAVE_REG_HOLD_DATA(97,  int16,     MIN_OUT_TEMP, MAX_OUT_TEMP, RO, 1, (void*)&pSystem->sAmbientOut_T)
        
    SLAVE_REG_HOLD_DATA(98,  uint16,    MIN_HUMI,     MAX_HUMI,     RO, 1, (void*)&pSystem->usAmbientIn_H)  
    SLAVE_REG_HOLD_DATA(99,  uint16,    0,               100,       RO, 1, (void*)&pSystem->usAmbientOut_H)  
    SLAVE_REG_HOLD_DATA(113,  uint8,    85,              170,       RW, 1, (void*)&pSystem->psModularRoofList[0]->eSwitchCmd) 
    SLAVE_REG_HOLD_DATA(114,  uint8,    0,                5,        RO, 1, (void*)&pSystem->psModularRoofList[0]->eRunningMode) 
    SLAVE_REG_HOLD_DATA(115,  uint8,    0,                3,        RO, 1, (void*)&pSystem->psModularRoofList[0]->eFuncMode)
        
    SLAVE_REG_HOLD_DATA(116, uint16,    0,   5000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usCO2PPMSelf)     
    SLAVE_REG_HOLD_DATA(117,  int16, -200,   1400, RO, 1, (void*)&pSystem->psModularRoofList[0]->sRetAir_T)   
    SLAVE_REG_HOLD_DATA(118,  int16, -200,   1400, RO, 1, (void*)&pSystem->psModularRoofList[0]->sSupAir_T)
    SLAVE_REG_HOLD_DATA(119, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usRetAir_Vol)    
    SLAVE_REG_HOLD_DATA(120, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usSupAir_Vol)
        
    SLAVE_REG_HOLD_DATA(121, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usFreAir_Vol)   
    SLAVE_REG_HOLD_DATA(122, uint16,    0,  65000, RW, 1, (void*)&pSystem->psModularRoofList[0]->usFreAirSet_Vol)
    SLAVE_REG_HOLD_DATA(124, uint16,    0,   1000, RO, 1, (void*)&pSystem->psModularRoofList[0]->usFreAirDamper_Ang)
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
    SLAVE_REG_HOLD_DATA(145,  uint8,   85,    170, RW, 1, (void*)&pSystem->psModularRoofList[1]->eSwitchCmd) 
        
    SLAVE_REG_HOLD_DATA(146,  uint8,    0,     5, RO, 1, (void*)&pSystem->psModularRoofList[1]->eRunningMode)     
    SLAVE_REG_HOLD_DATA(147,  uint8,    0,     3, RO, 1, (void*)&pSystem->psModularRoofList[1]->eFuncMode)
    SLAVE_REG_HOLD_DATA(148, uint16,    0,  5000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usCO2PPMSelf)                                                                                      
    SLAVE_REG_HOLD_DATA(149,  int16, -200,  1400, RO, 1, (void*)&pSystem->psModularRoofList[1]->sRetAir_T)   
    SLAVE_REG_HOLD_DATA(150,  int16, -200,  1400, RO, 1, (void*)&pSystem->psModularRoofList[1]->sSupAir_T)
        
    SLAVE_REG_HOLD_DATA(151, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usRetAir_Vol)       
    SLAVE_REG_HOLD_DATA(152, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usSupAir_Vol)   
    SLAVE_REG_HOLD_DATA(153, uint16,    0,  65000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usFreAir_Vol)                                                                                        
    SLAVE_REG_HOLD_DATA(154, uint16,    0,  65000, RW, 1, (void*)&pSystem->psModularRoofList[1]->usFreAirSet_Vol)
    SLAVE_REG_HOLD_DATA(156, uint16,    0,   1000, RO, 1, (void*)&pSystem->psModularRoofList[1]->usFreAirDamper_Ang)
        
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
    SLAVE_REG_HOLD_DATA(179, uint16,    0,    500, RO, 1, (void*)&pSystem->psExAirFanList[0]->usRunningFreq); 
    SLAVE_REG_HOLD_DATA(181, uint16,    0,  65535, RO, 1, (void*)&pSystem->psExAirFanList[0]->Device.usRunTime_H);
    SLAVE_REG_HOLD_DATA(191, uint16,    0,  65535, RO, 1, (void*)&pSystem->psExAirFanList[1]->Device.usRunTime_H);
    SLAVE_REG_HOLD_DATA(201, uint16,    0,  65535, RO, 1, (void*)&pSystem->psExAirFanList[2]->Device.usRunTime_H);
    
    SLAVE_REG_HOLD_DATA(211, uint16, 0,            65535,        RO, 1, (void*)&pSystem->psExAirFanList[3]->Device.usRunTime_H);
    SLAVE_REG_HOLD_DATA(235, uint16, MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->psCO2SenList[0]->usAvgCO2PPM) 
    SLAVE_REG_HOLD_DATA(236, uint16, MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->psCO2SenList[1]->usAvgCO2PPM)  
    SLAVE_REG_HOLD_DATA(237, uint16, MIN_CO2_PPM,  MAX_CO2_PPM,  RO, 1, (void*)&pSystem->psCO2SenList[2]->usAvgCO2PPM)
    SLAVE_REG_HOLD_DATA(241,  int16, MIN_OUT_TEMP, MAX_OUT_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenOutList[0]->sAvgTemp)
        
    SLAVE_REG_HOLD_DATA(242, uint16, MIN_HUMI,    MAX_HUMI,    RO, 1, (void*)&pSystem->psTempHumiSenOutList[0]->usAvgHumi)     
    SLAVE_REG_HOLD_DATA(250,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[0]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(251,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[1]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(252,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[2]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(253,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[3]->sAvgTemp)
        
    SLAVE_REG_HOLD_DATA(254,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[4]->sAvgTemp)                           
    SLAVE_REG_HOLD_DATA(255,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[5]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(256,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[6]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(257,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[7]->sAvgTemp)
    SLAVE_REG_HOLD_DATA(258,  int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[8]->sAvgTemp)
        
    SLAVE_REG_HOLD_DATA(259,   int16, MIN_IN_TEMP, MAX_IN_TEMP, RO, 1, (void*)&pSystem->psTempHumiSenInList[9]->sAvgTemp)                           
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
        
    SLAVE_REG_HOLD_DATA(286,  uint16, 0, 65535, RO, 1, (void*)&pSystem->pUnitMeter->usPower)
    SLAVE_REG_HOLD_DATA(287,  uint16, 0, 65535, RO, 1, (void*)&pSystem->pUnitMeter->usTotalEnergy_L)
    SLAVE_REG_HOLD_DATA(288,  uint16, 0, 65535, RO, 1, (void*)&pSystem->pUnitMeter->usTotalEnergy_H)
    SLAVE_REG_HOLD_DATA(305,  uint16, 0, 65535, RO, 1, (void*)&pSystem->pExAirFanMeter->usPower)
    SLAVE_REG_HOLD_DATA(306,  uint16, 0, 65535, RO, 1, (void*)&pSystem->pExAirFanMeter->usTotalEnergy_L)
        
    SLAVE_REG_HOLD_DATA(307,  uint16, 0, 65535, RO, 1, (void*)&pSystem->pExAirFanMeter->usTotalEnergy_H)   
    
SLAVE_END_DATA_BUF(0, 307)    
    
    /******************************线圈数据域*************************/ 
SLAVE_BEGIN_DATA_BUF(&pThis->sBMS_BitCoilBuf,  &pThis->sBMSCommData.sMBCoilTable)
    
    SLAVE_COIL_BIT_DATA(4,  RW, (void*)&pSystem->xAlarmEnable) 
    SLAVE_COIL_BIT_DATA(16, RO, (void*)&pSystem->xTempSenInErr) 
    SLAVE_COIL_BIT_DATA(17, RO, (void*)&pSystem->xHumiSenInErr) 
    SLAVE_COIL_BIT_DATA(18, RO, (void*)&pSystem->xCO2SenErr) 
    SLAVE_COIL_BIT_DATA(24, RW, (void*)&pSystem->psModularRoofList[0]->xErrClean)
    
    SLAVE_COIL_BIT_DATA(25, RO, (void*)&pSystem->psModularRoofList[0]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(26, RO, (void*)&pSystem->psModularRoofList[0]->xStopErrFlag)
    SLAVE_COIL_BIT_DATA(27, RO, (void*)&pSystem->psModularRoofList[0]->Device.xErrFlag) 
    SLAVE_COIL_BIT_DATA(28, RO, (void*)&pSystem->psModularRoofList[0]->Device.xAlarmFlag)
    SLAVE_COIL_BIT_DATA(29, RO, (void*)&pSystem->psModularRoofList[0]->xCommErr)
    
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
    SLAVE_COIL_BIT_DATA(93, RO, (void*)&pSystem->psModularRoofList[1]->xCommErr)                                                            
    SLAVE_COIL_BIT_DATA(94, RO, (void*)&pSystem->psModularRoofList[1]->psSupAirFan->Device.eRunningState)
        
    SLAVE_COIL_BIT_DATA(95,  RO, (void*)&pSystem->psModularRoofList[1]->xSupAirDamperState)    
    SLAVE_COIL_BIT_DATA(96,  RO, (void*)&pSystem->psModularRoofList[1]->xWetMode)
    SLAVE_COIL_BIT_DATA(98,  RO, (void*)&pSystem->psModularRoofList[1]->xRetAirSenErr)
    SLAVE_COIL_BIT_DATA(99,  RO, (void*)&pSystem->psModularRoofList[1]->xFreAirSenErr)                                                                
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
    SLAVE_COIL_BIT_DATA(152, RO, (void*)&pSystem->pUnitMeter->sMBSlaveDev.xOnLine)     
    SLAVE_COIL_BIT_DATA(153, RO, (void*)&pSystem->pExAirFanMeter->sMBSlaveDev.xOnLine) 
    SLAVE_COIL_BIT_DATA(160, RW, (void*)&pSystem->psExAirFanList[0]->eSwitchCmd)

    SLAVE_COIL_BIT_DATA(161, RO, (void*)&pSystem->psExAirFanList[0]->xExAirFanErr)    
    SLAVE_COIL_BIT_DATA(162, RO, (void*)&pSystem->psExAirFanList[0]->Device.eRunningState)
    SLAVE_COIL_BIT_DATA(168, RW, (void*)&pSystem->psExAirFanList[1]->eSwitchCmd)
    SLAVE_COIL_BIT_DATA(169, RO, (void*)&pSystem->psExAirFanList[1]->xExAirFanErr)    
    SLAVE_COIL_BIT_DATA(170, RO, (void*)&pSystem->psExAirFanList[1]->Device.eRunningState)    
     
    SLAVE_COIL_BIT_DATA(176, RW, (void*)&pSystem->psExAirFanList[2]->eSwitchCmd)
    SLAVE_COIL_BIT_DATA(177, RO, (void*)&pSystem->psExAirFanList[2]->xExAirFanErr)    
    SLAVE_COIL_BIT_DATA(178, RO, (void*)&pSystem->psExAirFanList[2]->Device.eRunningState)  
    SLAVE_COIL_BIT_DATA(184, RW, (void*)&pSystem->psExAirFanList[3]->eSwitchCmd)
    SLAVE_COIL_BIT_DATA(185, RO, (void*)&pSystem->psExAirFanList[3]->xExAirFanErr)    
        
    SLAVE_COIL_BIT_DATA(186, RO, (void*)&pSystem->psExAirFanList[3]->Device.eRunningState)  
    SLAVE_COIL_BIT_DATA(192, RO, (void*)&pSystem->psCO2SenList[0]->xCO2SenErr)
    SLAVE_COIL_BIT_DATA(193, RO, (void*)&pSystem->psCO2SenList[1]->xCO2SenErr)
    SLAVE_COIL_BIT_DATA(194, RO, (void*)&pSystem->psCO2SenList[2]->xCO2SenErr)
    SLAVE_COIL_BIT_DATA(200, RO, (void*)&pSystem->xTempSenOutErr)
        
    SLAVE_COIL_BIT_DATA(201, RO, (void*)&pSystem->xHumiSenOutErr)
    SLAVE_COIL_BIT_DATA(208, RO, (void*)&pSystem->psTempHumiSenInList[0]->xTempSenErr)     
    SLAVE_COIL_BIT_DATA(209, RO, (void*)&pSystem->psTempHumiSenInList[1]->xTempSenErr)
    SLAVE_COIL_BIT_DATA(210, RO, (void*)&pSystem->psTempHumiSenInList[2]->xTempSenErr)     
    SLAVE_COIL_BIT_DATA(211, RO, (void*)&pSystem->psTempHumiSenInList[3]->xTempSenErr)    
    
    SLAVE_COIL_BIT_DATA(212, RO, (void*)&pSystem->psTempHumiSenInList[4]->xTempSenErr)     
    SLAVE_COIL_BIT_DATA(213, RO, (void*)&pSystem->psTempHumiSenInList[5]->xTempSenErr)
    SLAVE_COIL_BIT_DATA(214, RO, (void*)&pSystem->psTempHumiSenInList[6]->xTempSenErr)     
    SLAVE_COIL_BIT_DATA(215, RO, (void*)&pSystem->psTempHumiSenInList[7]->xTempSenErr) 
    SLAVE_COIL_BIT_DATA(216, RO, (void*)&pSystem->psTempHumiSenInList[8]->xTempSenErr) 
        
    SLAVE_COIL_BIT_DATA(217, RO, (void*)&pSystem->psTempHumiSenInList[9]->xTempSenErr)
    SLAVE_COIL_BIT_DATA(218, RO, (void*)&pSystem->psTempHumiSenInList[10]->xTempSenErr)     
    SLAVE_COIL_BIT_DATA(219, RO, (void*)&pSystem->psTempHumiSenInList[11]->xTempSenErr)   
    SLAVE_COIL_BIT_DATA(224, RO, (void*)&pSystem->psTempHumiSenInList[0]->xHumiSenErr)     
    SLAVE_COIL_BIT_DATA(225, RO, (void*)&pSystem->psTempHumiSenInList[1]->xHumiSenErr)
        
    SLAVE_COIL_BIT_DATA(226, RO, (void*)&pSystem->psTempHumiSenInList[2]->xHumiSenErr)     
    SLAVE_COIL_BIT_DATA(227, RO, (void*)&pSystem->psTempHumiSenInList[3]->xHumiSenErr)                                                                 
    SLAVE_COIL_BIT_DATA(228, RO, (void*)&pSystem->psTempHumiSenInList[4]->xHumiSenErr)     
    SLAVE_COIL_BIT_DATA(229, RO, (void*)&pSystem->psTempHumiSenInList[5]->xHumiSenErr)
    SLAVE_COIL_BIT_DATA(230, RO, (void*)&pSystem->psTempHumiSenInList[6]->xHumiSenErr) 
        
    SLAVE_COIL_BIT_DATA(231, RO, (void*)&pSystem->psTempHumiSenInList[7]->xHumiSenErr) 
    SLAVE_COIL_BIT_DATA(232, RO, (void*)&pSystem->psTempHumiSenInList[8]->xHumiSenErr)     
    SLAVE_COIL_BIT_DATA(233, RO, (void*)&pSystem->psTempHumiSenInList[9]->xHumiSenErr)
    SLAVE_COIL_BIT_DATA(234, RO, (void*)&pSystem->psTempHumiSenInList[10]->xHumiSenErr)     
    SLAVE_COIL_BIT_DATA(235, RO, (void*)&pSystem->psTempHumiSenInList[11]->xHumiSenErr)   
        
SLAVE_END_DATA_BUF(0, 240)    
   
    pThis->sBMSCommData.pxSlaveDataMapIndex = xBMS_DevDataMapIndex;         //绑定映射函数
    pThis->psBMSInfo->sMBCommInfo.psSlaveCurData = &pThis->sBMSCommData;
}

/*BMS数据监控*/
void vBMS_MonitorRegist(BMS* pt)
{
    OS_ERR err = OS_ERR_NONE;
    BMS* pThis = (BMS*)pt;

    OSSemCreate( &(pThis->sValChange), "sValChange", 0, &err );  //事件消息量初始化
    
    MONITOR(&pThis->eSystemMode,  uint8, &pThis->sValChange)
    MONITOR(&pThis->eRunningMode, uint8, &pThis->sValChange)
    
    MONITOR(&pThis->usTempSet,         uint16, &pThis->sValChange)
    MONITOR(&pThis->usFreAirSet_Vol_H, uint16, &pThis->sValChange)
    MONITOR(&pThis->usFreAirSet_Vol_L, uint16, &pThis->sValChange)
    
    MONITOR(&pThis->ucExAirCoolRatio, uint8, &pThis->sValChange)
    MONITOR(&pThis->ucExAirHeatRatio, uint8, &pThis->sValChange)
    
    MONITOR(&pThis->usHumidityMin, uint16, &pThis->sValChange)
    MONITOR(&pThis->usHumidityMax, uint16, &pThis->sValChange)
    
    MONITOR(&pThis->usCO2AdjustThr_V,  uint16, &pThis->sValChange)
    MONITOR(&pThis->usCO2AdjustDeviat, uint16, &pThis->sValChange)
        
    MONITOR(&pThis->usExAirFanFreq,    uint16, &pThis->sValChange) 
    MONITOR(&pThis->usExAirFanMinFreq, uint16, &pThis->sValChange) 
    MONITOR(&pThis->usExAirFanMaxFreq, uint16, &pThis->sValChange)
    
    MONITOR(&pThis->usExAirFanRated_Vol_H, uint16, &pThis->sValChange) 
    MONITOR(&pThis->usExAirFanRated_Vol_L, uint16,&pThis->sValChange)
    
    MONITOR(&pThis->eExAirFanType, uint8, &pThis->sValChange)
}

/*BMS数据默认值初始化*/
void vBMS_InitDefaultData(BMS* pt)
{
    BMS*    pThis   = (BMS*)pt;
    
    DATA_INIT(pThis->eSystemMode,  MODE_CLOSE)
    DATA_INIT(pThis->eRunningMode, RUN_MODE_COOL)
    
    DATA_INIT(pThis->usHumidityMin,     55)
    DATA_INIT(pThis->usHumidityMax,     65) 
    DATA_INIT(pThis->usCO2AdjustThr_V,  2700)
    DATA_INIT(pThis->usCO2AdjustDeviat, 270)

    DATA_INIT(pThis->usTempSet,           260)
    DATA_INIT(pThis->usFreAirSet_Vol_H,     0)
    DATA_INIT(pThis->usFreAirSet_Vol_L, 60000)
    
    DATA_INIT(pThis->usExAirFanRated_Vol_H,  0)
    DATA_INIT(pThis->usExAirFanRated_Vol_L,  36000)
    DATA_INIT(pThis->usExAirFanFreq,         220)
    DATA_INIT(pThis->usExAirFanMinFreq,      220)
    DATA_INIT(pThis->usExAirFanMaxFreq,      500) 
    DATA_INIT(pThis->eExAirFanType,           1)
    
    DATA_INIT(pThis->ucExAirCoolRatio,    90)
    DATA_INIT(pThis->ucExAirHeatRatio,    90)
    DATA_INIT(pThis->eExAirFanType,       TYPE_CONSTANT_VARIABLE)
    
    
//    myprintf("pThis->usModeChangeTime_1 %d\n", pThis->ucExAirCoolRatio);
}

void vBMS_Init(BMS* pt)
{
    BMS* pThis = (BMS*)pt;  
    pThis->psBMSInfo = psMBGetSlaveInfo();

    vBMS_InitBMSCommData(pThis);
    vBMS_InitDefaultData(pThis);
    vBMS_MonitorRegist(pThis);     
}

CTOR(BMS)   //BMS构造函数
  
END_CTOR

BMS* BMS_Core()
{
    System* pSystem = NULL;
    if(psBMS == NULL)
    {
        psBMS = (BMS*)&BMSCore;
        pSystem = (System*)System_Core();
        
        if(psBMS != NULL && pSystem != NULL)
        {
            vBMS_Init(psBMS);
        }
    }
    return psBMS;
}

