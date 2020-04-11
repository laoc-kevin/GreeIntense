#include "lpc_Pinsel.h"
#include "lpc_gpio.h"
#include "lpc_ssp.h"
#include "lpc_pwm.h"
#include "app_output.h"
#include "app_val.h"
#include "my_rtt_printf.h"


const IODef* DOList[DO_NUM]={ &DOutput1,
                              &DOutput2,
                              &DOutput3,
							  &DOutput4,
							  &DOutput5,
							  &DOutput6,
							  &DOutput7,
							  &DOutput8,
							  &DOutput9,
							  &DOutput10,
							  &DOutput11,
							  &DOutput12,
							  &DOutput13
                      };
			  
/**************************************************************
*变量声明
***************************************************************/
uint8_t *pDigitalOutput[DO_NUM] = {NULL};
sAOData pAnalogOutputData[AO_NUM] ; 
					  
/**************************************************************
*函数声明
***************************************************************/
static IODef AppDigitalIOBind( uint8_t Channel);
static void AppSetDAC7760IO(const IODef* cpsIO );
static void AppClrDAC7760IO(const IODef* cpsIO );
static void AppSendDataToDAC7760(uint8_t ucSuperIO, uint32_t ulData);
static void AppSetPWMData(uint8_t pwmChannel, uint32_t data);
static void AppOutputSet(void);

/**************************************************************
*@brief DO接口设置
***************************************************************/
void AppDigitalOutputConfig( uint8_t channel, uint8_t* value )
{
	if( (channel > 0) && (channel <=DO_NUM ) && (value != NULL))
	{
		pDigitalOutput[channel-1] = value;
	}
}

/**************************************************************
*@brief AO接口设置
***************************************************************/
void AppAnalogOutputConfig( uint8_t channel, int16_t min, int16_t max, void* value )
{
	if( (channel > 0) && (channel <= AO_NUM) && (value != NULL))
	{
		pAnalogOutputData[channel-1].Max  = max;
		pAnalogOutputData[channel-1].Min  = min;
		pAnalogOutputData[channel-1].OutputValue  = value;
	}
}


/**************************************************************
*@brief 数字量输出初始化程序
***************************************************************/
void AppDigitalOutputInit(void)
{
	uint8_t i = 0;
	uint32_t bitValue = 0;
	for(i = 0; i < DO_NUM; i++)
	{
		(void)PINSEL_ConfigPin( DOList[i]->Port, DOList[i]->Pin, 0);

		bitValue = 1 << DOList[i]->Pin;
		GPIO_SetDir( DOList[i]->Port, bitValue, 1 );
		GPIO_ClearValue( DOList[i]->Port, bitValue );
//		GPIO_SetValue( DOList[i]->Port, bitValue );
	}
}

/***************************************************
*@brief  数字量输出接口绑定
*@param  Channel    通道，DO1~DO13
								
***************************************************/
static IODef AppDigitalIOBind( uint8_t Channel)
{
	IODef cpsIO ;
	if ( (Channel > 0) && (Channel <=DO_NUM ) )
	{
		switch ( Channel )
		{
			case 1:
				cpsIO = DOutput1;
			break;
			case 2:
				cpsIO = DOutput2;
			break;
			case 3:
				cpsIO = DOutput3;
			break;
			case 4:
				cpsIO = DOutput4;
			break;
			case 5:
				cpsIO = DOutput5;
			break;
			case 6:
				cpsIO = DOutput6;
			break;
			case 7:
				cpsIO = DOutput7;
			break;
			case 8:
				cpsIO = DOutput8;
			break;
			case 9:
				cpsIO = DOutput9;
			break;
			case 10:
				cpsIO = DOutput10;
			break;
			case 11:
				cpsIO = DOutput11;
			break;
			case 12:
				cpsIO = DOutput12;
			break;
			case 13:
				cpsIO = DOutput13;
			break;
			default:break;
		}
	}	
	return cpsIO;
}
/***************************************************
*@brief  数字量输出函数
*@param  Channel    通道，DO1~DO13
*@param  eCtrl      IO输出控制： 0：off   1：on  								
***************************************************/
void AppDigitalOutputCtrl( uint8_t Channel, CtrlEn eCtrl)
{
	uint32_t ulPortValue = 0;
	IODef cpsIO ;
	cpsIO = AppDigitalIOBind(Channel );
	
	ulPortValue = GPIO_ReadValue(cpsIO.Port);

	if ( eCtrl > 0 )
	{
		if( ((ulPortValue >> cpsIO.Pin) & 1) == 0 )
        {
			GPIO_SetValue(cpsIO.Port, 1 << cpsIO.Pin );
			myprintf("GPIO_SetValue Port: %d   Pin: %d\n",cpsIO.Port,cpsIO.Pin);
		}			
	}
	else
	{
		if( ((ulPortValue >> cpsIO.Pin) & 1 ) == 1 )
        {	
			 GPIO_ClearValue(cpsIO.Port, 1 << cpsIO.Pin );	
			myprintf("GPIO_ClearValue Port: %d   Pin: %d\n",cpsIO.Port,cpsIO.Pin);
			
		}			
	}
}

