#include "modularRoof.h"

/*************************************************************
*                         机组                               *
**************************************************************/
ABS_CTOR(AirUnit)  //机组构造函数
    SUPER_CTOR(Device);
END_CTOR


/*************************************************************
*                         屋顶机机组                         *
**************************************************************/

BOOL xModularRoof_init(ModularRoof* pt, uint8_t ucSupAirFanNum, uint8_t ucCompNum)
{
    uint8_t n;
    
    ModularRoof* pThis = (ModularRoof*)pt;
    
    pThis->ucSupAirFanNum = ucSupAirFanNum;
    pThis->ucCompNum      = ucCompNum;
    
    pThis->psSupAirFanList = (SupAirFan**) calloc(ucSupAirFanNum, sizeof(SupAirFan*));    //存储的是对象指针
    pThis->psCompList      = (Compressor**)calloc(ucCompNum, sizeof(Compressor*));
    
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





