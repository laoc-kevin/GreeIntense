#include "modularRoof.h"

#define DTU247_PROTOCOL_TYPE_ID   1

/*************************************************************
*                         模块                               *
**************************************************************/

void vModular_Init(Modular* pt)
{
    uint8_t n = 0;
    Modular* pThis = (Modular*)pt;
    
    for(n=0; n < AMBIENT_OUT_FAN_NUM; n++)
    {
        pThis->psAmbientOutFanList[n] = (AmbientOutFan*)AmbientOutFan_new();
    }
    for(n=0; n < COMP_NUM; n++)
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
    }
}

/*机组关闭*/
void vModularRoof_SwitchClose(IDevSwitch* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevSwitch, ModularRoof);
    pThis->eSwitchCmd = CMD_CLOSE; 
}

/*机组运行状态设置*/
void vModularRoof_SetRunningMode(IDevRunning* pt, eRunningMode eMode)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevRunning, ModularRoof);
    if( (pThis->sMBSlaveDev.xOnLine == TRUE) && (pThis->xStopErrFlag == FALSE) )   //无故障则开启
    {
        pThis->eRunningMode = eMode; 
    }  
}

/*机组数据默认值初始化*/
void vModularRoof_InitDefaultData(ModularRoof* pt)
{
    ModularRoof* pThis = (ModularRoof*)pt;
    
    DATA_INIT(pThis->eSwitchCmd,  CMD_CLOSE)
   
    
    DATA_INIT(pThis->usCoolTempSet, 260)
    DATA_INIT(pThis->usHeatTempSet, 260)

    DATA_INIT(pThis->usFreAirSet_Vol, 30000)
    
    DATA_INIT(pThis->usHumidityMin,    55)
    DATA_INIT(pThis->usHumidityMax,    65)
}

