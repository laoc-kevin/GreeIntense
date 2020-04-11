#include "lpc_pinsel.h"
#include "lpc_gpio.h"
#include "lpc_adc.h"
#include "includes.h"
#include "app_input.h"
#include "my_rtt_printf.h"
//采样电流误差,单位uA*10，每个20个单位的采样偏差，400、420、440、、、
const int8_t DeltaUAList[81] = {	32,32,32,33,32,32,32,31,31,31,32,32,31,31,31,31,32,31,
								31,30,30,29,31,31,30,30,29,29,28,29,29,29,28,28,27,29,
								29,28,28,27,26,27,27,26,25,23,21,19,18,16,14,13,12,11,
								13,12,11,11,11,10,11,11,10,9, 9, 8, 10, 8, 8, 7, 6, 6,
								6, 6, 5, 3, 1, 0,-15,-42,-60};
const int16_t RealUAList[81] = {	432,452,472,493,512,532,552,571,591,
								611,632,652,671,691,711,731,752,771,
								791,810,830,849,871,891,910,930,949,
								969,988,1009,1029,1049,1068,1088,1107,1129,
								1149,1168,1188,1207,1226,1247,1267,1286,1305,
								1323,1341,1359,1378,1396,1414,1433,1452,1471,
								1493,1512,1531,1551,1571,1590,1611,1631,1650,
								1669,1689,1708,1730,1748,1768,1787,1806,1826,
								1846,1866,1885,1903,1921,1940,1945,1938,1940};

/***************************全局变量*************************************/
uint8_t ControllerID;               /* 控制器ID 通过拨码设置*/
                                
uint8_t	SaInput;
								
uint8_t LogicInput1;
uint8_t	LogicInput2;
								
int16_t AnalogInputuA[8];

sAIData pAnalogInputData[8] ; 
uint8_t *pDigitalInputData[16] = {NULL};
								
/*****************************************************************
*函数声明
******************************************************************/
static void AppInputIOInit(void);
static uint32_t AppInputAnalogSampling(void);
static int16_t AppInputCalibrateuA(int16_t uA);
static int16_t AppInputAnalogToUA(uint32_t Input);
static uint8_t AppInputGet4051Channel(void);
static void AppInputSet4051Channel(uint8_t Channel);
static void AppInputReceive(void);

/**************************************************************
*@brief DI接口设置
***************************************************************/
void AppDigitalInputConfig( uint8_t channel, uint8_t* value )
{
	if( (channel > 0) && (channel <= 15) && (value != NULL))
	{
		pDigitalInputData[channel-1] = value;
	}
}

/**************************************************************
*@brief AI接口设置
***************************************************************/
void AppAnalogInputConfig( uint8_t channel, int16_t min, int16_t max, void* value )
{
	
	if( (channel > 0) && (channel <= 8) && (value != NULL))
	{
		pAnalogInputData[channel-1].Max = max;
		pAnalogInputData[channel-1].Min = min;
		pAnalogInputData[channel-1].InputValue = value;
	}
}

/******************************************************************
*@brief 输入量所有管脚初始化								
******************************************************************/
static void AppInputIOInit(void)
{
	GPIO_Init();
	
	//设置管脚功能，4051A---P1.27，4051B---P1.28，4051C---P1.29  两块4051共用A、B、C管脚
	
	//拨码数字量输入P0.28，数字输入1P0.29，数字输入2P1.19，4~20mA模拟输入P0.25
	(void)PINSEL_ConfigPin(1, 27, 0);
	(void)PINSEL_ConfigPin(1, 28, 0);
	(void)PINSEL_ConfigPin(1, 29, 0);
	
	(void)PINSEL_ConfigPin(0, 28, 0);
	(void)PINSEL_ConfigPin(0, 29, 0);
	(void)PINSEL_ConfigPin(1, 19, 0);
	
	(void)PINSEL_SetAnalogPinMode(0, 25, ENABLE);
	(void)PINSEL_ConfigPin(0, 25, 1);
	
	//设置管脚输入输出模式
	GPIO_SetDir(1, 1<<27, 1);
	GPIO_SetDir(1, 1<<28, 1);
	GPIO_SetDir(1, 1<<29, 1);
	
	GPIO_SetDir(0, 1<<28, 0);
	GPIO_SetDir(0, 1<<29|1<<30, 0); //P0.29和P0.30两个管脚共享方向寄存器
	GPIO_SetDir(1, 1<<19, 0); 
	
	GPIO_SetDir(1, 1<<25, 0); 		//快测口
	
	//设置输出管脚初始为低电平
	GPIO_ClearValue(1, 1<<27 | 1<<28 | 1<<29);
	
	//ADC初始化
	ADC_Init(LPC_ADC, 200000);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);	
}

/******************************************************************
*@brief  模拟输入采样函数，采7次，去除最大、最小值取平均	
*@return 采样电流值
******************************************************************/
static uint32_t AppInputAnalogSampling(void)
{
	uint8_t i, n;
	uint32_t hits[7];											//存取7次采样值
	uint32_t max = 0;
	uint32_t min = 0;
	uint32_t sum = 0;
	OS_ERR err = OS_ERR_NONE;
	
	//连续采样7次，并求其中的最大值和最小值
	for(i = 0; i < 7; i++)
	{
		n = 0;
		
		ADC_StartCmd(LPC_ADC, ADC_START_NOW);
		
		while ( ((ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_2, ADC_DATA_DONE)) == 0) &&  (n < 10) )
		{
//			(void)OSTimeDlyHMSM(0, 0, 0, 1, OS_OPT_TIME_HMSM_STRICT, &err);
//			n++;
			//等待数据获取完毕
		}
		hits[i] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);
		if(i > 0)
		{
			if(1 == i)
			{
				max = hits[i];
				min = hits[i];
			}
			if(max < hits[i])
			{
				max = hits[i];									//求最大值
			}
			if(min > hits[i])
			{
				min = hits[i];									//求最小值
			}
		}
	}
	for(i = 1; i < 7; i++)	//第一次采样值不要
	{
		sum += hits[i];
	}
	return (sum - max - min) / 4;
}

/******************************************************************
*@brief  模拟量输入转换成实际的电流值，单位为100uA
*@return 实际电流值								
******************************************************************/
static int16_t AppInputAnalogToUA(uint32_t Input)
{
	uint32_t VInput = 0;	//输入电压
	int16_t uA = 0;
	VInput = 100 * 3300 * Input / (0xFFF);  //12位采样 3.3v的参考电压  根据原理图计算
	uA = (int16_t)(VInput /150);

	return uA;
}

/******************************************************************
*@brief 获取当前输入通道							
******************************************************************/
static uint8_t AppInputGet4051Channel(void)
{
	uint32_t IOValve;
	IOValve = GPIO_ReadValue(1);
	return (uint8_t)(IOValve>>27 & 0x07);		//注意，类型强转优先级比位移运算高
}

/******************************************************************
*@brief 设置当前输入通道，通过设置74HC4051芯片的选择管脚
*@param  channel	1~8
******************************************************************/
static void AppInputSet4051Channel(uint8_t Channel)
{
	switch(Channel)
	{
		case 1:
			GPIO_ClearValue(1, 0x07<<27);
		break;
		case 2:
			GPIO_ClearValue(1, 0x06<<27);
			GPIO_SetValue(1, 0x01<<27);
		break;
		case 3:
			GPIO_ClearValue(1, 0x05<<27);
			GPIO_SetValue(1, 0x02<<27);
		break;
		case 4:
			GPIO_ClearValue(1, 0x04<<27);
			GPIO_SetValue(1, 0x03<<27);
		break;
		case 5:
			GPIO_ClearValue(1, 0x03<<27);
			GPIO_SetValue(1, 0x04<<27);
		break;
		case 6:
			GPIO_ClearValue(1, 0x02<<27);
			GPIO_SetValue(1, 0x05<<27);
		break;
		case 7:
			GPIO_ClearValue(1, 0x01<<27);
			GPIO_SetValue(1, 0x06<<27);
		break;
		case 8:
			GPIO_SetValue(1, 0x07<<27);
		break;
		default:
			GPIO_ClearValue(1, 0x07<<27);
		break;
	}
}

/******************************************************************
*@brief 输入数据接收函数，包括拨码、数字量和模拟量								
******************************************************************/
static void AppInputReceive(void)
{
	int16_t i, ucMax, ucMin, n;
	uint8_t SaInputTmp=0; 
	uint8_t LogicInput1Tmp=0; 
	uint8_t LogicInput2Tmp = 0;
	uint32_t AnalogInputTmp[8] = {0};
	
	OS_ERR err = OS_ERR_NONE;
	
	//将模拟输入转换为实际电流值
	for(i = 0; i < 8; i++)
	{
		n = 0;
		AppInputSet4051Channel( i+1 );   //输入通道设置
		
		/* 等待4051设置完毕 */
		while( (AppInputGet4051Channel() != i) && ( n < 10 ) )
        {
			(void)OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
			n++;
		};
			
		SaInputTmp |= (uint8_t)(GPIO_ReadValue(0)>>28 & 1) << i;
		LogicInput1Tmp |= (uint8_t)(GPIO_ReadValue(0)>>29 & 1) << i;
		LogicInput2Tmp |= (uint8_t)(GPIO_ReadValue(1)>>19 & 1) << i;
				
		AnalogInputTmp[i] = AppInputAnalogSampling();			
		AnalogInputuA[i] = AppInputAnalogToUA(AnalogInputTmp[i]);		
	}
	
	SaInput = SaInputTmp;
	LogicInput1 = LogicInput1Tmp;
	LogicInput2 = LogicInput2Tmp;
	
	for(i = 0; i < 8; i++)
	{
		if( pAnalogInputData[i].InputValue  != NULL )
		{
			ucMax = pAnalogInputData[i].Max;
			ucMin = pAnalogInputData[i].Min;
			*((int16_t*)(pAnalogInputData[i].InputValue)) = AppGetAIRealValue( i+1, ucMin, ucMax);
		}	
	}
	
	for(i = 0; i < 15; i++)
	{
		if( pDigitalInputData[i] !=NULL )
		{
			*(uint8_t*)pDigitalInputData[i] = AppGetDIRealValue( i+1 );
		}
    }
}


/******************************************************************
*@brief  校准电流
*@param  uA	电流值
*@return 实际电流值								
******************************************************************/
static int16_t AppInputCalibrateuA(int16_t uA)
{
	uint8_t i = 0;
	int16_t delta = 0;
	int16_t deltaMin = 0;
	uint8_t deltaMinIndex = 0;

	if (uA > RealUAList[0])
	{
		deltaMin = uA - RealUAList[0];
	}
	else
	{
		deltaMin = RealUAList[0] - uA;
	}
	
	for (i = 0; i < 81; i++)
	{
		if (uA > RealUAList[i])
		{
			delta = uA - RealUAList[i];
		}
		else
		{
			delta = RealUAList[i] - uA;
		}
		
		if(deltaMin > delta)
		{
			deltaMin = delta;
			deltaMinIndex = i;
		}
	}

	return (uA - DeltaUAList[deltaMinIndex]);
}


/******************************************************************
*@brief 将AI输入转换成实际值*10
*@param  min	量程最小值
*@param  max	量程最大值
*@param  uA	    电流值
*@return 实际值	
******************************************************************/
int16_t AppInputConvertToReal(int16_t min, int16_t max, int16_t uA)
{
	int16_t uA_C = 0;
	int16_t real = 0;

	uA_C = AppInputCalibrateuA(uA);
	if (uA_C >= 400)
	{
		real = (uA_C - 400) * (max - min) / (2000 - 400) + min;  //对应4~20mA
	}
	else
	{
		real = 0;
	}
	return real;
}

/******************************************************************
*@brief 提取AI的电流值
*@param  channel	AI通道  A1~A8
*@return 实际电流值
******************************************************************/
int16_t AppGetAIuA(uint8_t channel)
{
	int16_t sRealuA = 0;
	
	if((channel > 0) && (channel <= 8))
	{
		switch(channel)
		{
			case 1:			
				sRealuA = AnalogInputuA[2];
			break;
				
			case 2:	
				sRealuA = AnalogInputuA[1];	
			break;
				
			case 3:	
				sRealuA = AnalogInputuA[0];	
			break;
			
			case 4:	
				sRealuA = AnalogInputuA[3];	
			break;
			
			case 5:	
				sRealuA = AnalogInputuA[7];	
			break;
			
			case 6:	
				sRealuA = AnalogInputuA[5];	
			break;
			
			case 7:	
				sRealuA = AnalogInputuA[4];	
			break;
			
			case 8:	
				sRealuA = AnalogInputuA[6];	
			break;
			default:break;
		}	
	}
	return sRealuA;
}

