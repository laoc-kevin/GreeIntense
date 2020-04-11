/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent_m.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/

#include "mbconfig.h"
#include "mb_m.h"
#include "mbport_m.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Defines ------------------------------------------*/


/* ----------------------- Variables ----------------------------------------*/


/* ----------------------- Start implementation -----------------------------*/

/**********************************************************************
 * @brief  modbus协议栈事件初始化函数
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL
xMBMasterPortEventInit( sMBMasterPortInfo* psMBPortInfo )
{
	OS_ERR err = OS_ERR_NONE;
	
	OSSemCreate( &(psMBPortInfo->MasterEventSem), "MasterEventSem", 0, &err );             //主栈事件消息量
    OSSemCreate( &(psMBPortInfo->MasterEventSem), "MasterErrorEventSem", 0, &err );   //主栈错误消息量
	
	psMBPortInfo->xEventInQueue = FALSE;
	psMBPortInfo->xErrorEventInQueue = FALSE;
    return TRUE;
}

/**********************************************************************
 * @brief  modbus主栈事件发送
 * @param  eEvent  当前事件
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL
xMBMasterPortEventPost( sMBMasterPortInfo* psMBPortInfo, eMBMasterEventType eEvent )
{
	OS_ERR err = OS_ERR_NONE;
    psMBPortInfo->xEventInQueue = TRUE;
    psMBPortInfo->eQueuedEvent = eEvent;
	(void)OSSemPost( &(psMBPortInfo->MasterEventSem), OS_OPT_POST_ALL, &err );
    return TRUE;
}

/**********************************************************************
 * @brief  获取modbus主栈最新的事件
 * @param  eEvent  当前事件
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL
xMBMasterPortEventGet( sMBMasterPortInfo* psMBPortInfo, eMBMasterEventType * eEvent )
{
    BOOL xEventHappened = FALSE;
	CPU_TS ts = 0;
    OS_ERR err = OS_ERR_NONE;
	
    (void)OSSemPend( &(psMBPortInfo->MasterEventSem), 0, OS_OPT_PEND_BLOCKING, &ts, &err );
	(void)OSSemSet( &(psMBPortInfo->MasterEventSem), 0, &err);
	
    if( psMBPortInfo->xEventInQueue )
    {
		switch(psMBPortInfo->eQueuedEvent)
		{
		case EV_MASTER_READY:
			*eEvent = EV_MASTER_READY;
			break;
		case EV_MASTER_FRAME_RECEIVED:
			*eEvent = EV_MASTER_FRAME_RECEIVED;
			break;
		case EV_MASTER_EXECUTE:
			*eEvent = EV_MASTER_EXECUTE;
			break;
		case EV_MASTER_FRAME_SENT:
			*eEvent = EV_MASTER_FRAME_SENT;
			break;
		case EV_MASTER_ERROR_PROCESS:
			*eEvent = EV_MASTER_ERROR_PROCESS;
			break;
		case EV_MASTER_ERROR_RECEIVE_DATA:
			*eEvent = EV_MASTER_ERROR_RECEIVE_DATA;
			break;
		case EV_MASTER_ERROR_RESPOND_DATA:
			*eEvent = EV_MASTER_ERROR_RESPOND_DATA;
			break;
		default: break;
		}
        psMBPortInfo->xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }

    return xEventHappened;
}
/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMasterOsResInit( void )
{
   
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
BOOL xMBMasterRunResTake( LONG lTimeOut )
{
    /*If waiting time is 0 .It will wait forever */
