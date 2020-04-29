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
 * File: $Id: mbfuncinput.c,v 1.10 2007/09/12 10:15:56 wolti Exp $
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
#include "mbutils.h"
#include "mbdict.h"
#include "mbmap.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0 

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_READ_ADDR_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_READ_REGCNT_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX         ( 0x007D )

#define MB_PDU_FUNC_READ_RSP_BYTECNT_OFF    ( MB_PDU_DATA_OFF )


/* ----------------------- Start implementation -----------------------------*/

#if MB_FUNC_READ_INPUT_ENABLED > 0
/***********************************************************************************
 * @brief 读输入寄存器功能函数
 * @param pucFrame       Modbus的PDU缓冲区数据指针
 * @param  usLen         缓冲区长度
 * @return eMBException  异常码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBException 
eMBSlaveFuncReadInputRegister(sMBSlaveInfo* psMBSlaveInfo, UCHAR* pucFrame, USHORT* usLen )
{
    USHORT          usRegAddress;
    USHORT          usRegCount;
    UCHAR          *pucFrameCur;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    if( *usLen == (MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN) )
    {
        usRegAddress = (USHORT)( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
        usRegAddress |= (USHORT)( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );
        usRegAddress++;

        usRegCount = (USHORT)( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8 );
        usRegCount |= (USHORT)( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1] );

        /* Check if the number of registers to read is valid. If not
         * return Modbus illegal data value exception. 
         */
        if( (usRegCount >= 1) && (usRegCount < MB_PDU_FUNC_READ_REGCNT_MAX) )
        {
            /* Set the current PDU data pointer to the beginning. */
            pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
            *usLen = MB_PDU_FUNC_OFF;

            /* First byte contains the function code. */
            *pucFrameCur++ = MB_FUNC_READ_INPUT_REGISTER;
            *usLen += 1;

            /* Second byte in the response contain the number of bytes. */
            *pucFrameCur++ = ( UCHAR )( usRegCount * 2 );
            *usLen += 1;

            eRegStatus = eMBSlaveRegInputCB(psMBSlaveInfo, pucFrameCur, usRegAddress, usRegCount);

            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBSlaveError2Exception( eRegStatus );
            }
            else
            {
                *usLen += usRegCount * 2;
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        /* Can't be a valid read input register request because the length
         * is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

/***********************************************************************************
 * @brief 输入寄存器回调函数（读、连续读）
 * @param pucRegBuffer  回调函数将Modbus寄存器的当前值写入缓冲区
 * @param usAddress     寄存器起始地址1~65535
 * @param usNRegs       寄存器数量
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveRegInputCB(sMBSlaveInfo* psMBSlaveInfo, UCHAR* pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    USHORT          iRegIndex;
    USHORT          REG_INPUT_START, REG_INPUT_END;
    
	USHORT          usRegInValue;
	SHORT           sRegInValue;
	int8_t          cRegInValue;
    
    eMBErrorCode           eStatus = MB_ENOERR;
	sMBSlaveRegData*  pvRegInValue = NULL;
	
    const sMBSlaveDataTable* psRegInputBuf = psMBSlaveInfo->sMBCommInfo.psSlaveCurData->psMBRegInTable;  //从栈通讯协议表
  
    REG_INPUT_START = psRegInputBuf->usStartAddr;
    REG_INPUT_END = psRegInputBuf->usEndAddr;

    /* it already plus one in modbus function method. */
    usAddress--;

    if( (usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_END) )
    {
		iRegIndex = usAddress ;
        while (usNRegs > 0)
        {
			(void)eMBSlaveRegInMap(psMBSlaveInfo, iRegIndex, &pvRegInValue);
			
			if( (pvRegInValue != NULL) && (pvRegInValue->ucAccessMode != WO) )
			{		
				if (pvRegInValue->ucDataType == uint16)
				{
					usRegInValue = (USHORT)(*(USHORT*)pvRegInValue->pvValue);
				}
                else if(pvRegInValue->ucDataType == uint8)
                {
					usRegInValue = (USHORT)(*(UCHAR*)pvRegInValue->pvValue);
				}
				else if(pvRegInValue->ucDataType == int16)
				{
					usRegInValue = (USHORT)(*(SHORT*)pvRegInValue->pvValue);
				}
				else if(pvRegInValue->ucDataType == int8)
				{
					usRegInValue = (USHORT)(*(int8_t*)pvRegInValue->pvValue);
				}
				
                if( (pvRegInValue->fTransmitMultiple != 0) && (pvRegInValue->fTransmitMultiple != 1) )
                {
                    usRegInValue *=  pvRegInValue->fTransmitMultiple;
                }  
				*pucRegBuffer++ = (UCHAR)(usRegInValue >> 8);
                *pucRegBuffer++ = (UCHAR)(usRegInValue & 0xFF);	
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
