#include "sensor.h"
#include "modularRoof.h"
#include "md_timer.h"

#define MODULAR_ROOF_PROTOCOL_TYPE_ID   0
#define MODULAR_HEART_BEAT_PERIOD_S     20

#define MODULAR_TIME_OUT_S              5
#define MODULAR_TIME_OUT_DELAY_S        20

/*************************************************************
*                         模块                               *
**************************************************************/
void vModular_Init(Modular* pt)
{
    uint8_t n = 0;
    Modular* pThis = (Modular*)pt;
    
    for(n=0; n < AMBIENT_OUT_FAN_NUM; n++)    //室外风机
    {
        pThis->psAmbientOutFanList[n] = (AmbientOutFan*)AmbientOutFan_new();
    }
    for(n=0; n < COMP_NUM; n++)               //压缩机
    {
        pThis->psCompList[n] = (Compressor*)Compressor_new();
    } 
}

CTOR(Modular)   //模块构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init,  vModular_Init);
END_CTOR

/*************************************************************
*                         屋顶机机组                         *
**************************************************************/

/*机组开启*/
void vModularRoof_SwitchOpen(IDevSwitch* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevSwitch, ModularRoof);
    if( (pThis->sMBSlaveDev.xOnLine == TRUE) && (pThis->xStopErrFlag == FALSE) )   //无故障则开启
    {
        pThis->eSwitchCmd = CMD_OPEN;  
        
#if DEBUG_ENABLE > 0
    pThis->Device.eRunningState = STATE_RUN;
    myprintf("vModularRoof_SwitchOpen %d\n", pThis->eSwitchCmd);
#endif        
    }
}

/*机组关闭*/
void vModularRoof_SwitchClose(IDevSwitch* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevSwitch, ModularRoof);
    pThis->eSwitchCmd = CMD_CLOSE;
    
#if DEBUG_ENABLE > 0
     pThis->Device.eRunningState = STATE_STOP;
    myprintf("vModularRoof_SwitchClose %d\n", pThis->eSwitchCmd);
#endif    
}

/*机组运行状态设置*/
void vModularRoof_SetRunningMode(ModularRoof* pt, eRunningMode eMode)
{
    ModularRoof* pThis = (ModularRoof*)pt;
    pThis->eRunningMode = eMode; 
    
#if DEBUG_ENABLE > 0
    myprintf("vModularRoof_SetRunningMode %d\n", pThis->eRunningMode);
#endif      
}

/*机组数据默认值初始化*/
void vModularRoof_InitDefaultData(ModularRoof* pt)
{
    ModularRoof* pThis = (ModularRoof*)pt;
   
    DATA_INIT(pThis->eSwitchCmd,   CMD_CLOSE)
    DATA_INIT(pThis->eRunningMode, RUN_MODE_HEAT)
    
    DATA_INIT(pThis->usCoolTempSet, 260)
    DATA_INIT(pThis->usHeatTempSet, 200)

    DATA_INIT(pThis->usFreAirSet_Vol, 30000)
    
    DATA_INIT(pThis->usHumidityMin,    55)
    DATA_INIT(pThis->usHumidityMax,    65)
    
    DATA_INIT(pThis->usCO2AdjustThr_V,  2700)
    DATA_INIT(pThis->usCO2AdjustDeviat,   50)
    
//    DATA_INIT(pThis->xErrClean,   1)

//    myprintf("pThis->eRunningMode %d  eRunningMode  %d\n", *(uint8_t*)p, pThis->eRunningMode); 
}

