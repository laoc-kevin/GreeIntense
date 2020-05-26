#ifndef _MD_EEPROM_H_
#define _MD_EEPROM_H_

#include "includes.h"
#include "lpc_types.h"

#define EEPROM_DATA(arg1, arg2)  (void)xRegistEEPROMData(arg1, &arg2);

typedef enum   /*运行模式*/
{
    TYPE_UINT_8    = 1,   
    TYPE_INT_8     = 2,   
    
    TYPE_UINT_16   = 3,     
    TYPE_INT_16    = 4,   
    
    TYPE_UINT_32   = 5,     
    TYPE_INT_32    = 6, 

    TYPE_RUNTIME   = 7,    
    
}eEEPROMDataType;

void vReadEEPROMData(void);

BOOL xEEPROMDataIsReady(void);
BOOL xRegistEEPROMData(eEEPROMDataType eDataType, void* pData);

void vEEPROMInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size);

#endif