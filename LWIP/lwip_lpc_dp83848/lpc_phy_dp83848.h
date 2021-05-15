#ifndef _LPC_PHY_DP83848_H_
#define _LPC_PHY_DP83848_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** PHY device reset time out definition */
#define PHY_RESP_TOUT		0x400UL

/** \brief  DP83848 PHY register offsets */
#define DP8_BMCR_REG        0x00  /**< Basic Mode Control Register */
#define DP8_BMSR_REG        0x01  /**< Basic Mode Status Reg */
#define DP8_PHY_REG_IDR1    0x02  /**< PHY Identifier 1*/
#define DP8_PHY_REG_IDR2	0x03  /**< PHY Identifier 2*/
#define DP8_ANADV_REG       0x04  /**< Auto_Neg Advt Reg  */
#define DP8_ANLPA_REG       0x05  /**< Auto_neg Link Partner Ability Reg */
#define DP8_ANEEXP_REG      0x06  /**< Auto-neg Expansion Reg  */
#define DP8_PHY_STAT_REG    0x10  /**< PHY Status Register  */
#define DP8_PHY_INT_CTL_REG 0x11  /**< PHY Interrupt Control Register */
#define DP8_PHY_STS_REG     0x19  /**< PHY Status Register  */

/** \brief DP83848 Control register definitions */
#define DP8_RESET          (1 << 15)  /**< 1= S/W Reset */
#define DP8_LOOPBACK       (1 << 14)  /**< 1=loopback Enabled */
#define DP8_SPEED_SELECT   (1 << 13)  /**< 1=Select 100MBps */
#define DP8_AUTONEG        (1 << 12)  /**< 1=Enable auto-negotiation */
#define DP8_POWER_DOWN     (1 << 11)  /**< 1=Power down PHY */
#define DP8_ISOLATE        (1 << 10)  /**< 1=Isolate PHY */
#define DP8_RESTART_AUTONEG (1 << 9)  /**< 1=Restart auto-negoatiation */
#define DP8_DUPLEX_MODE    (1 << 8)   /**< 1=Full duplex mode */
#define DP8_COLLISION_TEST (1 << 7)   /**< 1=Perform collsion test */

/** \brief DP83848 Status register definitions */
#define DP8_100BASE_T4     (1 << 15)  /**< T4 mode */
#define DP8_100BASE_TX_FD  (1 << 14)  /**< 100MBps full duplex */
#define DP8_100BASE_TX_HD  (1 << 13)  /**< 100MBps half duplex */
#define DP8_10BASE_T_FD    (1 << 12)  /**< 100Bps full duplex */
#define DP8_10BASE_T_HD    (1 << 11)  /**< 10MBps half duplex */
#define DP8_MF_PREAMB_SUPPR (1 << 6)  /**< Preamble suppress */
#define DP8_AUTONEG_COMP   (1 << 5)   /**< Auto-negotation complete */
#define DP8_RMT_FAULT      (1 << 4)   /**< Fault */
#define DP8_AUTONEG_ABILITY (1 << 3)  /**< Auto-negotation supported */
#define DP8_LINK_STATUS    (1 << 2)   /**< 1=Link active */
#define DP8_JABBER_DETECT  (1 << 1)   /**< Jabber detect */
#define DP8_EXTEND_CAPAB   (1 << 0)   /**< Supports extended capabilities */

/** \brief DP83848 PHY status definitions */
#define DP8_REMOTEFAULT    (1 << 6)   /**< Remote fault */
#define DP8_FULLDUPLEX     (1 << 2)   /**< 1=full duplex */
#define DP8_SPEED10MBPS    (1 << 1)   /**< 1=10MBps speed */
#define DP8_VALID_LINK     (1 << 0)   /**< 1=Link active */

/** \brief DP83848 PHY ID register definitions */
#define DP8_PHYID1_OUI     0x2000     /**< Expected PHY ID1 */
#define DP8_PHYID2_OUI     0x5c90     /**< Expected PHY ID2 */

//The Common Registers that are using in all PHY IC with EMAC component of LPC1788
#define EMAC_PHY_REG_BMCR					DP8_BMCR_REG
#define EMAC_PHY_REG_BMSR					DP8_BMSR_REG
#define EMAC_PHY_REG_IDR1					DP8_PHY_REG_IDR1
#define EMAC_PHY_REG_IDR2					DP8_PHY_REG_IDR2

#define EMAC_PHY_BMCR_RESET					DP8_RESET
#define EMAC_PHY_BMCR_POWERDOWN				DP8_POWER_DOWN
#define EMAC_PHY_BMCR_SPEED_SEL     	    DP8_SPEED_SELECT
#define EMAC_PHY_BMCR_DUPLEX	     		DP8_DUPLEX_MODE
#define EMAC_PHY_BMCR_AN		     		DP8_AUTONEG


#define EMAC_PHY_BMSR_100BT4        	   	DP8_100BASE_T4
#define EMAC_PHY_BMSR_100TX_FULL			DP8_100BASE_TX_FD
#define EMAC_PHY_BMSR_100TX_HALF			DP8_100BASE_TX_HD
#define EMAC_PHY_BMSR_10BT_FULL				DP8_10BASE_T_FD
#define EMAC_PHY_BMSR_10BT_HALF				DP8_10BASE_T_HD
#define EMAC_PHY_BMSR_MF_PREAM				DP8_MF_PREAMB_SUPPR
#define EMAC_PHY_BMSR_REMOTE_FAULT			DP8_RMT_FAULT
#define EMAC_PHY_BMSR_LINK_ESTABLISHED		DP8_LINK_STATUS


//#define DP83848_PHY_ID1				(0x2000)

//#define DP83848_PHY_ID2_OUI			(0x0017) //Organizationally Unique Identifer Number
//#define DP83848_PHY_ID2_MANF_MODEL		(0x0009) //Manufacturer Model Number
//#define DP83848_PHY_ID2_CRIT			(((DP83848_PHY_ID2_OUI & 0x3F) << 6) | (DP83848_PHY_ID2_MANF_MODEL & 0x3F))

#define EMAC_PHY_ID1_CRIT				(DP8_PHYID1_OUI)
#define EMAC_PHY_ID2_CRIT				(DP8_PHYID2_OUI)



#ifdef __cplusplus
}
#endif

#endif

	
	