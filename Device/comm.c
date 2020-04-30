#include "comm.h"

void vComm_initCommData(Comm* pt)
{
    
}

void vComm_init(Comm* pt, sCommMasterInfo* psMaster, sCommSlaveInfo* psSlave)
{
    Comm* pThis = (Comm*)pt;   
    
    uint8_t ucSlaveAddr = ucGetControllerID();
    
    (void)xMBMasterRegistNode(&pThis->sMBMasterInfo, psMaster->eMode, psMaster->psMasterUart,
                               psMaster->pcMBPortName, psMaster->usMaxAddr, psMaster->usMinAddr, 
                               psSlave->usPrio, psMaster->bDTUEnable);
    
    (void)xMBSlaveRegisterNode(&pThis->sMBSlaveInfo, psSlave->eMode, psSlave->psSlaveUart,
                                psSlave->pcMBPortName, ucSlaveAddr, &pThis->sSlaveCurData, 
                                psSlave->usPrio);
}


