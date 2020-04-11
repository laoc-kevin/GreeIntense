#ifndef _MB_PORT_M_H
#define _MB_PORT_M_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#include "port.h"

/*! \ingroup modbus
 *  \brief TimerMode is Master 3 kind of Timer modes.
 */
typedef enum
{
	MB_TMODE_T35,                   /*!< Master receive frame T3.5 timeout. */
	MB_TMODE_RESPOND_TIMEOUT,       /*!< Master wait respond for slave. */
	MB_TMODE_CONVERT_DELAY          /*!< Master sent broadcast ,then delay sometime.*/
}eMBMasterTimerMode;

typedef enum
{
    EV_MASTER_READY                    = 1<<0,  /*!< Startup finished. */
    EV_MASTER_FRAME_RECEIVED           = 1<<1,  /*!< Frame received. */
    EV_MASTER_EXECUTE                  = 1<<2,  /*!< Execute function. */
    EV_MASTER_FRAME_SENT               = 1<<3,  /*!< Frame sent. */
    EV_MASTER_ERROR_PROCESS            = 1<<4,  /*!< Frame error process. */
    EV_MASTER_PROCESS_SUCCESS          = 1<<5,  /*!< Request process success. */
    EV_MASTER_ERROR_RESPOND_TIMEOUT    = 1<<6,  /*!< Request respond timeout. */
    EV_MASTER_ERROR_RECEIVE_DATA       = 1<<7,  /*!< Request receive data error. */
    EV_MASTER_ERROR_EXECUTE_FUNCTION   = 1<<8,  /*!< Request execute function error. */
	EV_MASTER_ERROR_RESPOND_DATA       = 1<<9,  /*!< Respond data error. */
} eMBMasterEventType;

typedef enum
{
    EV_ERROR_RESPOND_TIMEOUT,         /*!< Slave respond timeout. */
    EV_ERROR_RECEIVE_DATA,            /*!< Receive frame data error. */
    EV_ERROR_EXECUTE_FUNCTION,        /*!< Execute function error. */
	EV_ERROR_RESPOND_DATA,            /*!< Respond frame data error. */
} eMBMasterErrorEventType;

typedef struct
{
	const UART_Def* const psMBMasterUart;
	
	OS_TMR MasterPortTmr;
    OS_TMR ConvertDelayTmr;
    OS_TMR RespondTimeoutTmr;
	
	eMBMasterEventType eQueuedEvent;
	eMBMasterTimerMode  eMasterCurTimerMode;
	
	OS_SEM MasterEventSem;
    OS_SEM MasterErrorEventSem;
	
    BOOL   xEventInQueue;
    BOOL   xErrorEventInQueue;
	
    const  CHAR* const pcMasterPortName; 
}sMBMasterPortInfo;

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0 


/* -----------------------Master Serial port functions ----------------------------*/

BOOL xMBMasterPortSerialInit( sMBMasterPortInfo* psMBPortInfo );

void vMBMasterPortClose( sMBMasterPortInfo* psMBPortInfo );

void xMBMasterPortSerialClose( sMBMasterPortInfo* psMBPortInfo );

void vMBMasterPortSerialEnable( sMBMasterPortInfo* psMBPortInfo, BOOL xRxEnable, BOOL xTxEnable );

INLINE BOOL xMBMasterPortSerialGetByte( const sMBMasterPortInfo* psMBPortInfo, CHAR * pucByte );

INLINE BOOL xMBMasterPortSerialPutByte( sMBMasterPortInfo* psMBPortInfo, CHAR ucByte );

void prvvMasterUARTTxReadyISR( const CHAR* pcMBPortName );

void prvvMasterUARTRxISR( const CHAR* pcMBPortName );


/* -----------------------Master Serial port event functions ----------------------------*/

BOOL xMBMasterPortEventInit( sMBMasterPortInfo* psMBPortInfo );

BOOL xMBMasterPortEventPost( sMBMasterPortInfo* psMBPortInfo, eMBMasterEventType eEvent );

BOOL xMBMasterPortEventGet( sMBMasterPortInfo* psMBPortInfo, eMBMasterEventType* peEvent );

void vMBMasterOsResInit( void );

BOOL xMBMasterRunResTake( LONG lTimeOut );

void vMBMasterRunResRelease( void );


/* ----------------------- Timers functions ---------------------------------*/

BOOL xMBMasterPortTmrsInit(sMBMasterPortInfo* psMBPortInfo, USHORT usTim1Timerout50us);

void xMBMasterPortTmrsClose(sMBMasterPortInfo* psMBPortInfo);

INLINE void     vMBMasterPortTmrsEnable( sMBMasterPortInfo* psMBPortInfo );

INLINE void     vMBMasterPortTmrsConvertDelayEnable( sMBMasterPortInfo* psMBPortInfo );

INLINE void     vMBMasterPortTmrsRespondTimeoutEnable( sMBMasterPortInfo* psMBPortInfo );

INLINE void     vMBMasterPortTmrsDisable( sMBMasterPortInfo* psMBPortInfo );


/* ----------------- Callback for the master error process ------------------*/

void vMBMasterErrorCBRespondTimeout( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
	                                 const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterErrorCBReceiveData( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
	                              const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterErrorCBExecuteFunction( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
                                      const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterErrorCBRespondData( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
                                  const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterCBRequestSuccess( sMBMasterPortInfo* psMBPortInfo );

#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif