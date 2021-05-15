/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               LWIP.c
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-3-10
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC407x_8x_177x_8x.h"
#include "lpc_emac.h"
#include "lpc_phy.h"
#include "lpc_phy_dp83848.h"
#include "lpc_emac_config.h"
#include "lwip/memp.h"
#include "lwIP.h"
#include "lwIP/tcp.h"
#include "lwIP/udp.h"
#include "lwIP/tcpip.h"
#include "netif/etharp.h"
#include "lwIP/dhcp.h"
#include "ethernetif.h"
#include "arch/sys_arch.h"
#include "arch/cc.h"
#include "lwipconfig.h"
#include <LPC407x_8x_177x_8x.h>
#include <includes.h>
#include "bsp_int.h"


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/* Private define ------------------------------------------------------------*/
#define DP83848_PHY            /* Ethernet pins mapped on HY-RedBull V3.0 Board */

#define PHY_ADDRESS       0x01 /* Relative to HY-RedBull V3.0 Board */

/* #define MII_MODE */

#define RMII_MODE              /* STM32F107 connect PHY using RMII mode	*/

#define MAX_DHCP_TRIES        4
 
/* Private variables ---------------------------------------------------------*/
extern uint8_t ControllerID;
static struct netif netif;
static uint32_t IPaddress = 0;

uint8_t macaddress[6]={ emacETHADDR0, emacETHADDR1, emacETHADDR2, emacETHADDR3, emacETHADDR4, emacETHADDR5 };

/* Private function prototypes -----------------------------------------------*/
static void list_if                (void);
static void TcpipInitDone          (void *arg);
static void NVIC_Configuration     (void);
//static void USART_Configuration    (void);

/*******************************************************************************
* Function Name  : list_if
* Description    : display ip address in serial port debug windows
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void list_if(void)
{
   
}

/*******************************************************************************
* Function Name  : TcpipInitDone
* Description    : TcpipInitDone wait for tcpip init being done
* Input          : - arg: the semaphore to be signaled
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void TcpipInitDone(void *arg)
{
    sys_sem_t *sem;
    sem = arg;
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
   
}


/*******************************************************************************
* Function Name  : ETH_IRQHandler
* Description    : Ethernet ISR
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void ETH_IRQHandler(void)
{
	CPU_SR         cpu_sr;
	
	EMAC_IntClear();
	
    while(EMAC_GetRxFrameSize() != 0)
    {
        ethernetif_input(&netif);
    }
    
    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */    
}

/*******************************************************************************
* Function Name  : Init_lwIP
* Description    : Init_lwIP initialize the LwIP
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Init_lwIP(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
	
    sys_sem_t sem;

    sys_init();
    
    /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
    mem_init();
    
    /* Initializes the memory pools defined by MEMP_NUM_x.*/
    memp_init();
    
    pbuf_init();	
    netif_init();
    
//	USART_Configuration();

    myprintf("TCP/IP initializing... \r\n");
	
    sys_sem_new(&sem, 0);
    tcpip_init(TcpipInitDone, NULL);
	
//    (void)sys_sem_wait(&sem);
//    (void)sys_sem_free(&sem);
    
#if LWIP_DHCP
   /* 启用DHCP服务器 */
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else
    /* 启用静态IP */
    IP4_ADDR(&ipaddr, emacIPADDR0, emacIPADDR1, emacIPADDR2, emacIPADDR3 );
    IP4_ADDR(&netmask,emacNET_MASK0, emacNET_MASK1, emacNET_MASK2, emacNET_MASK3 );
//    IP4_ADDR(&gw, emacGATEWAY_ADDR0, emacGATEWAY_ADDR1, emacGATEWAY_ADDR2, emacGATEWAY_ADDR3 );
#endif  
    Set_MAC_Address(macaddress);
    
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&netif);
    
#if LWIP_DHCP
    dhcp_start(&netif);
#endif
    netif_set_up(&netif);
	BSP_IntEn (BSP_INT_ID_ETHERNET);  
	myprintf("TCP/IP initialized. \r\n");
}

/*******************************************************************************
* Function Name  : Display_IPAddress
* Description    : Display_IPAddress Display IP Address
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Display_IPAddress(void)
{
//    if(IPaddress != netif.ip_addr.addr)
//    {   
//	    /* IP 地址发生变化*/
//        __IO uint8_t iptab[4];
//        uint8_t iptxt[20];
//        
//        /* read the new IP address */
//        IPaddress = netif.ip_addr.addr;
//        
//        iptab[0] = (uint8_t)(IPaddress >> 24);
//        iptab[1] = (uint8_t)(IPaddress >> 16);
//        iptab[2] = (uint8_t)(IPaddress >> 8);
//        iptab[3] = (uint8_t)(IPaddress);
//        
//        myprintf((char*)iptxt, "   %d.%d.%d.%d    ", iptab[3], iptab[2], iptab[1], iptab[0]);
//        
//        list_if();
//        
//        /* Display the new IP address */
//#if LWIP_DHCP
//        if(netif.flags & NETIF_FLAG_DHCP)
//        {   
//		    /* IP由DHCP指定 */
//            /* Display the IP address */
//		    GUI_Text( ( MAX_X - ( strlen("  IP assigned   ") * 8 ) ) / 2 , MAX_Y/2 - 40,  "  IP assigned   ", White, Black);
//		    GUI_Text( ( MAX_X - ( strlen("by DHCP server  ") * 8 ) ) / 2 , MAX_Y/2 - 20,  " by DHCP server ", White, Black);
//		    GUI_Text( ( MAX_X - ( strlen((void*)iptxt) * 8 ) ) / 2       , MAX_Y/2     ,       iptxt        , White, Black);	
//		    GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 + 20,  "                ", White, Black);
//        }
//        else
//#endif  
//        /* 静态IP地址 */
//        {   
//		    /* Display the IP address */
//		    GUI_Text( ( MAX_X - ( strlen(" Static IP Addr ") * 8 ) ) / 2 , MAX_Y/2 - 40,  " Static IP Addr ", White, Black);
//		    GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 - 20,  "                ", White, Black);
//		    GUI_Text( ( MAX_X - ( strlen((void*)iptxt) * 8 ) ) / 2       , MAX_Y/2     ,       iptxt        , White, Black);
//		    GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 + 20,  "                ", White, Black);

//        }
//    }
//#if LWIP_DHCP
//    else if(IPaddress == 0)
//    {   
//	    /* 等待DHCP分配IP */
//		GUI_Text( ( MAX_X - ( strlen("   Looking for  ") * 8 ) ) / 2 , MAX_Y/2 - 40,  "   Looking for  ", White, Black);
//		GUI_Text( ( MAX_X - ( strlen("   DHCP server  ") * 8 ) ) / 2 , MAX_Y/2 - 20,  "   DHCP server  ", White, Black);
//		GUI_Text( ( MAX_X - ( strlen(" please wait... ") * 8 ) ) / 2 , MAX_Y/2     ,  " please wait... ", White, Black);
//		GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 + 20,  "                ", White, Black);
//        
//        /* If no response from a DHCP server for MAX_DHCP_TRIES times */
//        /* stop the dhcp client and set a static IP address */
//        if(netif.dhcp->tries > MAX_DHCP_TRIES) 
//        {   
//		    /* 超出DHCP重试次数，改用静态IP */
//            struct ip_addr ipaddr;
//            struct ip_addr netmask;
//            struct ip_addr gw;
//            
//		    GUI_Text( ( MAX_X - ( strlen("  DHCP timeout  ") * 8 ) ) / 2, MAX_Y/2 + 20, "  DHCP timeout  ", White, Black);

//            dhcp_stop(&netif);
//            
//            IP4_ADDR(&ipaddr, emacIPADDR0, emacIPADDR1, emacIPADDR2, emacIPADDR3 );
//            IP4_ADDR(&netmask,emacNET_MASK0, emacNET_MASK1, emacNET_MASK2, emacNET_MASK3 );
//            IP4_ADDR(&gw, emacGATEWAY_ADDR0, emacGATEWAY_ADDR1, emacGATEWAY_ADDR2, emacGATEWAY_ADDR3 );
//            
//            netif_set_addr(&netif, &ipaddr , &netmask, &gw);
//            
//            list_if();
//        }
//    }
//#endif
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART */
//  USART_SendData(USART2, (uint8_t) ch);

//  /* Loop until the end of transmission */
//  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
//  {}

  return ch;
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

