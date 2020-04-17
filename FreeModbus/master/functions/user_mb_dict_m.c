/************************************************************************
该文件需要开发者自己维护，通讯表所涉及的变量都位于app_val.c和MasterSlave.c
两个文件，同时需要更改动user_mb_map_m.c的对应映射关系
*************************************************************************/

#include "user_mb_dict_m.h"
#include "app_val.h"

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

sMasterBitCoilData SecCentrifugeCH_CoilBuf[]=
{
    {17,  0,  RW,  (UCHAR*)&CHFaultStateClear   },
	{33,  0,  RO,  (UCHAR*)&CHCommHMIFault      },
	{37,  0,  RW,  (UCHAR*)&CHWSwState          },
	{38,  0,  RW,  (UCHAR*)&CWSwState           },
    {43,  0,  RO,  (UCHAR*)&CHRunState          },
	
	{44,  0,  RW,  (UCHAR*)&CHCom1RunState  },
	{47,  0,  RO,  (UCHAR*)&CHProtectFlag   }, 
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
const sMBDevDataTable psMRegInTable[MB_SLAVE_TYPES];
#endif


#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

const sMBDevDataTable psMRegHoldTable [MB_SLAVE_TYPES] = 
{
	{(sMasterRegHoldData*)SecCentrifugeCH_RegHoldBuf, 0, 180, sizeof(SecCentrifugeCH_RegHoldBuf)/sizeof(sMasterRegHoldData)},
};
#endif


#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
const sMBDevDataTable psMCoilTable [MB_SLAVE_TYPES]=
{
    {(sMasterBitCoilData*)SecCentrifugeCH_CoilBuf, 17, 47, sizeof(SecCentrifugeCH_CoilBuf)/sizeof(sMasterBitCoilData)}
};
#endif


#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
const sMBDevDataTable psMDiscInTable [MB_SLAVE_TYPES]=
{
	{NULL, 0, 0},
};
#endif

