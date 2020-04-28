#ifndef _USER_MB_DICT_H
#define _USER_MB_DICT_H

#include "port.h"
#include "mbconfig.h"
#include "mbframe.h"

#define SLAVE_PROTOCOL_TYPE_ID    0

typedef struct        /* 从栈寄存器数据结构 */
{
	const USHORT                      usAddr;             //地址
    const UCHAR                       ucDataType;         //数据类型
    const LONG                        lMinVal;            //最小值
    const LONG                        lMaxVal;            //最大值
    const UCHAR                       ucAccessMode;       //访问权限
    const float                       fTransmitMultiple;  //传输因子
	void* const                       pvValue;            //变量指针       
}sMBSlaveRegData;     		

typedef struct       /* 从栈线圈和离散量数据结构 */
{
    const USHORT                      usAddr;           //地址
    const UCHAR                       ucAccessMode;     //访问权限
	UCHAR* const                      pvValue;          //变量指针     
}sMBSlaveBitData;  


typedef struct        /*CPN数据结构 */
{
	const USHORT                      usAddr;              //地址
	const UCHAR                       ucDataType;          //数据类型
    const UCHAR                       ucValType;           //变量类型
    const LONG                        lMinVal;             //最小值
    const LONG                        lMaxVal;             //最大值
    const UCHAR                       ucAccessMode;        //访问权限
    const USHORT                      fTransmitMultiple;   //传输因子
    void * const                      pvValue;             //变量指针   
}sMBSlaveCPNData;  						


typedef struct   /* 从栈字典数据列表结构 */
{
    const void* const  pvDataBuf;             //协议数据域
	const USHORT       usStartAddr;           //起始地址
	const USHORT       usEndAddr;             //末尾地址
    const USHORT       usDataCount;           //协议点位总数
}sMBSlaveDataTable;

typedef USHORT (*usMBSlaveDataMap)(eDataType eDataType, USHORT usAddr); //字典映射函数

typedef struct            /* 从栈通讯字典数据结构 */  
{
	const sMBSlaveDataTable* const psMBRegInTable;       //输入寄存器数据表
	const sMBSlaveDataTable* const psMBRegHoldTable;     //保持寄存器数据表
	const sMBSlaveDataTable* const psMBCoilTable;        //线圈数据表
	const sMBSlaveDataTable* const psMBDiscInTable;      //离散量数据表
    
#if MB_SLAVE_CPN_ENABLED > 0
    const sMBSlaveDataTable* const psMBCPNTable;         //CPN数据表 
#endif   
    usMBSlaveDataMap               psMBSlaveDataMap;     //从栈字典映射函数
    
}sMBSlaveDataInfo; 

typedef struct                 /* 从栈通讯参数信息 */   
{
    UCHAR               ucSlaveAddr;                //从栈通讯地址
	UCHAR               ucDataReady;                //数据是否准备好

    sMBSlaveDataInfo*   psSlaveCurData;             //从栈当前数据域   
}sMBSlaveCommInfo; 

#endif