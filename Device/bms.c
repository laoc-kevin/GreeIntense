#include "bms.h"
#include "system.h"
#include "md_modbus.h"
#include "mbmap.h"

/*通讯映射函数*/
USHORT usBMS_DevDataMapIndex(eDataType eDataType, USHORT usAddr)
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
    		return MB_MRE_NO_REG;
    	break;
    }
    return i;  
}

/*系统从栈通讯数据表初始化*/
void vBMS_InitBMSData(BMS* pt, void* pSysData)
{
    BMS*    pThis   = (BMS*)pt;
    System* pSystem = (System*)pSysData;
    
    
SLAVE_PBUF_INDEX_ALLOC()    
    
SLAVE_BEGIN_DATA_BUF(pThis->sBMS_RegHoldBuf, &pThis->sBMSCommData.sMBRegHoldTable)    
    
    SLAVE_REG_DATA(0,  uint16,  0,  65535, 0, RO,  (void*)&pSystem->usFreAirSet_Vol)
    SLAVE_REG_DATA(2,  uint16,  0,  65535, 0, RO,  (void*)&pSystem->usFreAirSet_Vol)
    SLAVE_REG_DATA(10, uint16,  0,  65535, 0, RO,  (void*)&pSystem->usFreAirSet_Vol)
    
SLAVE_END_DATA_BUF(0, 10)    
    
    
    pThis->sBMSCommData.psMBSlaveDataMapIndex = usBMS_DevDataMapIndex;         //绑定映射函数
    pThis->psBMSInfo->sMBCommInfo.psSlaveCurData = &pThis->sBMSCommData;
}


void vBMS_Init(BMS* pt, sMBSlaveInfo* psBMSInfo, void* pSysData)
{
    BMS* pThis = (BMS*)pt;   
    pThis->psBMSInfo = psBMSInfo;
    
    vBMS_InitBMSData(pThis, pSysData);
    vModbusInit();
}

void vBMS_RegistMonitor(BMS* pt)
{

}

CTOR(BMS)   //BMS构造函数

      FUNCTION_SETTING(init, vBMS_Init);

END_CTOR
