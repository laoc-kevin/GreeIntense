#include "lpc_gpio.h"
#include "lpc_clkpwr.h"
#include "md_led.h"
#include "lpc_pinsel.h"
#include <os.h>
#include <my_rtt_printf.h>


/**********************************************************************
 * @brief   初始化所有led，包括LedModbus1,LedModbus2,LedCAN,LedMCU
 * @param   NONE
 * @return	NONE
 *********************************************************************/ 
void vLedInit(void)
{
	GPIO_Init();
	
	(void)PINSEL_ConfigPin(LedModbus1.Port, LedModbus1.Pin, 0);
	(void)PINSEL_ConfigPin(LedModbus2.Port, LedModbus2.Pin, 0);
	(void)PINSEL_ConfigPin(LedCAN.Port, LedCAN.Pin, 0);
	(void)PINSEL_ConfigPin(LedMCU.Port, LedMCU.Pin, 0);
	
	GPIO_SetDir(LedModbus1.Port, 1<<LedModbus1.Pin, 1);
	GPIO_SetDir(LedModbus2.Port, 1<<LedModbus2.Pin, 1);
	GPIO_SetDir(LedCAN.Port, 1<<LedCAN.Pin, 1);
	GPIO_SetDir(LedMCU.Port, 1<<LedMCU.Pin, 1);

	GPIO_SetValue(LedModbus1.Port, 1<<LedModbus1.Pin);
	GPIO_SetValue(LedModbus2.Port, 1<<LedModbus2.Pin);
	GPIO_SetValue(LedCAN.Port, 1<<LedCAN.Pin);
	GPIO_SetValue(LedMCU.Port, 1<<LedMCU.Pin);
}

/**********************************************************************
 * @brief   开LED
 * @param   led   LED接口
 * @return	NONE
 *********************************************************************/
void vLedOn(const IODef* led)
{
	GPIO_ClearValue(led->Port, 1<<led->Pin);
}

/**********************************************************************
 * @brief   关LED
 * @param   led   LED接口
 * @return	NONE
 *********************************************************************/
void vLedOff(const IODef* led)
{
	GPIO_SetValue(led->Port, 1<<led->Pin);
}

/**********************************************************************
 * @brief   LED状态改变
 * @param   led     LED接口
 * @param   state 	0:关LED
 * 					1:开LED
 * @return	NONE
 *********************************************************************/
void vLedStateChange(const IODef* ledName,uint8_t state)
{
	if(state)
	{
		GPIO_ClearValue(ledName->Port, 1<<ledName->Pin);
	}
	else 
	{
		GPIO_SetValue(ledName->Port, 1<<ledName->Pin);
	}	
}