/***************************************************
*@brief 数字量输出取反
*@param  Channel    通道，DO1~DO13
***************************************************/
void AppDigitalOutputToggle( uint8_t Channel )
{
	uint32_t ulMask;
	CtrlEn eCtrl;
    
	IODef cpsIO ;
	cpsIO = AppDigitalIOBind(Channel);
	
	ulMask = 1 << cpsIO.Pin;
	eCtrl = (CtrlEn)( (GPIO_ReadValue(cpsIO.Port) & ulMask) >> cpsIO.Pin );
	
	if( eCtrl )
	{
		AppDigitalOutputCtrl(Channel, 0);
	}
	else
	{
		AppDigitalOutputCtrl(Channel, 1);
	}
}

/***************************************************
*@brief DAC7760的管脚置位
*@param cpsIO	控制器数字量输出I/O
*								
***************************************************/
static void AppSetDAC7760IO(const IODef* cpsIO )
{
	GPIO_SetValue(cpsIO->Port, 1 << cpsIO->Pin);
}

/***************************************************
*@brief DAC7760的管脚清零
*@param cpsIO	控制器数字量输出I/O								
***************************************************/
static void AppClrDAC7760IO(const IODef* cpsIO )
{
	GPIO_ClearValue(cpsIO->Port, 1 << cpsIO->Pin);
}

/***************************************************
*@brief DAC7760初始化配置
*@author		laoc
*@date			2019-02-17							
***************************************************/
void AppDAC7760Init(void)
{
	uint8_t n;
	OS_ERR err = OS_ERR_NONE;
	
	SSP_CFG_Type SSP1_ConfigStruct = {SSP_DATABIT_8, 0, 0, 0, SSP_FRAME_SPI, 1000000};
	GPIO_Init();
	
	 //初始化SPI1
	(void)PINSEL_ConfigPin( SPI1SCLK.Port, SPI1SCLK.Pin, 2);				
	(void)PINSEL_ConfigPin( SPI1MISO.Port, SPI1MISO.Pin, 2);
	(void)PINSEL_ConfigPin( SPI1MOSI.Port, SPI1MOSI.Pin, 2);
	
	//寄存器初始化
	
	SSP_ConfigStructInit(&SSP1_ConfigStruct);	//默认设置
	SSP1_ConfigStruct.ClockRate = 3000000;
	SSP_Init(LPC_SSP1, &SSP1_ConfigStruct);
	SSP_Cmd(LPC_SSP1, ENABLE);
		
#if USE_SUPER_IO_1 > 0
	(void)PINSEL_ConfigPin( DAC1ALARM.Port, DAC1ALARM.Pin, 0);		//该管脚在DAC7760报警时变为低电平
	GPIO_SetDir( DAC1ALARM.Port, 1 << DAC1ALARM.Pin, 0);
	GPIO_SetValue( DAC1ALARM.Port, 1<<DAC1ALARM.Pin);

	(void)PINSEL_ConfigPin( DAC1CLR.Port, DAC1CLR.Pin, 0);			//该管脚为高电平时将清空DAC7760的输入，即CPU输出到DAC7760的数据
	GPIO_SetDir( DAC1CLR.Port, 1 << DAC1CLR.Pin, 1);
	GPIO_ClearValue( DAC1CLR.Port, 1 << DAC1CLR.Pin);

	(void)PINSEL_ConfigPin( DAC1LATCH.Port, DAC1LATCH.Pin, 0);		//该管脚出现上升沿时会将DAC7760的输入数据转为输出数据，即更新DAC7760的输出值
	GPIO_SetDir( DAC1LATCH.Port, 1 << DAC1LATCH.Pin, 1);
	GPIO_ClearValue( DAC1LATCH.Port, 1<< DAC1LATCH.Pin);
	
	AppSendDataToDAC7760(1, DAC7760_REG_CFG);                       //写配置寄存器，配置为电流输出
	AppSendDataToDAC7760(1, Iout4To20 | DAC7760_REG_OUT);           //写控制寄存器  并配置输出量程为4~20mA
	AppSendDataToDAC7760(1, DAC7760_REG_DATA);                      //输出值置0  
	
#endif
	
#if USE_SUPER_IO_2 > 0
	(void)PINSEL_ConfigPin( DAC2ALARM.Port, DAC2ALARM.Pin, 0);		//该管脚在DAC7760报警时变为低电平
	GPIO_SetDir( DAC2ALARM.Port, 1 << DAC2ALARM.Pin, 0);
	GPIO_SetValue( DAC2ALARM.Port, 1<< DAC2ALARM.Pin);

	(void)PINSEL_ConfigPin( DAC2CLR.Port, DAC2CLR.Pin, 0);			//该管脚为高电平时将清空DAC7760的输入，即CPU输出到DAC7760的数据
	GPIO_SetDir( DAC2CLR.Port, 1 << DAC2CLR.Pin, 1);
	GPIO_ClearValue( DAC2CLR.Port, 1 << DAC2CLR.Pin);

	(void)PINSEL_ConfigPin( DAC2LATCH.Port, DAC2LATCH.Pin, 0);		//该管脚出现上升沿时会将DAC7760的输入数据转为输出数据，即更新DAC7760的输出值
	GPIO_SetDir( DAC2LATCH.Port, 1 << DAC2LATCH.Pin, 1);
	GPIO_ClearValue( DAC2LATCH.Port, 1 << DAC2LATCH.Pin);
	
	AppSendDataToDAC7760(2, DAC7760_REG_CFG);
	AppSendDataToDAC7760(2, Iout4To20 | DAC7760_REG_OUT);
	AppSendDataToDAC7760(2, DAC7760_REG_DATA);
#endif

	while( (LPC_SSP1->SR == 0) && ( n < 10 ))
	{
//	    (void)OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
//		n++;    
	}
}

