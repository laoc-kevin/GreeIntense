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

/*通讯映射函数*/
USHORT usModularRoof_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID,  USHORT usAddr)
{

    
    
    
    
    
}


/*系统通讯数据表初始化*/
void vModularRoof_InitDevCommData(IDevCom* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevCom, ModularRoof);
    
    
    
    
    
    
    
    
    
    
    pThis->sDevCommData.psMBDevDataMapIndex = usModularRoof_DevDataMapIndex;    //绑定映射函数
    pThis->sMBDev.psDevDataInfo = &(pThis->sDevCommData);
}

/*向通讯主栈中注册设备*/
void vModularRoof_RegistDev(IDevCom* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevCom, ModularRoof);
    
    (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBDev);
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
    FUNCTION_SETTING(init, vModularRoof_init);
END_CTOR





