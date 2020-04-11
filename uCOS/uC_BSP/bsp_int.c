/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
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
*                                        BOARD SUPPORT PACKAGE
*
*                                             NXP LPC43xx
*                                               on the
*                                 ARROW LPC-4350-DB1 DEVELOPMENT BOARD
*
* Filename      : bsp_int.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_INT_MODULE
#include  <bsp_int.h>
#include  <lib_def.h>
#include  <cpu.h>
#include  <os.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_ID_MAX];


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy(void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntClr()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id      Interrupt to clear.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClr (CPU_INT08U  int_id)
{

}


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_INT08U  int_id)
{
    if (int_id < BSP_INT_ID_MAX) {
        CPU_IntSrcDis(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_INT08U  int_id)
{
    if (int_id < BSP_INT_ID_MAX) {
        CPU_IntSrcEn(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_INT08U     int_id,
                      CPU_FNCT_VOID  isr)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_ID_MAX) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (CPU_INT08U  int_id,
                      CPU_INT08U  prio)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_ID_MAX) {
        CPU_CRITICAL_ENTER();
        CPU_IntSrcPrioSet(int_id + 16, prio);
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < BSP_INT_ID_MAX; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandlerWDT                (void)  { BSP_IntHandler(BSP_INT_ID_WDT);             }
void  BSP_IntHandlerTIMER0             (void)  { BSP_IntHandler(BSP_INT_ID_TIMER0);          }
void  BSP_IntHandlerTIMER1             (void)  { BSP_IntHandler(BSP_INT_ID_TIMER1);          }
void  BSP_IntHandlerTIMER2             (void)  { BSP_IntHandler(BSP_INT_ID_TIMER2);          }
void  BSP_IntHandlerTIMER3             (void)  { BSP_IntHandler(BSP_INT_ID_TIMER3);          }
void  BSP_IntHandlerUART0              (void)  { BSP_IntHandler(BSP_INT_ID_UART0);           }
void  BSP_IntHandlerUART1              (void)  { BSP_IntHandler(BSP_INT_ID_UART1);           }
void  BSP_IntHandlerUART2              (void)  { BSP_IntHandler(BSP_INT_ID_UART2);           }
void  BSP_IntHandlerUART3              (void)  { BSP_IntHandler(BSP_INT_ID_UART3);           }
void  BSP_IntHandlerPWM1               (void)  { BSP_IntHandler(BSP_INT_ID_PWM1);            }
void  BSP_IntHandlerI2C0               (void)  { BSP_IntHandler(BSP_INT_ID_I2C0);            }
void  BSP_IntHandlerI2C1               (void)  { BSP_IntHandler(BSP_INT_ID_I2C1);            }
void  BSP_IntHandlerI2C2               (void)  { BSP_IntHandler(BSP_INT_ID_I2C2);            }
void  BSP_IntHandlerSSP0               (void)  { BSP_IntHandler(BSP_INT_ID_SSP0);            }
void  BSP_IntHandlerSSP1               (void)  { BSP_IntHandler(BSP_INT_ID_SSP1);            }
void  BSP_IntHandlerPLL0               (void)  { BSP_IntHandler(BSP_INT_ID_PLL0);            }
void  BSP_IntHandlerRTC                (void)  { BSP_IntHandler(BSP_INT_ID_RTC);             }
void  BSP_IntHandlerEINT0              (void)  { BSP_IntHandler(BSP_INT_ID_EINT0);           }
void  BSP_IntHandlerEINT1              (void)  { BSP_IntHandler(BSP_INT_ID_EINT1);           }
void  BSP_IntHandlerEINT2              (void)  { BSP_IntHandler(BSP_INT_ID_EINT2);           }
void  BSP_IntHandlerEINT3              (void)  { BSP_IntHandler(BSP_INT_ID_EINT3);           }
void  BSP_IntHandlerADC                (void)  { BSP_IntHandler(BSP_INT_ID_ADC);             }
void  BSP_IntHandlerBOD                (void)  { BSP_IntHandler(BSP_INT_ID_BOD);             }
void  BSP_IntHandlerUSB                (void)  { BSP_IntHandler(BSP_INT_ID_USB);             }
void  BSP_IntHandlerCAN                (void)  { BSP_IntHandler(BSP_INT_ID_CAN);             }
void  BSP_IntHandlerDMA                (void)  { BSP_IntHandler(BSP_INT_ID_DMA);             }
void  BSP_IntHandlerI2S                (void)  { BSP_IntHandler(BSP_INT_ID_I2S);             }
void  BSP_IntHandlerETHERNET           (void)  { BSP_IntHandler(BSP_INT_ID_ETHERNET);        }
void  BSP_IntHandlerSDCARD             (void)  { BSP_IntHandler(BSP_INT_ID_SDCARD);          }
void  BSP_IntHandlerPWM                (void)  { BSP_IntHandler(BSP_INT_ID_PWM);             }
void  BSP_IntHandlerQEI                (void)  { BSP_IntHandler(BSP_INT_ID_QEI);             }
void  BSP_IntHandlerPLL1               (void)  { BSP_IntHandler(BSP_INT_ID_PLL1);            }
void  BSP_IntHandlerUSB_ACT            (void)  { BSP_IntHandler(BSP_INT_ID_USB_ACT);         }
void  BSP_IntHandlerCAN_ACT            (void)  { BSP_IntHandler(BSP_INT_ID_CAN_ACT);         }
void  BSP_IntHandlerUART4              (void)  { BSP_IntHandler(BSP_INT_ID_UART4);           }
void  BSP_IntHandlerSSP2               (void)  { BSP_IntHandler(BSP_INT_ID_SSP2);            }
void  BSP_IntHandlerLCD                (void)  { BSP_IntHandler(BSP_INT_ID_LCD);             }
void  BSP_IntHandlerGPIO               (void)  { BSP_IntHandler(BSP_INT_ID_GPIO);            }
void  BSP_IntHandlerPWM0               (void)  { BSP_IntHandler(BSP_INT_ID_PWM0);            }
void  BSP_IntHandlerEEPROM             (void)  { BSP_IntHandler(BSP_INT_ID_EEPROM);          }


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandler (CPU_INT08U  int_id)
{
    CPU_FNCT_VOID  isr;
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();                                       /* Tell the OS that we are starting an ISR            */

    OSIntEnter();

    CPU_CRITICAL_EXIT();

    if (int_id < BSP_INT_ID_MAX) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

    OSIntExit();                                                /* Tell the OS that we are leaving the ISR            */
}


/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{
    while (DEF_TRUE) {
        ;
    }
}
