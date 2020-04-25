#include "modularRoof.h"

/*****************************机组****************************/
ABS_CTOR(AirUnit)  //机组构造函数
    SUPER_CTOR(Device);
END_CTOR


/*****************************屋顶机机组****************************/
BOOL xModularRoof_init(void* pt, uint8_t ucSupAirFanNum, uint8_t ucCompNum)
{
    uint8_t n;
    
    ModularRoof* pThis = (ModularRoof*)pt;
    
    pThis->ucSupAirFanNum = ucSupAirFanNum;
    pThis->ucCompNum      = ucCompNum;
    
    pThis->psSupAirFanList = (SupAirFan**)  calloc(ucSupAirFanNum, sizeof(SupAirFan*));    //存储的是对象指针
    pThis->psCompList      = (Compressor**) calloc(ucCompNum, sizeof(Compressor*));
    
    if( (pThis->psSupAirFanList == NULL) || (pThis->psCompList == NULL) )
    {
         return FALSE;
    }
    else
    {
        for(n=0; n < pThis->ucSupAirFanNum; n++)
        {
            pThis->psSupAirFanList[n] = (SupAirFan*)SupAirFan_new();     //实例化对象
        }
        
        for(n=0; n<pThis->ucCompNum; n++)
        {
            pThis->psCompList[n] = (Compressor*)Compressor_new();
        }
    }
    return TRUE;
}

CTOR(ModularRoof)   //屋顶机构造函数
    SUPER_CTOR(AirUnit);
    FUNCTION_SETTING(init, xModularRoof_init);
END_CTOR


/*****************************屋顶机机组系统****************************/
BOOL xModularRoofSys_init(void* pt, uint8_t ucModularRoofNum, uint8_t ucExAirFanNum, sFreqInfo* psExAirFan, const sMBMasterInfo* psMBMasterInfo)
{
    uint8_t n;
    
    ModularRoofSys* pThis = (ModularRoofSys*)pt;
    
    pThis->ucModularRoofNum = ucModularRoofNum;
    pThis->ucExAirFanNum    = ucExAirFanNum;
    
    pThis->psModularRoofList = (ModularRoof**)calloc(ucModularRoofNum, sizeof(ModularRoof*));    //存储的是对象指针
    pThis->psExAirFanList    = (ExAirFan**)   calloc(ucExAirFanNum, sizeof(ExAirFan*));
    
    if( (pThis->psModularRoofList == NULL) || (pThis->psExAirFanList == NULL) )
    {
         return FALSE;
    }
    else
    {
        for(n=0; n < pThis->ucModularRoofNum; n++)
        {
            pThis->psModularRoofList[n] = (ModularRoof*)ModularRoof_new();     //实例化对象
        }  
        for(n=0; n<pThis->ucExAirFanNum; n++)
        {
            ExAirFan* psFan = (ExAirFan*)ExAirFan_new();     //实例化对象
            
            psFan->Fan.init(psFan, psExAirFan[n].eFanFreqType);      //频率类型
            
            psFan->IDevFreq.registFreq_IO(psFan, psExAirFan[n].ucFreq_AO, psExAirFan[n].ucFreq_AI,  //频率接口注册
                                          psExAirFan[n].usMinFreq, psExAirFan[n].usMaxFreq);   
            psFan->IDevSwitch.registSwitch_IO(psFan, psExAirFan[n].ucSwitch_DO);                    //启停接口注册
 
            pThis->psExAirFanList[n] = psFan; 
        }
    }
    pThis->IDevCom.initCommDevData(pThis);                 //初始化设备通讯数据表   
    pThis->IDevCom.registCommDev(pThis, psMBMasterInfo);   //向通讯主栈中注册设备
   
    return TRUE;
}

void vModularRoofSys_registCommDev(void* pt, const sMBMasterInfo* psMBMasterInfo)
{
    ModularRoofSys* pThis = (ModularRoofSys*)pt;
    
    if(pThis->psDevDataInfo != NULL)
    {
         pThis->psDevInfo = psMBMasterRegistDev(psMBMasterInfo, pThis->psDevDataInfo);
    }
}

void vModularRoofSys_initCommDevData(void* pt)
{
    
}


CTOR(ModularRoofSys)   //屋顶机机组构造函数
    SUPER_CTOR(AirUnit);
    FUNCTION_SETTING(init, xModularRoofSys_init);
    FUNCTION_SETTING(IDevCom.registCommDev, vModularRoofSys_registCommDev);
    FUNCTION_SETTING(IDevCom.initCommDevData, vModularRoofSys_initCommDevData);
END_CTOR


