/**********************************************************************
* $Id$		lpc_phy_dp83848.c			2011-11-20
*//**
* @file		lpc_phy_dp83848.c
* @brief	DP83848C PHY status and control.
* @version	1.0
* @date		20 Nov. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/snmp.h"
#include "lpc_emac_config.h"
#include "lpc_emac.h"
#include "lpc_phy.h"
#include  <os.h>
#include "lpc_phy_dp83848.h"
#include "my_rtt_printf.h"
/** @defgroup dp83848_phy	PHY status and control for the DP83848.
 * @ingroup lwip_phy
 *
 * Various functions for controlling and monitoring the status of the
 * DP83848 PHY. In polled (standalone) systems, the PHY state must be
 * monitored as part of the application. In a threaded (RTOS) system,
 * the PHY state is monitored by the PHY handler thread. The MAC
 * driver will not transmit unless the PHY link is active.
 * @{
 */


/** \brief PHY status structure used to indicate current status of PHY.
 */
typedef struct {
	u32_t     phy_speed_100mbs:1; /**< 10/100 MBS connection speed flag. */
	u32_t     phy_full_duplex:1;  /**< Half/full duplex connection speed flag. */
	u32_t     phy_link_active:1;  /**< Phy link active flag. */
} PHY_STATUS_TYPE;

/** \brief  PHY update flags */
static PHY_STATUS_TYPE physts;

/** \brief  Last PHY update flags, used for determing if something has changed */
static PHY_STATUS_TYPE olddphysts;

/** \brief  PHY update counter for state machine */
static s32_t phyustate;

/** \brief  Update PHY status from passed value

    This function updates the current PHY status based on the
	passed PHY status word. The PHY status indicate if the link
	is active, the connection speed, and duplex.

	\param [in]    netif   NETIF structure
	\param [in]    linksts Status word from PHY
	\return        1 if the status has changed, otherwise 0
 */
static s32_t lpc_update_phy_sts(struct netif *netif, u32_t linksts)
{
	s32_t changed = 0;

	/* Update link active status */
	if (linksts & DP8_VALID_LINK)
		physts.phy_link_active = 1;
	else
		physts.phy_link_active = 0;

	/* Full or half duplex */
	if (linksts & DP8_FULLDUPLEX)
		physts.phy_full_duplex = 1;
	else
		physts.phy_full_duplex = 0;

	/* Configure 100MBit/10MBit mode. */
	if (linksts & DP8_SPEED10MBPS)
		physts.phy_speed_100mbs = 1;
	else
		physts.phy_speed_100mbs = 0;

	if (physts.phy_speed_100mbs != olddphysts.phy_speed_100mbs) {
		changed = 1;
		if (physts.phy_speed_100mbs) {
			/* 100MBit mode. */
			EMAC_SetPHYSpeed(1);

			NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);
		}
		else {
			/* 10MBit mode. */
			EMAC_SetPHYSpeed(0);

			NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 10000000);
		}

		olddphysts.phy_speed_100mbs = physts.phy_speed_100mbs;
	}

	if (physts.phy_full_duplex != olddphysts.phy_full_duplex) {
		changed = 1;
		if (physts.phy_full_duplex)
			EMAC_SetFullDuplexMode(1);
		else
			EMAC_SetFullDuplexMode(0);

		olddphysts.phy_full_duplex = physts.phy_full_duplex;
	}

	if (physts.phy_link_active != olddphysts.phy_link_active) {
		changed = 1;
		if (physts.phy_link_active)
			netif_set_link_up(netif);
		else
			netif_set_link_down(netif);

		olddphysts.phy_link_active = physts.phy_link_active;
	}

	return changed;
}

/** \brief  Initialize the DP83848 PHY.

    This function initializes the DP83848 PHY. It will block until
	complete. This function is called as part of the EMAC driver
	initialization. Configuration of the PHY at startup is
	controlled by setting up configuration defines in lpc_phy.h.

	\param [in]     netif   NETIF structure
	\return         ERR_OK if the setup was successful, otherwise ERR_TIMEOUT
 */