/******************************************************************
*@brief 提取AI的实际值
*@param  channel	AI通道  AI1~AI8
*@param  min	    量程最小值
*@param  max	    量程最大值
*@return 实际值	
******************************************************************/
int16_t AppGetAIRealValue(uint8_t channel, int16_t ucMin, int16_t ucMax)
{
	int16_t sRealValue = 0;
	
	if((channel > 0) && (channel <= 8))
	{
		/*AI的定义与原理图不是一一对应，需参照原理图*/
		switch(channel)
		{
			case 1:			
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[2]);
			break;
				
			case 2:	
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[1]);	
			break;
				
			case 3:	
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[0]);	
			break;
			
			case 4:	
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[3]);	
			break;
			
			case 5:	
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[7]);	
			break;
			
			case 6:	
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[5]);	
			break;
			
			case 7:	
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[4]);	
			break;
			
			case 8:	
				sRealValue = AppInputConvertToReal(ucMin, ucMax, AnalogInputuA[6]);	
			break;
			default:break;
		}	
	}
	return sRealValue;
}

/******************************************************************
*@brief  修改AI通道量程
*@param  channel	AI通道  AI1~AI8
*@param  min	    量程最小值
*@param  max	    量程最大值

******************************************************************/
void AppSetAIRange(uint8_t channel, int16_t ucMin, int16_t ucMax)
{
	if( (channel > 0) && (channel <= 8)  )
	{
		pAnalogInputData[channel-1].Max = ucMax;
		pAnalogInputData[channel-1].Min = ucMin;
	}
}

/******************************************************************
*@brief 提取DI的值
*@param  DACNum	    DAC通道  1或2
*@param  bit	    偏移
*@return 实际值	
******************************************************************/
uint8_t AppGetDIBit(uint8_t DACNum, uint8_t bit)
{
	uint8_t uBit;
	if (DACNum == 1)
	{
		uBit = (LogicInput1 >> bit) & 0x01;
	
	}
	else
	{
		uBit = (LogicInput2 >> bit) & 0x01;
		
	}
	
	if(uBit)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/******************************************************************
*@brief 提取DI的值
*@param  channel	DI通道  D1~D15
******************************************************************/
uint8_t AppGetDIRealValue(uint8_t channel)
{
	uint8_t uBit = 0;
	
	if ( (channel > 0) && (channel <= 15) )
	{
		switch (channel)
		{
			case 1:
				uBit = AppGetDIBit(2, 4);
			break;
			
			case 2:
				uBit = AppGetDIBit(2, 6);
			break;
			
			case 3:
				uBit = AppGetDIBit(2, 2);
			break;
			
			case 4:
				uBit = AppGetDIBit(2, 1);
			break;
			
			case 5:
				uBit = AppGetDIBit(2, 0);
			break;
			
			case 6:
				uBit = AppGetDIBit(2, 3);
			break;
			
			case 7:
				uBit = AppGetDIBit(2, 5);
			break;
			
			case 8:
				uBit = AppGetDIBit(1, 4);
			break;
			
			case 9:
				uBit = AppGetDIBit(1, 6);
			break;
			
			case 10:
				uBit = AppGetDIBit(1, 2);
			break;
			
			case 11:
				uBit = AppGetDIBit(1, 1);
			break;
			
			case 12:
				uBit = AppGetDIBit(1, 0);
			break;
			
			case 13:
				uBit = AppGetDIBit(1, 3);
			break;
			
			case 14:
				uBit = AppGetDIBit(1, 7);
			break;
			
			case 15:
				uBit = AppGetDIBit(1, 5);
			break;
			default:break;
		}
	}
    return uBit;
}

/******************************************************************
*@brief 提取拨码值								
******************************************************************/
uint8_t AppGetSaInput(void)
{
	return SaInput;	
}

/******************************************************************
*@brief 读取输入量任务函数							
******************************************************************/
void AppInputReceiveTask(void *p_arg)
{
	OS_ERR err = OS_ERR_NONE;
	
	AppInputIOInit();
	
	while(DEF_TRUE)
	{
		AppInputReceive();
		
		OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}



