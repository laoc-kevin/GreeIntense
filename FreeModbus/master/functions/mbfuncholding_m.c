/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
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
 * File: $Id: mbfuncholding_m.c,v 1.60 2013/09/02 14:13:40 Armink Add Master Functions  Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb_m.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"
#include "mbfunc_m.h"
#include "mbutils_m.h"
#include "mbdict_m.h"
#include "mbmap_m.h"
#include "mbscan_m.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_REQ_READ_ADDR_OFF                ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_REQ_READ_REGCNT_OFF              ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_REQ_READ_SIZE                    ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX             ( 0x007D )
#define MB_PDU_FUNC_READ_BYTECNT_OFF            ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_READ_VALUES_OFF             ( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_READ_SIZE_MIN               ( 1 )

#define MB_PDU_REQ_WRITE_ADDR_OFF               ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_REQ_WRITE_VALUE_OFF              ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_REQ_WRITE_SIZE                   ( 4 )
#define MB_PDU_FUNC_WRITE_ADDR_OFF              ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_WRITE_VALUE_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE                  ( 4 )

#define MB_PDU_REQ_WRITE_MUL_ADDR_OFF           ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_REQ_WRITE_MUL_REGCNT_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_REQ_WRITE_MUL_BYTECNT_OFF        ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_REQ_WRITE_MUL_VALUES_OFF         ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_REQ_WRITE_MUL_SIZE_MIN           ( 5 )
#define MB_PDU_REQ_WRITE_MUL_REGCNT_MAX         ( 0x0078 )
#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF          ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE              ( 4 )

#define MB_PDU_REQ_READWRITE_READ_ADDR_OFF      ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_REQ_READWRITE_READ_REGCNT_OFF    ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_REQ_READWRITE_WRITE_ADDR_OFF     ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF   ( MB_PDU_DATA_OFF + 6 )
#define MB_PDU_REQ_READWRITE_WRITE_BYTECNT_OFF  ( MB_PDU_DATA_OFF + 8 )
#define MB_PDU_REQ_READWRITE_WRITE_VALUES_OFF   ( MB_PDU_DATA_OFF + 9 )
#define MB_PDU_REQ_READWRITE_SIZE_MIN           ( 9 )
#define MB_PDU_FUNC_READWRITE_READ_BYTECNT_OFF  ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_READWRITE_READ_VALUES_OFF   ( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_READWRITE_SIZE_MIN          ( 1 )

