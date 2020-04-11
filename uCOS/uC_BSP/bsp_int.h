/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                              (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                     MICRIUM BOARD SUPPORT PACKAGE
*                                         INTERRUPT CONTROLLER
*
*                                             NXP LPC43xx
*                                               on the
*                                 ARROW LPC-4350-DB1 DEVELOPMENT BOARD
*
*
* Filename      : bsp_int.h
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP_INT present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_INT_PRESENT
#define  BSP_INT_PRESENT


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/


#ifdef   BSP_INT_MODULE
#define  BSP_INT_EXT
#else
#define  BSP_INT_EXT  extern
#endif

/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_ID_WDT                              0u
#define  BSP_INT_ID_TIMER0                           1u
#define  BSP_INT_ID_TIMER1                           2u
#define  BSP_INT_ID_TIMER2                           3u
#define  BSP_INT_ID_TIMER3                           4u
#define  BSP_INT_ID_UART0                            5u
#define  BSP_INT_ID_UART1                            6u
#define  BSP_INT_ID_UART2                            7u
#define  BSP_INT_ID_UART3                            8u
#define  BSP_INT_ID_PWM1                             9u
#define  BSP_INT_ID_I2C0                            10u
#define  BSP_INT_ID_I2C1                            11u
#define  BSP_INT_ID_I2C2                            12u
#define  BSP_INT_ID_Reserved                        13u
#define  BSP_INT_ID_SSP0                            14u
#define  BSP_INT_ID_SSP1                            15u
#define  BSP_INT_ID_PLL0                            16u
#define  BSP_INT_ID_RTC                             17u
#define  BSP_INT_ID_EINT0                           18u
#define  BSP_INT_ID_EINT1                           19u
#define  BSP_INT_ID_EINT2                           20u
#define  BSP_INT_ID_EINT3                           21u
#define  BSP_INT_ID_ADC                             22u
#define  BSP_INT_ID_BOD                             23u
#define  BSP_INT_ID_USB                             24u
#define  BSP_INT_ID_CAN                             25u
#define  BSP_INT_ID_DMA                             26u
#define  BSP_INT_ID_I2S                             27u
#define  BSP_INT_ID_ETHERNET                        28u
#define  BSP_INT_ID_SDCARD                          29u
#define  BSP_INT_ID_PWM                             30u
#define  BSP_INT_ID_QEI                             31u
#define  BSP_INT_ID_PLL1                            32u
#define  BSP_INT_ID_USB_ACT                         33u
#define  BSP_INT_ID_CAN_ACT                         34u
#define  BSP_INT_ID_UART4                           35u
#define  BSP_INT_ID_SSP2                            36u
#define  BSP_INT_ID_LCD                             37u
#define  BSP_INT_ID_GPIO                            38u
#define  BSP_INT_ID_PWM0                            39u
#define  BSP_INT_ID_EEPROM                          40u

#define  BSP_INT_ID_MAX                             41u


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  BSP_IntClr                       (CPU_INT08U     int_id);

void  BSP_IntDis                       (CPU_INT08U     int_id);

void  BSP_IntDisAll                    (void);

void  BSP_IntEn                        (CPU_INT08U     int_id);

void  BSP_IntInit                      (void);

void  BSP_IntVectSet                   (CPU_INT08U     int_id,
                                        CPU_FNCT_VOID  isr_fnct);

void  BSP_IntPrioSet                   (CPU_INT08U     int_id,
                                        CPU_INT08U     prio);

void  BSP_IntHandler                   (CPU_INT08U     int_id);

void  BSP_IntHandlerWDT                (void);
void  BSP_IntHandlerTIMER0             (void);
void  BSP_IntHandlerTIMER1             (void);
void  BSP_IntHandlerTIMER2             (void);
void  BSP_IntHandlerTIMER3             (void);
void  BSP_IntHandlerUART0              (void);
void  BSP_IntHandlerUART1              (void);
void  BSP_IntHandlerUART2              (void);
void  BSP_IntHandlerUART3              (void);
void  BSP_IntHandlerPWM1               (void);
void  BSP_IntHandlerI2C0               (void);
void  BSP_IntHandlerI2C1               (void);
void  BSP_IntHandlerI2C2               (void);
void  BSP_IntHandlerSSP0               (void);
void  BSP_IntHandlerSSP1               (void);
void  BSP_IntHandlerPLL0               (void);
void  BSP_IntHandlerRTC                (void);
void  BSP_IntHandlerEINT0              (void);
void  BSP_IntHandlerEINT1              (void);
void  BSP_IntHandlerEINT2              (void);
void  BSP_IntHandlerEINT3              (void);
void  BSP_IntHandlerADC                (void);
void  BSP_IntHandlerBOD                (void);
void  BSP_IntHandlerUSB                (void);
void  BSP_IntHandlerCAN                (void);
void  BSP_IntHandlerDMA                (void);
void  BSP_IntHandlerI2S                (void);
void  BSP_IntHandlerETHERNET           (void);
void  BSP_IntHandlerSDCARD             (void);
void  BSP_IntHandlerPWM                (void);
void  BSP_IntHandlerQEI                (void);
void  BSP_IntHandlerPLL1               (void);
void  BSP_IntHandlerUSB_ACT            (void);
void  BSP_IntHandlerCAN_ACT            (void);
void  BSP_IntHandlerUART4              (void);
void  BSP_IntHandlerSSP2               (void);
void  BSP_IntHandlerLCD                (void);
void  BSP_IntHandlerGPIO               (void);
void  BSP_IntHandlerPWM0               (void);
void  BSP_IntHandlerEEPROM             (void);


/*
*********************************************************************************************************
*                                              ERROR CHECKING
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */

