/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                                             NXP LPC43xx
*                                               on the
*                                 ARROW LPC-4350-DB1 DEVELOPMENT BOARD
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FT
*                 FF
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include  <bsp.h>
#include  <bsp_int.h>
#include  <cpu.h>

#include "lpc_clkpwr.h"

/*
*********************************************************************************************************
*                                      REGISTER BASE ADDRESSES
*********************************************************************************************************
*/

#define  BSP_APB1_BASE_ADDR                       0x40080000
#define  BSP_SC_BASE_ADDR                        (BSP_APB1_BASE_ADDR + 0x7C000)


/*
*********************************************************************************************************
*                                           LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_BIT_SC_FLASHCFG_FLASHTIM_SAFE        DEF_BIT_MASK(0x005u, 12u)
#define  BSP_BIT_SC_FLASHCFG_RESET_VALUE          DEF_BIT_MASK(0x03Au,  0u)

#define  BSP_BIT_SC_SCS_EMCBC                     DEF_BIT_02
#define  BSP_BIT_SC_SCS_OSCRS                     DEF_BIT_04
#define  BSP_BIT_SC_SCS_OSCEN                     DEF_BIT_05
#define  BSP_BIT_SC_SCS_OSCSTAT                   DEF_BIT_06

#define  BSP_BIT_SC_CLKSRCSEL_CLKSRC              DEF_BIT_00

#define  BSP_BIT_SC_PLL0CFG_MSEL_M10              DEF_BIT_MASK(0x09u, 0u)
#define  BSP_BIT_SC_PLL0CON_PLLE                  DEF_BIT_00
#define  BSP_BIT_SC_PLL0FEED_SEQ_1                DEF_BIT_MASK(0xAAu, 0u)
#define  BSP_BIT_SC_PLL0FEED_SEQ_2                DEF_BIT_MASK(0x55u, 0u)
#define  BSP_BIT_SC_PLL0STAT_PLOCK                DEF_BIT_10

#define  BSP_BIT_SC_PLL1CFG_MSEL_M8               DEF_BIT_MASK(0x07u, 0u)
#define  BSP_BIT_SC_PLL1CON_PLLE                  DEF_BIT_00
#define  BSP_BIT_SC_PLL1FEED_SEQ_1                DEF_BIT_MASK(0xAAu, 0u)
#define  BSP_BIT_SC_PLL1FEED_SEQ_2                DEF_BIT_MASK(0x55u, 0u)
#define  BSP_BIT_SC_PLL1STAT_PLOCK                DEF_BIT_10

#define  DEF_BIT_SC_CCLKSEL_CCLKDIV_1             DEF_BIT_MASK(0x01u, 0u)
#define  DEF_BIT_SC_CCLKSEL_CCLKSEL               DEF_BIT_08

#define  DEF_BIT_SC_PCLKSEL_PCLKDIV_2             DEF_BIT_MASK(0x02u, 0u)

#define  DEF_BIT_SC_CLKOUTCFG_CLKOUT_EN           DEF_BIT_08

#define  BSP_REG_TO_VAL                           0x000FFFFFu


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

typedef  struct  bsp_lpc_sc_reg {
    CPU_REG32  FLASHCFG;                                        /* Flash Accelerator Configuration Register             */
    CPU_REG32  RESERVED0[31];
    CPU_REG32  PLL0CON;                                         /* PLL0 Control Register                                */
    CPU_REG32  PLL0CFG;                                         /* PLL0 Configuration Register                          */
    CPU_REG32  PLL0STAT;                                        /* PLL0 Status Register                                 */
    CPU_REG32  PLL0FEED;                                        /* PLL0 Feed Register                                   */
    CPU_REG32  RESERVED1[4];
    CPU_REG32  PLL1CON;                                         /* PLL1 Control Register                                */
    CPU_REG32  PLL1CFG;                                         /* PLL1 Configuration Register                          */
    CPU_REG32  PLL1STAT;                                        /* PLL1 Status Register                                 */
    CPU_REG32  PLL1FEED;                                        /* PLL1 Feed Register                                   */
    CPU_REG32  RESERVED2[4];
    CPU_REG32  PCON;                                            /* Power Control Register                               */
    CPU_REG32  PCONP;                                           /* Power Control for Peripherals Register               */
    CPU_REG32  PCONP1;                                          /* Power Control for Peripherals Register               */
    CPU_REG32  RESERVED3[13];
    CPU_REG32  EMCCLKSEL;                                       /* External Memory Controller Clock Selection Register  */
    CPU_REG32  CCLKSEL;                                         /* CPU Clock Selection Register                         */
    CPU_REG32  USBCLKSEL;                                       /* USB Clock Selection Register                         */
    CPU_REG32  CLKSRCSEL;                                       /* Clock Source Select Register                         */
    CPU_REG32  CANSLEEPCLR;                                     /* CAN Sleep Clear Register                             */
    CPU_REG32  CANWAKEFLAGS;                                    /* CAN Wake-up Flags Register                           */
    CPU_REG32  RESERVED4[10];
    CPU_REG32  EXTINT;                                          /* External Interrupt Flag Register                     */
    CPU_REG32  RESERVED5[1];
    CPU_REG32  EXTMODE;                                         /* External Interrupt Mode Register                     */
    CPU_REG32  EXTPOLAR;                                        /* External Interrupt Polarity Register                 */
    CPU_REG32  RESERVED6[12];
    CPU_REG32  RSID;                                            /* Reset Source Identification Register                 */
    CPU_REG32  RESERVED7[7];
    CPU_REG32  SCS;                                             /* System Controls and Status Register                  */
    CPU_REG32  IRCTRIM;                                         /* lock Dividers                                        */
    CPU_REG32  PCLKSEL;                                         /* Peripheral Clock Selection Register                  */
    CPU_REG32  RESERVED8;
    CPU_REG32  PBOOST;                                          /* Power Boost control register                         */
    CPU_REG32  SPIFICLKSEL;
    CPU_REG32  LCD_CFG;                                         /* LCD Configuration and clocking control Register      */
    CPU_REG32  RESERVED10[1];
    CPU_REG32  USBIntSt;                                        /* USB Interrupt Status Register                        */
    CPU_REG32  DMAREQSEL;                                       /* DMA Request Select Register                          */
    CPU_REG32  CLKOUTCFG;                                       /* Clock Output Configuration Register                  */
    CPU_REG32  RSTCON0;                                         /* RESET Control0 Register                              */
    CPU_REG32  RSTCON1;                                         /* RESET Control1 Register                              */
    CPU_REG32  RESERVED11[2];
    CPU_REG32  EMCDLYCTL;                                       /* SDRAM programmable delays                            */
    CPU_REG32  EMCCAL;                                          /* Calibration of programmable delays                   */
} BSP_LPC_SC_REG;


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


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


