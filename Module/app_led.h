#ifndef _APP_LED_H_
#define _APP_LED_H_

#include <stdint.h>
#include "lpc_pinsel.h"
#include "app_io.h"
typedef enum 
{
	Led_OFF=0,
	Led_ON
}Led_State;

void LedStateChange(const IODef* ledName,uint8_t state);
void AppLedInit(void);
void LedTask(void * p_arg);
void AppLedOn(const IODef* led);
void AppLedOff(const IODef* led);
#endif
