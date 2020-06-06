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

typedef struct                                /* 主栈接口定义  */
{
	const sUART_Def* psMBMasterUart;               //主栈通讯串口结构
	
	OS_TMR sMasterPortTmr;                        //主栈3.5字符间隔定时器
    OS_TMR sConvertDelayTmr;                      //主栈转换延时定时器
    OS_TMR sRespondTimeoutTmr;                    //主栈等待响应定时器
	
	eMBMasterEventType  eQueuedEvent;             //主栈事件
	eMBMasterTimerMode  eCurTimerMode;            //当前定时器模式
	
    OS_SEM sMBIdleSem;                            //主栈空闲消息量
	OS_SEM sMBEventSem;                           //主栈事件消息量
    OS_SEM sMBWaitFinishSem;                      //主栈等待消息量
	
    BOOL   xEventInQueue;                         //主栈有新事件
    BOOL   xWaitFinishInQueue;                    //主栈有新错误事件
	
    const  CHAR* pcMBPortName;                    //主栈接口名称
}sMBMasterPort;

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0 


/* -----------------------Master Serial port functions ----------------------------*/

BOOL xMBMasterPortSerialInit( sMBMasterPort* psMBPort );

void vMBMasterPortClose( sMBMasterPort* psMBPort );

void xMBMasterPortSerialClose( sMBMasterPort* psMBPort );

void vMBMasterPortSerialEnable( sMBMasterPort* psMBPort, BOOL xRxEnable, BOOL xTxEnable );

INLINE BOOL xMBMasterPortSerialGetByte( const sMBMasterPort* psMBPort, CHAR * pucByte );

INLINE BOOL xMBMasterPortSerialPutByte( sMBMasterPort* psMBPort, CHAR ucByte );

void prvvMasterUARTTxReadyISR( const CHAR* pcMBPortName );

void prvvMasterUARTRxISR( const CHAR* pcMBPortName );


/* -----------------------Master Serial port event functions ----------------------------*/

BOOL xMBMasterPortEventInit( sMBMasterPort* psMBPort );

BOOL xMBMasterPortEventPost( sMBMasterPort* psMBPort, eMBMasterEventType eEvent );

BOOL xMBMasterPortEventGet( sMBMasterPort* psMBPort, eMBMasterEventType* peEvent );

void vMBMasterOsResInit( void );

BOOL xMBMasterRunResTake( LONG lTimeOut );

void vMBMasterRunResRelease( void );


/* ----------------------- Timers functions ---------------------------------*/

BOOL xMBsMasterPortTmrsInit(sMBMasterPort* psMBPort, USHORT usTim1Timerout50us);

void xMBsMasterPortTmrsClose(sMBMasterPort* psMBPort);

INLINE void     vMBsMasterPortTmrsEnable( sMBMasterPort* psMBPort );

INLINE void     vMBsMasterPortTmrsConvertDelayEnable( sMBMasterPort* psMBPort );

INLINE void     vMBsMasterPortTmrsRespondTimeoutEnable( sMBMasterPort* psMBPort );

INLINE void     vMBsMasterPortTmrsDisable( sMBMasterPort* psMBPort );


/* ----------------- Callback for the master error process ------------------*/

void vMBMasterErrorCBRespondTimeout( sMBMasterPort* psMBPort, UCHAR ucDestAddr, 
	                                 const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterErrorCBReceiveData( sMBMasterPort* psMBPort, UCHAR ucDestAddr, 
	                              const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterErrorCBExecuteFunction( sMBMasterPort* psMBPort, UCHAR ucDestAddr, 
                                      const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterErrorCBRespondData( sMBMasterPort* psMBPort, UCHAR ucDestAddr, 
                                  const UCHAR* pucPDUData, USHORT ucPDULength );

void vMBMasterCBRequestSuccess( sMBMasterPort* psMBPort );

#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif