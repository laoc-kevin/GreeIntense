/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */
 
#include "includes.h"
#include "lwip/opt.h"

#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwIP.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "lwipconfig.h"
#include "ethernetif.h"
#include "netif/etharp.h"
//#include "netif/ppp_oe.h"
#include "lpc_emac.h"
#include "lpc_emac_config.h"
#include "lpc_phy.h"
#include "lpc_pinsel.h"
/* Private define ------------------------------------------------------------*/
#define ETH_RXBUFNB        4    /* DMA接收缓冲区数目 */
#define ETH_TXBUFNB        2    /* DMA发送缓冲区数目 */


/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

static struct netif *pNetif;

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Private variables ---------------------------------------------------------*/
uint8_t  __attribute__ ((aligned (4))) Tx_Buff[EMAC_MAX_FRAME_SIZE];/* Ethernet buffers */

uint8_t  MACaddr[ETH_ADRLEN] ;


static EMAC_CFG_Type EMAC_ConfigStruct;

static err_t Ethernet_Initialize(struct netif *netif);
static err_t Ethernet_Configuration (struct netif *netif);
static int32_t Ethernet_PhyInit( EMAC_PHY_CFG_Type* PhyCfg);
static void Ethernet_FrameReceive(uint16_t* pData, uint32_t size);
/**
 * Setting the MAC address.
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void Set_MAC_Address(uint8_t* macadd)
{
  MACaddr[0] = macadd[0];
  MACaddr[1] = macadd[1];
  MACaddr[2] = macadd[2];
  MACaddr[3] = macadd[3];
  MACaddr[4] = macadd[4];
  MACaddr[5] = macadd[5];
  
  setEmacAddr( MACaddr );  
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static err_t
low_level_init(struct netif *netif)
{
    struct ethernetif *ethernetif = netif->state;
	
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */
	netif->hwaddr[0] =  MACaddr[0];
	netif->hwaddr[1] =  MACaddr[1];
	netif->hwaddr[2] =  MACaddr[2];
	netif->hwaddr[3] =  MACaddr[3];
	netif->hwaddr[4] =  MACaddr[4];
	netif->hwaddr[5] =  MACaddr[5];

	/* maximum transfer unit */
	netif->mtu = 1500;
	  
	/* device capabilitie s */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	 
	/* Do whatever else is needed to initialize interface. */ 
	
	return (err_t)Ethernet_Initialize(netif);
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */



static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
    struct ethernetif *ethernetif = netif->state;
	struct pbuf *q;
	EMAC_PACKETBUF_Type pDataStruct;

    uint32_t l = 0;
	uint8_t *buffer = (uint8_t *)Tx_Buff;
	uint8_t n=0;
	
	struct eth_hdr* ethhdr;
	ethhdr = (struct eth_hdr *)p->payload;
	
	 //myprintf("low_level_output: dest:%d:%d:%d:%d:%d:%d, src:%d:%d:%d:%d:%d:%d, type:%d ip_addr %d\n",
//     ethhdr->dest.addr[0],ethhdr->dest.addr[1], ethhdr->dest.addr[2],
//     ethhdr->dest.addr[3], ethhdr->dest.addr[4],ethhdr->dest.addr[5],
//     ethhdr->src.addr[0], ethhdr->src.addr[1], ethhdr->src.addr[2],
//     ethhdr->src.addr[3], ethhdr->src.addr[4], ethhdr->src.addr[5],
//     htons(ethhdr->type), netif->ip_addr .addr );

#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

	for(q = p; q != NULL; q = q->next) 
	{
		/* Send the data from the pbuf to the interface, one pbuf at a
		   time. The size of the data in each pbuf is kept in the ->len
		   variable. */
	    memcpy((u8_t*)&buffer[l], q->payload, q->len);
		l = l + q->len;	
	}
	
	pDataStruct.pbDataBuf = (uint32_t*)buffer;
    pDataStruct.ulDataLen = l;

    EMAC_WritePacketBuffer(&pDataStruct);

#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
	  
	LINK_STATS_INC(link.xmit);

	//myprintf("***********************************************\n\n");
	return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
    struct ethernetif *ethernetif = netif->state;
    struct pbuf *p, *q;
    uint8_t *buffer;
	uint32_t l = 0;
    uint32_t len;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
	buffer = (uint8_t *)EMAC_GetRxBuffer();
    len = EMAC_GetRxFrameSize();

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  
    if (p != NULL) 
    {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
        for(q = p; q != NULL; q = q->next) 
	    {
		  /* Read enough bytes to fill this pbuf in the chain. The
		   * available data in the pbuf is given by the q->len
		   * variable.
		   * This does not necessarily have to be a memcpy, you can also preallocate
		   * pbufs for a DMA-enabled MAC and after receiving truncate it to the
		   * actually received size. In this case, ensure the tot_len member of the
		   * pbuf is the sum of the chained pbuf len members.
		   */
			memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
			l = l + q->len;
        }
  
#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } 
	else
	{
//		drop packet();
		LINK_STATS_INC(link.memerr);
		LINK_STATS_INC(link.drop);
    }

//	  myprintf("EMAC_GetRxFrameSize %d \n", len);
    return p;  
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(struct netif *netif)
{
  struct ethernetif *ethernetif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  ethernetif = netif->state;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
  /* no packet could be read, silently ignore this */
  if (p == NULL) return;
  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = p->payload;

  switch (htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:
  case ETHTYPE_ARP:
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
     
  if (netif->input(p, netif)!=ERR_OK)    /* 将pbuf传递给上层协议栈 */
     { LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));	 
       pbuf_free(p);
       p = NULL;
     }
    break;

  default:
    pbuf_free(p);
    p = NULL;
    break;
  }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */

err_t ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  pNetif = netif;
	
  LWIP_ASSERT("netif != NULL", (netif != NULL));
    
  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  /* initialize the hardware */
  return (err_t)low_level_init(netif);
  
}

/*******************************************************************************
* Function Name  : Ethernet_Initialize
* Description    : Ethernet Initialize function
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
err_t Ethernet_Initialize(struct netif *netif)
{	 
    /* Configure Ethernet Pins. */
#if RMII > 0
    PINSEL_ConfigPin(1,0,1);
	PINSEL_ConfigPin(1,1,1);
	PINSEL_ConfigPin(1,4,1);
	PINSEL_ConfigPin(1,8,1);
	PINSEL_ConfigPin(1,9,1);
	PINSEL_ConfigPin(1,10,1);
	PINSEL_ConfigPin(1,14,1);
	PINSEL_ConfigPin(1,15,1);
	PINSEL_ConfigPin(1,16,1);
	PINSEL_ConfigPin(1,17,1);
#else
//  LPC_IOCON->P1_0  &= ~0x07;	/*  ENET I/O config */
//  LPC_IOCON->P1_0  |= 0x01;		/* ENET_TXD0 */
//  LPC_IOCON->P1_1  &= ~0x07;	
//  LPC_IOCON->P1_1  |= 0x01;		/* ENET_TXD1 */
//  LPC_IOCON->P1_2  &= ~0x07;
//  LPC_IOCON->P1_2  |= 0x01;		/* ENET_TXD2 */
//  LPC_IOCON->P1_3  &= ~0x07;	
//  LPC_IOCON->P1_3  |= 0x01;		/* ENET_TXD3 */
//  LPC_IOCON->P1_4  &= ~0x07;
//  LPC_IOCON->P1_4  |= 0x01;		/* ENET_TXEN */
//  LPC_IOCON->P1_5  &= ~0x07;	
//  LPC_IOCON->P1_5  |= 0x01;		/* ENET_TXER */
//  LPC_IOCON->P1_6  &= ~0x07;	
//  LPC_IOCON->P1_6  |= 0x01;		/* ENET_TX_CLK */
//  LPC_IOCON->P1_7  &= ~0x07;	
//  LPC_IOCON->P1_7  |= 0x01;		/* ENET_COL */
//  LPC_IOCON->P1_8  &= ~0x07;	
//  LPC_IOCON->P1_8  |= 0x01;		/* ENET_CRS */
//  LPC_IOCON->P1_9  &= ~0x07;	
//  LPC_IOCON->P1_9  |= 0x01;		/* ENET_RXD0 */
//  LPC_IOCON->P1_10 &= ~0x07;	
//  LPC_IOCON->P1_10 |= 0x01;		/* ENET_RXD1 */
//  LPC_IOCON->P1_11 &= ~0x07;	
//  LPC_IOCON->P1_11 |= 0x01;		/* ENET_RXD2 */
//  LPC_IOCON->P1_12 &= ~0x07;	
//  LPC_IOCON->P1_12 |= 0x01;		/* ENET_RXD3 */
//  LPC_IOCON->P1_13 &= ~0x07;	
//  LPC_IOCON->P1_13 |= 0x01;		/* ENET_RX_DV */
//  LPC_IOCON->P1_14 &= ~0x07;	
//  LPC_IOCON->P1_14 |= 0x01;		/* ENET_RX_ER */
//  LPC_IOCON->P1_15 &= ~0x07;	
//  LPC_IOCON->P1_15 |= 0x01;		/* ENET_RX_CLK/ENET_REF_CLK */
#endif

    return (err_t)Ethernet_Configuration(netif);
}

/*******************************************************************************
* Function Name  : Ethernet_Configuration
* Description    : Configures the Ethernet Interface
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static err_t Ethernet_Configuration(struct netif *netif)
{
	EMAC_ConfigStruct.bPhyAddr = LPC_PHYDEF_PHYADDR;
	EMAC_ConfigStruct.nMaxFrameSize = EMAC_MAX_FRAME_SIZE;
	EMAC_ConfigStruct.pbEMAC_Addr = MACaddr;
	EMAC_ConfigStruct.PhyCfg.Mode = EMAC_MODE_AUTO;
	EMAC_ConfigStruct.pfnPHYInit = &Ethernet_PhyInit;
	EMAC_ConfigStruct.pfnFrameReceive = &Ethernet_FrameReceive;

	if(EMAC_Init(&EMAC_ConfigStruct))
	{
		return ERR_OK;
	}
	else
	{
		return ERR_IF;
	}
}

/*******************************************************************************
* Function Name  : Ethernet_PhyInit
* Description    : Configures the Ethernet Interface
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static int32_t Ethernet_PhyInit( EMAC_PHY_CFG_Type* PhyCfg)
{
	return (int32_t)lpc_phy_init(pNetif);
}


/*******************************************************************************
* Function Name  : Ethernet_pfnFrameReceive
* Description    : Init_lwIP initialize the LwIP
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Ethernet_FrameReceive(uint16_t* pData, uint32_t size)
{
//	myprintf("ethernetif_input \n");
	ethernetif_input(pNetif);
}

#endif /* 0 */
