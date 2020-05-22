#include "md_eeprom.h"
#include "lpc_eeprom.h"

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
#define INT32_PAGE_ADDR	6			    //int32类型参数记忆EEPROM存储器初始页地址

#define E16_PAGE_OFFSET 	0			//E16类型参数记忆EEPROM页寄存器偏移量
#define E16_PAGE_ADDR		8			//E16类型参数记忆EEPROM存储器初始页地址

#define E32_PAGE_OFFSET 	16			//E32类型参数记忆EEPROM页寄存器偏移量
#define E32_PAGE_ADDR		8			//E32类型参数记忆EEPROM存储器初始页地址

#define UINT8_SAVE_COUNT	20			//uint8类型的参数记忆数量
#define UINT8_SAVE_SIZE		UINT8_SAVE_COUNT * sizeof(uint8_t)	//uint8类型记忆字节长度

#define INT8_SAVE_COUNT		2			//int8类型的参数记忆数量
#define INT8_SAVE_SIZE		INT8_SAVE_COUNT * sizeof(int8_t)	//int8类型记忆字节长度

#define UINT16_SAVE_COUNT	60			//uint16类型的参数记忆数量
#define UINT16_SAVE_SIZE	UINT16_SAVE_COUNT * sizeof(uint16_t)	//uint16类型记忆字节长度

#define INT16_SAVE_COUNT	20			//int16类型的参数记忆数量
#define INT16_SAVE_SIZE		INT16_SAVE_COUNT * sizeof(int16_t)		//int16类型记忆字节长度

#define UINT32_SAVE_COUNT	5			//uint32类型的参数记忆数量
#define UINT32_SAVE_SIZE	UINT32_SAVE_COUNT * sizeof(uint32_t)	//uint32类型记忆字节长度

#define INT32_SAVE_COUNT	5			//uint32类型的参数记忆数量
#define INT32_SAVE_SIZE	    INT32_SAVE_COUNT * sizeof(int32_t)	//uint32类型记忆字节长度

//#define E16_SAVE_COUNT		8			//能耗统计uint16类型的参数记忆数量
//#define E16_SAVE_SIZE		E16_SAVE_COUNT * sizeof(uint16_t)		//能耗统计uint16类型记忆字节长度

//#define E32_SAVE_COUNT		8			//能耗统计uint32类型的参数记忆数量
//#define E32_SAVE_SIZE		E32_SAVE_COUNT * sizeof(uint32_t)		//能耗统计uint32类型记忆字节长度
 
#define UINT8_WRITE_INTV	    10	    //uint8类型的参数记忆周期
                                        
#define INT8_WRITE_INTV		    10	    //int8类型的参数记忆周期
                                        
#define UINT16_WRITE_INTV	    10	    //uint16类型的参数记忆周期
                                        
#define INT16_WRITE_INTV	    10	    //int16类型的参数记忆周期
                                        
#define UINT32_WRITE_INTV	    7200    //uint32类型的参数记忆周期
                                        
#define INT32_WRITE_INTV	    7200    //uint32类型的参数记忆周期

BOOL     EEPROMDataReady = FALSE;

uint8_t  DataBufUint8Count = 0;
uint8_t  DataBufInt8Count  = 0;

uint8_t  DataBufUint16Count = 0;
uint8_t  DataBufInt16Count = 0;

uint8_t  DataBufUint32Count = 0;
uint8_t  DataBufInt32Count = 0;

uint16_t  DataBufUint8ChangedTimes = 0;
uint16_t  DataBufInt8ChangedTimes  = 0;

uint16_t  DataBufUint16ChangedTimes = 0;
uint16_t  DataBufInt16ChangedTimes = 0;

uint16_t  DataBufUint32ChangedTimes = 0;
uint16_t  DataBufInt32ChangedTimes = 0;


uint8_t*  pDataBufUint8[UINT8_SAVE_COUNT]  = {NULL};
int8_t*   pDataBufInt8[INT8_SAVE_COUNT]    = {NULL};

uint16_t* pDataBufUint16[UINT16_SAVE_COUNT] = {NULL};
int16_t*  pDataBufInt16[INT16_SAVE_COUNT]   = {NULL};

uint32_t* pDataBufUint32[UINT32_SAVE_COUNT] = {NULL};
int32_t*  pDataBufInt32[INT32_SAVE_COUNT]   = {NULL};

uint8_t __attribute__((aligned (4))) DataBufUint8[UINT8_SAVE_COUNT] = {0};
int8_t  __attribute__((aligned (4))) DataBufInt8[INT8_SAVE_COUNT]   = {0};

uint16_t __attribute__((aligned (4))) DataBufUint16[UINT16_SAVE_COUNT] = {0};
int16_t  __attribute__((aligned (4))) DataBufInt16[INT16_SAVE_COUNT]   = {0};

uint32_t __attribute__((aligned (4))) DataBufUint32[UINT32_SAVE_COUNT] = {0};
int32_t  __attribute__((aligned (4))) DataBufInt32[INT32_SAVE_COUNT]   = {0};

