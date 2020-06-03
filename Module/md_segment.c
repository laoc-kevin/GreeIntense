#include "lpc_ssp.h"
#include "lpc_pinsel.h"

#include "md_input.h"
#include "md_segment.h"
#include "md_event.h"
#include "md_led.h"

uint8_t SegmentID = 0;

uint8_t DISP_TAB_LEFT[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
uint8_t DISP_TAB_RIGHT[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};


//将接收到的拨码值转化成真正的地址值，使之与硬件实际相对应
//SaInput.0---Bit4, SaInput.1---Bit0, SaInput.2---Bit1, SaInput.3---Bit3, SaInput.4---Bit2
uint8_t ucSegmentConvertToID(void)
{
	uint8_t SaInput = ucGetSaInput();
	// return (~((SaInput>>1&1) + (SaInput>>1&2) + (SaInput>>2&4) + (SaInput&8) + ((SaInput&1)<<4)))&31;
	return (~(((SaInput & 1) << 4) | ((SaInput & 2) >> 1) | ((SaInput & 4) >> 1) | (SaInput & 8) | ((SaInput & 16) >> 2))) & 31;
}

void vSegmentShowToggle(void)
{
	static uint8_t position = 0;
	if (!position) //显示左边
	{
		SSP_SendData(LPC_SSP0, (CPU_INT16U)DISP_TAB_LEFT[SegmentID/10]);
	}
	else
	{
		SSP_SendData(LPC_SSP0, (CPU_INT16U)DISP_TAB_RIGHT[SegmentID%10]);
	}
	position = ~position;
}


//数码管显示任务
void vSegmentTask(void *p_arg)
{
	OS_ERR os_err = OS_ERR_NONE;
    
    SegmentID = ucGetControllerID();
	while(DEF_TRUE)
	{
        vSegmentShowToggle();
        OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_HMSM_STRICT, &os_err);
	}
}

//初始化
void vSegmentInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
	SSP_CFG_Type SSP0_ConfigStruct = {SSP_DATABIT_8, 0, 0, 0, SSP_FRAME_SPI, 10000000};
	//管脚功能配置
	(void)PINSEL_ConfigPin(0, 15, 2);
	(void)PINSEL_ConfigPin(0, 18, 2);

	SSP_ConfigStructInit(&SSP0_ConfigStruct);
	SSP_Init(LPC_SSP0, &SSP0_ConfigStruct);
	SSP_Cmd(LPC_SSP0, ENABLE);
    
    vLedInit();
    
    (void)eTaskCreate(p_tcb, vSegmentTask, NULL, prio, p_stk_base, stk_size);
}
