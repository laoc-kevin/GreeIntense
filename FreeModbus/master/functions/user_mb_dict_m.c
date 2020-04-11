/************************************************************************
该文件需要开发者自己维护，通讯表所涉及的变量都位于app_val.c和MasterSlave.c
两个文件，同时需要更改动user_mb_map_m.c的对应映射关系
*************************************************************************/

#include "user_mb_dict_m.h"
#include "app_val.h"

//sMBSlaveDevInfo SlaveDeviceList[ MB_MAX_SLAVE_ADDR ];

//sMBTestSlaveCommand usMBTestList[ MB_SLAVE_TYPES-2 ] = { {SEC_PROTOCOL_REG_IN_VALUE, READ_REG_HOLD, SEC_PROTOCOL_REG_IN_ADDR, SEC_PROTOCOL_TYPE_ID},
//                                }; //自动寻址通讯字典中寄存器地址表与命令

/**********************************************************************************************
*敏捷群控发GPRS数据集   GPRS虚拟从设备地址为200
***********************************************************************************************/
sMasterRegHoldData DTU200_RegHoldBuf[]=        
{
	/***********************  0~19    ********************************/	
    {0, uint16,  0,  65535, 0, WO,  1,  (void*)&QK_ProductID   },   
    {1, uint16,  0,  65535, 0, WO,  1,  (void*)&DTUProtocolVer  },
    {2, uint16,  0,  65535, 0, WO,  1,  (void*)&HexVer         },
};

/**********************************************************************************************
*二代离心机机组协议（高压比离心机0x00E, 直流变频离心机0x0013, 常规离心机0x0024, 磁悬浮离心机0x002E）
***********************************************************************************************/
sMasterRegHoldData SecCentrifugeCH_RegHoldBuf[]=
{
	{0,   uint16,    0,  65535,  0, WO, 1, (void*)&CHID           },   
    {1,   uint16,    0,  65535,  0, RW, 1, (void*)&CHProtocolVer  },
    {9,   uint16,    0,      1,  0, RO, 1, (void*)&CHRemoteEn     },   
	{23,  uint16,   20,     50,  0, WO, 1, (void*)&CWAntiFreezeT  },	
    {180,  int16, -300,   1050,  0, WO, 1, (void*)&CHCHWIT        },
};

uint8_t CHRYWPState1 = 0;
uint8_t CHRYWPState2 = 0;
uint8_t CHRYWPState3 = 0;
uint8_t CHRYWPState4 = 0;
uint8_t CHRYWPState5 = 0;
uint8_t CHRYWPState6 = 0;
uint8_t CHRYWPState7 = 0;
uint8_t CHRYWPState8 = 0;


sMasterBitCoilData SecCentrifugeCH_CoilBuf[]=
{
    {17,  0,  RW,  (UCHAR*)&CHFaultStateClear   },
	{33,  0,  RO,  (UCHAR*)&CHCommHMIFault      },
	{37,  0,  RW,  (UCHAR*)&CHWSwState          },
	{38,  0,  RW,  (UCHAR*)&CWSwState           },
    {43,  0,  RO,  (UCHAR*)&CHRunState          },
	
	{44,  0,  RW,  (UCHAR*)&CHCom1RunState  },
	{47,  0,  RO,  (UCHAR*)&CHProtectFlag   }, 
	
	{76,  0,  RW,  (UCHAR*)&CHRYWPState     },
	{77,  0,  RW,  (UCHAR*)&CHRYWPState1    },
	{78,  0,  RW,  (UCHAR*)&CHRYWPState2    },
	{79,  0,  RW,  (UCHAR*)&CHRYWPState3    },
	{80,  0,  RW,  (UCHAR*)&CHRYWPState4    },
	{81,  0,  RW,  (UCHAR*)&CHRYWPState5    },
	{82,  0,  RW,  (UCHAR*)&CHRYWPState6    },
	{83,  0,  RW,  (UCHAR*)&CHRYWPState7    },
	{84,  0,  RW,  (UCHAR*)&CHRYWPState8    },
};



/**********************************************************************************************
*敏捷群控读GPRS数据集   GPRS虚拟从设备地址为247
***********************************************************************************************/
sMasterRegInData DTU247_RegInBuf[]=        
{   
	/*****************************  0~19    ********************************/
	{12, uint16,   0, 65535, RO, 1,  (void*)&DTU_Changed  },
};

#if MB_FUNC_READ_INPUT_ENABLED > 0

const sMBIndexTable psMRegInTable[MB_SLAVE_TYPES]=
{
	{NULL, 0, 0},
    {(sMasterRegInData*)DTU247_RegInBuf, DTU247_PROTOCOL_TYPE_ID, sizeof(DTU247_RegInBuf)/sizeof(sMasterRegInData)},
	
};

#endif


#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

const sMBIndexTable psMRegHoldTable [MB_SLAVE_TYPES] = 
{
    {(sMasterRegHoldData*)DTU200_RegHoldBuf, DTU200_PROTOCOL_TYPE_ID, sizeof(DTU200_RegHoldBuf)/sizeof(sMasterRegHoldData)},
	{NULL, 0, 0},
	{(sMasterRegHoldData*)SecCentrifugeCH_RegHoldBuf, SEC_PROTOCOL_TYPE_ID, sizeof(SecCentrifugeCH_RegHoldBuf)/sizeof(sMasterRegHoldData)},
};

#endif

#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0

const sMBIndexTable psMCoilTable [MB_SLAVE_TYPES]=
{
	{NULL, 0, 0},
	{NULL, 0, 0},
    {(sMasterBitCoilData*)SecCentrifugeCH_CoilBuf, SEC_PROTOCOL_TYPE_ID, sizeof(SecCentrifugeCH_CoilBuf)/sizeof(sMasterBitCoilData)}
};


#endif


#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0

const sMBIndexTable psMDiscInTable [MB_SLAVE_TYPES]=
{
	{NULL, 0, 0},
};

#endif

///**********************************************************************
// * @brief   插入设备进设备链表
// * @param   psMBMasterInfo  主栈信息块 
// * @param   Address         从设备地址 
// * @return	sMBSlaveDevInfo
// * @author  laoc
// * @date    2019.01.22
// *********************************************************************/
void vMBMasterDevDataRegister( sMBSlaveDevInfo* psMBDev )
{
   
}











