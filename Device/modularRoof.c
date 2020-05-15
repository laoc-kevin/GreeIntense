#include "modularRoof.h"

#define DTU247_PROTOCOL_TYPE_ID   1


/*************************************************************
*                         屋顶机机组                         *
**************************************************************/

/*机组开启*/
void vModularRoof_SwitchOpen(IDevSwitch* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevSwitch, ModularRoof);
    
    
    
    
    
    
    pThis->ucSwitchCmd = ON;  
}

/*机组关闭*/
void vModularRoof_SwitchClose(IDevSwitch* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevSwitch, ModularRoof);
    
    
    
    
    
    
    
    pThis->ucSwitchCmd = OFF; 
}


/*机组运行状态设置*/
void vModularRoof_SetRunningMode(IDevRunning* pt, eRunningMode eMode)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevRunning, ModularRoof);
    
    
    
    
    
    pThis->eRunningMode = eMode;    
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
void vModularRoof_InitDevCommData(IDevCom* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevCom, ModularRoof);
    
    sMBDevDataTable* psMBRegHoldTable = &pThis->sDevCommData.sMBRegHoldTable; 
    sMBDevDataTable*    psMBCoilTable = &pThis->sDevCommData.sMBCoilTable; 
    sMBTestDevCmd*            psMBCmd = &pThis->sDevCommData.sMBDevCmdTable;
    
    uint8_t CHID = 0x13;		        //机型ID

MASTER_PBUF_INDEX_ALLOC()
MASTER_TEST_CMD_INIT(psMBCmd, 1, READ_REG_HOLD, 1)  
    
    /******************************保持寄存器数据域*************************/
MASTER_BEGIN_DATA_BUF(pThis->sModularRoof_RegHoldBuf, psMBRegHoldTable)      
    MASTER_REG_HOLD_DATA(1, uint8, 0, 65535, 0, WO, 1, (void*)&CHID);
    MASTER_REG_HOLD_DATA(5, uint8, 0, 65535, 0, RO, 1, (void*)&CHID)
    MASTER_REG_HOLD_DATA(7, uint8, 0, 65535, 0, RW, 1, (void*)&CHID)
    MASTER_REG_HOLD_DATA(8, uint8, 0, 65535, 0, WO, 1, (void*)&CHID)  
MASTER_END_DATA_BUF(1, 8)
    
    /******************************线圈数据域*************************/ 
MASTER_BEGIN_DATA_BUF(pThis->sModularRoof_BitCoilBuf, psMBCoilTable)    
    MASTER_COIL_BIT_DATA(1,  0, WO, (void*)&CHID);
    MASTER_COIL_BIT_DATA(5,  0, RO, (void*)&CHID);
    MASTER_COIL_BIT_DATA(7,  0, RW, (void*)&CHID);
    MASTER_COIL_BIT_DATA(10, 0, WO, (void*)&CHID);    
MASTER_END_DATA_BUF(1, 10)  
    
    pThis->sDevCommData.ucProtocolID = 0;
    pThis->sDevCommData.pxDevDataMapIndex = xModularRoof_DevDataMapIndex;  //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}

/*向通讯主栈中注册设备*/
void vModularRoof_RegistDev(IDevCom* pt)
{
    ModularRoof* pThis = SUB_PTR(pt, IDevCom, ModularRoof);
    
   (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBSlaveDev);
}

/*机组初始化*/
void vModularRoof_Init(ModularRoof* pt, sMBMasterInfo* psMBMasterInfo)
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
    FUNCTION_SETTING(init,                       vModularRoof_Init);

    FUNCTION_SETTING(IDevSwitch.switchOpen,      vModularRoof_SwitchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose,     vModularRoof_SwitchClose);

//    FUNCTION_SETTING(IDevRunning.setRunningMode, vModularRoof_SetRunningMode);
END_CTOR