/*通讯映射函数*/
BOOL xModularRoof_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID, USHORT usAddr, USHORT* psIndex)
{
    USHORT i = 0;
    switch(ucProtocolID)
	{
        case MODULAR_ROOF_PROTOCOL_TYPE_ID:
            if(eDataType == RegHoldData)
            {
                switch(usAddr)
                {
                    case 0 :  i = 0 ;  break;
                    case 2 :  i = 1 ;  break;
                    case 3 :  i = 2 ;  break;
                    case 5 :  i = 3 ;  break;
                    case 6 :  i = 4 ;  break;
                    case 8 :  i = 5 ;  break;
                    case 9 :  i = 6 ;  break;
                    case 10:  i = 7 ;  break;
                    case 11:  i = 8 ;  break;
                    case 12:  i = 9 ;  break;
                    case 16:  i = 10;  break;
                    case 17:  i = 11;  break;
                    case 18:  i = 12;  break;
                    case 37:  i = 13;  break;
                    case 38:  i = 14;  break;
                    case 39:  i = 15;  break;
                    case 40:  i = 16;  break;
                    case 44:  i = 17;  break;
                    case 45:  i = 18;  break;
                    case 46:  i = 19;  break;
                    case 47:  i = 20;  break;
                    case 48:  i = 21;  break;
                    case 49:  i = 22;  break;
                    case 51:  i = 23;  break;
                    case 52:  i = 24;  break;
                    case 53:  i = 25;  break;
                    case 54:  i = 26;  break;
                    case 55:  i = 27;  break;

                    default:
                		return FALSE;
                	break;
                }
            }
            else if(eDataType == CoilData)
            {
                switch(usAddr)
                {
                    case 0  :  i = 0 ;  break;
                    case 1  :  i = 1 ;  break;
                    case 2  :  i = 2 ;  break;
                    case 3  :  i = 3 ;  break;
                    case 10 :  i = 4 ;  break;
                    case 16 :  i = 5 ;  break;
                    case 17 :  i = 6 ;  break;
                    case 18 :  i = 7 ;  break;
                    case 48 :  i = 8 ;  break;
                    case 54 :  i = 9 ;  break;
                    case 55	:  i = 10;  break;
                    case 64	:  i = 11;  break;
                    case 65	:  i = 12;  break;
                    case 66	:  i = 13;  break;
                    case 67	:  i = 14;  break;
                    case 80	:  i = 15;  break;
                    case 81	:  i = 16;  break;
                    case 82	:  i = 17;  break;
                    case 83	:  i = 18;  break;
                    case 96	:  i = 19;  break;
                    case 97	:  i = 20;  break;
                    case 98	:  i = 21;  break;
                    case 99	:  i = 22;  break;
                    case 112:  i = 23;  break;
                    case 113:  i = 24;  break;
                    case 114:  i = 25;  break;
                    case 115:  i = 26;  break;
                    case 144:  i = 27;  break;
                    case 145:  i = 28;  break;
                    case 264:  i = 29;  break;
                    case 265:  i = 30;  break;
                    case 384:  i = 31;  break;
                    case 385:  i = 32;  break;
                    case 404:  i = 33;  break;
                    case 405:  i = 34;  break;
                    case 621:  i = 35;  break;
                    case 622:  i = 36;  break;
                    case 630:  i = 37;  break;
                    case 631:  i = 38;  break;
  
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

/*机组通讯数据表初始化*/
void vModularRoof_InitDevCommData(ModularRoof* pt)
{
    ModularRoof* pThis = (ModularRoof*)pt;
    
MASTER_PBUF_INDEX_ALLOC()
    
MASTER_TEST_CMD_INIT(&pThis->sDevCommData.sMBDevCmdTable, 0, READ_REG_HOLD, 0x302A, TRUE)     //测试命令
MASTER_HEART_BEAT_INIT(&pThis->sDevCommData.sMBDevHeartBeat, 0, READ_REG_HOLD, 0x302A, MODULAR_HEART_BEAT_PERIOD_S, TRUE)  //心跳帧
    
    /******************************保持寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(&pThis->sModularRoof_RegHoldBuf, &pThis->sDevCommData.sMBRegHoldTable)
    
    MASTER_REG_HOLD_DATA(0, uint16,   0, 65535,  0x302A,  RO, 1, (void*)&pThis->usUnitID)
    MASTER_REG_HOLD_DATA(2,  uint8,  85,   170,    0x55,  RW, 1, (void*)&pThis->eSwitchCmd)
    MASTER_REG_HOLD_DATA(3,  uint8,   1,     4,       1,  RW, 1, (void*)&pThis->eRunningMode)
    MASTER_REG_HOLD_DATA(5, uint16, 160,   350,     260,  RW, 1, (void*)&pThis->usCoolTempSet) 
    MASTER_REG_HOLD_DATA(6, uint16, 160,   350,     200,  RW, 1, (void*)&pThis->usHeatTempSet)

    MASTER_REG_HOLD_DATA(8,  uint16,   0, 65000,  30000,  RW, 1, (void*)&pThis->usFreAirSet_Vol)
    MASTER_REG_HOLD_DATA(9,  uint16,   0,   100,     55,  RW, 1, (void*)&pThis->usHumidityMin)
    MASTER_REG_HOLD_DATA(10, uint16,   0,   100,     65,  RW, 1, (void*)&pThis->usHumidityMax)
    MASTER_REG_HOLD_DATA(11, uint16,   0,  5000,   2700,  RW, 1, (void*)&pThis->usCO2AdjustThr_V) 
    MASTER_REG_HOLD_DATA(12, uint16,   5,   500,     50,  RW, 1, (void*)&pThis->usCO2AdjustDeviat)
     
    MASTER_REG_HOLD_DATA(16,  int16,    MIN_IN_TEMP,  MAX_IN_TEMP,  0,  RW, 1, (void*)&pThis->sAmbientIn_T)
    MASTER_REG_HOLD_DATA(17, uint16,    MIN_HUMI,     MAX_HUMI,     0,  RW, 1, (void*)&pThis->usAmbientIn_H)
    MASTER_REG_HOLD_DATA(18, uint16,    MIN_CO2_PPM,  MAX_CO2_PPM,  0,  RW, 1, (void*)&pThis->usCO2PPM)   
    MASTER_REG_HOLD_DATA(37,  uint8,    0,            5,            0,  RO, 1, (void*)&pThis->psModularList[0]->ucModularState) 
    MASTER_REG_HOLD_DATA(38,  uint8,    0,            5,            0,  RO, 1, (void*)&pThis->psModularList[1]->ucModularState)   
                                                                                          
    MASTER_REG_HOLD_DATA(39, uint8,     0,     5,     0,  RO, 1, (void*)&pThis->psModularList[2]->ucModularState) 
    MASTER_REG_HOLD_DATA(40, uint8,     0,     5,     0,  RO, 1, (void*)&pThis->psModularList[3]->ucModularState)   
    MASTER_REG_HOLD_DATA(44, int16,  -200,  1400,     0,  RO, 1, (void*)&pThis->sRetAir_T)      
    MASTER_REG_HOLD_DATA(45, int16,  -200,  1400,     0,  RO, 1, (void*)&pThis->sSupAir_T)
    MASTER_REG_HOLD_DATA(46, int16,  -400,   700,     0,  RO, 1, (void*)&pThis->sAmbientInSelf_T)
   
    MASTER_REG_HOLD_DATA(47, uint16,    0,   100,     0,  RO, 1, (void*)&pThis->usAmbientInSelf_H)
    MASTER_REG_HOLD_DATA(48, int16,  -400,   700,     0,  RO, 1, (void*)&pThis->sAmbientOutSelf_T) 
    MASTER_REG_HOLD_DATA(49, uint16,    0,   100,     0,  RO, 1, (void*)&pThis->usAmbientOutSelf_H)
    MASTER_REG_HOLD_DATA(51, uint16,    0,  1000,  1000,  RO, 1, (void*)&pThis->usFreAirDamper_Ang)
    MASTER_REG_HOLD_DATA(52, uint16,    0,  5000,     0,  RO, 1, (void*)&pThis->usCO2PPMSelf)         
   
    MASTER_REG_HOLD_DATA(53, uint16,    0,  65000,    0,  RO, 1, (void*)&pThis->usFreAir_Vol)
    MASTER_REG_HOLD_DATA(54, uint16,    0,  65000,    0,  RO, 1, (void*)&pThis->usSupAir_Vol)
    MASTER_REG_HOLD_DATA(55, uint16,    0,  65000,    0,  RO, 1, (void*)&pThis->usRetAir_Vol)

MASTER_END_DATA_BUF(0, 55)
    
    /******************************线圈数据域*************************/ 
MASTER_BEGIN_DATA_BUF(&pThis->sModularRoof_BitCoilBuf, &pThis->sDevCommData.sMBCoilTable) 
    
    MASTER_COIL_BIT_DATA(0,  0, RO, (void*)&pThis->Device.eRunningState);   
    MASTER_COIL_BIT_DATA(1,  0, RO, (void*)&pThis->xStopErrFlag);
    MASTER_COIL_BIT_DATA(2,  0, RO, (void*)&pThis->Device.xErrFlag);
    MASTER_COIL_BIT_DATA(3,  0, RO, (void*)&pThis->Device.xAlarmFlag); 
    MASTER_COIL_BIT_DATA(10, 0, RW, (void*)&pThis->xErrClean);
    
    MASTER_COIL_BIT_DATA(16, 0, RW, (void*)&pThis->xTempSenInErr); 
    MASTER_COIL_BIT_DATA(17, 0, RW, (void*)&pThis->xHumiSenInErr);
    MASTER_COIL_BIT_DATA(18, 0, RW, (void*)&pThis->xCO2SenErr);
    MASTER_COIL_BIT_DATA(48, 0, RO, (void*)&pThis->psSupAirFan->Device.eRunningState);
    MASTER_COIL_BIT_DATA(54, 0, RO, (void*)&pThis->xSupAirDamperState); 
    
    MASTER_COIL_BIT_DATA(55,  0, RO, (void*)&pThis->xWetMode);  
    MASTER_COIL_BIT_DATA(64,  0, RO, (void*)&pThis->psModularList[0]->psCompList[0]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(65,  0, RO, (void*)&pThis->psModularList[0]->psCompList[1]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(66,  0, RO, (void*)&pThis->psModularList[0]->psAmbientOutFanList[0]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(67,  0, RO, (void*)&pThis->psModularList[0]->psAmbientOutFanList[1]->Device.eRunningState);
    
    MASTER_COIL_BIT_DATA(80,  0, RO, (void*)&pThis->psModularList[1]->psCompList[0]->Device.eRunningState);  
    MASTER_COIL_BIT_DATA(81,  0, RO, (void*)&pThis->psModularList[1]->psCompList[1]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(82,  0, RO, (void*)&pThis->psModularList[1]->psAmbientOutFanList[0]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(83,  0, RO, (void*)&pThis->psModularList[1]->psAmbientOutFanList[1]->Device.eRunningState);
    MASTER_COIL_BIT_DATA(96,  0, RO, (void*)&pThis->psModularList[2]->psCompList[0]->Device.eRunningState); 
    
    MASTER_COIL_BIT_DATA(97,  0, RO, (void*)&pThis->psModularList[2]->psCompList[1]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(98,  0, RO, (void*)&pThis->psModularList[2]->psAmbientOutFanList[0]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(99,  0, RO, (void*)&pThis->psModularList[2]->psAmbientOutFanList[1]->Device.eRunningState);
    MASTER_COIL_BIT_DATA(112, 0, RO, (void*)&pThis->psModularList[3]->psCompList[0]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(113, 0, RO, (void*)&pThis->psModularList[3]->psCompList[1]->Device.eRunningState); 
    
    MASTER_COIL_BIT_DATA(114, 0, RO, (void*)&pThis->psModularList[3]->psAmbientOutFanList[0]->Device.eRunningState); 
    MASTER_COIL_BIT_DATA(115, 0, RO, (void*)&pThis->psModularList[3]->psAmbientOutFanList[1]->Device.eRunningState);
    MASTER_COIL_BIT_DATA(144, 0, RO, (void*)&pThis->psModularList[0]->xTempSenOutErr); 
    MASTER_COIL_BIT_DATA(145, 0, RO, (void*)&pThis->psModularList[0]->xHumiSenOutErr);
    MASTER_COIL_BIT_DATA(264, 0, RO, (void*)&pThis->psModularList[1]->xTempSenOutErr); 
    
    MASTER_COIL_BIT_DATA(265, 0, RO, (void*)&pThis->psModularList[1]->xHumiSenOutErr); 
    MASTER_COIL_BIT_DATA(384, 0, RO, (void*)&pThis->psModularList[2]->xTempSenOutErr); 
    MASTER_COIL_BIT_DATA(385, 0, RO, (void*)&pThis->psModularList[2]->xHumiSenOutErr); 
    MASTER_COIL_BIT_DATA(404, 0, RO, (void*)&pThis->psModularList[3]->xTempSenOutErr); 
    MASTER_COIL_BIT_DATA(405, 0, RO, (void*)&pThis->psModularList[3]->xHumiSenOutErr); 
    
    MASTER_COIL_BIT_DATA(621, 0, RO, (void*)&pThis->xFreAirSenErr);
    MASTER_COIL_BIT_DATA(622, 0, RO, (void*)&pThis->xRetAirSenErr);
    MASTER_COIL_BIT_DATA(630, 0, RO, (void*)&pThis->xCO2SenSelfErr_1);
    MASTER_COIL_BIT_DATA(631, 0, RO, (void*)&pThis->xCO2SenSelfErr_2);
    
MASTER_END_DATA_BUF(0, 631)  
    
//    myprintf("&sMBRegHoldTable %d  sModularRoof_RegHoldBuf %d\n", 
//    &pThis->sDevCommData.sMBRegHoldTable, &pThis->sModularRoof_RegHoldBuf);
    
    pThis->sDevCommData.ucProtocolID = MODULAR_ROOF_PROTOCOL_TYPE_ID;
    pThis->sDevCommData.pxDevDataMapIndex = xModularRoof_DevDataMapIndex;  //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}

/*向通讯主栈中注册设备*/
void vModularRoof_RegistDev(ModularRoof* pt)
{
    ModularRoof* pThis = (ModularRoof*)pt;
   (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBSlaveDev);
}

/*机组数据监控*/
void vModularRoof_RegistMonitor(ModularRoof* pt)
{
    OS_ERR err = OS_ERR_NONE;
    ModularRoof* pThis = (ModularRoof*)pt;

    OSSemCreate( &(pThis->sValChange), "sValChange", 0, &err );  //事件消息量初始化
    
    MONITOR(&pThis->Device.eRunningState,  uint8, &pThis->sValChange)
    MONITOR(&pThis->sSupAir_T,             int16, &pThis->sValChange)
    MONITOR(&pThis->usFreAir_Vol,         uint16, &pThis->sValChange)
                                       
    MONITOR(&pThis->sAmbientInSelf_T,      int16, &pThis->sValChange)
    MONITOR(&pThis->usAmbientInSelf_H,    uint16, &pThis->sValChange)
                                       
    MONITOR(&pThis->sAmbientOutSelf_T,     int16, &pThis->sValChange)
    MONITOR(&pThis->usAmbientOutSelf_H,   uint16, &pThis->sValChange)
                                       
    MONITOR(&pThis->usCO2PPMSelf,         uint16, &pThis->sValChange)
                                        
    MONITOR(&pThis->xStopErrFlag,          uint8, &pThis->sValChange)
    MONITOR(&pThis->xCommErr,              int16, &pThis->sValChange)  
}

/*机组EEPROM数据注册*/
void vModularRoof_RegistEEPROMData(ModularRoof* pt)
{
    ModularRoof* pThis = (ModularRoof*)pt;
    
    EEPROM_DATA(TYPE_RUNTIME, pThis->Device.ulRunTime_S)
}

/*机组所有压缩机是否关闭*/
BOOL xModularRoof_UnitCompsClosed(ModularRoof* pt)
{
    uint8_t  n, m;
    ModularRoof* pThis = (ModularRoof*)pt;
    
    Modular*     pModular     = NULL;
    Compressor*  pComp        = NULL;
    
    for(n=0; n< MODULAR_NUM; n++)
    {
        pModular = pThis->psModularList[n];
        for(m=0; m < COMP_NUM; m++)
        {
            pComp = pModular->psCompList[m];
            if(pComp->Device.eRunningState == STATE_RUN)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}


void vModularRoof_TimeoutInd(void * p_tmr, void * p_arg)  //定时器中断服务函数
{
    uint8_t n = 0;
    ModularRoof* pThis = (ModularRoof*)p_arg;

    pThis->xCommErr = (pThis->sMBSlaveDev.xOnLine == TRUE) ? FALSE:TRUE;
    if(pThis->xStopErrFlag == TRUE)
    {
        vModularRoof_SwitchClose(SUPER_PTR(pThis, IDevSwitch));
    }
    
    if(xModularRoof_UnitCompsClosed(pThis) == TRUE) //压缩机是否全部关闭
    {
        pThis->xCompRunning = FALSE;
    }
    else
    {
        pThis->xCompRunning =TRUE;
    }
}

/*机组初始化*/
void vModularRoof_Init(ModularRoof* pt, sMBMasterInfo* psMBMasterInfo, UCHAR ucDevAddr, uint8_t ucDevIndex)
{
    uint8_t n = 0;
    ModularRoof* pThis     = (ModularRoof*)pt;
    Modular*     psModular = NULL;
    
    pThis->psMBMasterInfo        = psMBMasterInfo;
    pThis->psSupAirFan           = (SupAirFan*)SupAirFan_new();  //送风风机
    pThis->sMBSlaveDev.ucDevAddr = ucDevAddr;
    pThis->Device.ucDevIndex     = ucDevIndex;
    
    for(n=0; n < MODULAR_NUM; n++)
    {
        psModular = (Modular*)Modular_new();
        if(psModular != NULL)
        {
            psModular->init(psModular);
            pThis->psModularList[n] = psModular;        //模块列表       
        }       
    } 

    vModularRoof_RegistEEPROMData(pThis);   //EEPROM数据注册
    vModularRoof_RegistMonitor(pThis);      //注册监控数据
   
    vModularRoof_InitDevCommData(pThis);    //初始化设备通讯数据表 
    vModularRoof_InitDefaultData(pThis);    //初始化默认数据
    vModularRoof_RegistDev(pThis);          //向通讯主栈中注册设备

    //机组周期定时器
    (void)xTimerRegist(&pThis->sModularRoofTmr, MODULAR_TIME_OUT_DELAY_S, MODULAR_TIME_OUT_S, 
                        OS_OPT_TMR_PERIODIC, vModularRoof_TimeoutInd, pThis, FALSE);     
}

CTOR(ModularRoof)   //屋顶机构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init,                       vModularRoof_Init);

    FUNCTION_SETTING(IDevSwitch.switchOpen,      vModularRoof_SwitchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose,     vModularRoof_SwitchClose);

    FUNCTION_SETTING(setRunningMode, vModularRoof_SetRunningMode);
END_CTOR





