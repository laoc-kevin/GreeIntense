#include "comm.h"
#include "md_modbus.h"

/*通讯映射函数*/
USHORT usComm_DevDataMapIndex(eDataType eDataType, USHORT usAddr)
{
  

   
}

/*系统从栈通讯数据表初始化*/
void vComm_InitSlaveCommData(Comm* pt)
{
    Comm* pThis = (Comm*)pt;
    
    
    
    pThis->sSlaveCommData.psMBSlaveDataMapIndex = usComm_DevDataMapIndex;    //绑定映射函数
}


void vComm_Init(Comm* pt, sCommInfo* psComm)
{
    Comm* pThis = (Comm*)pt;   

    pThis->psSystem = psComm->psSystem;
    pThis->psMBSlaveInfo = psComm->psMBSlaveInfo;
    pThis->psMBSlaveInfo->sMBCommInfo.psSlaveCurData = &pThis->sSlaveCommData;
    
    vComm_InitSlaveCommData(pThis);
    
    vModbusInit();
    
}


