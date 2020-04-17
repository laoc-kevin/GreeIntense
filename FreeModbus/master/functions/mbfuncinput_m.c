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
 * File: $Id: mbfuncinput_m.c,v 1.60 2013/10/12 14:23:40 Armink Add Master Functions  Exp $
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
#include "user_mb_dict_m.h"
#include "user_mb_scan_m.h"
#include "user_mb_map_m.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_REQ_READ_ADDR_OFF            ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_REQ_READ_REGCNT_OFF          ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_REQ_READ_SIZE                ( 4 )
#define MB_PDU_FUNC_READ_BYTECNT_OFF        ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_READ_VALUES_OFF         ( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_READ_SIZE_MIN           ( 1 )

#define MB_PDU_FUNC_READ_RSP_BYTECNT_OFF    ( MB_PDU_DATA_OFF )

/* ----------------------- Start implementation -----------------------------*/
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

#if MB_FUNC_READ_INPUT_ENABLED > 0
 /***********************************************************************************
 * @brief  主栈读输入寄存器
 * @param  ucSndAddr      从栈地址
 * @param  usRegAddr      起始地址
 * @param  usNRegs        个数
 * @param  lTimeOut       主栈等待时间（0则一直等待）
 * @return error          错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterReqReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                                   USHORT usRegAddr, USHORT usNRegs, LONG lTimeOut )
{
    UCHAR                 *ucMBFrame;
	
    eMBMasterReqErrCode    eErrStatus = MB_MRE_NO_ERR;
	
    sMBMasterPortInfo*     psMBPortInfo= psMBMasterInfo->psMBPortInfo;      //硬件结构
	sMBMasterDevsInfo*     psMBDevsInfo = psMBMasterInfo->psMBDevsInfo;   //从设备状态表
	
    if( (ucSndAddr < psMBDevsInfo->ucSlaveDevMinAddr) || (ucSndAddr > psMBDevsInfo->ucSlaveDevMaxAddr) ) 
	{
		eErrStatus = MB_MRE_ILL_ARG;
	}		
    else if ( xMBMasterRunResTake(lTimeOut) == FALSE ) 
	{
		eErrStatus = MB_MRE_MASTER_BUSY;
	}
    else
    {
		vMBMasterGetPDUSndBuf(psMBMasterInfo, &ucMBFrame);
		vMBMasterSetDestAddress(psMBMasterInfo, ucSndAddr);
		
		ucMBFrame[MB_PDU_FUNC_OFF]                = MB_FUNC_READ_INPUT_REGISTER;
		ucMBFrame[MB_PDU_REQ_READ_ADDR_OFF]       = usRegAddr >> 8;
		ucMBFrame[MB_PDU_REQ_READ_ADDR_OFF + 1]   = usRegAddr;
		ucMBFrame[MB_PDU_REQ_READ_REGCNT_OFF]     = usNRegs >> 8;
		ucMBFrame[MB_PDU_REQ_READ_REGCNT_OFF + 1] = usNRegs;
		
		vMBMasterSetPDUSndLength( psMBMasterInfo, MB_PDU_SIZE_MIN + MB_PDU_REQ_READ_SIZE );
		
		(void)xMBMasterPortEventPost(psMBPortInfo, EV_MASTER_FRAME_SENT);
		eErrStatus = eMBMasterWaitRequestFinish(psMBPortInfo);
    }
    return eErrStatus;
}

/***********************************************************************************
 * @brief  读输入寄存器功能函数
 * @param  pucFrame      Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBException
eMBMasterFuncReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR * pucFrame, USHORT * usLen )
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
        usRegAddress = ( USHORT )( ucMBFrame[MB_PDU_REQ_READ_ADDR_OFF] << 8 );
        usRegAddress |= ( USHORT )( ucMBFrame[MB_PDU_REQ_READ_ADDR_OFF + 1] );
        usRegAddress++;

        usRegCount = ( USHORT )( ucMBFrame[MB_PDU_REQ_READ_REGCNT_OFF] << 8 );
        usRegCount |= ( USHORT )( ucMBFrame[MB_PDU_REQ_READ_REGCNT_OFF + 1] );

        /* Check if the number of registers to read is valid. If not
         * return Modbus illegal data value exception.
         */
        if( (usRegCount >= 1) && (2 * usRegCount == pucFrame[MB_PDU_FUNC_READ_BYTECNT_OFF]) )
        {
            /* Make callback to fill the buffer. */
            eRegStatus = eMBMasterRegInputCB( psMBMasterInfo, &pucFrame[MB_PDU_FUNC_READ_VALUES_OFF], 
			                                  usRegAddress, usRegCount );
            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBMasterError2Exception(eRegStatus);
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

/**
 * Modbus master input register callback function.
 *
 * @param pucRegBuffer input register buffer
 * @param usAddress input register address
 * @param usNRegs input register number
 *
 * @return result
 */
eMBErrorCode eMBMasterRegInputCB( sMBMasterInfo* psMBMasterInfo, UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    USHORT          iRegIndex, n;
    USHORT          REG_INPUT_START, REG_INPUT_END;
    USHORT          usProtocolType, nSlaveTypes;
	
	USHORT          usRegInValue;
	SHORT           sRegInValue;
	CHAR            cRegInValue;
    
	eMBErrorCode            eStatus = MB_ENOERR;
	sMasterRegInData*  pvRegInValue = NULL;
    
	sMBSlaveDevInfo*        psMBSlaveDevCur = psMBMasterInfo->psMBDevsInfo->psMBSlaveDevCur ;     //当前从设备
    const sMBDevDataTable*    psRegInputBuf = psMBSlaveDevCur->psDevCurData->psMBRegInTable;     //从设备通讯协议表
    UCHAR                      ucMBDestAddr = ucMBMasterGetDestAddress(psMBMasterInfo);           //从设备通讯地址
    
     /* 主栈处于测试从设备状态 */		
    if(psMBMasterInfo->xMBRunInTestMode)
    {  
        return MB_ENOERR;
    }  
   
    if(psMBSlaveDevCur->ucDevAddr != usAddress) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, usAddress);
        psMBMasterInfo->psMBDevsInfo->psMBSlaveDevCur = psMBSlaveDevCur;
        psRegInputBuf = psMBSlaveDevCur->psDevCurData->psMBRegInTable;
    }
	if( (psRegInputBuf->pvDataBuf == NULL) || (psRegInputBuf->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_ENOREG;
	}

	REG_INPUT_START = psRegInputBuf->usStartAddr;
    REG_INPUT_END = psRegInputBuf->usEndAddr;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ( (usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_END) )
    {
        iRegIndex = usAddress;
        
        while (usNRegs > 0)
        {
		    (void)eMBMasterRegInMap(psMBMasterInfo, ucMBDestAddr, iRegIndex, &pvRegInValue);    //扫描字典
			
			usRegInValue = ( (USHORT)(*pucRegBuffer++) ) << 8;
			usRegInValue |=( (USHORT)(*pucRegBuffer++) ) & 0xFF;

		    if( (pvRegInValue != NULL) && (pvRegInValue->pvValue != NULL) && (pvRegInValue->ucAccessMode != WO) )
			{
			    if( (pvRegInValue->fTransmitMultiple != 0) && (pvRegInValue->fTransmitMultiple != 1))
			    {
			    	usRegInValue = (USHORT)((float)usRegInValue / (float)pvRegInValue->fTransmitMultiple);      //传输因子
			    }
				
				if (pvRegInValue->ucDataType == uint16)
				{
					if( (usRegInValue >= pvRegInValue->lMinVal) && (usRegInValue <= pvRegInValue->lMaxVal) )
					{
						*(USHORT*)pvRegInValue->pvValue  = (USHORT)usRegInValue;
					}
					else
					{
						eStatus = MB_EINVAL;
						return eStatus;
					}								
				}
				else if(pvRegInValue->ucDataType == uint8)
				{			
					if( (usRegInValue >= pvRegInValue->lMinVal) && (usRegInValue <= pvRegInValue->lMaxVal) )
					{
						*(UCHAR*)pvRegInValue->pvValue = (UCHAR)usRegInValue;
					}
					else
					{
						eStatus = MB_EINVAL;
						return eStatus;
					}	
				}
				else if(pvRegInValue->ucDataType == int16)
				{	
					sRegInValue = (SHORT)usRegInValue;
					if( ( usRegInValue >= (SHORT)pvRegInValue->lMinVal) && ( usRegInValue <= (SHORT)pvRegInValue->lMaxVal) )
					{		
						*(SHORT*)pvRegInValue->pvValue = (SHORT)sRegInValue ;			   
					}
					else
					{
						eStatus = MB_EINVAL;
						return eStatus;
					}	
				}
                else if(pvRegInValue->ucDataType == int8)
				{	
                    cRegInValue = (int8_t)usRegInValue;
					
					if( (cRegInValue >= (int8_t)pvRegInValue->lMinVal) && (cRegInValue <= (int8_t)pvRegInValue->lMaxVal) )
					{		
						*(CHAR*)pvRegInValue->pvValue = (int8_t)cRegInValue;				   
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
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

#endif

#endif
