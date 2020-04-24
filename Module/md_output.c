#include "lpc_Pinsel.h"
#include "lpc_gpio.h"
#include "lpc_ssp.h"
#include "lpc_pwm.h"
#include "md_output.h"
#include "app_val.h"
#include "my_rtt_printf.h"

#define DAC7760_REG_OUT      0x551000    //
#define DAC7760_REG_CFG      0x570000    //VOUT and IOUT terminals are tied together.
#define DAC7760_REG_DATA     0x010000    //

#define	USE_SUPER_IO_1		1
#define	USE_SUPER_IO_2		1

#define AO_NUM              8
#define DO_NUM              13
#define PWM_NUM             6

#define PWM_MATCH_VALUE     200          //PWM匹配值，对应20mA电流

#define SET_VALUE_DAC7760_CLR1			vSetDAC7760IO(&DAC1CLR)
#define	CLR_VALUE_DAC7760_CLR1			vClrDAC7760IO(&DAC1CLR)
#define	SET_VALUE_DAC7760_LATCH1		vSetDAC7760IO(&DAC1LATCH)
#define	CLR_VALUE_DAC7760_LATCH1		vClrDAC7760IO(&DAC1LATCH)
                                   
#define SET_VALUE_DAC7760_CLR2			vSetDAC7760IO(&DAC2CLR)
#define	CLR_VALUE_DAC7760_CLR2			vClrDAC7760IO(&DAC2CLR)
#define	SET_VALUE_DAC7760_LATCH2		vSetDAC7760IO(&DAC2LATCH)
#define	CLR_VALUE_DAC7760_LATCH2		vClrDAC7760IO(&DAC2LATCH)	

const IODef* DO_IOList[DO_NUM]={ &DOutput1, &DOutput2, &DOutput3, &DOutput4, &DOutput5, &DOutput6,
							     &DOutput7, &DOutput8, &DOutput9, &DOutput10, &DOutput11, &DOutput12,
							     &DOutput13
                               };

const IODef* PWM_IOList[PWM_NUM]={ &AOutput1, &AOutput2, &AOutput3, &AOutput4, &AOutput5, &AOutput6 };
                               		  
/**************************************************************
*变量声明
***************************************************************/
sDOData DigitalOutputData[DO_NUM];
sAOData AnalogOutputData[AO_NUM]; 
					  
/**************************************************************
*函数声明
***************************************************************/
static void  vDAC7760Init(void);
static void  vDigitalOutputInit(void);
static void  vPWM1Init(void);

static void  vSetDAC7760IO(const IODef* psIO);
static void  vClrDAC7760IO(const IODef* psIO);

static void  vSendDataToDAC7760(uint8_t ucSuperIO, uint32_t ulData);
static void  vSetPWMData(uint8_t pwmChannel, uint32_t ulData);
static void  vOutputSet(void);

static void vSetDAC7760OutputuA(uint8_t ucSuperIO, uint16_t uA);
static void vSetPWMRealVal(uint8_t pwmChannel, int32_t lMin, int32_t lMax, uint32_t ulRealData);
static void vSetDAC7760RealVal(uint8_t ucSuperIO, int32_t lMin, int32_t lMax, uint32_t ulRealData);


/**************************************************************
*@brief AO接口注册
***************************************************************/
void vAnalogOutputRegister(uint8_t ucChannel, int32_t lMin, int32_t lMax)
{
	if( (ucChannel > 0) && (ucChannel <= AO_NUM) )
	{
		AnalogOutputData[ucChannel-1].lMax    = lMax;
		AnalogOutputData[ucChannel-1].lMin    = lMin;
	}
}

/**************************************************************
*@brief 数字量输出初始化程序
***************************************************************/
void vDigitalOutputInit(void)
{
	uint8_t i = 0;
	uint32_t ulBitVal = 0;
    
	for(i = 0; i < DO_NUM; i++)
	{
		(void)PINSEL_ConfigPin( DO_IOList[i]->Port, DO_IOList[i]->Pin, 0);

		ulBitVal = 1 << DO_IOList[i]->Pin;
        
		GPIO_SetDir( DO_IOList[i]->Port, ulBitVal, 1 );
		GPIO_ClearValue( DO_IOList[i]->Port, ulBitVal );
	}
}

/***************************************************
*@brief  获取DO输出值 
*@param  Channel       通道，AO1~AO8
*@param  realData	   实际值
*@author laoc
*@date	 2019-02-17							
***************************************************/
eCtrlEn eGetDORealValue(uint8_t ucChannel)
{
	
}