void MyDelay(int ticks);
void MyDelay(int ticks)
{
	while(ticks > 0)
	{
		ticks--;
	}
}

/*************************************************************************
*@brief	  超级IO模拟量输出数据发送函数	原理：根据DAC7760芯片的时序图来进行设置
*@param	  ucSuperIO 	使用的超级IO口，1或者2
*@return  ulData	    数据	格式：1字节寄存器地址+2字节数据
*@author  laoc
*@date	  2019-02-17
***************************************************************************/
static void AppSendDataToDAC7760(uint8_t ucSuperIO, uint32_t ulData)
{
	uint8_t i, n;
	OS_ERR err = OS_ERR_NONE;
	
	uint8_t ucSpidata[3];
	ucSpidata[2] = ulData & 0xFF;
	ucSpidata[1] = (ulData >> 8) & 0xFF;
	ucSpidata[0] = (ulData >> 16) & 0xFF;
	
	if( ucSuperIO == 1)
	{
		SET_VALUE_DAC7760_CLR1;												//CLR管脚置高电平，清空7760输入								
		SET_VALUE_DAC7760_LATCH1;											//LATCH管脚置高电平
		MyDelay(20);														//延时保证DAC7760输出清零
		
		CLR_VALUE_DAC7760_CLR1;												//CLR管脚置低电平，以便输入新的数据
		MyDelay(20);
		
		for(i = 0; i <= 2; i++)
		{
			SSP_SendData(LPC_SSP1, ucSpidata[i]);
		}

		/* 等待数据发送完毕 */
		n = 0;
		while( ((LPC_SSP1->SR & 0x01) == 0) && ( n < 10 ))
        {
//		    (void)OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
//		     n++;    
		}								//等待数据发送完毕
		CLR_VALUE_DAC7760_LATCH1;											//置LATCH为低电平
		MyDelay(50);
			
		SET_VALUE_DAC7760_LATCH1;											//产生上升沿，将发送到DAC7760的数据转化为输出
		MyDelay(50);
	}
	
	else if( ucSuperIO == 2 )
	{
		SET_VALUE_DAC7760_CLR2;
		SET_VALUE_DAC7760_LATCH2;
		MyDelay(20);
		
		CLR_VALUE_DAC7760_CLR2;
		MyDelay(20);
		
		for(i = 0; i <= 2; i++)
		{
			SSP_SendData(LPC_SSP1, ucSpidata[i]);
		}
		
		/* 等待数据发送完毕 */
		
		n = 0;
		while( ((LPC_SSP1->SR & 0x01) == 0) && ( n < 10 ))
        {
//		    (void)OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
//		     n++;    
		}
		
		CLR_VALUE_DAC7760_LATCH2;
		MyDelay(50);
			
		SET_VALUE_DAC7760_LATCH2;
		MyDelay(50);
	}
}

/***************************************************
*@brief  设置超级IO电流值，电流单位uA     公式：Iout = 16000 * data / 4096 + 4000
*@param  ucSuperIO     使用的超级IO口，1或者2
*@param  uA	          设置的电流值
*@author laoc
*@date	 2019-02-17							
***************************************************/
void AppSetDAC7760OutputuA(uint8_t ucSuperIO, uint16_t uA)
{
	uint16_t ulDataToSet;
	ulDataToSet = (uA - 4000) * 4096 / 16000;
	
	if(ulDataToSet >= 4096)
	{
		ulDataToSet = 4095;
	}
	
	AppSendDataToDAC7760(ucSuperIO, DAC7760_REG_DATA | ulDataToSet << 4);
}


/***************************************************
*@brief  超级IO发送真实值 单位uA
*@param  ucSuperIO     使用的超级IO口，1或者2
*@param  min	       量程最小值
*@param  max	       量程最大值
*@param  realData	   实际值
*@author laoc
*@date	 2019-02-17							
***************************************************/
void AppSetDAC7760RealValue(uint8_t ucSuperIO, int16_t min, int16_t max, int16_t realData)
{
	uint16_t uA;
	uA = 16000 * (realData - min) / (max - min) + 4000;      //对应4~20mA的量程
	AppSetDAC7760OutputuA(ucSuperIO, uA);
}

/****************************************************
*@brief  初始化PWM1，普通4~20mA
*@author laoc
*@date	 2019-02-17							
***************************************************/
void AppPWM1Init(void)
{
	uint8_t pwmChannel = 0;
	PWM_TIMERCFG_Type PWMTimerCfg = {PWM_TIMER_PRESCALE_TICKVAL, {0,0,0}, 1};
	PWM_MATCHCFG_Type PWMMatchCfg;
	
	PWMTimerCfg.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
	PWMTimerCfg.PrescaleValue = 1;
	PWM_Init(1, PWM_MODE_TIMER, (void *)&PWMTimerCfg);
	
	//管脚功能配置
	(void)PINSEL_ConfigPin(AOutput1.Port, AOutput1.Pin, 2);
	(void)PINSEL_ConfigPin(AOutput2.Port, AOutput2.Pin, 2);
	(void)PINSEL_ConfigPin(AOutput3.Port, AOutput3.Pin, 2);
	(void)PINSEL_ConfigPin(AOutput4.Port, AOutput4.Pin, 2);
	(void)PINSEL_ConfigPin(AOutput5.Port, AOutput5.Pin, 2);
	(void)PINSEL_ConfigPin(AOutput6.Port, AOutput6.Pin, 2);
	
	PWM_MatchUpdate(1, 0, 200, PWM_MATCH_UPDATE_NOW);    //设置MR0的值为200
	PWMMatchCfg.MatchChannel = 0;
	PWMMatchCfg.IntOnMatch = DISABLE;
	PWMMatchCfg.ResetOnMatch = ENABLE;                   //复位
	PWMMatchCfg.StopOnMatch = DISABLE;
	PWM_ConfigMatch(1, &PWMMatchCfg);
	
	//通道一默认为单沿，且不能设置为双沿，所以不用配置
	for (pwmChannel = 2; pwmChannel < 7; pwmChannel++)
	{
		PWM_ChannelConfig(1, pwmChannel, PWM_CHANNEL_SINGLE_EDGE);   //pwm单沿
	}
	
	for (pwmChannel = 1; pwmChannel < 7; pwmChannel++)               
	{
		PWM_MatchUpdate(1, pwmChannel, 0, PWM_MATCH_UPDATE_NOW);    //MRx值为0
		PWMMatchCfg.MatchChannel = pwmChannel;
		PWMMatchCfg.IntOnMatch = DISABLE;
		PWMMatchCfg.ResetOnMatch = DISABLE;              //不复位
		PWMMatchCfg.StopOnMatch = DISABLE;
		PWM_ConfigMatch(1, &PWMMatchCfg);
		
		PWM_ChannelCmd(1, pwmChannel, ENABLE);
	}
	PWM_ResetCounter(1);
	PWM_CounterCmd(1, ENABLE);
	PWM_Cmd(1, ENABLE);
}
	
