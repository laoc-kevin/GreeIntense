#include "app_config.h"
#include "lpc_eeprom.h"
#include "md_eeprom.h"
#include "md_event.h"
#include "my_rtt_printf.h"

#define EEPROM_READ_DATA_DELAY_MS       50
#define EEPROM_WRITE_DATA_DELAY_MS      200

#define EEPROM_WRITE_DATA_INTERVAL_S    1

#define UINT8_PAGE_OFFSET 	0			//uint8类型参数记忆EEPROM页寄存器偏移量
#define UINT8_PAGE_ADDR 	0			//uint8类型参数记忆EEPROM存储器初始页地址

#define INT8_PAGE_OFFSET 	58			//uint8类型参数记忆EEPROM页寄存器偏移量
#define INT8_PAGE_ADDR 		2			//uint8类型参数记忆EEPROM存储器初始页地址

#define UINT16_PAGE_OFFSET 	0			//uint16类型参数记忆EEPROM页寄存器偏移量
#define UINT16_PAGE_ADDR	3			//uint16类型参数记忆EEPROM存储器初始页地址

#define INT16_PAGE_OFFSET 	0			//int16类型参数记忆EEPROM页寄存器偏移量
#define INT16_PAGE_ADDR		5			//int16类型参数记忆EEPROM存储器初始页地址

#define UINT32_PAGE_OFFSET 	0			//uint32类型参数记忆EEPROM页寄存器偏移量
#define UINT32_PAGE_ADDR	6			//uint32类型参数记忆EEPROM存储器初始页地址

#define INT32_PAGE_OFFSET 	0			//int32类型参数记忆EEPROM页寄存器偏移量
#define INT32_PAGE_ADDR	    7			//int32类型参数记忆EEPROM存储器初始页地址

#define RUNTIME_PAGE_OFFSET 0			//RUNTIME类型参数记忆EEPROM页寄存器偏移量
#define RUNTIME_PAGE_ADDR   8			//RUNTIME类型参数记忆EEPROM存储器初始页地址

#define E32_PAGE_OFFSET 	16			//E32类型参数记忆EEPROM页寄存器偏移量
#define E32_PAGE_ADDR		9			//E32类型参数记忆EEPROM存储器初始页地址


/*根据实际需要记忆参数的个数进行修改*/
#define UINT8_SAVE_COUNT	10			//uint8类型的参数记忆数量
#define UINT8_SAVE_SIZE		UINT8_SAVE_COUNT * sizeof(uint8_t)	//uint8类型记忆字节长度

#define INT8_SAVE_COUNT		2			//int8类型的参数记忆数量
#define INT8_SAVE_SIZE		INT8_SAVE_COUNT * sizeof(int8_t)	//int8类型记忆字节长度

#define UINT16_SAVE_COUNT	50			//uint16类型的参数记忆数量
#define UINT16_SAVE_SIZE	UINT16_SAVE_COUNT * sizeof(uint16_t)	//uint16类型记忆字节长度

#define INT16_SAVE_COUNT	10			//int16类型的参数记忆数量
#define INT16_SAVE_SIZE		INT16_SAVE_COUNT * sizeof(int16_t)		//int16类型记忆字节长度

#define UINT32_SAVE_COUNT	6			//uint32类型的参数记忆数量
#define UINT32_SAVE_SIZE	UINT32_SAVE_COUNT * sizeof(uint32_t)	//uint32类型记忆字节长度

#define INT32_SAVE_COUNT	5			//int32类型的参数记忆数量
#define INT32_SAVE_SIZE	    INT32_SAVE_COUNT * sizeof(int32_t)	//uint32类型记忆字节长度

#define RUNTIME_SAVE_COUNT  8			//运行时间类型的参数记忆数量
#define RUNTIME_SAVE_SIZE   RUNTIME_SAVE_COUNT * sizeof(uint32_t)		//运行时间类型记忆字节长度

#define E32_SAVE_COUNT		8			//能耗统计uint32类型的参数记忆数量
#define E32_SAVE_SIZE		E32_SAVE_COUNT * sizeof(uint32_t)		//能耗统计uint32类型记忆字节长度
 

