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

eMBErrorCode eMBCPNInit( UART_Def *Uart );
void            eMBCPNStart( void );
void            eMBCPNStop( void );
eMBErrorCode    eMBCPNReceive( UCHAR * pucSouAddress, UCHAR * pucDestAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    eMBCPNSend( UCHAR ucSourAddress, UCHAR ucDestAddress, const UCHAR * pucFrame, USHORT usLength );
BOOL            xMBCPNReceiveFSM( void );
BOOL            xMBCPNTransmitFSM( void );
BOOL            xMBCPNTimerT35Expired( void );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif