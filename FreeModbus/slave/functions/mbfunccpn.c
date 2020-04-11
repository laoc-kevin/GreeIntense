/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbfuncholding.c,v 1.12 2007/02/18 23:48:22 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"
#include "app_led.h"

/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/

/***********************************************************************************
 * @brief 写CPN变量功能函数
 * @param pucFrame       Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
#if MB_FUNC_CPN_WRITE_ENABLED > 0

eMBException
eMBFuncWriteCPNValue( UCHAR * pucFrame, USHORT * usLen )
{
	UCHAR          ucValCount;
    UCHAR          *pucFrameCur;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;
	
	if( (*usLen >= ( MB_CPN_PDU_SIZE_MIN )) 
		&& (*usLen <= ( MB_CPN_PDU_SIZE_MAX )) )
    {
	    ucValCount = ( UCHAR )( *(pucFrame + MB_CPN_PDU_VALUE_COUNT_OFF) );
       	
		/* Set the current PDU data pointer to the beginning. */
        pucFrameCur = pucFrame;
        *usLen = MB_CPN_PDU_FUNC_OFF;
		
		/* First byte contains the function code. */
        *pucFrameCur++ = MB_FUNC_CPN_WRITE - MB_CPN_FUNC_CODE_OFF_TO_REAL;
        *usLen += 1;
		
		/* Second byte in the response contain the number of values. */
        *pucFrameCur++ = ucValCount;
        *usLen += 1;
		
		/* The source address and destination address should exchange in the response  */
		
//		/* Third byte in the response contains the source address.*/ 
//        *pucFrameCur++ = ucDestAddr;
//        *usLen += 1;
//		
//		/* Four byte in the response contains the destination address.*/ 
//        *pucFrameCur++ = ucSourAddr;

        *usLen += 2;
		pucFrameCur += 2;
		
		/* Make callback to fill the buffer. */
        eRegStatus = eMBWriteCPNCB( pucFrameCur, ucValCount, usLen );

        /* If an error occured convert it into a Modbus exception. */
		if( eRegStatus != MB_ENOERR )
		{
			eStatus = prveMBError2Exception( eRegStatus );
		}
		else
		{
			
		}
	}
	else
    {
        /* Can't be a valid request because the length is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif

/***********************************************************************************
 * @brief 读CPN变量功能函数
 * @param pucFrame       Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/

#if MB_FUNC_CPN_READ_ENABLED > 0
eMBException
eMBFuncReadCPNValue( UCHAR * pucFrame, USHORT * usLen )
{
	UCHAR          ucValCount; 	
    UCHAR          *pucFrameCur;
    USHORT         usDatabytes;
    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;
	
	if( (*usLen >= ( MB_CPN_PDU_SIZE_MIN )) 
		&& (*usLen <= ( MB_CPN_PDU_SIZE_MAX )) )
    {
	    ucValCount = ( UCHAR )( *(pucFrame + MB_CPN_PDU_VALUE_COUNT_OFF) );
    
		/* Set the current PDU data pointer to the beginning. */
        pucFrameCur = pucFrame;
        *usLen = MB_CPN_PDU_FUNC_OFF;
		
		/* First byte contains the function code. */
        *pucFrameCur++ = MB_FUNC_CPN_READ - MB_CPN_FUNC_CODE_OFF_TO_REAL;
        *usLen += 1;
		
		/* Second byte in the response contain the number of values. */
        *pucFrameCur++ = ucValCount;
        *usLen += 1;
		
//		/* The source address and destination address should exchange in the response  */
//		
//		/* Third byte in the response contains the source address.*/ 
//        *pucFrameCur++ = ucDestAddr;
//        *usLen += 1;
//		
//		/* Four byte in the response contains the destination address.*/ 
//        *pucFrameCur++ = ucSourAddr;
        
		*usLen += 2;
		pucFrameCur += 2;
		
		/* Make callback to fill the buffer. */
        eRegStatus = eMBReadCPNCB( pucFrameCur, ucValCount, usLen);
		
         /* If an error occured convert it into a Modbus exception. */
		if( eRegStatus != MB_ENOERR )
		{
			eStatus = prveMBError2Exception( eRegStatus );
		}	
	}
	else
    {
        /* Can't be a valid request because the length is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
	
	
	
}

#endif
