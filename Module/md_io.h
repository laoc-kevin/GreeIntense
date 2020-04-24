#ifndef _MD_IO_H
#define _MD_IO_H

#include "includes.h"

typedef enum
{
	OFF = 0,
	ON  = 1
}eCtrlEn;

typedef struct
{
	uint8_t Port;
	uint8_t Pin;
}IODef;

//敏捷群控管脚配置
extern const IODef LedMCU;	//MCU_WORK
extern const IODef LedModbus1;	//RS485_LED1
extern const IODef LedModbus2;	//RS485_LED2
extern const IODef LedCAN;	//CAN_LED

extern const IODef IO4051A;	//4051多路输入选择管脚A
extern const IODef IO4051B;	//4051多路输入选择管脚B
extern const IODef IO4051C;	//4051多路输入选择管脚C
extern const IODef SAInput;	//拨码输入
extern const IODef DInput1;	//数字量输入1
extern const IODef DInput2;	//数字量输入2
extern const IODef AInput;	//模拟量输入

extern const IODef AOutput1;	//AO_PWM1
extern const IODef AOutput2;	//AO_PWM2
extern const IODef AOutput3;	//AO_PWM3
extern const IODef AOutput4;	//AO_PWM4
extern const IODef AOutput5;	//AO_PWM5
extern const IODef AOutput6;	//AO_PWM6

extern const IODef DAC1LATCH;	//DAC7760_LATCH
extern const IODef DAC1CLR;	    //DAC7760_CLR
extern const IODef DAC1ALARM;	//DAC7760_ALARM
extern const IODef DAC2LATCH;	//DAC7760_LATCH
extern const IODef DAC2CLR;	    //DAC7760_CLR
extern const IODef DAC2ALARM;	//DAC7760_ALARM
extern const IODef SPI1MISO;	//SPI1_MISO
extern const IODef SPI1MOSI;	//SPI1_MOSI
extern const IODef SPI1SCLK;	//SPI1_SCLK

extern const IODef DOutput1;	//数字量输出1
extern const IODef DOutput2;	//数字量输出2
extern const IODef DOutput3;	//数字量输出3
extern const IODef DOutput4;	//数字量输出4
extern const IODef DOutput5;	//数字量输出5
extern const IODef DOutput6;	//数字量输出6
extern const IODef DOutput7;	//数字量输出7
extern const IODef DOutput8;	//数字量输出8
extern const IODef DOutput9;	//数字量输出9
extern const IODef DOutput10;	//数字量输出10
extern const IODef DOutput11;	//数字量输出11
extern const IODef DOutput12;	//数字量输出12
extern const IODef DOutput13;	//数字量输出13

extern const IODef QuickTest;	//快速测试

extern const IODef ISP;	//ISP烧写

extern const IODef SegSPI0MOSI;	//数码管SPI0输出
extern const IODef SegSPI0CLK;	//数码管SPI0时钟

extern const IODef CAN1Tx;	//CAN1Tx
extern const IODef CAN1Rx;	//CAN1Rx
extern const IODef CAN1Inv;	//CAN1Inv
extern const IODef CAN1Addr;	//CAN1Addr

extern const IODef Uart1Tx;	//Uart1Tx
extern const IODef Uart1Rx;	//Uart1Rx
extern const IODef Uart1DE;	//Uart1DE
extern const IODef Uart1Inv;	//Uart1Inv
extern const IODef Uart0Tx;	//Uart0Tx
extern const IODef Uart0Rx;	//Uart0Rx
extern const IODef Uart0DE;	//Uart0DE
extern const IODef Uart0Inv;	//Uart0Inv

extern const IODef EthMDC;
extern const IODef EthMDIO;
extern const IODef EthRMIIRxER;
extern const IODef EthRMIIRefClk;
extern const IODef EthRMIIRXD0;
extern const IODef EthRMIIRXD1;
extern const IODef EthRMIIRxEn;
extern const IODef EthRMIICRS;
extern const IODef EthRMIITXD0;
extern const IODef EthRMIITXD1;

	
#endif