#define UINT8_WRITE_INTV	    1	    //uint8类型的参数记忆周期                                       
#define INT8_WRITE_INTV		    1	    //int8类型的参数记忆周期
                                        
#define UINT16_WRITE_INTV	    1	    //uint16类型的参数记忆周期                                       
#define INT16_WRITE_INTV	    1	    //int16类型的参数记忆周期
                                        
#define UINT32_WRITE_INTV	    1       //uint32类型的参数记忆周期                                       
#define INT32_WRITE_INTV	    1       //uint32类型的参数记忆周期

#define RUNTIME_WRITE_INTV	    7200    //运行时间类型的参数记忆周期
#define E32_WRITE_INTV	        7200    //能耗统计uint32类型的参数记忆周期

BOOL     EEPROMDataReady = FALSE;
BOOL     EEPROMFirstRun  = TRUE; //主板第一次上电

uint8_t  DataBufUint8Count = 0;
uint8_t  DataBufInt8Count  = 0;

uint8_t  DataBufUint16Count = 0;
uint8_t  DataBufInt16Count = 0;

uint8_t  DataBufUint32Count = 0;
uint8_t  DataBufInt32Count = 0;

uint8_t  DataBufRuntimeCount = 0;
uint8_t  DataBufE32Count = 0;


uint16_t  DataBufUint8ChangedTimes = 0;
uint16_t  DataBufInt8ChangedTimes  = 0;

uint16_t  DataBufUint16ChangedTimes = 0;
uint16_t  DataBufInt16ChangedTimes = 0;

uint16_t  DataBufUint32ChangedTimes = 0;
uint16_t  DataBufInt32ChangedTimes = 0;

uint16_t  DataBufRuntimeChangedTimes = 0;
uint16_t  DataBufE32ChangedTimes = 0;


uint8_t*  pDataBufUint8[UINT8_SAVE_COUNT]  = {NULL};
int8_t*   pDataBufInt8[INT8_SAVE_COUNT]    = {NULL};

uint16_t* pDataBufUint16[UINT16_SAVE_COUNT] = {NULL};
int16_t*  pDataBufInt16[INT16_SAVE_COUNT]   = {NULL};

uint32_t* pDataBufUint32[UINT32_SAVE_COUNT] = {NULL};
int32_t*  pDataBufInt32[INT32_SAVE_COUNT]   = {NULL};

uint32_t* pDataBufRuntime[RUNTIME_SAVE_COUNT] = {NULL};
uint32_t* pDataBufE32[E32_SAVE_COUNT] = {NULL};


uint8_t __attribute__((aligned (4))) DataBufFirstRunUint8[UINT8_SAVE_COUNT] = {0};
uint8_t __attribute__((aligned (4))) DataBufUint8[UINT8_SAVE_COUNT] = {0};
int8_t  __attribute__((aligned (4))) DataBufInt8[INT8_SAVE_COUNT]   = {0};

uint16_t __attribute__((aligned (4))) DataBufUint16[UINT16_SAVE_COUNT] = {0};
int16_t  __attribute__((aligned (4))) DataBufInt16[INT16_SAVE_COUNT]   = {0};

uint32_t __attribute__((aligned (4))) DataBufUint32[UINT32_SAVE_COUNT] = {0};
int32_t  __attribute__((aligned (4))) DataBufInt32[INT32_SAVE_COUNT]   = {0};

uint32_t __attribute__((aligned (4))) DataBufRuntime[RUNTIME_SAVE_COUNT] = {0};

//uint16_t __attribute__ ((aligned (4))) ReadBufE16[E16_SAVE_COUNT];
//uint16_t __attribute__ ((aligned (4))) WriteBufE16[E16_SAVE_COUNT] = {0};

uint32_t __attribute__ ((aligned (4))) DataBufE32[E32_SAVE_COUNT] = {0};

