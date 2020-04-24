#include "md_io.h"

//敏捷群控管脚配置
const IODef LedMCU 		= {1, 31};	//MCU_WORK
const IODef LedModbus1 	= {3, 26};	//RS485_LED1
const IODef LedModbus2 	= {0, 27};	//RS485_LED2
const IODef LedCAN 		= {3, 25};	//CAN_LED

const IODef IO4051A 	= {1, 27};	//4051多路输入选择管脚A
const IODef IO4051B 	= {1, 28};	//4051多路输入选择管脚B
const IODef IO4051C 	= {1, 29};	//4051多路输入选择管脚C
const IODef SAInput		= {0, 28};	//拨码输入
const IODef DInput1		= {0, 29};	//数字量输入1
const IODef DInput2 	= {1, 19};	//数字量输入2
const IODef AInput 		= {0, 25};	//模拟量输入

const IODef AOutput1 	= {1, 18};	//AO_PWM1
const IODef AOutput2 	= {1, 20};	//AO_PWM2
const IODef AOutput3 	= {1, 21};	//AO_PWM3
const IODef AOutput4 	= {1, 23};	//AO_PWM4
const IODef AOutput5 	= {1, 24};	//AO_PWM5
const IODef AOutput6 	= {1, 26};	//AO_PWM6

const IODef DAC1LATCH   = {0, 5};	//DAC7760_LATCH
const IODef DAC1CLR	    = {0, 4};	//DAC7760_CLR
const IODef DAC1ALARM	= {4, 28};	//DAC7760_ALARM
const IODef DAC2LATCH   = {2, 5};	//DAC7760_LATCH
const IODef DAC2CLR	    = {2, 4};	//DAC7760_CLR
const IODef DAC2ALARM	= {2, 3};	//DAC7760_ALARM
const IODef SPI1MISO	= {0, 8};	//SPI1_MISO
const IODef SPI1MOSI	= {0, 9};	//SPI1_MOSI
const IODef SPI1SCLK	= {0, 7};	//SPI1_SCLK

const IODef DOutput1	= {0, 0};	//数字量输出1
const IODef DOutput2	= {0, 1};	//数字量输出2
const IODef DOutput3	= {0, 10};	//数字量输出3
const IODef DOutput4	= {0, 11};	//数字量输出4
const IODef DOutput5	= {2, 13};	//数字量输出5
const IODef DOutput6	= {2, 12};	//数字量输出6
const IODef DOutput7	= {0, 19};	//数字量输出7
const IODef DOutput8	= {0, 16};	//数字量输出8
const IODef DOutput9	= {2, 9};	//数字量输出9
const IODef DOutput10	= {2, 8};	//数字量输出10
const IODef DOutput11	= {2, 7};	//数字量输出11
const IODef DOutput12	= {2, 6};	//数字量输出12
const IODef DOutput13	= {2, 11};	//数字量输出13

const IODef QuickTest	= {1, 25};	//快速测试

const IODef ISP			= {2, 10};	//ISP烧写

const IODef SegSPI0MOSI	= {0, 18};	//数码管SPI0输出
const IODef	SegSPI0CLK	= {0, 15};	//数码管SPI0时钟

const IODef CAN1Tx		= {0, 22};	//CAN1Tx
const IODef CAN1Rx		= {0, 21};	//CAN1Rx
const IODef CAN1Inv		= {0, 20};	//CAN1Inv
const IODef CAN1Addr	= {4, 29};	//CAN1Addr

const IODef Uart1Tx		= {2, 0};	//Uart1Tx
const IODef Uart1Rx		= {2, 1};	//Uart1Rx
const IODef Uart1DE		= {0, 6};	//Uart1DE
const IODef Uart1Inv	= {2, 2};	//Uart1Inv
const IODef Uart0Tx		= {0, 2};	//Uart0Tx
const IODef Uart0Rx		= {0, 3};	//Uart0Rx
const IODef Uart0DE		= {5, 4};	//Uart0DE
const IODef Uart0Inv	= {0, 26};	//Uart0Inv

const IODef EthMDC			= {1, 16};
const IODef EthMDIO			= {1, 17};
const IODef EthRMIIRxER		= {1, 14};
const IODef EthRMIIRefClk 	= {1, 15};
const IODef EthRMIIRXD0		= {1, 9};
const IODef EthRMIIRXD1		= {1, 10};
const IODef EthRMIIRxEn		= {1, 4};
const IODef EthRMIICRS		= {1, 8};
const IODef EthRMIITXD0		= {1, 0};
const IODef EthRMIITXD1		= {1, 1};

