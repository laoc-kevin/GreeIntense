#include "meter.h"

/*************************************************************
*                         电表                               *
**************************************************************/

/* 电表数据监控*/
void vMeter_RegistMonitor(Meter* pt)
{
    OS_ERR err = OS_ERR_NONE;
    Meter* pThis = (Meter*)pt;
    
    OSSemCreate( &(pThis->sValChange), "sValChange", 0, &err );  //事件消息量初始化

    MONITOR(&pThis->usTotalEnergy_H, &pThis->sValChange)
}

/*电表EEPROM数据注册*/
void vMeter_RegistEEPROMData(Meter* pt)
{
    Meter* pThis = (Meter*)pt;
    
    EEPROM_DATA(TYPE_E32, pThis->usTotalEnergy_H)
}

/*向通讯主栈中注册设备*/
void vMeter_RegistDev(Meter* pt)
{
    Meter* pThis = (Meter*)pt;
    (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBSlaveDev);
}

/*通讯映射函数*/
BOOL xMeter_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID, USHORT usAddr, USHORT* psIndex)
{
    USHORT i = 0;
    switch(ucProtocolID)
	{
        case 0:
            if(eDataType == RegHoldData)
            {
                switch(usAddr)
                {
                    case 1	  :  i = 0 ;  break;
                    case 0X30 :  i = 1 ;  break;
                       
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

/*通讯数据表初始化*/
void vMeter_InitDevCommData(Meter* pt)
{
    Meter* pThis = (Meter*)pt;

    sMBDevDataTable* psMBRegHoldTable = &pThis->sDevCommData.sMBRegHoldTable; 
    sMBTestDevCmd*            psMBCmd = &pThis->sDevCommData.sMBDevCmdTable;
    
    
MASTER_PBUF_INDEX_ALLOC()
MASTER_TEST_CMD_INIT(psMBCmd, 0x30, READ_REG_HOLD, pThis->sMBSlaveDev.ucDevAddr, FALSE)  
    
    /******************************保持寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(pThis->sSensor_RegHoldBuf, psMBRegHoldTable)
    
    MASTER_REG_HOLD_DATA(0x30, uint8, 1, 255, 0, RW, pThis->sMBSlaveDev.ucDevAddr, (void*)&pThis->sMBSlaveDev.ucDevAddr)
        
MASTER_END_DATA_BUF(1, 0x30)

    pThis->sDevCommData.ucProtocolID = 0;
    pThis->sDevCommData.pxDevDataMapIndex = xMeter_DevDataMapIndex;    //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}

/*电表初始化*/
void vMeter_Init(Meter* pt)
{
    Meter* pThis = (Meter*)pt;
    
    vMeter_RegistEEPROMData(pThis);   //EEPROM数据注册
    vMeter_RegistMonitor(pThis);      //注册监控数据

    vMeter_InitDevCommData(pThis);    //初始化设备通讯数据表 
    vMeter_RegistDev(pThis);          //向通讯主栈中注册设备
}


CTOR(Meter)    //电表构造函数
    SUPER_CTOR(Device);
    
    FUNCTION_SETTING(init, vMeter_Init);
END_CTOR