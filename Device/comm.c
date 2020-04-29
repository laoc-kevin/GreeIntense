#include "comm.h"

void vComm_initCommData(Comm* pt)
{
    
}

void vComm_init(Comm* pt, sCommMasterInfo* psMaster, sCommSlaveInfo* psSlave)
{
     Comm* pThis = (Comm*)pt;   
    
    uint8_t ucSlaveAddr = ucGetControllerID();
    
    xMBSlaveRegisterNode(pThis->psMBSlaveInfo, psSlave->eMode, psSlave->psSlaveUart,
                         psSlave->pcMBPortName, ucSlaveAddr, psSlave->psSlaveCurData, 
                         psSlave->usPrio);
}