/*通讯映射函数*/
BOOL xModularRoof_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID, USHORT usAddr, USHORT* psIndex)
{
    USHORT i = 0;
    switch(ucProtocolID)
	{
        case DTU247_PROTOCOL_TYPE_ID:
            if(eDataType == RegHoldData)
            {
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
            }
            else if(eDataType == CoilData)
            {
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
    
    sMBDevDataTable* psMBRegHoldTable = &pThis->sDevCommData.sMBRegHoldTable; 
    sMBDevDataTable* psMBCoilTable    = &pThis->sDevCommData.sMBCoilTable; 
    sMBTestDevCmd*   psMBCmd          = &pThis->sDevCommData.sMBDevCmdTable;

MASTER_PBUF_INDEX_ALLOC()
MASTER_TEST_CMD_INIT(psMBCmd, 0, READ_REG_HOLD, 0x302A, TRUE)  
    
    /******************************保持寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(pThis->sModularRoof_RegHoldBuf, psMBRegHoldTable)
    
    MASTER_REG_HOLD_DATA(0, uint16,   0, 65535,  0x302A,  RO, 1, (void*)&pThis->usUnitID)
    MASTER_REG_HOLD_DATA(2, uint16,   0, 65535,    0x55,  RW, 1, (void*)&pThis->eSwitchCmd)
    MASTER_REG_HOLD_DATA(3, uint16,   0, 65535,       0,  RW, 1, (void*)&pThis->eRunningMode)
    MASTER_REG_HOLD_DATA(5, uint16, 160,   350,     260,  RW, 1, (void*)&pThis->usCoolTempSet) 
    MASTER_REG_HOLD_DATA(6, uint16, 160,   350,      20,  RW, 1, (void*)&pThis->usHeatTempSet)

    MASTER_REG_HOLD_DATA(8,  uint16,   0, 65000,  30000,  RW, 1, (void*)&pThis->usFreAirSet_Vol)
    MASTER_REG_HOLD_DATA(9,  uint16,   0,   100,     55,  RW, 1, (void*)&pThis->usHumidityMin)
    MASTER_REG_HOLD_DATA(10, uint16,   0,   100,     65,  RW, 1, (void*)&pThis->usHumidityMax)
    MASTER_REG_HOLD_DATA(11, uint16,   0,  5000,   2700,  RW, 1, (void*)&pThis->usCO2AdjustThr_V) 
    MASTER_REG_HOLD_DATA(12, uint16,   5,   500,     50,  RW, 1, (void*)&pThis->usCO2AdjustDeviat)
        
    MASTER_REG_HOLD_DATA(16, int16,  -400,  1200,     0,  WO, 1, (void*)&pThis->sAmbientIn_T)
    MASTER_REG_HOLD_DATA(17, uint16,    0,   100,     0,  WO, 1, (void*)&pThis->usAmbientIn_H)
    MASTER_REG_HOLD_DATA(18, uint16,    0,  5000,     0,  WO, 1, (void*)&pThis->usCO2PPM)   
    MASTER_REG_HOLD_DATA(37, uint16,    0,     5,     0,  RO, 1, (void*)&pThis->psModularList[0]->eSystemState) 
    MASTER_REG_HOLD_DATA(38, uint16,    0,     5,     0,  RO, 1, (void*)&pThis->psModularList[1]->eSystemState)   
   
    MASTER_REG_HOLD_DATA(39, uint16,    0,     5,     0,  RO, 1, (void*)&pThis->psModularList[2]->eSystemState) 
    MASTER_REG_HOLD_DATA(40, uint16,    0,     5,     0,  RO, 1, (void*)&pThis->psModularList[3]->eSystemState)   
    MASTER_REG_HOLD_DATA(44, int16,  -200,  1400,     0,  RO, 1, (void*)&pThis->sRetAir_T)      
    MASTER_REG_HOLD_DATA(45, int16,  -200,  1400,     0,  RO, 1, (void*)&pThis->sSupAir_T)
    MASTER_REG_HOLD_DATA(46, int16,  -400,   700,     0,  RO, 1, (void*)&pThis->sAmbientInSelf_T)
   
    MASTER_REG_HOLD_DATA(47, uint16,    0,   100,     0,  RO, 1, (void*)&pThis->usAmbientInSelf_H)
    MASTER_REG_HOLD_DATA(48, int16,  -400,   700,     0,  RO, 1, (void*)&pThis->sAmbientOutSelf_T) 
    MASTER_REG_HOLD_DATA(49, uint16,    0,   100,     0,  RO, 1, (void*)&pThis->usAmbientOutSelf_H)
    MASTER_REG_HOLD_DATA(51, uint16,    0,    100,  100,  RO, 1, (void*)&pThis->usFreAirDamper_Ang)
    MASTER_REG_HOLD_DATA(52, uint16,    0,   5000,    0,  RO, 1, (void*)&pThis->usCO2PPMSelf)         
   
    MASTER_REG_HOLD_DATA(53, uint16,    0,  65000,    0,  RO, 1, (void*)&pThis->usFreAir_Vol)
    MASTER_REG_HOLD_DATA(54, uint16,    0,  65000,    0,  RO, 1, (void*)&pThis->usSupAir_Vol)
    MASTER_REG_HOLD_DATA(55, uint16,    0,  65000,    0,  RO, 1, (void*)&pThis->usRetAir_Vol)

MASTER_END_DATA_BUF(0, 55)
    
    /******************************线圈数据域*************************/ 
MASTER_BEGIN_DATA_BUF(pThis->sModularRoof_BitCoilBuf, psMBCoilTable) 
    
    MASTER_COIL_BIT_DATA(0,  0, RO, (void*)&pThis->Device.eRunningState);
    MASTER_COIL_BIT_DATA(1,  0, RO, (void*)&pThis->xStopErrFlag);
    MASTER_COIL_BIT_DATA(2,  0, RO, (void*)&pThis->Device.xErrFlag);
    MASTER_COIL_BIT_DATA(3,  0, RO, (void*)&pThis->Device.xAlarmFlag); 
    MASTER_COIL_BIT_DATA(16, 0, WO, (void*)&pThis->xTempSenInErr); 
    
    MASTER_COIL_BIT_DATA(17, 0, WO, (void*)&pThis->xHumiSenInErr);
    MASTER_COIL_BIT_DATA(18, 0, WO, (void*)&pThis->xCO2SenErr);
    MASTER_COIL_BIT_DATA(48, 0, RO, (void*)&pThis->psSupAirFan->Device.eRunningState);
    MASTER_COIL_BIT_DATA(54, 0, RO, (void*)&pThis->xSupAirDamper); 
    MASTER_COIL_BIT_DATA(55, 0, RO, (void*)&pThis->xWetMode); 
    
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
    
MASTER_END_DATA_BUF(1, 10)  
    
    pThis->sDevCommData.ucProtocolID = 1;
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
    ModularRoof* pThis = (ModularRoof*)pt;

    MONITOR(&pThis->sSupAir_T,           &pThis->sValChange)
    MONITOR(&pThis->usFreAir_Vol,        &pThis->sValChange)
    
    MONITOR(&pThis->sAmbientInSelf_T,    &pThis->sValChange)
    MONITOR(&pThis->usAmbientInSelf_H,   &pThis->sValChange)
    
    MONITOR(&pThis->sAmbientOutSelf_T,   &pThis->sValChange)
    MONITOR(&pThis->usAmbientOutSelf_H,  &pThis->sValChange)
    
    MONITOR(&pThis->usCO2PPMSelf,        &pThis->sValChange)
    
    MONITOR(&pThis->xStopErrFlag,        &pThis->sValChange)
    MONITOR(&pThis->sMBSlaveDev.xOnLine, &pThis->sValChange)  
}

/*机组EEPROM数据注册*/
void vModularRoof_RegistEEPROMData(ModularRoof* pt)
{
    ModularRoof* pThis = (ModularRoof*)pt;
    
    EEPROM_DATA(TYPE_RUNTIME, pThis->Device.ulRunTime)
}


/*机组初始化*/
void vModularRoof_Init(ModularRoof* pt, sMBMasterInfo* psMBMasterInfo)
{
    uint8_t n = 0;
    ModularRoof* pThis     = (ModularRoof*)pt;
    Modular*     psModular = NULL;
    
    pThis->psMBMasterInfo = psMBMasterInfo;

    vModularRoof_RegistEEPROMData(pThis);   //EEPROM数据注册
    vModularRoof_RegistMonitor(pThis);      //注册监控数据
   
    vModularRoof_InitDevCommData(pThis);    //初始化设备通讯数据表 
    vModularRoof_InitDefaultData(pThis);    //初始化默认数据
    vModularRoof_RegistDev(pThis);          //向通讯主栈中注册设备
    
    pThis->psSupAirFan = (SupAirFan*)SupAirFan_new();     //实例化对象
    for(n=0; n < MODULAR_NUM; n++)
    {
        psModular = (Modular*)Modular_new();
        psModular->init(psModular);
        
        pThis->psModularList[n] = psModular;   //模块列表       
    } 
}

CTOR(ModularRoof)   //屋顶机构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init,                       vModularRoof_Init);

    FUNCTION_SETTING(IDevSwitch.switchOpen,      vModularRoof_SwitchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose,     vModularRoof_SwitchClose);

    FUNCTION_SETTING(IDevRunning.setRunningMode, vModularRoof_SetRunningMode);
END_CTOR