//	usTimeOut = lTimeOut;
    return  TRUE ;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMasterRunResRelease( void )
{
    /* release resource */
   
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
	                                 const UCHAR* pucPDUData, USHORT ucPDULength ) 
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
	 OS_ERR err = OS_ERR_NONE;	
    (void)xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_ERROR_RESPOND_TIMEOUT );
	psMBPortInfo->xErrorEventInQueue = TRUE;
	
	(void)OSSemPost( &(psMBPortInfo->MasterEventSem), OS_OPT_POST_ALL, &err );	
	
    /* You can add your code under here. */
}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBReceiveData( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
	                              const UCHAR* pucPDUData, USHORT ucPDULength ) 
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    OS_ERR err = OS_ERR_NONE;
    (void)xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_ERROR_RECEIVE_DATA );
	psMBPortInfo->xErrorEventInQueue = TRUE;
    (void)OSSemPost( &(psMBPortInfo->MasterEventSem), OS_OPT_POST_ALL, &err );
    /* You can add your code under here. */
   
}
		
void vMBMasterErrorCBRespondData( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
                                  const UCHAR* pucPDUData, USHORT ucPDULength ) 
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    OS_ERR err = OS_ERR_NONE;
    (void)xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_ERROR_RESPOND_DATA );
	
    vMBMasterPortTmrsRespondTimeoutEnable(psMBPortInfo);
			
//	xErrorEventInQueue = TRUE;
//    (void)OSSemPost(&MasterErrorEventSem, OS_OPT_POST_ALL, &err);
    /* You can add your code under here. */
   
}


/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction( sMBMasterPortInfo* psMBPortInfo, UCHAR ucDestAddress, 
                                      const UCHAR* pucPDUData, USHORT ucPDULength ) 
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    OS_ERR err = OS_ERR_NONE;
    (void)xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_ERROR_EXECUTE_FUNCTION );
	psMBPortInfo->xErrorEventInQueue = TRUE;		
    (void)OSSemPost( &(psMBPortInfo->MasterEventSem), OS_OPT_POST_ALL, &err );
    /* You can add your code under here. */
   
}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 */
void vMBMasterCBRequestSuccess( sMBMasterPortInfo* psMBPortInfo ) 
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    OS_ERR err = OS_ERR_NONE;
    (void)xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_PROCESS_SUCCESS );
    psMBPortInfo->xErrorEventInQueue = TRUE;
    (void)OSSemPost( &(psMBPortInfo->MasterEventSem), OS_OPT_POST_ALL, &err);
	
    /* You can add your code under here. */
}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish( sMBMasterPortInfo* psMBPortInfo ) 
{
    CPU_TS ts = 0;
    OS_ERR err = OS_ERR_NONE;
    eMBMasterReqErrCode    eErrStatus = MB_MRE_NO_ERR;	

	(void)OSSemPend( &(psMBPortInfo->MasterEventSem), 0, OS_OPT_PEND_BLOCKING, &ts, &err );
	(void)OSSemSet( &(psMBPortInfo->MasterEventSem), 0, &err );
	if( psMBPortInfo->xErrorEventInQueue )
    {
		switch(psMBPortInfo->eQueuedEvent)
		{
		case EV_MASTER_PROCESS_SUCCESS:
			break;
		case EV_MASTER_ERROR_RESPOND_TIMEOUT:
		{
			eErrStatus = MB_MRE_TIMEDOUT;
			break;
		}
		case EV_MASTER_ERROR_RECEIVE_DATA:
		{
			eErrStatus = MB_MRE_REV_DATA;
			break;
		}
		case EV_MASTER_ERROR_EXECUTE_FUNCTION:
		{
			eErrStatus = MB_MRE_EXE_FUN;
			break;
		}
				
		default:	
			break;
		}
//		(void)OSSemPost(&MasterIdleSem, OS_OPT_POST_ALL, &err);
	}
//   myprintf("eMBMasterWaitRequestFinish\n" );
	psMBPortInfo->xErrorEventInQueue = FALSE;

    return eErrStatus;
}

/**********************************************************************
 * @brief  返回modbus主栈最新的事件
 * @param  eEvent  当前事件
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBMasterEventType xMBMasterPortCurrentEvent( const sMBMasterPortInfo* psMBPortInfo )
{
	return psMBPortInfo->eQueuedEvent;       	
}

#endif
