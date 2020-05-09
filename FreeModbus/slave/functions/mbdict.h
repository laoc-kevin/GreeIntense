#ifndef _USER_MB_DICT_H
#define _USER_MB_DICT_H

#include "port.h"
#include "mbconfig.h"
#include "mbframe.h"

#define SLAVE_PROTOCOL_TYPE_ID    0

typedef struct        /* 从栈寄存器数据结构 */
{
	USHORT  usAddr;             //地址
    UCHAR   ucDataType;         //数据类型
    LONG    lMinVal;            //最小值
    LONG    lMaxVal;            //最大值
    UCHAR   ucAccessMode;       //访问权限
    float   fTransmitMultiple;  //传输因子
	void*   pvValue;            //变量指针       
}sMBSlaveRegData;     		

typedef struct       /* 从栈线圈和离散量数据结构 */
{
    USHORT    usAddr;           //地址
    UCHAR     ucAccessMode;     //访问权限
	UCHAR*    pvValue;          //变量指针     
}sMBSlaveBitData;  


typedef struct        /*CPN数据结构 */
{
	USHORT  usAddr;              //地址
	UCHAR   ucDataType;          //数据类型
    UCHAR   ucValType;           //变量类型
    LONG    lMinVal;             //最小值
    LONG    lMaxVal;             //最大值
    UCHAR   ucAccessMode;        //访问权限
    USHORT  fTransmitMultiple;   //传输因子
    void*   pvValue;             //变量指针   
}sMBSlaveCPNData;  						


typedef struct   /* 从栈字典数据列表结构 */
{
    void*    pvDataBuf;             //协议数据域
	USHORT   usStartAddr;           //起始地址
	USHORT   usEndAddr;             //末尾地址
    USHORT   usDataCount;           //协议点位总数
}sMBSlaveDataTable;

typedef USHORT (*psMBSlaveDataMapIndex)(eDataType eDataType, USHORT usAddr); //字典映射函数

typedef struct            /*从栈通讯字典数据结构*/  
{
	sMBSlaveDataTable   sMBRegInTable;       //输入寄存器数据表
	sMBSlaveDataTable   sMBRegHoldTable;     //保持寄存器数据表
	sMBSlaveDataTable   sMBCoilTable;        //线圈数据表
	sMBSlaveDataTable   sMBDiscInTable;      //离散量数据表
                            
#if MB_SLAVE_CPN_ENABLED > 0 
    sMBSlaveDataTable   sMBCPNTable;         //CPN数据表 
#endif   
    psMBSlaveDataMapIndex  psMBSlaveDataMapIndex; //从栈字典映射函数
    
}sMBSlaveCommData; 

typedef struct          /*从栈通讯参数信息*/   
{
    UCHAR*              pcSlaveAddr;                //从栈通讯地址
	UCHAR               ucDataReady;                //数据是否准备好

    sMBSlaveCommData*   psSlaveCurData;             //从栈当前数据域   
}sMBSlaveCommInfo; 

#endif