//uint16_t __attribute__ ((aligned (4))) ReadBufE16[E16_SAVE_COUNT];
//uint16_t __attribute__ ((aligned (4))) WriteBufE16[E16_SAVE_COUNT] = {0};

//uint32_t __attribute__ ((aligned (4))) ReadBufE32[E32_SAVE_COUNT];
//uint32_t __attribute__ ((aligned (4))) WriteBufE32[E32_SAVE_COUNT] = {0};

BOOL xDataUint8Changed(void)
{
	uint8_t i = 0;
	BOOL    xChanged = FALSE;

	for (i = 0; i < UINT8_SAVE_COUNT; i++)
	{
		if( (pDataBufUint8[i] != NULL) && (DataBufUint8[i] == *pDataBufUint8[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufUint8[i] = *pDataBufUint8[i];
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
		if( (pDataBufUint16[i] != NULL) && (DataBufUint16[i] == *pDataBufUint16[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufUint16[i] = *pDataBufUint16[i];
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
		if( (pDataBufUint32[i] != NULL) && (DataBufUint32[i] == *pDataBufUint32[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufUint32[i] = *pDataBufUint32[i];
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
		if( (pDataBufInt8[i] != NULL) && (DataBufInt8[i] == *pDataBufInt8[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufInt8[i] = *pDataBufInt8[i];
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
		if( (pDataBufInt16[i] != NULL) && (DataBufInt16[i] == *pDataBufInt16[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufInt16[i] = *pDataBufInt16[i];
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
		if( (pDataBufInt32[i] != NULL) && (DataBufInt32[i] == *pDataBufInt32[i]) ) //检查值是否已经改变
		{
			xChanged = TRUE;
			DataBufInt32[i] = *pDataBufInt32[i];
		}
	}
	return xChanged;
}

void vReadEEPROMData(void)
{
    uint8_t i = 0;
    OS_ERR err = OS_ERR_NONE;
    
    EEPROM_Read(UINT8_PAGE_OFFSET, UINT8_PAGE_ADDR, (void*)DataBufUint8, MODE_8_BIT, UINT8_SAVE_SIZE);
    OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    
    EEPROM_Read(INT8_PAGE_OFFSET, INT8_PAGE_ADDR, (void*)DataBufInt8, MODE_8_BIT, INT8_SAVE_SIZE);
    OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    
    EEPROM_Read(UINT16_PAGE_OFFSET, UINT16_PAGE_ADDR, (void*)DataBufUint16, MODE_16_BIT, UINT16_SAVE_SIZE);
    OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    
    EEPROM_Read(INT16_PAGE_OFFSET, INT16_PAGE_ADDR, (void*)DataBufInt16, MODE_16_BIT, INT16_SAVE_SIZE);
    OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    
    EEPROM_Read(UINT32_PAGE_OFFSET, UINT32_PAGE_ADDR, (void*)DataBufUint32, MODE_32_BIT, UINT32_SAVE_SIZE);
    OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    
    EEPROM_Read(INT32_PAGE_OFFSET, INT32_PAGE_ADDR, (void*)DataBufInt32, MODE_32_BIT, INT32_SAVE_SIZE);
    OSTimeDlyHMSM(0, 0, 0, EEPROM_READ_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
    
    for(i=0; i<DataBufUint8Count; i++)
    {
        if(pDataBufUint8[i] != NULL)
        {
            *pDataBufUint8[i] = DataBufUint8[i];
        }
    }
    for(i=0; i<DataBufInt8Count; i++)
    {
        if(pDataBufInt8[i] != NULL)
        {
            *pDataBufInt8[i] = DataBufInt8[i];
        }
    }
    
    for(i=0; i<DataBufUint16Count; i++)
    {
        if(pDataBufUint16[i] != NULL)
        {
            *pDataBufUint16[i] = DataBufUint16[i];
        }
    }
    for(i=0; i<DataBufInt16Count; i++)
    {
        if(pDataBufInt16[i] != NULL)
        {
            *pDataBufInt16[i] = DataBufInt16[i];
        }
    }
    
    for(i=0; i<DataBufUint32Count; i++)
    {
        if(pDataBufUint32[i] != NULL)
        {
            *pDataBufUint32[i] = DataBufUint32[i];
        }
    }
    for(i=0; i<DataBufInt32Count; i++)
    {
        if(pDataBufInt32[i] != NULL)
        {
            *pDataBufInt32[i] = DataBufInt32[i];
        }
    }
    EEPROMDataReady = TRUE;
}

BOOL xEEPROMDataIsReady(void)
{
    return EEPROMDataReady;
}
    
void vWriteEEPROMData(void)
{
    uint8_t i = 0;
    OS_ERR err = OS_ERR_NONE;
    
    if( xDataUint8Changed() )
    {
        DataBufUint8ChangedTimes++;
        if(DataBufUint8ChangedTimes > UINT8_WRITE_INTV)
        {
            EEPROM_Write(UINT8_PAGE_OFFSET, UINT8_PAGE_ADDR, (void*)DataBufUint8, MODE_8_BIT, UINT8_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufUint8ChangedTimes = 0;
        } 
    }
    if( xDataInt8Changed() )
    {
        DataBufInt8ChangedTimes++;
        if(DataBufInt8ChangedTimes > INT8_WRITE_INTV)
        {
            EEPROM_Write(INT8_PAGE_OFFSET, INT8_PAGE_ADDR, (void*)DataBufInt8, MODE_8_BIT, INT8_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufInt8ChangedTimes = 0;
        } 
    }
    
    if( xDataUint16Changed() )
    {
        DataBufUint16ChangedTimes++;
        if(DataBufUint16ChangedTimes > UINT16_WRITE_INTV)
        {
            EEPROM_Write(UINT16_PAGE_OFFSET, UINT16_PAGE_ADDR, (void*)DataBufUint16, MODE_16_BIT, UINT16_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufUint16ChangedTimes = 0;
        }  
    }
    if( xDataInt16Changed() )
    {
        DataBufInt16ChangedTimes++;
        if(DataBufInt16ChangedTimes > INT16_WRITE_INTV)
        {
            EEPROM_Write(INT16_PAGE_OFFSET, INT16_PAGE_ADDR, (void*)DataBufInt16, MODE_16_BIT, INT16_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufInt16ChangedTimes = 0;
        } 
    }
    
    if( xDataUint32Changed() )
    {
        DataBufUint32ChangedTimes++;
        if(DataBufUint32ChangedTimes > UINT32_WRITE_INTV)
        {
            EEPROM_Write(UINT32_PAGE_OFFSET, UINT32_PAGE_ADDR, (void*)DataBufUint32, MODE_32_BIT, UINT32_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufUint32ChangedTimes = 0;
        }     
    }
    if( xDataInt32Changed() )
    {
        DataBufInt32ChangedTimes++;
        if(DataBufInt32ChangedTimes > INT32_WRITE_INTV)
        {
            EEPROM_Write(INT32_PAGE_OFFSET, INT32_PAGE_ADDR, (void*)DataBufInt32, MODE_32_BIT, INT32_SAVE_SIZE);
            OSTimeDlyHMSM(0, 0, 0, EEPROM_WRITE_DATA_DELAY_MS, OS_OPT_TIME_HMSM_STRICT, &err);
            DataBufInt32ChangedTimes = 0;
        } 
    }
}

BOOL xRegistEEPROMData(eEEPROMDataType eDataType, void* pData)
{
    if(pData == NULL)
    {
        return FALSE;
    }
    switch(eDataType)
    {
        case TYPE_UINT_8:
            if(DataBufUint8Count < UINT8_SAVE_COUNT)
            {
                pDataBufUint8[DataBufUint8Count] = (uint8_t*)pData;
                DataBufUint8Count++;
            }
            else
            {
                return FALSE;
            }                
        break;
        
        case TYPE_INT_8:
            if(DataBufInt8Count < INT8_SAVE_COUNT)
            {
                pDataBufInt8[DataBufInt8Count] = (int8_t*)pData;
                DataBufInt8Count++;
            }
            else
            {
                return FALSE;
            }                
        break;
        
        case TYPE_UINT_16:
            if(DataBufUint16Count < UINT16_SAVE_COUNT)
            {
                pDataBufUint16[DataBufUint16Count] = (uint16_t*)pData;
                DataBufUint16Count++;
            }
            else
            {
                return FALSE;
            }                
        break;    
        
        case TYPE_INT_16:
            if(DataBufInt16Count < INT16_SAVE_COUNT)
            {
                pDataBufInt16[DataBufInt16Count] = (int16_t*)pData;
                DataBufInt16Count++;
            }
            else
            {
                return FALSE;
            }                
        break;            
           
         case TYPE_UINT_32:
            if(DataBufUint32Count < UINT32_SAVE_COUNT)
            {
                pDataBufUint32[DataBufUint32Count] = (uint32_t*)pData;
                DataBufUint32Count++;
            }
            else
            {
                return FALSE;
            }                
        break;    
        
        case TYPE_INT_32:
            if(DataBufInt32Count < INT32_SAVE_COUNT)
            {
                pDataBufInt32[DataBufInt32Count] = (int32_t*)pData;
                DataBufInt32Count++;
            }
            else
            {
                return FALSE;
            }                
        break;                    
        default: break;
    }
    return TRUE;
}

void vEEPROMDataTask(void * p_arg)
{
    OS_ERR err = OS_ERR_NONE;
    
    EEPROM_Init();    
    while(DEF_TRUE)
	{
        OSTimeDlyHMSM(0, 0, EEPROM_WRITE_DATA_INTERVAL_S, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        vWriteEEPROMData();
    }
}