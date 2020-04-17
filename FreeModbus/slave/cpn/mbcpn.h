#ifndef _MB_CPN_H
#define _MB_CPN_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#include "port.h"
#include "mb.h"
#include "mbconfig.h"


#define MB_CPN_FUNC_WRITE_CODE 0x05
#define MB_CPN_FUNC_READ_CODE 0x06

#if MB_SLAVE_CPN_ENABLED > 0


typedef struct         /* CPN变量 */
{
	uint16_t Name;     //变量名称（ID）
	uint8_t  Type;     //变量类型       0：uint   6：float
	uint8_t  Length;   //数据长度
} eCPNValue;                    

eMBErrorCode    eMBSlaveCPNInit(sMBSlaveInfo* psMBSlaveInfo);
void            eMBSlaveCPNStart(sMBSlaveInfo* psMBSlaveInfo);
void            eMBSlaveCPNStop(sMBSlaveInfo* psMBSlaveInfo);

eMBErrorCode    eMBSlaveCPNReceive(sMBSlaveInfo* psMBSlaveInfo, UCHAR * pucSourAddr, 
                                   UCHAR * pucDestAddr, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    eMBSlaveCPNSend(sMBSlaveInfo* psMBSlaveInfo, UCHAR ucSourAddr, UCHAR ucDestAddr, 
                                const UCHAR * pucFrame, USHORT usLength );

BOOL            xMBSlaveCPNReceiveFSM( sMBSlaveInfo* psMBSlaveInfo );
BOOL            xMBSlaveCPNTransmitFSM( sMBSlaveInfo* psMBSlaveInfo );
BOOL            xMBSlaveCPNTimerT35Expired( sMBSlaveInfo* psMBSlaveInfo );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif