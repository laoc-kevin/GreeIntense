/**********************************************************************
* $Id$		lpc_phy.h			2011-11-20
*//**
* @file		lpc_phy.h
* @brief	Common PHY definitions used with all PHYs
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

#ifndef __LPC_PHY_H_
#define __LPC_PHY_H_

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lpc_emac.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LPC_PHY_DP83848	
//#include "lpc_phy_dp83848.h"
#endif

#ifdef LPC_PHY_LAN8720
/* Build the DP83848 variant of the PHY support driver */
//#include "lpc_phy_lan8720.h"
#endif	
	
s32_t lpc_phy_sts_sm(struct netif *netif);
err_t lpc_phy_init(struct netif *netif);

/* These PHY functions are usually part of the EMAC driver */
void lpc_mii_write_noblock(u32_t PhyReg, u32_t Value);
err_t lpc_mii_write(u32_t PhyReg, u32_t Value);
u32_t lpc_mii_is_busy(void);
u32_t lpc_mii_read_data(void);
err_t lpc_mii_read(u32_t PhyReg, u32_t *data);
void lpc_mii_read_noblock(u32_t PhyReg);
void lpc_emac_set_duplex(int full_duplex);
void lpc_emac_set_speed(int mbs_100);

#ifdef __cplusplus
}
#endif

#endif /* __LPC_PHY_H_ */

/* --------------------------------- End Of File ------------------------------ */
