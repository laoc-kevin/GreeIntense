/****************************************Copyright (c)**************************************************                         

**--------------File Info-------------------------------------------------------------------------------
** File name:			lwipconfig.h
** Descriptions:		lwip
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-3-1
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

#ifndef __LWIPCONFIG_H
#define __LWIPCONFIG_H

/* Includes ------------------------------------------------------------------*/

/* 定义MAC地址 */   
#define emacETHADDR0    0x00
#define emacETHADDR1    0x24
#define emacETHADDR2    0x01
#define emacETHADDR3    0x14
#define emacETHADDR4    0x7f
#define emacETHADDR5    0xbe
 
/* 定义IP地址 */
#define	 emacIPADDR0  192
#define	 emacIPADDR1  168
#define	 emacIPADDR2  1
#define	 emacIPADDR3  202

/* 定义网关地址 */
#define emacGATEWAY_ADDR0  192
#define emacGATEWAY_ADDR1  168
#define emacGATEWAY_ADDR2  1
#define emacGATEWAY_ADDR3  254

/* 定义掩码地址 */
#define emacNET_MASK0  255
#define emacNET_MASK1  255
#define emacNET_MASK2  255
#define emacNET_MASK3  0

/* Definitions */
#define ETH_ADRLEN      6         /* Ethernet Address Length in bytes        */
#define IP_ADRLEN       4         /* IP Address Length in bytes              */
#define OS_HEADER_LEN   4         /* TCPnet 'os_frame' header size           */
                                  /* Frame Header length common for all      */
#define PHY_HEADER_LEN  (2*ETH_ADRLEN + 2) /* network interfaces.            */
#define ETH_MTU         1514      /* Ethernet Frame Max Transfer Unit        */
#define PPP_PROT_IP     0x0021    /* PPP Protocol type: IP                   */
#define TCP_DEF_WINSIZE 4380      /* TCP default window size                 */
#define PASSW_SZ        20        /* Authentication Password Buffer size     */

/* Network Interfaces */
#define NETIF_ETH       0         /* Network interface: Ethernet             */
#define NETIF_PPP       1         /* Network interface: PPP                  */
#define NETIF_SLIP      2         /* Network interface: Slip                 */
#define NETIF_LOCAL     3         /* Network interface: Localhost (loopback) */
#define NETIF_NULL      4         /* Network interface: Null (none)          */

/* Telnet Definitions */
#define TNET_LBUFSZ     96        /* Command Line buffer size (bytes)        */
#define TNET_HISTSZ     128       /* Command History buffer size (bytes)     */
#define TNET_FIFOSZ     128       /* Input character Fifo buffer (bytes)     */

/* SNMP-MIB Definitions */
#define MIB_INTEGER     0x02      /* MIB entry type INTEGER                  */
#define MIB_OCTET_STR   0x04      /* MIB entry type OCTET_STRING             */
#define MIB_OBJECT_ID   0x06      /* MIB entry type OBJECT_IDENTIFIER        */
#define MIB_IP_ADDR     0x40      /* MIB entry type IP ADDRESS (U8[4])       */
#define MIB_COUNTER     0x41      /* MIB entry type COUNTER (U32)            */
#define MIB_GAUGE       0x42      /* MIB entry type GAUGE (U32)              */
#define MIB_TIME_TICKS  0x43      /* MIB entry type TIME_TICKS               */
#define MIB_ATR_RO      0x80      /* MIB entry attribute READ_ONLY           */
#define MIB_OIDSZ       17        /* Max.size of Object ID value             */
#define MIB_STRSZ       110       /* Max.size of Octet String variable       */
#define MIB_READ        0         /* MIB entry Read access                   */
#define MIB_WRITE       1         /* MIB entry Write access                  */

/* SNMP-MIB Macros */
#define MIB_STR(s)      sizeof(s)-1, s
#define MIB_INT(o)      sizeof(o), (void *)&o
#define MIB_IP(ip)      4, (void *)&ip 
#define OID0(f,s)       (f*40 + s) 

/* Debug Module Definitions */
#define MODULE_MEM      0         /* Dynamic Memory Module ID                */
#define MODULE_ETH      1         /* Ethernet Module ID                      */
#define MODULE_PPP      2         /* PPP Module ID                           */
#define MODULE_SLIP     3         /* SLIP Module ID                          */
#define MODULE_ARP      4         /* ARP Module ID                           */
#define MODULE_IP       5         /* IP Module ID                            */
#define MODULE_ICMP     6         /* ICMP Module ID                          */
#define MODULE_IGMP     7         /* IGMP Module ID                          */
#define MODULE_UDP      8         /* UDP Module ID                           */
#define MODULE_TCP      9         /* TCP Module ID                           */
#define MODULE_NBNS     10        /* NBNS Module ID                          */
#define MODULE_DHCP     11        /* DHCP Module ID                          */
#define MODULE_DNS      12        /* DNS Module ID                           */
#define MODULE_SNMP     13        /* SNMP Module ID                          */
#define MODULE_BSD      14        /* BSD Socket Module ID                    */
#define MODULE_HTTP     15        /* HTTP Server Module ID                   */
#define MODULE_FTP      16        /* FTP Server Module ID                    */
#define MODULE_FTPC     17        /* FTP Client Module ID                    */
#define MODULE_TNET     18        /* Telnet Server Module ID                 */
#define MODULE_TFTP     19        /* TFTP Server Module ID                   */
#define MODULE_TFTPC    20        /* TFTP Client Module ID                   */
#define MODULE_SMTP     21        /* SMTP Client Module ID                   */

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