/***************************************************
*@brief  数字量输出函数
*@param  Channel    通道，DO1~DO13
*@param  eCtrl      IO输出控制： 0：off   1：on  								
***************************************************/
void vDigitalOutputCtrl( uint8_t ucChannel, eCtrlEn eCtrl)
{
    if((ucChannel > 0) && (ucChannel <= AO_NUM))
    {
        const IODef*    psIO = DO_IOList[ucChannel-1];
	    uint32_t ulPortVal = GPIO_ReadValue(psIO->Port);
        
	    if(eCtrl > 0)
	    {
	    	if( ((ulPortVal >> psIO->Pin) & 0x01) == OFF )   
            {
	    		GPIO_SetValue(psIO->Port, 1 << psIO->Pin );    //输出开启，继电器闭合
	    		myprintf("GPIO_SetValue Port: %d   Pin: %d\n",psIO->Port, psIO->Pin);
	    	}			
	    }
	    else
	    {
	    	if( ((ulPortVal >> psIO->Pin) & 0x01) == ON )
            {	
	    		GPIO_ClearValue(psIO->Port, 1 << psIO->Pin );	//输出关闭，继电器断开
	    		myprintf("GPIO_ClearValue Port: %d   Pin: %d\n",psIO->Port, psIO->Pin);	
	    	}			
	    }
    }
}

/***************************************************
*@brief 数字量输出取反
*@param  Channel    通道，DO1~DO13
***************************************************/
void vDigitalOutputToggle( uint8_t ucChannel )
{
    if((ucChannel > 0) && (ucChannel <= AO_NUM))
    {
        const IODef*  psIO = DO_IOList[ucChannel-1];
	    uint32_t    ulMask = 1 << psIO->Pin;  
        eCtrlEn      eCtrl = ( (GPIO_ReadValue(psIO->Port) & ulMask) >> psIO->Pin ) & 0x01;
	   
        if(eCtrl)
	    {
	    	vDigitalOutputCtrl(ucChannel, OFF);    //输出关闭，继电器断开
	    }
	    else
	    {
	    	vDigitalOutputCtrl(ucChannel, ON);     //输出开启，继电器闭合
	    }
    }
}

/***************************************************
*@brief DAC7760的管脚置位
*@param psIO	控制器数字量输出I/O
*								
***************************************************/
static void vSetDAC7760IO(const IODef* psIO )
{
	GPIO_SetValue(psIO->Port, 1 << psIO->Pin);
}

/***************************************************
*@brief DAC7760的管脚清零
*@param psIO	控制器数字量输出I/O								
***************************************************/
static void vClrDAC7760IO(const IODef* psIO )
{
	GPIO_ClearValue(psIO->Port, 1 << psIO->Pin);
}

