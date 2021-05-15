#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "err.h"
#include "netif.h"
#include "stdint.h"


struct netif *ethernetif_register(void);
int ethernetif_poll(void);
void Set_MAC_Address(unsigned char* macadd);
err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);

/* DP83848C PHY Registers */
#define PHY_REG_BMCR        0x00        /* Basic Mode Control Register       */
#define PHY_REG_BMSR        0x01        /* Basic Mode Status Register        */
#define PHY_REG_IDR1        0x02        /* PHY Identifier 1                  */
#define PHY_REG_IDR2        0x03        /* PHY Identifier 2                  */
#define PHY_REG_ANAR        0x04        /* Auto-Negotiation Advertisement    */
#define PHY_REG_ANLPAR      0x05        /* Auto-Neg. Link Partner Abitily    */
#define PHY_REG_ANER        0x06        /* Auto-Neg. Expansion Register      */
#define PHY_REG_ANNPTR      0x07        /* Auto-Neg. Next Page TX            */

#endif 