/* ----------------------- Start implementation -----------------------------*/
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0

 /***********************************************************************************
 * @brief  主栈写单个保持寄存器
 * @param  ucSndAddr      从栈地址
 * @param  usRegAddr      起始地址
 * @param  usRegData      数值
 * @param  lTimeOut       主栈等待时间（0则一直等待）
 * @return error          错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode
eMBMasterReqWriteHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, USHORT usRegAddr, 
                                  USHORT usRegData, LONG lTimeOut )
{
    UCHAR                 *ucMBFrame;
	
    eMBMasterReqErrCode     eErrStatus = MB_MRE_NO_ERR;
    sMBMasterPortInfo*    psMBPortInfo = &psMBMasterInfo->sMBPortInfo;      //硬件结构
	sMBMasterDevsInfo*    psMBDevsInfo = &psMBMasterInfo->sMBDevsInfo;    //从设备状态信息
	
    if( (ucSndAddr < psMBDevsInfo->ucSlaveDevMinAddr) || (ucSndAddr > psMBDevsInfo->ucSlaveDevMaxAddr) ) 
	{
		eErrStatus = MB_MRE_ILL_ARG;
	}		
    else if ( xMBMasterRunResTake( lTimeOut ) == FALSE ) 
	{
		eErrStatus = MB_MRE_MASTER_BUSY;
	}
    else
    {
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
		vMBMasterSetDestAddress(psMBMasterInfo, ucSndAddr);
		*( ucMBFrame + MB_PDU_FUNC_OFF )                = MB_FUNC_WRITE_REGISTER;
		*( ucMBFrame + MB_PDU_REQ_WRITE_ADDR_OFF )      = usRegAddr >> 8;
		*( ucMBFrame + MB_PDU_REQ_WRITE_ADDR_OFF + 1 )  = usRegAddr;
		*( ucMBFrame + MB_PDU_REQ_WRITE_VALUE_OFF )     = usRegData >> 8;
		*( ucMBFrame + MB_PDU_REQ_WRITE_VALUE_OFF + 1 ) = usRegData ;
		
		vMBMasterSetPDUSndLength( psMBMasterInfo, MB_PDU_SIZE_MIN + MB_PDU_REQ_WRITE_SIZE );
		( void ) xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_FRAME_SENT );
		eErrStatus = eMBMasterWaitRequestFinish(psMBPortInfo);
    }
    return eErrStatus;
}

/***********************************************************************************
 * @brief  写单个保持寄存器功能函数
 * @param  pucFrame      Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBException
eMBMasterFuncWriteHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR* pucFrame, USHORT* usLen )
{
    USHORT          usRegAddress;
    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    if( *usLen == ( MB_PDU_SIZE_MIN + MB_PDU_FUNC_WRITE_SIZE ) )
    {
        usRegAddress  = ( USHORT )( (*(pucFrame + MB_PDU_FUNC_WRITE_ADDR_OFF)) << 8 );
        usRegAddress |= ( USHORT )( *(pucFrame + MB_PDU_FUNC_WRITE_ADDR_OFF + 1) );
        usRegAddress++;

        /* Make callback to update the value. */
        eRegStatus = eMBMasterRegHoldingCB( psMBMasterInfo, pucFrame + MB_PDU_FUNC_WRITE_VALUE_OFF,
                                            usRegAddress, 1, MB_REG_WRITE );

        /* If an error occured convert it into a Modbus exception. */
        if( eRegStatus != MB_ENOERR )
        {
            eStatus = prveMBMasterError2Exception( eRegStatus );
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

#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0

 /***********************************************************************************
 * @brief  主栈写多个保持寄存器
 * @param  ucSndAddr      从栈地址
 * @param  usRegAddr      起始地址
 * @param  usNRegs        个数
 * @param  pusDataBuffer  数值数据指针
 * @param  lTimeOut       主栈等待时间（0则一直等待）
 * @return error          错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterReqWriteMultipleHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr,
		                             USHORT usRegAddr, USHORT usNRegs, USHORT* pusDataBuffer, LONG lTimeOut )
{
	UCHAR L;
    UCHAR                 *ucMBFrame;
    USHORT                 usRegIndex = 0;
	
    eMBMasterReqErrCode     eErrStatus = MB_MRE_NO_ERR;
    sMBMasterPortInfo*    psMBPortInfo = &psMBMasterInfo->sMBPortInfo;      //硬件结构
	sMBMasterDevsInfo*    psMBDevsInfo = &psMBMasterInfo->sMBDevsInfo;    //从设备状态信息
	
    if( (ucSndAddr < psMBDevsInfo->ucSlaveDevMinAddr) || (ucSndAddr > psMBDevsInfo->ucSlaveDevMaxAddr) ) 
	{
		eErrStatus = MB_MRE_ILL_ARG;
	}		
    else if ( xMBMasterRunResTake( lTimeOut ) == FALSE ) 
	{
		eErrStatus = MB_MRE_MASTER_BUSY;
	}
    else
    {
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
		vMBMasterSetDestAddress(psMBMasterInfo, ucSndAddr);
		*( ucMBFrame + MB_PDU_FUNC_OFF )                     = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
		*( ucMBFrame + MB_PDU_REQ_WRITE_MUL_ADDR_OFF )       = usRegAddr >> 8;
		*( ucMBFrame + MB_PDU_REQ_WRITE_MUL_ADDR_OFF + 1 )   = usRegAddr;
		*( ucMBFrame + MB_PDU_REQ_WRITE_MUL_REGCNT_OFF )     = usNRegs >> 8;
		*( ucMBFrame + MB_PDU_REQ_WRITE_MUL_REGCNT_OFF + 1 ) = usNRegs ;
		*( ucMBFrame + MB_PDU_REQ_WRITE_MUL_BYTECNT_OFF )   = usNRegs * 2;
		ucMBFrame += MB_PDU_REQ_WRITE_MUL_VALUES_OFF;
		
		while( usNRegs > usRegIndex)
		{
			*ucMBFrame++ = (UCHAR)( *(pusDataBuffer + usRegIndex) >> 8);
			*ucMBFrame++ = (UCHAR)( *(pusDataBuffer + (usRegIndex++)) );
		}
		vMBMasterSetPDUSndLength(psMBMasterInfo, MB_PDU_SIZE_MIN + MB_PDU_REQ_WRITE_MUL_SIZE_MIN + 2*usNRegs );
		
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
	
		( void ) xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_FRAME_SENT );
		eErrStatus = eMBMasterWaitRequestFinish(psMBPortInfo);
    }
    return eErrStatus;
}

/***********************************************************************************
 * @brief  写多个保持寄存器功能函数
 * @param  pucFrame      Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBException
eMBMasterFuncWriteMultipleHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR* pucFrame, USHORT* usLen )
{
    UCHAR          *ucMBFrame;
    USHORT          usRegAddress;
    USHORT          usRegCount;
    UCHAR           ucRegByteCount;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    /* If this request is broadcast, the *usLen is not need check. */
    if( ( *usLen == MB_PDU_SIZE_MIN + MB_PDU_FUNC_WRITE_MUL_SIZE ) || xMBMasterRequestIsBroadcast(psMBMasterInfo) )
    {
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
        usRegAddress = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_WRITE_MUL_ADDR_OFF )) << 8 );
        usRegAddress |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_WRITE_MUL_ADDR_OFF + 1) );
        usRegAddress++;

        usRegCount = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_WRITE_MUL_REGCNT_OFF )) << 8 );
        usRegCount |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_WRITE_MUL_REGCNT_OFF + 1) );
        ucRegByteCount = *(ucMBFrame + MB_PDU_REQ_WRITE_MUL_BYTECNT_OFF);

        if( ucRegByteCount == 2 * usRegCount )
        {
            /* Make callback to update the register values. */
            eRegStatus =
                eMBMasterRegHoldingCB( psMBMasterInfo, ucMBFrame + MB_PDU_REQ_WRITE_MUL_VALUES_OFF,
                                       usRegAddress, usRegCount, MB_REG_WRITE );

            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBMasterError2Exception( eRegStatus );
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
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

#if MB_FUNC_READ_HOLDING_ENABLED > 0

 /***********************************************************************************
 * @brief  主栈读保持寄存器
 * @param  ucSndAddr      从栈地址
 * @param  usRegAddr      起始地址
 * @param  lTimeOut       主栈等待时间（0则一直等待）
 * @return error          错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode
eMBMasterReqReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, USHORT usRegAddr, USHORT usNRegs, LONG lTimeOut )
{
    UCHAR* ucMBFrame;
    
    eMBMasterReqErrCode     eErrStatus = MB_MRE_NO_ERR;
    sMBMasterPortInfo*    psMBPortInfo = &psMBMasterInfo->sMBPortInfo;      //硬件结构
	sMBMasterDevsInfo*    psMBDevsInfo = &psMBMasterInfo->sMBDevsInfo;    //从设备状态信息
	
    if( (ucSndAddr < psMBDevsInfo->ucSlaveDevMinAddr) || (ucSndAddr > psMBDevsInfo->ucSlaveDevMaxAddr) ) 
	{
		eErrStatus = MB_MRE_ILL_ARG;
	}		
    else if ( xMBMasterRunResTake( lTimeOut ) == FALSE ) 
	{
		eErrStatus = MB_MRE_MASTER_BUSY;
	}
    else
    {
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
		vMBMasterSetDestAddress(psMBMasterInfo, ucSndAddr);
		*( ucMBFrame + MB_PDU_FUNC_OFF )                = MB_FUNC_READ_HOLDING_REGISTER;
		*( ucMBFrame + MB_PDU_REQ_READ_ADDR_OFF )       = usRegAddr >> 8;
		*( ucMBFrame + MB_PDU_REQ_READ_ADDR_OFF + 1 )   = usRegAddr;
		*( ucMBFrame + MB_PDU_REQ_READ_REGCNT_OFF )     = usNRegs >> 8;
		*( ucMBFrame + MB_PDU_REQ_READ_REGCNT_OFF + 1 ) = usNRegs;
		
		vMBMasterSetPDUSndLength(psMBMasterInfo, MB_PDU_SIZE_MIN + MB_PDU_REQ_READ_SIZE );
		( void ) xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_FRAME_SENT );
		eErrStatus = eMBMasterWaitRequestFinish(psMBPortInfo);
    }
    return eErrStatus;
}

/***********************************************************************************
 * @brief  读保持寄存器功能函数
 * @param  pucFrame      Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBException
eMBMasterFuncReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR * pucFrame, USHORT * usLen )
{
    UCHAR          *ucMBFrame;
    USHORT          usRegAddress;
    USHORT          usRegCount;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    /* If this request is broadcast, and it's read mode. This request don't need execute. */
    if ( xMBMasterRequestIsBroadcast(psMBMasterInfo) )
    {
    	eStatus = MB_EX_NONE;
    }
    else if( *usLen >= MB_PDU_SIZE_MIN + MB_PDU_FUNC_READ_SIZE_MIN )
    {
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
        usRegAddress = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_READ_ADDR_OFF)) << 8 );
        usRegAddress |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_READ_ADDR_OFF + 1) ) ;
        usRegAddress++;

        usRegCount = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_READ_REGCNT_OFF)) << 8 );
        usRegCount |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_READ_REGCNT_OFF + 1) );

        /* Check if the number of registers to read is valid. If not
         * return Modbus illegal data value exception.
         */
        if( (usRegCount >= 1) && ( 2*usRegCount == *(pucFrame + MB_PDU_FUNC_READ_BYTECNT_OFF)) )
        {
            /* Make callback to fill the buffer. */
            eRegStatus = eMBMasterRegHoldingCB(psMBMasterInfo, pucFrame + MB_PDU_FUNC_READ_VALUES_OFF, usRegAddress, usRegCount, MB_REG_READ );
            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBMasterError2Exception( eRegStatus );
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
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

#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0

 /***********************************************************************************
 * @brief  主栈读写多个保持寄存器
 * @param  ucSndAddr            从栈地址
 * @param  usReadRegAddr        读起始地址
 * @param  usNReadRegs          个数
 * @param  pusDataBuffer        数值数据指针
  * @param  usWriteRegAddr      写起始地址
 * @param  usNWriteRegs         个数
 * @param  lTimeOut             主栈等待时间（0则一直等待）
 * @return error                错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode
eMBMasterReqReadWriteMultipleHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr,
		                                      USHORT usReadRegAddr, USHORT usNReadRegs, USHORT * pusDataBuffer,
		                                      USHORT usWriteRegAddr, USHORT usNWriteRegs, LONG lTimeOut )
{
    UCHAR                 *ucMBFrame;
    USHORT                 usRegIndex = 0;
	
    eMBMasterReqErrCode     eErrStatus = MB_MRE_NO_ERR;
    sMBMasterPortInfo*    psMBPortInfo = &psMBMasterInfo->sMBPortInfo;      //硬件结构
	sMBMasterDevsInfo*    psMBDevsInfo = &psMBMasterInfo->sMBDevsInfo;    //从设备状态信息
	
    if( (ucSndAddr < psMBDevsInfo->ucSlaveDevMinAddr) || (ucSndAddr > psMBDevsInfo->ucSlaveDevMaxAddr) ) 
	{
		eErrStatus = MB_MRE_ILL_ARG;
	}		
    else if ( xMBMasterRunResTake( lTimeOut ) == FALSE ) 
	{
		eErrStatus = MB_MRE_MASTER_BUSY;
	}
    else
    {
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
		vMBMasterSetDestAddress(psMBMasterInfo, ucSndAddr);
		*( ucMBFrame + MB_PDU_FUNC_OFF )                           = MB_FUNC_READWRITE_MULTIPLE_REGISTERS;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_READ_ADDR_OFF )        = usReadRegAddr >> 8;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_READ_ADDR_OFF + 1 )    = usReadRegAddr;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_READ_REGCNT_OFF )      = usNReadRegs >> 8;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_READ_REGCNT_OFF + 1 )  = usNReadRegs ;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_ADDR_OFF )       = usWriteRegAddr >> 8;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_ADDR_OFF + 1 )   = usWriteRegAddr;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF )     = usNWriteRegs >> 8;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF + 1 ) = usNWriteRegs ;
		*( ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_BYTECNT_OFF )    = usNWriteRegs * 2;
		ucMBFrame += MB_PDU_REQ_READWRITE_WRITE_VALUES_OFF;
		
		while( usNWriteRegs > usRegIndex)
		{
			*ucMBFrame++ = (UCHAR)( *(pusDataBuffer + usRegIndex) >> 8);
			*ucMBFrame++ = (UCHAR)( *(pusDataBuffer + (usRegIndex++)) );
		}
		
		vMBMasterSetPDUSndLength(psMBMasterInfo, MB_PDU_SIZE_MIN + MB_PDU_REQ_READWRITE_SIZE_MIN + 2*usNWriteRegs);
		( void ) xMBMasterPortEventPost( psMBPortInfo, EV_MASTER_FRAME_SENT );
		eErrStatus = eMBMasterWaitRequestFinish(psMBPortInfo);
    }
    return eErrStatus;
}

/***********************************************************************************
 * @brief 读写保持寄存器功能函数
 * @param pucFrame       Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBException
eMBMasterFuncReadWriteMultipleHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR * pucFrame, USHORT * usLen )
{
    USHORT          usRegReadAddress;
    USHORT          usRegReadCount;
    USHORT          usRegWriteAddress;
    USHORT          usRegWriteCount;
    UCHAR          *ucMBFrame;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;
    
    /* If this request is broadcast, and it's read mode. This request don't need execute. */
    if ( xMBMasterRequestIsBroadcast(psMBMasterInfo) )
    {
    	eStatus = MB_EX_NONE;
    }
    else if( *usLen >= MB_PDU_SIZE_MIN + MB_PDU_FUNC_READWRITE_SIZE_MIN )
    {
    	vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
        usRegReadAddress = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_READWRITE_READ_ADDR_OFF)) << 8U );
        usRegReadAddress |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_READWRITE_READ_ADDR_OFF + 1) );
        usRegReadAddress++;

        usRegReadCount = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_READWRITE_READ_REGCNT_OFF)) << 8U );
        usRegReadCount |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_READWRITE_READ_REGCNT_OFF + 1) );

        usRegWriteAddress = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_ADDR_OFF)) << 8U );
        usRegWriteAddress |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_ADDR_OFF + 1) );
        usRegWriteAddress++;

        usRegWriteCount = ( USHORT )( (*(ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF)) << 8U );
        usRegWriteCount |= ( USHORT )( *(ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_REGCNT_OFF + 1) );

        if( ( 2 * usRegReadCount ) == *(pucFrame + MB_PDU_FUNC_READWRITE_READ_BYTECNT_OFF) )
        {
            /* Make callback to update the register values. */
            eRegStatus = eMBMasterRegHoldingCB(psMBMasterInfo, ucMBFrame + MB_PDU_REQ_READWRITE_WRITE_VALUES_OFF,
                                           usRegWriteAddress, usRegWriteCount, MB_REG_WRITE );

            if( eRegStatus == MB_ENOERR )
            {
                /* Make the read callback. */
				eRegStatus = eMBMasterRegHoldingCB(psMBMasterInfo, pucFrame + MB_PDU_FUNC_READWRITE_READ_VALUES_OFF,
						                      usRegReadAddress, usRegReadCount, MB_REG_READ);
            }
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBMasterError2Exception( eRegStatus );
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    return eStatus;
}
#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0
/**
 * Modbus master holding register callback function.
 *
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode eMBMasterRegHoldingCB(sMBMasterInfo* psMBMasterInfo, UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{
    USHORT          iRegIndex, n, m, nSlaveTypes;
    USHORT          REG_HOLDING_START, REG_HOLDING_END;
    USHORT          usProtocolType;
	
	USHORT          usRegHoldValue;
    SHORT           sRegHoldValue;
	int8_t          cRegHoldValue;
    
    eMBErrorCode               eStatus = MB_ENOERR;
	sMasterRegHoldData* pvRegHoldValue = NULL;
    
    sMBSlaveDevInfo*        psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur ;     //当前从设备
    const sMBDevDataTable*     psRegHoldBuf = psMBSlaveDevCur->psDevCurData->psMBRegHoldTable;   //从设备通讯协议表
    UCHAR                      ucMBDestAddr = ucMBMasterGetDestAddress(psMBMasterInfo);           //从设备通讯地址
    
     /* 主栈处于测试从设备状态 */		
    if(psMBMasterInfo->xMBRunInTestMode)
    {
        return MB_ENOERR;
    }	
    
    if(psMBSlaveDevCur->ucDevAddr != usAddress) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, usAddress);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psRegHoldBuf = psMBSlaveDevCur->psDevCurData->psMBRegHoldTable;
    }
    if( (psRegHoldBuf->pvDataBuf  == NULL) || (psRegHoldBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_ENOREG;
	}
    
	REG_HOLDING_START = psRegHoldBuf->usStartAddr;
    REG_HOLDING_END = psRegHoldBuf->usEndAddr;
	
    /* if mode is read, the master will write the received date to buffer. */

    /* it already plus one in modbus function method. */
    usAddress--;

    if ( (usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_END))
    {
    	iRegIndex = usAddress ;
        switch (eMode)
        {
        /* read current register values from the protocol stack. */
        case MB_REG_READ:
           
           					
            while (usNRegs > 0)          
            {
				(void)eMBMasterRegHoldingMap(psMBMasterInfo, ucMBDestAddr, iRegIndex, &pvRegHoldValue);     //扫描字典中变量，找出对应的变量

				usRegHoldValue = ( (USHORT)(*pucRegBuffer++) ) << 8;
			    usRegHoldValue |=( (USHORT)(*pucRegBuffer++) ) & 0xFF;
				
				if( (pvRegHoldValue != NULL) && (pvRegHoldValue->pvValue != NULL) && (pvRegHoldValue->ucAccessMode != WO) )
				{
					if( (pvRegHoldValue->fTransmitMultiple != 0) && (pvRegHoldValue->fTransmitMultiple != 1) )
				    {
				    	usRegHoldValue = (USHORT)((float)usRegHoldValue / (float)pvRegHoldValue->fTransmitMultiple);     //传输因子
				    }
					
					if (pvRegHoldValue->ucDataType == uint16)
					{
						if( (usRegHoldValue >= (USHORT)pvRegHoldValue->lMinVal ) && (usRegHoldValue <= (USHORT)pvRegHoldValue->lMaxVal))
						{ 
							*(USHORT*)pvRegHoldValue->pvValue = (USHORT)usRegHoldValue;    //更新对应点位
                            pvRegHoldValue->usPreVal = (USHORT)usRegHoldValue;							
						}							
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}						
					}
					else if(pvRegHoldValue->ucDataType == uint8)
					{  
						if( ((UCHAR)usRegHoldValue >= (UCHAR)pvRegHoldValue->lMinVal ) && ((UCHAR)usRegHoldValue <= (UCHAR)pvRegHoldValue->lMaxVal) )
						{
							*(UCHAR*)pvRegHoldValue->pvValue = (UCHAR)usRegHoldValue;
                            pvRegHoldValue->usPreVal = (USHORT)usRegHoldValue;							
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}	
					}
					else if (pvRegHoldValue->ucDataType == int16)
					{
						sRegHoldValue = (SHORT)usRegHoldValue;
						
						 if( (sRegHoldValue >= (SHORT)pvRegHoldValue->lMinVal ) && (sRegHoldValue <= (SHORT)pvRegHoldValue->lMaxVal) )	
						{
							*(SHORT*)pvRegHoldValue->pvValue = (SHORT)sRegHoldValue;
							 pvRegHoldValue->usPreVal = (USHORT)sRegHoldValue;
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}						
					}
					else if(pvRegHoldValue->ucDataType == int8)
					{  	
                        cRegHoldValue = (int8_t)usRegHoldValue;
						
						if( (cRegHoldValue >= (int8_t)pvRegHoldValue->lMinVal ) && (cRegHoldValue <= (int8_t)pvRegHoldValue->lMaxVal) )		
						{
							*(int8_t*)pvRegHoldValue->pvValue = (int8_t)cRegHoldValue;
                             pvRegHoldValue->usPreVal = (USHORT)cRegHoldValue;							
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}	
					}
				}
				else
				{
					eStatus = MB_ENOREG;
				    return eStatus;
				}
                iRegIndex++;
                usNRegs--;
            }
            break;
            
        /* write current register values with new values from the protocol stack. */
            case MB_REG_WRITE:
	        break;
		
	        default: break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
#endif

#endif