/***************************************************
*@brief DAC7760初始化配置
*@author		laoc
*@date			2019-02-17							
***************************************************/
void vDAC7760Init(void)
{
	uint8_t n;
	OS_ERR err = OS_ERR_NONE;
	
	SSP_CFG_Type SSP1_ConfigStruct = {SSP_DATABIT_8, 0, 0, 0, SSP_FRAME_SPI, 1000000};
	GPIO_Init();
	
	 //初始化SPI1
	(void)PINSEL_ConfigPin(SPI1SCLK.Port, SPI1SCLK.Pin, 2);				
	(void)PINSEL_ConfigPin(SPI1MISO.Port, SPI1MISO.Pin, 2);
	(void)PINSEL_ConfigPin(SPI1MOSI.Port, SPI1MOSI.Pin, 2);
	
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
	
	vSendDataToDAC7760(1, DAC7760_REG_CFG);                       //写配置寄存器，配置为电流输出
	vSendDataToDAC7760(1, Iout4To20 | DAC7760_REG_OUT);           //写控制寄存器  并配置输出量程为4~20mA
	vSendDataToDAC7760(1, DAC7760_REG_DATA);                      //输出值置0  
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
	
	vSendDataToDAC7760(2, DAC7760_REG_CFG);
	vSendDataToDAC7760(2, Iout4To20 | DAC7760_REG_OUT);
	vSendDataToDAC7760(2, DAC7760_REG_DATA);
#endif

	while(LPC_SSP1->SR == 0){}
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
*@param   ulData	    数据	格式：1字节寄存器地址+2字节数据
*@author  laoc
*@date	  2019-02-17
***************************************************************************/
static void vSendDataToDAC7760(uint8_t ucSuperIO, uint32_t ulData)
{
	uint8_t i;
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

		while( (LPC_SSP1->SR & 0x01) == 0) {}				//等待数据发送完毕
		CLR_VALUE_DAC7760_LATCH1;							//置LATCH为低电平
		MyDelay(50);
			
		SET_VALUE_DAC7760_LATCH1;							//产生上升沿，将发送到DAC7760的数据转化为输出
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
		while( (LPC_SSP1->SR & 0x01) == 0 ){}
		
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
void vSetDAC7760OutputuA(uint8_t ucSuperIO, uint16_t uA)
{
	uint16_t ulDataToSet;
	ulDataToSet = (uA - 4000) * 4096 / 16000;
	
	if(ulDataToSet >= 4096)
	{
		ulDataToSet = 4095;
	}
	vSendDataToDAC7760(ucSuperIO, DAC7760_REG_DATA | ulDataToSet << 4);
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
void vSetDAC7760RealVal(uint8_t ucSuperIO, int32_t lMin, int32_t lMax, uint32_t ulRealData)
{
	uint16_t uA = 16000 * (ulRealData - lMin) / (lMax - lMin) + 4000;      //对应4~20mA的量程
	vSetDAC7760OutputuA(ucSuperIO, uA);
}

/****************************************************
*@brief  初始化PWM1，普通4~20mA
*@author laoc
*@date	 2019-02-17							
***************************************************/
void vPWM1Init(void)
{
	uint8_t pwmChannel = 0;
	PWM_TIMERCFG_Type PWMTimerCfg = {PWM_TIMER_PRESCALE_TICKVAL, {0,0,0}, 1};
	PWM_MATCHCFG_Type PWMMatchCfg;
	
	PWMTimerCfg.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
	PWMTimerCfg.PrescaleValue = 1;
	PWM_Init(1, PWM_MODE_TIMER, (void *)&PWMTimerCfg);
	
	//管脚功能配置
    
    for (pwmChannel = 0; pwmChannel < PWM_NUM; pwmChannel++)
    {
        (void)PINSEL_ConfigPin(PWM_IOList[pwmChannel]->Port, PWM_IOList[pwmChannel]->Pin, 2);
    }
	PWM_MatchUpdate(1, 0, PWM_MATCH_VALUE, PWM_MATCH_UPDATE_NOW);    //设置MR0的值为200
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
static void vSetPWMData(uint8_t pwmChannel, uint32_t data)
{
	PWM_MatchUpdate(1, pwmChannel, data, PWM_MATCH_UPDATE_NOW);
}

/***************************************************
*@brief  根据量程将真实的值转化为PWM输出 
*@param  pwmChannel    pwm通道，1~6
*@param  min	       量程最小值 = 实际值*10
*@param  max	       量程最大值 = 实际值*10
*@param  realData	   实际值
*@author laoc
*@date	 2019-02-17							
***************************************************/
void vSetPWMRealVal(uint8_t pwmChannel, int32_t lMin, int32_t lMax, uint32_t ulRealData)
{
	uint32_t ulData = 160 * (ulRealData - lMin) / (lMax - lMin) + 40;    //对应4~20mA的量程   20mA*10=200
	vSetPWMData(pwmChannel, ulData);
}

/***************************************************
*@brief  根据量程将真实的值转化为输出 
*@param  Channel       通道，AO1~AO8
*@param  realData	   实际值 = 目标值*10
*@author laoc
*@date	 2019-02-17							
***************************************************/
void vAnalogOutputSetRealVal(uint8_t ucChannel, uint32_t ulRealData)
{
	if((ucChannel > 0) && (ucChannel <= AO_NUM))
	{
        int32_t lMax = AnalogOutputData[ucChannel-1].lMax;
        int32_t lMin = AnalogOutputData[ucChannel-1].lMin;
        
        AnalogOutputData[ucChannel].lAOVal = ulRealData;
        
		if(ucChannel <=PWM_NUM)
		{
			vSetPWMRealVal(ucChannel, lMin, lMax, ulRealData);
		}
		else
		{
			vSetDAC7760RealVal(ucChannel-PWM_NUM, lMin, lMax, ulRealData);
		}			
	}
}

/******************************************************************
*@brief  修改AO通道量程
*@param  channel	AI通道  AO1~AO8
*@param  min	    量程最小值
*@param  max	    量程最大值
******************************************************************/
void vAnalogOutputSetRange(uint8_t ucChannel, int32_t lMin, int32_t lMax)
{
	if( (ucChannel > 0) && (ucChannel <= AO_NUM) )
	{
		AnalogOutputData[ucChannel-1].lMax = lMax;
		AnalogOutputData[ucChannel-1].lMin = lMin;
	}
}

/******************************************************************
*@brief 输出设置任务函数							
******************************************************************/
void vOutputSetTask(void *p_arg)
{
	OS_ERR err = OS_ERR_NONE;
	
	vDigitalOutputInit();
	vDAC7760Init();
	vPWM1Init();
}