/*
*********************************************************************************************************
*********************************************************************************************************
**                                        GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           BSP_PreInit()
*
* Description : Board Support Package Low Level Initialization.
*
* Argument(s) : none.
*
* Returns(s)  : none
*
* Caller(s)   : Startup code.
*
* Note(s)     : (1) The PLL feed sequence must be written to this register in order for the related PLL's
*                   configuration and control register changes to take effect.
*********************************************************************************************************
*/

void  BSP_PreInit (void)
{
    BSP_LPC_SC_REG *p_reg;
    CPU_INT32U      reg_to;


    p_reg = (BSP_LPC_SC_REG *)BSP_SC_BASE_ADDR;
                                                                /* Set safe Flash Accelerator setting.                  */
    p_reg->FLASHCFG = (BSP_BIT_SC_FLASHCFG_FLASHTIM_SAFE |
                       BSP_BIT_SC_FLASHCFG_RESET_VALUE);

#if __ARMVFP__                                                  /* Enable access to Floating-point coprocessor.         */
    CPU_REG_NVIC_CPACR = CPU_REG_NVIC_CPACR_CP10_FULL_ACCESS | CPU_REG_NVIC_CPACR_CP11_FULL_ACCESS;

    DEF_BIT_CLR(CPU_REG_SCB_FPCCR, DEF_BIT_31);                 /* Disable automatic FP register content                */
    DEF_BIT_CLR(CPU_REG_SCB_FPCCR, DEF_BIT_30);                 /* Disable Lazy context switch                          */
#endif

    p_reg->SCS |= BSP_BIT_SC_SCS_OSCEN;                         /* Enable main oscillator.                              */

    reg_to = BSP_REG_TO_VAL;
                                                                /* Wait for oscillator to be ready.                     */
    while ((DEF_BIT_IS_CLR(p_reg->SCS, BSP_BIT_SC_SCS_OSCSTAT)) &
           (reg_to > 0)) {
        reg_to--;
    }
                                                                /* Select the main osc. as the sysclk and PLL0 clk.     */
    p_reg->CLKSRCSEL |= BSP_BIT_SC_CLKSRCSEL_CLKSRC;

                                                                /* Configure PLL0.                                      */
    p_reg->PLL0CFG  |= BSP_BIT_SC_PLL0CFG_MSEL_M10;             /* Set PLL0 multiplier value to 10.                     */
    p_reg->PLL0CON  |= BSP_BIT_SC_PLL0CON_PLLE;                 /* Enable PLL0.                                         */
    p_reg->PLL0FEED |= BSP_BIT_SC_PLL0FEED_SEQ_1;               /* Write PLL0 feed sequences (see note 1).              */
    p_reg->PLL0FEED |= BSP_BIT_SC_PLL0FEED_SEQ_2;

    reg_to = BSP_REG_TO_VAL;                                    /* Configure lock timeout value.                        */
                                                                /* Wait for PLL0 to lock.                               */
    while ((DEF_BIT_IS_CLR(p_reg->PLL0STAT, BSP_BIT_SC_PLL0STAT_PLOCK)) &
           (reg_to > 0)) {
        reg_to--;
    }

                                                                /* Configure PLL1.                                      */
    p_reg->PLL1CFG  |= BSP_BIT_SC_PLL1CFG_MSEL_M8;              /* Set PLL1 multiplier value to 8.                      */
    p_reg->PLL1CON  |= BSP_BIT_SC_PLL1CON_PLLE;                 /* Enable PLL1.                                         */
    p_reg->PLL1FEED |= BSP_BIT_SC_PLL1FEED_SEQ_1;               /* Write PLL1 feed sequences (see note 1).              */
    p_reg->PLL1FEED |= BSP_BIT_SC_PLL1FEED_SEQ_2;

    reg_to = BSP_REG_TO_VAL;                                    /* Configure lock timeout value.                        */
                                                                /* Wait for PLL to lock.                                */
    while ((DEF_BIT_IS_CLR(p_reg->PLL1STAT, BSP_BIT_SC_PLL1STAT_PLOCK)) &
           (reg_to > 0)) {
        reg_to--;
    }

    p_reg->CCLKSEL   |= DEF_BIT_SC_CCLKSEL_CCLKDIV_1;           /* Select input clock divider.                          */
    p_reg->CCLKSEL   |= DEF_BIT_SC_CCLKSEL_CCLKSEL;             /* Use main PLL as the input to the CPU clock divider.  */
    p_reg->PCLKSEL   |= DEF_BIT_SC_PCLKSEL_PCLKDIV_2;           /* Select clock devide value for APB peripherals.       */
    p_reg->PCONP      = 0u;                                     /* Disable all peripherals to save power.               */
    p_reg->CLKOUTCFG |= DEF_BIT_SC_CLKOUTCFG_CLKOUT_EN;         /* Enable CLKOUT without glitches.                      */
}


/*
*********************************************************************************************************
*                                           BSP_PostInit()
*
* Description : Board Support Package Peripheral Initialization.
*
* Argument(s) : none.
*
* Returns(s)  : none
*
* Caller(s)   : Startup task.
*********************************************************************************************************
*/

void  BSP_PostInit (void)
{

}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                       CPU CLOCK FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkGet()
*
* Description : Gets the CPU clock frequency(Fclk).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*
*********************************************************************************************************
*/



CPU_INT32U  BSP_CPU_ClkGet (void)
{
  return CLKPWR_GetCLK(CLKPWR_CLKTYPE_CPU);
}
