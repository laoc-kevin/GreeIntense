/**********************************************************************
* $Id$		lpc_emac_config.h			2011-11-20
*//**
* @file		lpc_emac_config.h
* @brief	PHY and EMAC configuration file
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

#ifndef __LPC_EMAC_CONFIG_H
#define __LPC_EMAC_CONFIG_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup lwip_phy_config	LWIP PHY configuration
 * @ingroup lwip_phy
 *
 * Configuration options for the PHY connected to the LPC EMAC.
 * @{
 */

/** \brief  Enable the DP83848 PHY.
 *          Only 1 PHY type can be selected in a system. Comment out
 *          the define to remove the PHY. Uncomment to enable it.
 */

#define LPC_PHY_DP83848	/**< Enable this define to use the DP83849 PHY driver. */


/** \brief  Enable the LAN8720 PHY.
 *          Only 1 PHY type can be selected in a system. Comment out
 *          the define to remove the PHY. Uncomment to enable it.
 */
 
//#define LPC_PHY_LAN8720	/**< Enable this define to use the LAN8720 PHY driver. */

/** \brief  The PHY address connected the to MII/RMII
 */
#define LPC_PHYDEF_PHYADDR 1    /**< The PHY address on the PHY device. */

/** \brief  Enable autonegotiation mode.
 *          If this is enabled, the PHY will attempt to auto-negotiate the
 *          best link mode if the PHY supports it. If this is not enabled,
 *          the PHY_USE_FULL_DUPLEX and PHY_USE_100MBS defines will be
 *          used to select the link mode. Note that auto-negotiation may
 *          take a few seconds to complete.
 */
#define PHY_USE_AUTONEG  1 /**< Enables auto-negotiation mode. */

/** \brief  Sets up the PHY interface to either full duplex operation or
 *          half duplex operation if PHY_USE_AUTONEG is not enabled.
 */
#define PHY_USE_FULL_DUPLEX  1 /**< Sets duplex mode to full. */

/** \brief  Sets up the PHY interface to either 100MBS operation or 10MBS
 *          operation if PHY_USE_AUTONEG is not enabled.
 */
#define PHY_USE_100MBS 1 /**< Sets data rate to 100Mbps. */

/** \brief  Auto-Negotiation Enabled - bits 8 and 13 of this register are ignored
 *          when this bit is set.         
 */


/**		  
 * @}
 */

/** @defgroup lwip_emac_config	LWIP EMAC configuration
 * @ingroup lwip_emac
 *
 * Configuration options for the LPC EMAC.
 * @{
 */

/** \brief  Selects RMII or MII connection type in the EMAC peripheral
 */
#define LPC_EMAC_RMII 1         /**< Use the RMII or MII driver variant .*/

/** \brief  Set this define to '0' to use traditional copied buffers. If set
 *          to 0, receive buffers will be allocated statically for the EMAC
 *          interface per descriptor (LPC_NUM_BUFF_RXDESCS) and then copied
 *          into another allocated buffer that is passed to the network layer.
 *          If set to 1, receive buffers are allocated when needed for an
 *          empty receive descriptor and then sent directly to the network
 *          layer without a copy when a packet is received. New buffers must
 *          be allocated and re-queued for receive by periodically calling
 *          the lpc_rx_requeue() function.
 */
#define LPC_PBUF_RX_ZEROCOPY 1 /**< Set to '1' to use zero-copy pbufs for receive. */

/** \brief  Defines the number of descriptors used for RX. A higher number of
 *          descriptors allows for more receive packets before rejecting them,
 *          but uses more memory. 1 buffer is allocated per descriptor. This
 *          must be a minimum value of 2.
 */
#define LPC_NUM_BUFF_RXDESCS 4

/** \brief  Set this define to '1' to enable zero-copy pbufs for transmit.
 *          If this is used, the EMAC driver will not create or use any local
 *          buffers - this can save lots of memory.
 */
#define LPC_PBUF_TX_ZEROCOPY 1 /**< Set to '1' to use no-copy pbufs for transmit. */

/** \brief  Defines the number of descriptors used for TX. In zero-copy mode,
 *          this value designates the number of descriptors that can be assigned
 *          to buffers for transmit. When not in zero-copy mode, a buffer is
 *          also assigned to the descriptor, so memory use can be high if this
 *          value is set high. Use a low count when LPC_PBUF_TX_ZEROCOPY=0. Must
 *          be a minimum value of 2.
 */
#define LPC_NUM_BUFF_TXDESCS 4

/** \brief  Set this define to 1 to enable bounce buffers for transmit pbufs
 *          that cannot be sent via the zero-copy method. Some chained pbufs
 *          may have a payload address that links to an area of memory that
 *          cannot be used for transmit DMA operations. If this define is
 *          set to 1, an extra check will be made with the pbufs. If a buffer
 *          is determined to be non-usable for zero-copy, a temporary bounce
 *          buffer will be created and used instead.
 */
#define LPC_TX_PBUF_BOUNCE_EN 0

/**		  
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LPC_EMAC_CONFIG_H */

/* --------------------------------- End Of File ------------------------------ */
