#include "modularRoof.h"

/*************************************************************
*                         机组                               *
**************************************************************/
ABS_CTOR(Unit)  //机组构造函数
    SUPER_CTOR(Device);
END_CTOR


/*************************************************************
*                         屋顶机机组                         *
**************************************************************/

/*机组开启*/
void vModularRoof_SwitchOpen(IDevSwitch* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevSwitch, ModularRoof);
    pThis->Unit.ucSwitchCmd = ON; 
}

/*机组关闭*/
void vModularRoof_SwitchClose(IDevSwitch* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevSwitch, ModularRoof);
    pThis->Unit.ucSwitchCmd = OFF; 
}


/*机组运行状态设置*/
void vModularRoof_SetRunningMode(IDevRunning* pt, eRunMode eMode)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevRunning, ModularRoof);
    pThis->Unit.ucRunningMode = eMode;
    
}


/*通讯映射函数*/
USHORT usModularRoof_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID,  USHORT usAddr)
{

    
    
    
    
    
}


/*机组通讯数据表初始化*/
void vModularRoof_InitDevCommData(IDevCom* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevCom, ModularRoof);
    
    sMBDevDataTable* psMBRegHoldTable = &pThis->sDevCommData.sMBRegHoldTable; 
    sMBDevDataTable*    psMBCoilTable = &pThis->sDevCommData.sMBCoilTable; 
    
    /******************************保持寄存器数据域*************************/
    (*psMBRegHoldTable) = {
                             .pvDataBuf  = &pThis->sModularRoof_RegHoldBuf;
                          }
    
    psMBRegHoldTable->pvDataBuf   = &pThis->sModularRoof_RegHoldBuf;   //绑定保持寄存器数据域
    psMBRegHoldTable->usDataCount = MODULAR_ROOF_REG_HOLD_NUM;         //保持寄存器点位数
    psMBRegHoldTable->usStartAddr = 1;                                 //起始地址
    psMBRegHoldTable->usEndAddr   = MODULAR_ROOF_REG_HOLD_NUM;         //终止地址
    
    pThis->sModularRoof_RegHoldBuf = {{0, uint16, 0, 65535, 0, WO, 1, (void*)pt}};
    
    
    
    
    /******************************线圈数据域*************************/
    psMBCoilTable->pvDataBuf   = &pThis->sModularRoof_CoilBuf;      //绑定线圈数据域
    psMBCoilTable->usDataCount = MODULAR_ROOF_COIL_BIT_NUM;         //线圈点位数
    psMBCoilTable->usStartAddr = 1;                                 //起始地址
    psMBCoilTable->usEndAddr   = MODULAR_ROOF_COIL_BIT_NUM;         //终止地址
    
    
    
    pThis->sDevCommData.psMBDevDataMapIndex = usModularRoof_DevDataMapIndex;         //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}

/*向通讯主栈中注册设备*/
void vModularRoof_RegistDev(IDevCom* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevCom, ModularRoof);
    
    (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBSlaveDev);
}

/*机组初始化*/
void vModularRoof_init(ModularRoof* pt, sMBMasterInfo* psMBMasterInfo)
{
    uint8_t n;
    ModularRoof* pThis = (ModularRoof*)pt;

    pThis->psMBMasterInfo = psMBMasterInfo;

    vModularRoof_InitDevCommData(&pThis->IDevCom);    //初始化设备通讯数据表   
    vModularRoof_RegistDev(&pThis->IDevCom);          //向通讯主栈中注册设备
    
    for(n=0; n < SUP_AIR_FAN_NUM; n++)
    {
        pThis->psSupAirFanList[n] = (SupAirFan*)SupAirFan_new();     //实例化对象
    }
    
    for(n=0; n < COMP_NUM; n++)
    {
        pThis->psCompList[n] = (Compressor*)Compressor_new();
    }
}

CTOR(ModularRoof)   //屋顶机构造函数
    SUPER_CTOR(Unit);
    FUNCTION_SETTING(init,                       vModularRoof_init);

    FUNCTION_SETTING(IDevSwitch.switchOpen,      vModularRoof_SwitchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose,     vModularRoof_SwitchClose);

    FUNCTION_SETTING(IDevRunning.setRunningMode, vModularRoof_SetRunningMode);
END_CTOR