/****************************************************
*@brief  PWM输出方波占波比 = data / 200    data 0~200 对应电流0~20mA
*@param  pwmChannel     pwm通道，1~6
*@param  data           通道匹配值
*@author laoc
*@date	 2019-02-17							
***************************************************/
static void AppSetPWMData(uint8_t pwmChannel, uint32_t data)
{
	PWM_MatchUpdate(1, pwmChannel, data, PWM_MATCH_UPDATE_NOW);
}

/***************************************************
*@brief  根据量程将真实的值转化为PWM输出 
*@param  pwmChannel    pwm通道，1~6
*@param  min	       量程最小值
*@param  max	       量程最大值
*@param  realData	   实际值
*@author laoc
*@date	 2019-02-17							
***************************************************/
void AppSetPWMRealValue(uint8_t pwmChannel, int16_t min, int16_t max, int16_t realData)
{
	uint32_t data;
	data = 160 * (realData - min) / (max - min) + 40;    //对应4~20mA的量程   20mA*10=200
	AppSetPWMData(pwmChannel, data);
}


/***************************************************
*@brief  根据量程将真实的值转化为输出 
*@param  Channel       通道，AO1~AO8
*@param  min	       量程最小值
*@param  max	       量程最大值
*@param  realData	   实际值
*@author laoc
*@date	 2019-02-17							
***************************************************/
void AppSetAORealValue(uint8_t channel, int16_t min, int16_t max, int16_t realData)
{
	if((channel > 0) && (channel <= AO_NUM))
	{
		if( channel <=6 )
		{
			AppSetPWMRealValue( channel, min, max, realData );
		}
		else
		{
			AppSetDAC7760RealValue( channel - 6, min, max, realData );
		}			
	}
}

/***************************************************
*@brief  根据量程将真实的值转化为输出 
*@param  Channel       通道，AO0~AO7
*@param  min	       量程最小值
*@param  max	       量程最大值
*@param  realData	   实际值
*@author laoc
*@date	 2019-02-17							
***************************************************/
void AppGetAORealValue(uint8_t channel, int16_t min, int16_t max, int16_t realData)
{
	if(channel < 8)
	{
		
	}
}

/******************************************************************
*@brief  修改AO通道量程
*@param  channel	AI通道  AO1~AO8
*@param  min	    量程最小值
*@param  max	    量程最大值
******************************************************************/
void AppSetAORange(uint8_t channel, int16_t ucMin, int16_t ucMax)
{
	if( (channel > 0) && (channel <= AO_NUM) )
	{
		pAnalogOutputData[channel].Max = ucMax;
		pAnalogOutputData[channel].Min = ucMin;
	}
}

/******************************************************************
*@brief 设置数字量和模拟量的输出								
******************************************************************/
static void AppOutputSet(void)
{
	uint8_t i;
	int16_t ucMin, ucMax;
	
	for(i = 0; i < 6; i++)      //AO0~AO5
	{	
		if(pAnalogOutputData[i].OutputValue  != NULL)
		{
			ucMax = pAnalogOutputData[i].Max;
	        ucMin = pAnalogOutputData[i].Min;
		    AppSetPWMRealValue( i + 1, ucMin, ucMax, *(int16_t*)pAnalogOutputData[i].OutputValue );	
		}
			
	}
	
	for(i = 0; i < 2; i++)     //超级IO   AO6和AO7
	{	
		if(pAnalogOutputData[i].OutputValue != NULL)
		{
			ucMax = pAnalogOutputData[i + 6].Max;
			ucMin = pAnalogOutputData[i + 6].Min;
			AppSetDAC7760RealValue( i + 1, ucMin, ucMax, *(int16_t*)pAnalogOutputData[i].OutputValue );
        }			
	}
	
	for(i = 0; i < DO_NUM; i++)
	{
		if( pDigitalOutput[i] != NULL)
		{
			if( (*(uint8_t*)pDigitalOutput[i]) > 0 )
			{
				AppDigitalOutputCtrl( i+1, 1 );
			}
			else
			{
				AppDigitalOutputCtrl( i+1, 0 );
			}
		}
	}
}

/******************************************************************
*@brief 输出设置任务函数							
******************************************************************/
void AppOutputSetTask(void *p_arg)
{
	OS_ERR err = OS_ERR_NONE;
	
	AppDigitalOutputInit();
	AppDAC7760Init();
	AppPWM1Init();
	
	while(DEF_TRUE)
	{
		AppOutputSet();
		OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}