BOOL xDataUint8Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < UINT8_SAVE_COUNT; i++)
	{
		if( (pDataBufUint8[i] != NULL) && (DataBufUint8[i] != *(uint8_t*)pDataBufUint8[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufUint8[i] = *(uint8_t*)pDataBufUint8[i];
		}
	}
	return xChanged;
}

BOOL xDataUint16Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < UINT16_SAVE_COUNT; i++)
	{
		if( (pDataBufUint16[i] != NULL) && (DataBufUint16[i] != *(uint16_t*)pDataBufUint16[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufUint16[i] = *(uint16_t*)pDataBufUint16[i];
		}
	}
	return xChanged;
}

BOOL xDataUint32Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < UINT32_SAVE_COUNT; i++)
	{
		if( (pDataBufUint32[i] != NULL) && (DataBufUint32[i] != *(uint32_t*)pDataBufUint32[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufUint32[i] = *(uint32_t*)pDataBufUint32[i];
		}
	}
	return xChanged;
}

BOOL xDataInt8Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < INT8_SAVE_COUNT; i++)
	{
		if( (pDataBufInt8[i] != NULL) && (DataBufInt8[i] != *(int8_t*)pDataBufInt8[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufInt8[i] = *(int8_t*)pDataBufInt8[i];
		}
	}
	return xChanged;
}

BOOL xDataInt16Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < INT16_SAVE_COUNT; i++)
	{
		if( (pDataBufInt16[i] != NULL) && (DataBufInt16[i] != *(int16_t*)pDataBufInt16[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufInt16[i] = *(int16_t*)pDataBufInt16[i];
		}
	}
	return xChanged;
}

BOOL xDataInt32Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < INT32_SAVE_COUNT; i++)
	{
		if( (pDataBufInt32[i] != NULL) && (DataBufInt32[i] != *(int32_t*)pDataBufInt32[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufInt32[i] = *(int32_t*)pDataBufInt32[i];
		}
	}
	return xChanged;
}

BOOL xDataRuntimeChanged(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < RUNTIME_SAVE_COUNT; i++)
	{
		if( (pDataBufRuntime[i] != NULL) && (DataBufRuntime[i] != *(uint32_t*)pDataBufRuntime[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufRuntime[i] = *(uint32_t*)pDataBufRuntime[i];
		}
	}
	return xChanged;
}

BOOL xDataE32Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < E32_SAVE_COUNT; i++)
	{
		if( (pDataBufE32[i] != NULL) && (DataBufE32[i] != *(uint32_t*)pDataBufE32[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufE32[i] = *(uint32_t*)pDataBufE32[i];
		}
	}
	return xChanged;
}

/**********************************************************************
 * @brief  读EEPROM数据
 *********************************************************************/
void vReadEEPROMData(void)
{
    uint8_t i = 0;
    OS_ERR err = OS_ERR_NONE;

    if(DataBufUint8Count>0)
    {
        EEPROM_Read(UINT8_PAGE_OFFSET, UINT8_PAGE_ADDR, (void*)DataBufUint8, MODE_8_BIT, UINT8_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }        
    if(DataBufInt8Count>0)
    {
        EEPROM_Read(INT8_PAGE_OFFSET, INT8_PAGE_ADDR, (void*)DataBufInt8, MODE_8_BIT, INT8_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufUint16Count>0)
    {    
        EEPROM_Read(UINT16_PAGE_OFFSET, UINT16_PAGE_ADDR, (void*)DataBufUint16, MODE_16_BIT, UINT16_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufInt16Count>0)
    {    
        EEPROM_Read(INT16_PAGE_OFFSET, INT16_PAGE_ADDR, (void*)DataBufInt16, MODE_16_BIT, INT16_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufUint32Count>0)
    {    
        EEPROM_Read(UINT32_PAGE_OFFSET, UINT32_PAGE_ADDR, (void*)DataBufUint32, MODE_32_BIT, UINT32_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufInt32Count>0)
    {
        EEPROM_Read(INT32_PAGE_OFFSET, INT32_PAGE_ADDR, (void*)DataBufInt32, MODE_32_BIT, INT32_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufRuntimeCount>0)
    {
        EEPROM_Read(RUNTIME_PAGE_OFFSET, RUNTIME_PAGE_ADDR, (void*)DataBufRuntime, MODE_32_BIT, RUNTIME_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufE32Count>0)
    {
        EEPROM_Read(E32_PAGE_OFFSET, E32_PAGE_ADDR, (void*)DataBufE32, MODE_32_BIT, E32_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    myprintf("EEPROM_Read\n");
    
    for(i=0; i<DataBufUint8Count; i++)
    {
        if(pDataBufUint8[i] != NULL)
        {
            *(uint8_t*)pDataBufUint8[i] = DataBufUint8[i];
        }
    }
    for(i=0; i<DataBufInt8Count; i++)
    {
        if(pDataBufInt8[i] != NULL)
        {
            *(int8_t*)pDataBufInt8[i] = DataBufInt8[i];
        }
    }
    
    for(i=0; i<DataBufUint16Count; i++)
    {
        if(pDataBufUint16[i] != NULL)
        {
            *(uint16_t*)pDataBufUint16[i] = DataBufUint16[i];
        }
    }
    for(i=0; i<DataBufInt16Count; i++)
    {
        if(pDataBufInt16[i] != NULL)
        {
            *(int16_t*)pDataBufInt16[i] = DataBufInt16[i];
        }
    }
    for(i=0; i<DataBufUint32Count; i++)
    {
        if(pDataBufUint32[i] != NULL)
        {
            *(uint32_t*)pDataBufUint32[i] = DataBufUint32[i];
        }
    }
    for(i=0; i<DataBufInt32Count; i++)
    {
        if(pDataBufInt32[i] != NULL)
        {
            *(int32_t*)pDataBufInt32[i] = DataBufInt32[i];
        }
    }
    for(i=0; i<DataBufRuntimeCount; i++)
    {
        if(pDataBufRuntime[i] != NULL)
        {
            *(uint32_t*)pDataBufRuntime[i] = DataBufRuntime[i];
        }
    }
    for(i=0; i<DataBufE32Count; i++)
    {
        if(pDataBufE32[i] != NULL)
        {
            *(uint32_t*)pDataBufE32[i] = DataBufE32[i];
        }
    }
    
    myprintf("vReadEEPROMData ulExAirFanRated_Vol %ld  %ld \n\n", DataBufUint32[0], *(uint32_t*)pDataBufUint32[0]);
    EEPROMDataReady = TRUE;
}

BOOL xEEPROMDataIsReady(void)
{
    return EEPROMDataReady;
}
  
/**********************************************************************
 * @brief  写EEPROM数据
 *********************************************************************/
void vWriteEEPROMData(void)
{
    uint8_t i = 0;
    OS_ERR err = OS_ERR_NONE;
    
    if(xDataUint8Changed())
    {
        DataBufUint8ChangedTimes++;
        if(DataBufUint8ChangedTimes >= UINT8_WRITE_INTV)
        {
            EEPROM_Write(UINT8_PAGE_OFFSET, UINT8_PAGE_ADDR, (void*)DataBufUint8, MODE_8_BIT, UINT8_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufUint8ChangedTimes = 0;
            
            myprintf("xDataUint8Changed\n");
        } 
    }
    if(xDataInt8Changed())
    {
        DataBufInt8ChangedTimes++;
        if(DataBufInt8ChangedTimes >= INT8_WRITE_INTV)
        {
            EEPROM_Write(INT8_PAGE_OFFSET, INT8_PAGE_ADDR, (void*)DataBufInt8, MODE_8_BIT, INT8_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufInt8ChangedTimes = 0;
            
            myprintf("xDataInt8Changed\n");
        } 
    }
    
    if(xDataUint16Changed())
    {
        DataBufUint16ChangedTimes++;
        if(DataBufUint16ChangedTimes >= UINT16_WRITE_INTV)
        {
            EEPROM_Write(UINT16_PAGE_OFFSET, UINT16_PAGE_ADDR, (void*)DataBufUint16, MODE_16_BIT, UINT16_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufUint16ChangedTimes = 0;
            
            myprintf("xDataUint16Changed\n");
        }  
    }
    if(xDataInt16Changed())
    {
        DataBufInt16ChangedTimes++;
        if(DataBufInt16ChangedTimes >= INT16_WRITE_INTV)
        {
            EEPROM_Write(INT16_PAGE_OFFSET, INT16_PAGE_ADDR, (void*)DataBufInt16, MODE_16_BIT, INT16_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufInt16ChangedTimes = 0;
            
            myprintf("xDataInt16Changed\n");
        } 
    }
    if(xDataUint32Changed())
    {
        DataBufUint32ChangedTimes++;
        if(DataBufUint32ChangedTimes >= UINT32_WRITE_INTV)
        {
            EEPROM_Write(UINT32_PAGE_OFFSET, UINT32_PAGE_ADDR, (void*)DataBufUint32, MODE_32_BIT, UINT32_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufUint32ChangedTimes = 0;
            
            myprintf("xDataUint32Changed\n");
        }     
    }
    if(xDataInt32Changed())
    {
        DataBufInt32ChangedTimes++;
        if(DataBufInt32ChangedTimes >= INT32_WRITE_INTV)
        {
            EEPROM_Write(INT32_PAGE_OFFSET, INT32_PAGE_ADDR, (void*)DataBufInt32, MODE_32_BIT, INT32_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufInt32ChangedTimes = 0;
            
            myprintf("xDataInt32Changed\n");
        } 
    }
    if(xDataRuntimeChanged())
    {
        DataBufRuntimeChangedTimes++;
        if(DataBufRuntimeChangedTimes >= RUNTIME_WRITE_INTV)
        {
            EEPROM_Write(RUNTIME_PAGE_OFFSET, RUNTIME_PAGE_ADDR, (void*)DataBufRuntime, MODE_32_BIT, RUNTIME_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufRuntimeChangedTimes = 0;
            
            myprintf("xDataRuntimeChanged\n");
        } 
    }
    if(xDataE32Changed())
    {
        DataBufE32ChangedTimes++;
        if(DataBufE32ChangedTimes >= E32_WRITE_INTV)
        {
            EEPROM_Write(E32_PAGE_OFFSET, E32_PAGE_ADDR, (void*)DataBufE32, MODE_32_BIT, RUNTIME_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufE32ChangedTimes = 0;
            
            myprintf("xDataE32Changed\n");
        } 
    }
}

/**********************************************************************
* @brief  首次上电写EEPROM数据
 *********************************************************************/
void vWriteEEPROMDataFirstTime(void)
{
    OS_ERR err = OS_ERR_NONE;
    uint16_t usIndex = 0;
   
#if EEPROM_DATA_INIT > 0    //参数复位    

    for(usIndex=0; usIndex<DataBufRuntimeCount; usIndex++)  //运行时间复位
    {
        if(pDataBufRuntime[usIndex] != NULL)
        {
            *(uint32_t*)pDataBufRuntime[usIndex] = 0;
            DataBufRuntime[usIndex] = 0;
        }
    }
    for(usIndex=0; usIndex<DataBufE32Count; usIndex++)     //能耗复位
    {
        if(pDataBufE32[usIndex] != NULL)
        {
             *(uint32_t*)pDataBufE32[usIndex] = 0;
             DataBufE32[usIndex] = 0;
        }
    } 
#endif     
    if(DataBufUint8Count>0)
    {
        EEPROM_Write(UINT8_PAGE_OFFSET, UINT8_PAGE_ADDR, (void*)DataBufUint8, MODE_8_BIT, UINT8_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }        
    if(DataBufInt8Count>0)
    {
        EEPROM_Write(INT8_PAGE_OFFSET, INT8_PAGE_ADDR, (void*)DataBufInt8, MODE_8_BIT, INT8_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufUint16Count>0)
    {    
        EEPROM_Write(UINT16_PAGE_OFFSET, UINT16_PAGE_ADDR, (void*)DataBufUint16, MODE_16_BIT, UINT16_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufInt16Count>0)
    {    
        EEPROM_Write(INT16_PAGE_OFFSET, INT16_PAGE_ADDR, (void*)DataBufInt16, MODE_16_BIT, INT16_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufUint32Count>0)
    {    
        EEPROM_Write(UINT32_PAGE_OFFSET, UINT32_PAGE_ADDR, (void*)DataBufUint32, MODE_32_BIT, UINT32_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufInt32Count>0)
    {
        EEPROM_Write(INT32_PAGE_OFFSET, INT32_PAGE_ADDR, (void*)DataBufInt32, MODE_32_BIT, INT32_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufRuntimeCount>0)
    {
        EEPROM_Write(RUNTIME_PAGE_OFFSET, RUNTIME_PAGE_ADDR, (void*)DataBufRuntime, MODE_32_BIT, RUNTIME_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    if(DataBufE32Count>0)
    {
        EEPROM_Write(E32_PAGE_OFFSET, E32_PAGE_ADDR, (void*)DataBufE32, MODE_32_BIT, E32_SAVE_SIZE);
        OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    myprintf("vWriteEEPROMDataFirstTime ulExAirFanRated_Vol %ld  %ld \n\n", DataBufUint32[0], *(uint32_t*)pDataBufUint32[0]);
    
}

/**********************************************************************
 * @brief  注册EEPROM数据
 *********************************************************************/
BOOL xRegistEEPROMData(eEEPROMDataType eDataType, void* pData)
{
    uint16_t usIndex = 0;
    
    if(pData == NULL)
    {
        return FALSE;
    }
    switch(eDataType)
    {
        case TYPE_UINT_8:    
            for(usIndex=0; usIndex<DataBufUint8Count; usIndex++)
            {
                if(pDataBufUint8[usIndex] == (uint8_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufUint8Count < UINT8_SAVE_COUNT)
            {
                pDataBufUint8[DataBufUint8Count] = (uint8_t*)pData;
                DataBufUint8[DataBufUint8Count]  = *(uint8_t*)pData;
                DataBufUint8Count++;
            }
            else
            {
                myprintf("TYPE_UINT_8 over\n");
                return FALSE;
            }                
        break;
        
        case TYPE_INT_8:
            for(usIndex=0; usIndex<DataBufInt8Count; usIndex++)
            {
                if(pDataBufInt8[usIndex] == (int8_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufInt8Count < INT8_SAVE_COUNT)
            {
                pDataBufInt8[DataBufInt8Count] = (int8_t*)pData;
                DataBufInt8[DataBufInt8Count]  = *(int8_t*)pData;
                DataBufInt8Count++;
            }
            else
            {
                myprintf("TYPE_INT_8 over\n");
                return FALSE;
            }                
        break;
        
        case TYPE_UINT_16:
            for(usIndex=0; usIndex<DataBufUint16Count; usIndex++)
            {
                if(pDataBufUint16[usIndex] == (uint16_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufUint16Count < UINT16_SAVE_COUNT)
            {
                pDataBufUint16[DataBufUint16Count] = (uint16_t*)pData;
                DataBufUint16[DataBufUint16Count]  = *(uint16_t*)pData;
                DataBufUint16Count++;
            }
            else
            {
                myprintf("TYPE_UINT_16 over\n");
                return FALSE;
            }                
        break;    
        
        case TYPE_INT_16:
            for(usIndex=0; usIndex<DataBufInt16Count; usIndex++)
            {
                if(pDataBufInt16[usIndex] == (int16_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufInt16Count < INT16_SAVE_COUNT)
            {
                pDataBufInt16[DataBufInt16Count] = (int16_t*)pData;
                DataBufInt16[DataBufInt16Count]  = *(int16_t*)pData;
                DataBufInt16Count++;
            }
            else
            {
                myprintf("TYPE_INT_16 over\n");
                return FALSE;
            }                
        break;            
           
         case TYPE_UINT_32:
            for(usIndex=0; usIndex<DataBufUint32Count; usIndex++)
            {
                if(pDataBufUint32[usIndex] == (uint32_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufUint32Count < UINT32_SAVE_COUNT)
            {
                pDataBufUint32[DataBufUint32Count] = (uint32_t*)pData;
                DataBufUint32[DataBufUint32Count]  = *(uint32_t*)pData;
                DataBufUint32Count++;
            }
            else
            {
                myprintf("TYPE_UINT_32 over\n");
                return FALSE;
            }                
        break;    
        
        case TYPE_INT_32:
            for(usIndex=0; usIndex<DataBufInt32Count; usIndex++)
            {
                if(pDataBufInt32[usIndex] == (int32_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufInt32Count < INT32_SAVE_COUNT)
            {
                pDataBufInt32[DataBufInt32Count] = (int32_t*)pData;
                DataBufInt32[DataBufInt32Count]  = *(int32_t*)pData;
                DataBufInt32Count++;
            }
            else
            {
                myprintf("TYPE_INT_32 over\n");
                return FALSE;
            }                
        break; 

        case TYPE_RUNTIME:
            for(usIndex=0; usIndex<DataBufRuntimeCount; usIndex++)
            {
                if(pDataBufRuntime[usIndex] == (uint32_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufRuntimeCount < RUNTIME_SAVE_COUNT)
            {
                pDataBufRuntime[DataBufRuntimeCount] = (uint32_t*)pData;
                DataBufRuntime[DataBufRuntimeCount]  = *(uint32_t*)pData;
                DataBufRuntimeCount++;
            }
            else
            {
                myprintf("TYPE_RUNTIME over\n");
                return FALSE;
            }                
        break; 

        case TYPE_E32:
            for(usIndex=0; usIndex<DataBufE32Count; usIndex++)
            {
                if(pDataBufE32[usIndex] == (uint32_t*)pData)
                {
                    return FALSE;
                }
            }
            if(DataBufE32Count < E32_SAVE_COUNT)
            {
                pDataBufE32[DataBufE32Count] = (uint32_t*)pData;
                DataBufE32[DataBufE32Count] = *(uint32_t*)pData;
                DataBufE32Count++;
            }
            else
            {
                myprintf("TYPE_E32 over\n");
                return FALSE;
            }                
        break;               
        default: break;
    }
    return TRUE;
}

/**********************************************************************
 * @brief  轮询写EEPROM数据任务
 *********************************************************************/
void vEEPROMDataTask(void * p_arg)
{
    OS_ERR err = OS_ERR_NONE;
    EEPROM_Init(); 

#if EEPROM_USE_DEFAULT_DATA == 0    //不使用默认参数
    
#if EEPROM_DATA_INIT > 0    //参数复位
    
    vWriteEEPROMDataFirstTime();
#else 
    EEPROM_Read(UINT8_PAGE_OFFSET, UINT8_PAGE_ADDR, (void*)DataBufFirstRunUint8, MODE_8_BIT, UINT8_SAVE_SIZE);   //上电
    OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    
    EEPROMFirstRun = DataBufFirstRunUint8[DataBufUint8Count-1];  //特别注意一定要保证该变量位于最后位置 
    if(EEPROMFirstRun == TRUE)       //首次上电,先同步默认参数
    {
        EEPROMFirstRun = FALSE;
        DataBufUint8[DataBufUint8Count-1] = FALSE;
        
        vWriteEEPROMDataFirstTime();
    }
#endif 
    vReadEEPROMData();
#endif    
    
    while(DEF_TRUE)
	{
        OSTimeDlyHMSM(0, 0, EEPROM_WRITE_DATA_INTERVAL_S, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        vWriteEEPROMData();
    }
}

/**********************************************************************
 * @brief  EEPROM初始化
 *********************************************************************/
void vEEPROMInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
     EEPROM_DATA(TYPE_UINT_8, EEPROMFirstRun);
     (void)eTaskCreate(p_tcb, vEEPROMDataTask, NULL, prio, p_stk_base, stk_size);
}