u32_t tmp;
err_t lpc_phy_init(struct netif *netif)
{
	
	s32_t i;
	OS_ERR err = OS_ERR_NONE;
	
	physts.phy_speed_100mbs = olddphysts.phy_speed_100mbs = 1;
	physts.phy_full_duplex  = olddphysts.phy_full_duplex = 1;
	physts.phy_link_active  = olddphysts.phy_link_active = 1;
	phyustate = 0;

	/* Only first read and write are checked for failure */
	/* Put the DP83848C in reset mode and wait for completion */
	
	EMAC_Write_PHY(DP8_BMCR_REG, DP8_RESET, &err);
		
	for (i= PHY_RESP_TOUT; i > 0; i--) 
	{	
		tmp = EMAC_Read_PHY(DP8_BMCR_REG, &err);
		if (err != 0)
		{
			return ERR_TIMEOUT;
		}
		if (!(tmp & (DP8_RESET | DP8_POWER_DOWN)))
		{
			myprintf("DP8_BMCR_REG = %u \n",tmp);
			myprintf("DP8_RESET \n");
			break;
		}
		
		/* Timeout*/
	    if (i == 0)
	    {
		    return ERR_TIMEOUT;
	    }
	}

	while(((EMAC_Read_PHY(DP8_BMSR_REG, &err)) & DP8_LINK_STATUS) == 0)
	{
		OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);   /* 1 s */
		
		myprintf("DP8_LINK_STATUS = %u \n",DP8_BMSR_REG);
	}
	
    tmp = EMAC_Read_PHY(DP8_BMCR_REG, &err);
		
	if (!(tmp & DP8_AUTONEG))
	{
		EMAC_Write_PHY(DP8_BMCR_REG, DP8_AUTONEG|DP8_SPEED_SELECT, &err);
	}	
	
	for (i= 0; i < PHY_RESP_TOUT; i++) 
	{
		tmp = EMAC_Read_PHY(DP8_BMSR_REG, &err);
		
        OSTimeDlyHMSM(0, 0, 0, 1, OS_OPT_TIME_HMSM_STRICT, &err);   /* 1 ms */	
		
		if (err != 0)
		{
			return ERR_TIMEOUT;
		}
		if (tmp & DP8_AUTONEG_COMP)
		{
			myprintf("DP8_AUTONEG_COMP = %u \n",tmp);
			break;
		}
	}
	
	/* No point in waiting around for the link to go active if
	   the cable is unplugged, so set the current link status and
	   exit. */
	
	for (i = 0; i < PHY_RESP_TOUT; i++) 
	{
   	    tmp = EMAC_Read_PHY(DP8_PHY_STAT_REG, &err);
        if (tmp & DP8_VALID_LINK/*0x0001*/) 
		{
			myprintf("DP8_VALID_LINK \n");
            break;
        }
    }
	lpc_update_phy_sts(netif, tmp);
	return ERR_OK;
}

/** \brief  Phy status update state machine

    This function provides a state machine for maintaining the PHY
	status without blocking. It must be occasionally called for the
	PHY status to be maintained.

	\param [in]     netif   NETIF structure
 */
s32_t lpc_phy_sts_sm(struct netif *netif)
{
	s32_t changed = 0;

	switch (phyustate) {
		default:
		case 0:
			/* Read BMSR to clear faults */
			EMAC_Read_PHY_Noblock(DP8_PHY_STAT_REG);
			phyustate = 1;
			break;

		case 1:
			/* Wait for read status state */
			if (!EMAC_IsMiiBusy()) 
			{
				/* Update PHY status */
				changed = lpc_update_phy_sts(netif, EMAC_ReadData());
				phyustate = 0;
			}
			break;
	}
	return changed;
}

/*********************************************************************//**
 * @brief		Set specified PHY mode in EMAC peripheral
 * @param[in]	ulPHYState	Specified PHY mode, should be:
 * 							- EMAC_MODE_AUTO
 * 							- EMAC_MODE_10M_FULL
 * 							- EMAC_MODE_10M_HALF
 * 							- EMAC_MODE_100M_FULL
 * 							- EMAC_MODE_100M_HALF
 * @return		Return (0) if no error, otherwise return (-1)
 **********************************************************************/
err_t lpc_set_phy_mode(uint32_t ulPHYMode)
{
	int32_t id1, id2, tout, regv;
    err_t err;
	
	/* Check if this is a DP83848C PHY. */
	id1 = EMAC_Read_PHY (DP8_PHY_REG_IDR1, &err);
	id2 = EMAC_Read_PHY (DP8_PHY_REG_IDR2, &err);

	if ((id1 == EMAC_PHY_ID1_CRIT) && ((id2 >> 4) == EMAC_PHY_ID2_CRIT))
	{
		/* Configure the PHY device */
		switch(ulPHYMode)
		{
			case EMAC_MODE_AUTO:
				/* Use auto-negotiation about the link speed. */
				EMAC_Write_PHY (DP8_BMCR_REG, EMAC_PHY_AUTO_NEG, &err);
				/* Wait to complete Auto_Negotiation */
				for (tout = EMAC_PHY_RESP_TOUT; tout; tout--)
				{
					regv = EMAC_Read_PHY (DP8_BMCR_REG, &err);

					if (regv & EMAC_PHY_BMSR_AUTO_DONE)
					{
						/* Auto-negotiation Complete. */
						break;
					}

					if (tout == 0)
					{
						// Time out, return error
						return (-1);
					}
				}
				break;

			case EMAC_MODE_10M_FULL:
				/* Connect at 10MBit full-duplex */
				EMAC_Write_PHY (DP8_BMCR_REG, DP8_10BASE_T_FD, &err);
				break;

			case EMAC_MODE_10M_HALF:
				/* Connect at 10MBit half-duplex */
				EMAC_Write_PHY (DP8_BMCR_REG, DP8_10BASE_T_HD, &err);
				break;

			case EMAC_MODE_100M_FULL:
				/* Connect at 100MBit full-duplex */
				EMAC_Write_PHY (DP8_BMCR_REG, DP8_100BASE_TX_FD, &err);
				break;

			case EMAC_MODE_100M_HALF:
				/* Connect at 100MBit half-duplex */
				EMAC_Write_PHY (DP8_BMCR_REG, DP8_100BASE_TX_HD, &err);
				break;

			default:
				// un-supported
				return (-1);
		}
	}
	// It's not correct module ID
	else
	{
		return (-1);
	}

	// Update EMAC configuration with current PHY status
	if (EMAC_UpdatePHYStatus() < 0)
	{
		return (-1);
	}

	// Complete
	return (0);
}



/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
