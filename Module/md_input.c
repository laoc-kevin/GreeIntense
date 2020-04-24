#include "lpc_pinsel.h"
#include "lpc_gpio.h"
#include "lpc_adc.h"
#include "includes.h"
#include "md_input.h"
#include "my_rtt_printf.h"

#define AI_NUM 8
#define DI_NUM 15

//采样电流误差,单位uA*10，每个20个单位的采样偏差，400、420、440、、、
const int8_t DeltaUAList[81] = { 32,32,32,33,32,32,32,31,31,31,32,32,31,31,31,31,32,31,
								 31,30,30,29,31,31,30,30,29,29,28,29,29,29,28,28,27,29,
								 29,28,28,27,26,27,27,26,25,23,21,19,18,16,14,13,12,11,
								 13,12,11,11,11,10,11,11,10,9, 9, 8, 10, 8, 8, 7, 6, 6,
								 6, 6, 5, 3, 1, 0,-15,-42,-60};

const int16_t RealUAList[81] = { 432,452,472,493,512,532,552,571,591,
								 611,632,652,671,691,711,731,752,771,
								 791,810,830,849,871,891,910,930,949,
								 969,988,1009,1029,1049,1068,1088,1107,1129,
								 1149,1168,1188,1207,1226,1247,1267,1286,1305,
								 1323,1341,1359,1378,1396,1414,1433,1452,1471,
								 1493,1512,1531,1551,1571,1590,1611,1631,1650,
								 1669,1689,1708,1730,1748,1768,1787,1806,1826,
								 1846,1866,1885,1903,1921,1940,1945,1938,1940};

/***************************全局变量*************************************/
uint8_t ControllerID;           //控制器ID 通过拨码设置                           
uint8_t	SaInput;                //拨码值
								
uint8_t LogicInput1;            //DI接口临时存储
uint8_t	LogicInput2;            
								
int16_t AnalogInputuA[8];       //AI接口临时存储
                                
sAIData AnalogInputData[AI_NUM];   //AI接口数据                            
sDIData DigitalInputData[DI_NUM];  //DI接口数据
	
/*****************************************************************
*函数声明
******************************************************************/
static void     vInputIOInit(void);
                
static uint32_t ulAnalogInputSampling(void);
static int16_t  sAnalogInputCalibrateuA(int16_t s10_uA);
static int16_t  sAnalogInputAnalogToUA(uint32_t ulSample);
                
static uint8_t  ucInputGet4051Channel(void);
static void     vInputSet4051Channel(uint8_t ucChannel);
                
static void     vInputReceive(void);
static uint32_t ulAnalogInputConvertToReal(int32_t lMin, int32_t lMax, int16_t s10_uA);

static int16_t  sAnalogInputGetuA(uint8_t ucChannel);
static uint8_t  ucDigitalInputGetBit(uint8_t ucDACNum, uint8_t ucBit);

static uint8_t  ucSaInputConvertToID(void);

/**************************************************************
*@brief DI接口变量注册
***************************************************************/
void vDigitalInputRegister(uint8_t ucChannel, uint8_t* pvVal)
{
	if( (ucChannel > 0) && (ucChannel <= DI_NUM) && (pvVal != NULL))
	{
		DigitalInputData[ucChannel-1].pvDIVal = pvVal;
	}
}

/**************************************************************
*@brief AI接口变量注册
***************************************************************/
void vAnalogInputRegister(uint8_t ucChannel, int32_t lMin, int32_t lMax, void* pvVal)
{
	if( (ucChannel > 0) && (ucChannel <= AI_NUM) && (pvVal != NULL))
	{
		AnalogInputData[ucChannel-1].lMax    = lMax;
		AnalogInputData[ucChannel-1].lMin    = lMin;
		AnalogInputData[ucChannel-1].pvAIVal = pvVal;
	}
}

/******************************************************************
*@brief 输入量所有管脚初始化								
******************************************************************/
static void vInputIOInit(void)
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
static uint32_t ulAnalogInputSampling(void)
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
		ADC_StartCmd(LPC_ADC, ADC_START_NOW);
		
		while ( (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_2, ADC_DATA_DONE)) == 0)
		{
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
*@brief  模拟量输入转换成实际的电流值，单位为10uA  
*@return 实际电流值								
******************************************************************/
static int16_t  sAnalogInputToUA(uint32_t ulSample)
{
	uint32_t ulInput_V = 0;	   //参考电压
	int16_t  s10_uA = 0;
    
	ulInput_V = 100 * 3300 * ulSample / (1<<12);  //12位采样 3.3v的参考电压  根据原理图计算  VInput / 3.3v  =  ulSample /4096
	s10_uA = (int16_t)(ulInput_V /150);            // uA = VInput /150Ω  * 1000000

	return s10_uA;
}

/******************************************************************
*@brief 获取当前输入通道							
******************************************************************/
static uint8_t  ucInputGet4051Channel(void)
{
	uint32_t ulIOValue = GPIO_ReadValue(1); 
	return (uint8_t)(ulIOValue>>27 & 0x07);		//注意，类型强转优先级比位移运算高
}

/******************************************************************
*@brief 设置当前输入通道，通过设置74HC4051芯片的选择管脚
*@param  channel	1~8
******************************************************************/
static void vInputSet4051Channel(uint8_t ucChannel)
{
	switch(ucChannel)
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
static void vInputReceive(void)
{
	int16_t i, n;
    
	uint8_t SaInputTmp=0; 
	uint8_t LogicInput1Tmp=0; 
	uint8_t LogicInput2Tmp = 0;
    
    uint32_t ucMax, ucMin;
	uint32_t AnalogInputTmp[8] = {0};
	
	OS_ERR err = OS_ERR_NONE;
	
	//将模拟输入转换为实际电流值
	for(i = 0; i < 8; i++)
	{
		n = 0;
		vInputSet4051Channel( i+1 );   //输入通道设置
		
		/* 等待4051设置完毕 */
		while( (ucInputGet4051Channel() != i) && (n < 10) )
        {
			(void)OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
			n++;
		};
			
		SaInputTmp |= (uint8_t)(GPIO_ReadValue(0)>>28 & 1) << i;
		LogicInput1Tmp |= (uint8_t)(GPIO_ReadValue(0)>>29 & 1) << i;
		LogicInput2Tmp |= (uint8_t)(GPIO_ReadValue(1)>>19 & 1) << i;
				
		AnalogInputTmp[i] = ulAnalogInputSampling();			
		AnalogInputuA[i] = sAnalogInputToUA(AnalogInputTmp[i]);		
	}
	
	SaInput = SaInputTmp;
	LogicInput1 = LogicInput1Tmp;
	LogicInput2 = LogicInput2Tmp;
	
	for(i = 0; i < AI_NUM; i++)
	{
		if( AnalogInputData[i].pvAIVal != NULL )
		{
			*((int32_t*)(AnalogInputData[i].pvAIVal)) = ulAnalogInputGetRealVal(i+1);
		}	
	}
	for(i = 0; i < DI_NUM; i++)
	{
		if( DigitalInputData[i].pvDIVal !=NULL )
		{
			*(uint8_t*)(DigitalInputData[i].pvDIVal) = ucDigitalInputGetRealVal(i+1);
		}
    }
    ControllerID = ucSaInputConvertToID();
}

/******************************************************************
*@brief  校准电流
*@param  uA	电流值 单位10uA
*@return 实际电流值								
******************************************************************/
static int16_t sAnalogInputCalibrateuA(int16_t us10_uA)
{
	uint8_t i = 0;
	int16_t delta = 0;
	int16_t deltaMin = 0;
	uint8_t deltaMinIndex = 0;

	if (us10_uA > RealUAList[0])
	{
		deltaMin = us10_uA - RealUAList[0];
	}
	else
	{
		deltaMin = RealUAList[0] - us10_uA;
	}
	for (i = 0; i < 81; i++)
	{
		if (us10_uA > RealUAList[i])
		{
			delta = us10_uA - RealUAList[i];
		}
		else
		{
			delta = RealUAList[i] - us10_uA;
		}
		
		if(deltaMin > delta)
		{
			deltaMin = delta;
			deltaMinIndex = i;
		}
	}
	return (us10_uA - DeltaUAList[deltaMinIndex]);
}

/******************************************************************
*@brief 将AI输入转换成实际值*10
*@param  min	量程最小值 = 实际值*10
*@param  max	量程最大值 = 实际值*10
*@param  uA	    电流值  单位为10uA
*@return 实际值	
******************************************************************/
uint32_t ulInputConvertToReal(int32_t lMin, int32_t lMax, int16_t s10uA)
{
	int16_t  s10uA_C = 0;
	uint32_t ulReal = 0;

	s10uA_C = sAnalogInputCalibrateuA(s10uA);
	if (s10uA_C >= 400)
	{
		ulReal = (s10uA_C - 400) * (lMax - lMin) / (2000 - 400) + lMin;  //对应4~20mA
	}
	else
	{
		ulReal = 0;
	}
	return ulReal;
}

/******************************************************************
*@brief 提取AI的电流值 
*@param  channel	  AI通道  A1~A8
*@return 实际电流值   单位为10uA
******************************************************************/
int16_t sAnalogInputGetuA(uint8_t ucChannel)
{
	int16_t sReal10_uA = 0;
	
	if((ucChannel > 0) && (ucChannel <= AI_NUM))
	{
        /*AI的定义与原理图不是一一对应，需参照原理图*/
		switch(ucChannel)
		{
			case 1:			
				sReal10_uA = AnalogInputuA[2];
			     break;
			case 2:	
				sReal10_uA = AnalogInputuA[1];	
			    break;
			case 3:	
				sReal10_uA = AnalogInputuA[0];	
			    break;
			case 4:	
				sReal10_uA = AnalogInputuA[3];	
			    break;
			case 5:	
				sReal10_uA = AnalogInputuA[7];	
			    break;
			case 6:	
				sReal10_uA = AnalogInputuA[5];	
			    break;
			case 7:	
				sReal10_uA = AnalogInputuA[4];	
			    break;
			case 8:	
				sReal10_uA = AnalogInputuA[6];	
			    break;
			default:break;
		}	
	}
	return sReal10_uA;
}

/******************************************************************
*@brief 提取AI的实际值
*@param  channel	AI通道  AI1~AI8
*@param  min	    量程最小值
*@param  max	    量程最大值
*@return 实际值	
******************************************************************/
uint32_t ulAnalogInputGetRealVal(uint8_t ucChannel)
{
	uint32_t ulRealValue = 0;
	
	if((ucChannel > 0) && (ucChannel <= AI_NUM))
	{
        uint32_t lMin = AnalogInputData[ucChannel-1].lMin;
        uint32_t lMax = AnalogInputData[ucChannel-1].lMax;
        
        ulRealValue = ulInputConvertToReal(lMin, lMax, sAnalogInputGetuA(ucChannel));	
	}
	return ulRealValue;
}

/******************************************************************
*@brief  修改AI通道量程
*@param  channel	AI通道  AI1~AI8
*@param  min	    量程最小值
*@param  max	    量程最大值

******************************************************************/
void vAnalogInputSetRange(uint8_t ucChannel, int32_t lMin, int32_t lMax)
{
	if( (ucChannel > 0) && (ucChannel <= AI_NUM)  )
	{
		AnalogInputData[ucChannel-1].lMax = lMax;
		AnalogInputData[ucChannel-1].lMin = lMin;
	}
}

/******************************************************************
*@brief 提取DI的值
*@param  ucDACNum	    DAC通道  1或2
*@param  ucBit	        偏移
*@return 实际值	
******************************************************************/
uint8_t ucDigitalInputGetBit(uint8_t ucDACNum, uint8_t ucOffsetBit)
{
	if (ucDACNum == 1)
	{
		return (LogicInput1 >> ucOffsetBit) & 0x01;
	}
	else
	{
		return (LogicInput2 >> ucOffsetBit) & 0x01;
	}
}

/******************************************************************
*@brief 提取DI实际值
*@param  channel	DI通道  D1~D15
******************************************************************/
uint8_t ucDigitalInputGetRealVal(uint8_t ucChannel)
{
	uint8_t ucBit = 0;
	
	if( (ucChannel > 0) && (ucChannel <= DI_NUM) )
	{
		switch (ucChannel)
		{
			case 1:
				ucBit = ucDigitalInputGetBit(2, 4);
			    break;
			case 2:
				ucBit = ucDigitalInputGetBit(2, 6);
			    break;
			case 3:
				ucBit = ucDigitalInputGetBit(2, 2);
			    break;
			case 4:
				ucBit = ucDigitalInputGetBit(2, 1);
			    break;
			case 5:
				ucBit = ucDigitalInputGetBit(2, 0);
			    break;
			case 6:
				ucBit = ucDigitalInputGetBit(2, 3);
			    break;
			case 7:
				ucBit = ucDigitalInputGetBit(2, 5);
			    break;
			case 8:
				ucBit = ucDigitalInputGetBit(1, 4);
			    break;
			case 9:
				ucBit = ucDigitalInputGetBit(1, 6);
			    break;
			case 10:
				ucBit = ucDigitalInputGetBit(1, 2);
			    break;
			case 11:
				ucBit = ucDigitalInputGetBit(1, 1);
			    break;
			case 12:
				ucBit = ucDigitalInputGetBit(1, 0);
			    break;
			case 13:
				ucBit = ucDigitalInputGetBit(1, 3);
			    break;
			case 14:
				ucBit = ucDigitalInputGetBit(1, 7);
			    break;
			case 15:
				ucBit = ucDigitalInputGetBit(1, 5);
			    break;
			default:break;
		}
	}
    return ucBit;
}

/******************************************************************
*@brief 提取拨码值								
******************************************************************/
uint8_t ucGetSaInput(void)
{
	return SaInput;	
}

uint8_t ucGetControllerID(void)
{
    return ControllerID;
}

uint8_t ucSaInputConvertToID(void)
{
	return (~(((SaInput & 1) << 4) | ((SaInput & 2) >> 1) | ((SaInput & 4) >> 1) | (SaInput & 8) | ((SaInput & 16) >> 2))) & 31;
}

/******************************************************************
*@brief 读取输入量任务函数							
******************************************************************/
void vInputReceiveTask(void *p_arg)
{
	OS_ERR err = OS_ERR_NONE;
	
	vInputIOInit();
	while(DEF_TRUE)
	{
		vInputReceive();	
		OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}
