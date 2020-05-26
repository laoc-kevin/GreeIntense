#ifndef _MD_LED_H_
#define _MD_LED_H_

#include <stdint.h>
#include "lpc_pinsel.h"
#include "md_io.h"

typedef enum 
{
	Led_OFF=0,
	Led_ON
}Led_State;

void vLedStateChange(const IODef* ledName,uint8_t state);
void vLedInit(void);
void vLedOn(const IODef* led);
void vLedOff(const IODef* led);

#endif
