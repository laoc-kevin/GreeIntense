#ifndef _USER_MB_DICT_M_
#define _USER_MB_DICT_M_

#include "mbframe.h"
#include "mbconfig.h"
#include "port.h"

#define MODBUS_ACCESS_ADDR                      1     //自动寻址
#define MODBUS_ACCESS_ADDR_INTERVAL_TIMES       10     //自动寻址轮询间隔次数

/*************以下参数需要根据实际设备情况进行配置***********/
#define DTU_PROTOCOL_VERSIPON                   0x0B    //DTU通讯协议版本号 V1.1

#define MB_SLAVE_TYPES                          3
//#define MB_SLAVE_ADDR_START                     200 
//#define MB_MAX_SLAVE_ADDR                       247

#define DTU200_PROTOCOL_TYPE_ID                 0
#define DTU247_PROTOCOL_TYPE_ID                 1
#define SEC_PROTOCOL_TYPE_ID                    2



#define SEC_PROTOCOL_REG_IN_ADDR                10
#define SEC_PROTOCOL_REG_IN_VALUE               0x1

/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START        0
#define M_DISCRETE_INPUT_NDISCRETES   2000

#define M_COIL_START                  0
#define M_COIL_NCOILS                 2000

#define M_REG_INPUT_START             0
#define M_REG_INPUT_NREGS             2000

#define M_REG_HOLDING_START           0
#define M_REG_HOLDING_NREGS           60000

typedef enum      /* 测试模式 */        
{
	READ_REG_IN,
    READ_REG_HOLD,
	WRITE_REG_HOLD,
}eMasterCmdMode;

typedef struct        /* 主栈保持寄存器数据结构 */
{
	const USHORT                      Address;            //地址
    const UCHAR                       DataType;           //数据类型
    volatile USHORT                   PreValue;          //先前值
    const LONG                        MinValue;           //最小值
    const LONG                        MaxValue;           //最大值
    const UCHAR                       OperateMode;        //操作类型
    const float                       Multiple;           //计算因子
    void * const                      Value;              //变量指针    
}sMasterRegHoldData;     		

typedef struct        /* 主栈字典保持寄存器数据结构 */
{
	const USHORT                      Address;            //地址
    const UCHAR                       DataType;           //数据类型
    const LONG                        MinValue;           //最小值
    const LONG                        MaxValue;           //最大值
    const UCHAR                       OperateMode;        //操作类型
    const float                       Multiple;           //计算因子
    void * const                      Value;              //变量指针    
}sMasterRegInData; 

typedef struct       /* 主栈字典线圈数据结构 */
{
    const USHORT                      Address;           //地址
    volatile UCHAR                    PreValue;          //先前值
    const UCHAR                       OperateMode;       //操作类型
	UCHAR* const                      Value;             //变量指针   
}sMasterBitCoilData;  

typedef struct       /* 主栈字典离散量数据结构 */
{
    const USHORT                      Address;           //地址
    const UCHAR                       OperateMode;       //操作类型
	UCHAR* const                      Value;             //变量指针      
}sMasterBitDiscData;  


typedef struct   /* 用于测试从栈的指令数据结构 */       
{
	const USHORT              usValue;       //数值
	const eMasterCmdMode      eCmdMode;      //测试模式
	const UCHAR               ucAddr;        //测试点位通讯地址
}sMBTestDevCmd; 

typedef struct   /* 主栈字典数据列表结构 */
{
    const void* const  pvDataBuf;             //协议数据域
	const UCHAR        ucStartAddr;           //起始地址
	const UCHAR        ucEndAddr;             //末尾地址
    const USHORT       usDataCount;           //协议点位总数
}sMBDevDataTable;

typedef struct sMBSlaveDevDataInfo   /* 从设备通讯字典数据结构 */  
{
	const sMBDevDataTable* const psMBRegInTable;       //输入寄存器数据表
	const sMBDevDataTable* const psMBRegHoldTable;     //保持寄存器数据表
	const sMBDevDataTable* const psMBCoilTable;        //线圈数据表
	const sMBDevDataTable* const psMBDiscInTable;      //离散量数据表
    const sMBTestDevCmd*   const psMBDevCmdTable;      //用于测试从设备状态命令表
    const UCHAR                  ucProtocolID;         //协议ID
    struct sMBSlaveDevDataInfo*  pNext;                //下一个数据表
}sMBSlaveDevDataInfo; 

typedef struct sMBSlaveDevInfo   /* 从设备信息列表 */   
{
    UCHAR               ucDevAddr;                  //设备通讯地址
	UCHAR               ucRetryTimes;               //测试间隔
	UCHAR               ucOnLine;                   //是否在线
	UCHAR               ucDataReady;                //数据是否准备好
    UCHAR               ucProtocolID;               //协议ID
	UCHAR               ucSynchronized;             //是否同步
    UCHAR               ucDevOnTimeout;             //是否处于延时
    OS_TMR              DevOfflineTmr;              //设备掉线定时器
    
    sMBSlaveDevDataInfo*     psDevDataInfo;         //从设备数据域
    sMBSlaveDevDataInfo*     psDevCurData;          //从设备当前数据域   
    
    struct sMBSlaveDevInfo*  pNext;                 //下一个设备节点
    struct sMBSlaveDevInfo*  pLast;                 //尾设备节点
}sMBSlaveDevInfo; 

typedef struct    /* 主栈从设备状态结构  */
{
    UCHAR                           ucSlaveDevCount;   //从设备总数量     
	const UCHAR                     ucSlaveDevTypes;   //从设备种类数量
    
#ifndef MB_MASTER_DTU_ENABLED    
	const UCHAR                     ucSlaveMinAddr;    //从设备最小通讯地址
	const UCHAR                     ucSlaveMaxAddr;    //从设备最大通讯地址
    
#else           /*由于DTU通讯地址达到200，所以MaxAddr和MinAddr要动态切换 */   
    UCHAR                           ucSlaveMinAddr;    //从设备最小通讯地址
	UCHAR                           ucSlaveMaxAddr;    //从设备最大通讯地址
#endif  
	sMBSlaveDevInfo*               psMBSlaveDevsList; //当前在线从设备列表
    sMBSlaveDevInfo*               psMBSlaveDevCur;   //当前活动的设备
       
}sMBMasterDevsInfo; 

#endif