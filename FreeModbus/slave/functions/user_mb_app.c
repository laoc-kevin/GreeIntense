/*
 * FreeModbus Libary: user callback functions and buffer define in slave mode
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
 * File: $Id: user_mb_app.c,v 1.60 2013/11/23 11:49:05 Armink $
 */
#include "user_mb_app.h"
#include "lpc_mbdriver.h"
#include "user_mb_dict.h"
#include "cpu.h"
#include "mbframe.h"

//#include <math.h>

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0 || MB_SLAVE_CPN_ENABLED > 0

/*------------------------Slave mode use these variables----------------------*/
USHORT     usSDiscInStart                           = S_DISCRETE_INPUT_START;
USHORT     usSCoilStart                             = S_COIL_START;
USHORT     usSRegInStart                            = S_REG_INPUT_START;
USHORT     usSRegHoldStart                          = S_REG_HOLDING_START;
USHORT     usSValCPNStart                           = S_VAL_CPN_START;

/***********************************************************************************
 * @brief 输入寄存器回调函数（读、连续读）
 * @param pucRegBuffer  回调函数将Modbus寄存器的当前值写入缓冲区
 * @param usAddress     寄存器起始地址1~65535
 * @param usNRegs       寄存器数量
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
#if MB_FUNC_READ_INPUT_ENABLED > 0
	
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT          REG_INPUT_START;
    USHORT          REG_INPUT_NREGS;
    USHORT          usRegInStart;

	sMBRegData*  pvRegInValue = NULL;
	
	USHORT          usRegInValue;
	SHORT           sRegInValue;
	int8_t          cRegInValue;
	
    REG_INPUT_START = S_REG_INPUT_START;
    REG_INPUT_NREGS = S_REG_INPUT_NREGS;
    usRegInStart = usSRegInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_INPUT_START)
            && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
		iRegIndex = usAddress ;
        while (usNRegs > 0)
        {
			(void)eMBScanRegInMap(iRegIndex, &pvRegInValue);
			
			if( (pvRegInValue != NULL) && (pvRegInValue->OperateMode != WO) )
			{		
				if (pvRegInValue->DataType == uint16)
				{
					usRegInValue = (USHORT)(*(USHORT*)pvRegInValue->Value);
				}
                else if(pvRegInValue->DataType == uint8)
                {
					usRegInValue = (USHORT)(*(UCHAR*)pvRegInValue->Value);
				}
				else if(pvRegInValue->DataType == int16)
				{
					usRegInValue = (USHORT)(*(SHORT*)pvRegInValue->Value);
				}
				else if(pvRegInValue->DataType == int8)
				{
					usRegInValue = (USHORT)(*(int8_t*)pvRegInValue->Value);
				}
				
				usRegInValue *= pvRegInValue->Multiple;
				*pucRegBuffer++ = (UCHAR)( usRegInValue >> 8 );
                *pucRegBuffer++ = (UCHAR)( usRegInValue & 0xFF );	
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

/***********************************************************************************
 * @brief 保持寄存器回调函数（读、连续读、写、连续写）
 * @param pucRegBuffer  如果需要更新用户寄存器数值，这个缓冲区必须指向新的寄存器数值。
 *                      如果协议栈想知道当前的数值，回调函数必须将当前值写入这个缓冲区
 * 
 * @param usAddress     寄存器起始地址1~65535
 * @param usNRegs       寄存器数量
 * 
 * @param eMode         如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
 *                      如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
 * 
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT          REG_HOLDING_START, REG_HOLDING_NREGS, usRegHoldStart;
	
	USHORT          usRegHoldValue;
	SHORT           sRegHoldValue;
	int8_t          cRegHoldValue;
	
    OS_ERR err = OS_ERR_NONE;
	sMBRegData *          pvRegHoldValue;
	OS_TCB*         TaskTCB; 
	
    REG_HOLDING_START = S_REG_HOLDING_START;
    REG_HOLDING_NREGS = S_REG_HOLDING_NREGS;
    usRegHoldStart = usSRegHoldStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_HOLDING_START)
            && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = usAddress ;
        switch (eMode)
        {
        /* read current register values from the protocol stack. */
        case MB_REG_READ:
            while (usNRegs > 0)
            {
                (void)eMBScanRegHoldMap(iRegIndex, &pvRegHoldValue);
			
				if( (pvRegHoldValue != NULL) && (pvRegHoldValue->OperateMode != WO) )                        
				{	
					/* 根据数据类型进行指针转换，需要特别注意，转错会导致数值异常*/
					if (pvRegHoldValue->DataType == uint16)   
					{
						usRegHoldValue = (USHORT)(*(USHORT*)pvRegHoldValue->Value );
					}
					else if(pvRegHoldValue->DataType == uint8)
					{
						usRegHoldValue = (USHORT)(*(UCHAR*)pvRegHoldValue->Value );
					}
					else if(pvRegHoldValue->DataType == int16)
					{
						usRegHoldValue = (USHORT)(*(SHORT*)pvRegHoldValue->Value );
					}
					else if(pvRegHoldValue->DataType == int8)
					{
						usRegHoldValue = (USHORT)(*(int8_t*)pvRegHoldValue->Value );
					}
						
					usRegHoldValue *=  pvRegHoldValue->Multiple;
					
					*pucRegBuffer++ = (UCHAR)( usRegHoldValue >> 8 );
					*pucRegBuffer++ = (UCHAR)( usRegHoldValue & 0xFF );
					
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
     		while (usNRegs > 0)
            {
				(void)eMBScanRegHoldMap(iRegIndex, &pvRegHoldValue);                    //扫描保持寄存器字典，取对应的点
				
				usRegHoldValue = ((LONG)(*pucRegBuffer++)) << 8;
			    usRegHoldValue |=((LONG)(*pucRegBuffer++)) & 0xFF;
				
				if( (pvRegHoldValue != NULL) && (pvRegHoldValue->Value != NULL) && (pvRegHoldValue->OperateMode != RO))
				{
					if(pvRegHoldValue->Multiple != 0)
				    {
				    	usRegHoldValue /= pvRegHoldValue->Multiple;
				    }
					
					if (pvRegHoldValue->DataType == uint16)
					{
						 if( (usRegHoldValue >= pvRegHoldValue->MinValue ) && (usRegHoldValue <= pvRegHoldValue->MaxValue) )	
						{		
							if ( usRegHoldValue != *(USHORT*)pvRegHoldValue->Value ) //更新数据
							{
								*(USHORT*)pvRegHoldValue->Value = (USHORT)usRegHoldValue;	
							}		
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}		
					}
					else if(pvRegHoldValue->DataType == uint8)
					{				
					    if( (usRegHoldValue >= pvRegHoldValue->MinValue ) && (usRegHoldValue <= pvRegHoldValue->MaxValue) )
						{
							if ( usRegHoldValue != *(UCHAR*)pvRegHoldValue->Value )
							{
								*(UCHAR*)pvRegHoldValue->Value = (UCHAR)usRegHoldValue;					
							}							
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}
					}
					else if (pvRegHoldValue->DataType == int16)
					{
						sRegHoldValue = (SHORT)usRegHoldValue;
						 if( (sRegHoldValue >= (SHORT)pvRegHoldValue->MinValue ) && (sRegHoldValue <= (SHORT)pvRegHoldValue->MaxValue) )	
						{		
							if ( sRegHoldValue != *(SHORT*)pvRegHoldValue->Value )
							{
								*(SHORT*)pvRegHoldValue->Value = (SHORT)sRegHoldValue;							
							}		
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}		
					}
					
					else if(pvRegHoldValue->DataType == int8)
					{	
                         cRegHoldValue = (int8_t)usRegHoldValue;						
					    if( (cRegHoldValue >= (int8_t)pvRegHoldValue->MinValue ) && (cRegHoldValue <= (int8_t)pvRegHoldValue->MaxValue) )	
						{
							if ( cRegHoldValue != *(int8_t*)pvRegHoldValue->Value )
							{
								*(int8_t*)pvRegHoldValue->Value = (int8_t)cRegHoldValue;				
							}					
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

/***********************************************************************************
 * @brief 线圈状态寄存器回调函数（读、连续读、写、连续写）
 * @param pucRegBuffer  位组成一个字节，起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。
 *                      如果回调函数要写这个缓冲区，没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置位0。
 * 
 * @param usAddress     第一个线圈地址
 * @param usNCoils      请求的线圈个数
 * 
 * @param eMode         如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
 *                      如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
 * 
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0

eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNCoils, eMBRegisterMode eMode)
{
	USHORT          COIL_START, COIL_NCOILS, usCoilStart;
    eMBErrorCode    eStatus = MB_ENOERR;
	OS_TCB*         TaskTCB = NULL; 
	OS_ERR          err = OS_ERR_NONE;
    COIL_START = S_COIL_START;
    COIL_NCOILS = S_COIL_NCOILS;
    usCoilStart = usSCoilStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if( ( usAddress >= COIL_START ) &&
        ( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
    {
		switch ( eMode )
        {
        /* read current coil values from the protocol stack. */
        case MB_REG_READ:
            eStatus = xMBUtilGetBits( pucRegBuffer, usAddress, usNCoils, CoilData);
        break;

        case MB_REG_WRITE:	
            eStatus = xMBUtilSetBits( pucRegBuffer, usAddress, usNCoils, CoilData);
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

/*********************************************************************************************
 * @brief 离散输入量回调函数（读、连续读）
 * @param pucRegBuffer  用当前的线圈数据更新这个寄存器，起始寄存器对应的位处于该字节pucRegBuffer的最低位LSB。
 *                      如果回调函数要写这个缓冲区，没有用到的线圈（例如不是8个一组的线圈状态）对应的位的数值必须设置为0。
 * 
 * @param usAddress     离散输入的起始地址
 * @param usNDiscrete   离散输入点数量
 * 
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 ********************************************************************************************/
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usDiscreteInputStart;
    USHORT          DISCRETE_INPUT_START,DISCRETE_INPUT_NDISCRETES;
	OS_TCB*         TaskTCB = NULL;

    DISCRETE_INPUT_START = S_DISCRETE_INPUT_START;
    DISCRETE_INPUT_NDISCRETES = S_DISCRETE_INPUT_NDISCRETES;
    usDiscreteInputStart = usSDiscInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= DISCRETE_INPUT_START)
            && (usAddress + usNDiscrete    <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES))
    {
        /* read current coil values from the protocol stack. */ 
        eStatus = xMBUtilGetBits( pucRegBuffer, usAddress, usNDiscrete, DiscInData);
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

#endif

/***********************************************************************************
 * @brief  读CPN变量回调函数（读、连续读）
 * @param  pucRegBuffer  回调函数将变量当前值写入缓冲区
 * @param  ucValCount    变量个数
 * @param  usLen         缓冲区长度
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
#if MB_FUNC_CPN_READ_ENABLED > 0

eMBErrorCode eMBReadCPNCB( UCHAR * pucRegBuffer, UCHAR ucValCount, USHORT * usLen )
{
	float fNum;
    UCHAR *pcNum = NULL;
    UCHAR iExchange;
	USHORT          iValBitIndex, iValDataBitIndex, iNVal;
	USHORT          VAL_CPN_START, VAL_CPN_NREGS, VAL_CPN_DEFINE_SIZE;
	USHORT          usValCPNStart, usValCPNBytes, usValCPNType;
    eMBErrorCode    eStatus = MB_ENOERR;
    ULONG            ulValCPNName;
	ULONG            ulValCPNValue;
	
    sValCPNData *   pvCPNValue = 0;
	OS_TCB*         TaskTCB = NULL; 
     	
    VAL_CPN_START = S_VAL_CPN_START;
    VAL_CPN_NREGS = S_VAL_CPN_NREGS;
	VAL_CPN_DEFINE_SIZE  = MB_CPN_VALUE_DEFINE_SIZE;	
    usValCPNStart = usSValCPNStart;
    
    if ((ucValCount <= VAL_CPN_NREGS))
    {
        iValDataBitIndex = VAL_CPN_DEFINE_SIZE * ucValCount ;

		*usLen += iValDataBitIndex;

		for ( iNVal = 0; iNVal < ucValCount; iNVal++ )
        {
			iValBitIndex = VAL_CPN_DEFINE_SIZE * iNVal + MB_CPN_VALUE_DATA_NAME_OFF ;
			ulValCPNName =   (  (ULONG)(*(pucRegBuffer + iValBitIndex)) )                          //CPN变量名称
			                |(( (ULONG)(*(pucRegBuffer + iValBitIndex + 1)) ) << 8 )
			                |(( (ULONG)(*(pucRegBuffer + iValBitIndex + 2)) ) << 16)
			                |(( (ULONG)(*(pucRegBuffer + iValBitIndex + 3)) ) << 24)  ;
			
			iValBitIndex = VAL_CPN_DEFINE_SIZE * iNVal + MB_CPN_VALUE_DATA_BYTES_OFF;
			usValCPNBytes =   ( (USHORT)(*(pucRegBuffer + iValBitIndex)) )                        //CPN变量长度
			                | ( ((USHORT)(*(pucRegBuffer + iValBitIndex + 1)))<< 8 )  ;
			
			iValBitIndex = VAL_CPN_DEFINE_SIZE * iNVal + MB_CPN_VALUE_DATA_TYPE_OFF;
			usValCPNType =    ( (USHORT)(*(pucRegBuffer + iValBitIndex)) )                        //CPN变量类型
			                | ( ((USHORT)(*(pucRegBuffer + iValBitIndex + 1)))<< 8 )  ;
		
			(void)eMBScanCPNMap( ulValCPNName, &pvCPNValue);                                      //扫描CPN变量字典
			
			if( (pvCPNValue != NULL) && (pvCPNValue->Value != NULL))
			{
				switch (pvCPNValue->DataType )                                                    //根据数据类型进行指针转换
				{
					case uint8:
						ulValCPNValue = (ULONG)(*(UCHAR*)pvCPNValue->Value) ;
					break;
					case uint16:
						ulValCPNValue = (ULONG)(*(USHORT*)pvCPNValue->Value) ;
					break;
					case uint32:
						ulValCPNValue = (ULONG)(*(ULONG*)pvCPNValue->Value);
					break;
					
					case int8:
						ulValCPNValue = (ULONG)(*(int8_t*)pvCPNValue->Value) ;
					break;
					case int16:
						ulValCPNValue = (ULONG)(*(SHORT*)pvCPNValue->Value) ;
					break;
					case int32:
						ulValCPNValue = (ULONG)(*(LONG*)pvCPNValue->Value) ;
					break;

                   default: break;					
				}
				
				switch(pvCPNValue->ValueType )
				{
					case CPN_UINT8:
					    ulValCPNValue *= pvCPNValue->Multiple;
						*( pucRegBuffer + iValDataBitIndex ) = (UCHAR)( ulValCPNValue & 0xFF );;
						iValDataBitIndex += 1;
						*usLen += 1;
						break;
					case CPN_UINT16:
						ulValCPNValue *= pvCPNValue->Multiple;
						*( pucRegBuffer + iValDataBitIndex )    = (UCHAR)( ulValCPNValue & 0xFF );
					    *( pucRegBuffer + iValDataBitIndex + 1) = (UCHAR)( ulValCPNValue >> 8 );
						iValDataBitIndex += 2;
						*usLen += 2;
						break;
					
					case CPN_UINT32:
						ulValCPNValue *= pvCPNValue->Multiple;
						*( pucRegBuffer + iValDataBitIndex )    = (UCHAR)( ulValCPNValue & 0xFF ) ;
					    *( pucRegBuffer + iValDataBitIndex + 1) = (UCHAR)( ulValCPNValue >> 8 );
						*( pucRegBuffer + iValDataBitIndex + 2) = (UCHAR)( ulValCPNValue >> 16 );
						*( pucRegBuffer + iValDataBitIndex + 3) = (UCHAR)( ulValCPNValue >> 24 );
						iValDataBitIndex += 4;
						*usLen += 4;
					    break;
							
					case CPN_FLOAT:	
                        if(pvCPNValue->Multiple != 0)
						{
							fNum = (float)( ulValCPNValue / (float)pvCPNValue->Multiple );	
						}											
						pcNum = (UCHAR*)(&fNum);															
						*( pucRegBuffer + iValDataBitIndex )    = *pcNum;
						*( pucRegBuffer + iValDataBitIndex + 1) = *(pcNum + 1);
						*( pucRegBuffer + iValDataBitIndex + 2) = *(pcNum + 2);
						*( pucRegBuffer + iValDataBitIndex + 3) = *(pcNum + 3);
						
						iValDataBitIndex += 4;
						*usLen += 4;
					    break;
					default:break;	
				}	
			}
            else
			{
				switch(usValCPNType)
				{
					case CPN_UINT8:
					    ulValCPNValue = 0;
						*( pucRegBuffer + iValDataBitIndex ) = (UCHAR)( ulValCPNValue & 0xFF );;
						iValDataBitIndex += 1;
						*usLen += 1;
						break;
					case CPN_UINT16:
						ulValCPNValue = 0;
						*( pucRegBuffer + iValDataBitIndex )    = (UCHAR)( ulValCPNValue & 0xFF );
					    *( pucRegBuffer + iValDataBitIndex + 1) = (UCHAR)( ulValCPNValue >> 8 );
						iValDataBitIndex += 2;
						*usLen += 2;
						break;
					
					case CPN_UINT32:
						ulValCPNValue = 0;
						*( pucRegBuffer + iValDataBitIndex )    = (UCHAR)( ulValCPNValue & 0xFF ) ;
					    *( pucRegBuffer + iValDataBitIndex + 1) = (UCHAR)( ulValCPNValue >> 8 );
						*( pucRegBuffer + iValDataBitIndex + 2) = (UCHAR)( ulValCPNValue >> 16 );
						*( pucRegBuffer + iValDataBitIndex + 3) = (UCHAR)( ulValCPNValue >> 24 );
						iValDataBitIndex += 4;
						*usLen += 4;
					    break;
							
					case CPN_FLOAT:	
                 		ulValCPNValue = 0;	
                        if(pvCPNValue->Multiple != 0)
						{
							fNum = (float)( ulValCPNValue / (float)pvCPNValue->Multiple );	
						}									
						pcNum = (UCHAR*)(&fNum);
																			
						*( pucRegBuffer + iValDataBitIndex )    = *pcNum;
						*( pucRegBuffer + iValDataBitIndex + 1) = *(pcNum + 1);
						*( pucRegBuffer + iValDataBitIndex + 2) = *(pcNum + 2);
						*( pucRegBuffer + iValDataBitIndex + 3) = *(pcNum + 3);
						
						iValDataBitIndex += 4;
						*usLen += 4;
					    break;
					default:break;	
					}
				return MB_ENOREG;	
			}			
        }
    }
    else
	{
		return MB_EINVAL;
	}
    return eStatus;
}

#endif

/***********************************************************************************
 * @brief  写CPN变量回调函数（读、连续读）
 * @param  pucRegBuffer  回调函数将变量当前值写入缓冲区
 * @param  ucValCount    变量个数
 * @param  usLen         缓冲区长度
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
#if MB_FUNC_CPN_WRITE_ENABLED > 0

eMBErrorCode eMBWriteCPNCB( UCHAR * pucRegBuffer, UCHAR ucValCount, USHORT * usLen )
{
	float pfNum;
    UCHAR *pcNum = NULL;
	
	USHORT          iValBitIndex, iValDataBitIndex, iNVal;
	USHORT          VAL_CPN_START, VAL_CPN_NREGS, VAL_CPN_DEFINE_SIZE;
    USHORT          usValCPNStart, usValCPNBytes, usValCPNType;
    ULONG           ulValCPNName;
	ULONG           ulValCPNValue;
	LONG            lValCPNValue;
	SHORT           sValCPNValue;
    int8_t          cValCPNValue;
	
	eMBErrorCode    eStatus = MB_ENOERR;
    sValCPNData *   pvCPNValue = 0;
	OS_TCB*         TaskTCB = NULL; 
     
    VAL_CPN_START        = S_VAL_CPN_START;
    VAL_CPN_NREGS        = S_VAL_CPN_NREGS;
	VAL_CPN_DEFINE_SIZE  = MB_CPN_VALUE_DEFINE_SIZE;	
    usValCPNStart        = usSValCPNStart;
    
    if ((ucValCount <= VAL_CPN_NREGS))
    {
        iValDataBitIndex = VAL_CPN_DEFINE_SIZE * ucValCount;
		
		*usLen += iValDataBitIndex;
		
        for ( iNVal = 0; iNVal < ucValCount; iNVal++ )
        {
			iValBitIndex = VAL_CPN_DEFINE_SIZE * iNVal + MB_CPN_VALUE_DATA_NAME_OFF;
			
			ulValCPNName =   ((ULONG)(*(pucRegBuffer + iValBitIndex)))
			                |(( (ULONG)(*(pucRegBuffer + iValBitIndex + 1)) ) << 8 )
			                |(( (ULONG)(*(pucRegBuffer + iValBitIndex + 2)) ) << 16)
			                |(( (ULONG)(*(pucRegBuffer + iValBitIndex + 3)) ) << 24)  ;
			
			iValBitIndex = VAL_CPN_DEFINE_SIZE * iNVal + MB_CPN_VALUE_DATA_BYTES_OFF;
			usValCPNBytes =   ( (USHORT)(*(pucRegBuffer + iValBitIndex)) )
			                | ( ((USHORT)(*(pucRegBuffer + iValBitIndex + 1)))<< 8 )  ;
			
			iValBitIndex = VAL_CPN_DEFINE_SIZE * iNVal + MB_CPN_VALUE_DATA_TYPE_OFF;
			usValCPNType =    ( (USHORT)(*(pucRegBuffer + iValBitIndex)) )
			                | ( ((USHORT)(*(pucRegBuffer + iValBitIndex + 1)))<< 8 )  ;
						
			(void)eMBScanCPNMap( ulValCPNName, &pvCPNValue);
			
			if( (pvCPNValue != NULL) && (pvCPNValue->Value != NULL))
			{
				switch( pvCPNValue->ValueType )  //分数据类型处理
				{
					case CPN_UINT8:   
						ulValCPNValue = *(UCHAR*)(pucRegBuffer + iValDataBitIndex);
						iValDataBitIndex += 1;
						*usLen += 1;
					    if(pvCPNValue->Multiple != 0)  //传输因子
						{
							 ulValCPNValue /= pvCPNValue->Multiple;
						}
					break;
					
					case CPN_UINT16:
						ulValCPNValue =  ( *(USHORT*)(pucRegBuffer + iValDataBitIndex) ) 
							                |( *(USHORT*)(pucRegBuffer + iValDataBitIndex + 1)  << 8 );
						iValDataBitIndex += 2;
						*usLen += 2;
					    
					    if(pvCPNValue->Multiple != 0)
						{
							 ulValCPNValue /= pvCPNValue->Multiple;
						}
					break;
					
					case CPN_UINT32:
						ulValCPNValue =  ( *(ULONG*)(pucRegBuffer + iValDataBitIndex) ) 
											|( *(ULONG*)(pucRegBuffer + iValDataBitIndex + 1) << 8 )
											|( *(ULONG*)(pucRegBuffer + iValDataBitIndex + 2) << 16)
											|( *(ULONG*)(pucRegBuffer + iValDataBitIndex + 3) << 24);
						iValDataBitIndex += 4;
						*usLen += 4;
						 if(pvCPNValue->Multiple != 0)
						{
							 ulValCPNValue /= pvCPNValue->Multiple;
						}
					break;
						
					case CPN_FLOAT:		
						pcNum = (UCHAR*)&pfNum;	
						* pcNum      = *( pucRegBuffer + iValDataBitIndex ); 
						*(pcNum + 1) = *( pucRegBuffer + iValDataBitIndex + 1);
						*(pcNum + 2) = *( pucRegBuffer + iValDataBitIndex + 2);
						*(pcNum + 3) = *( pucRegBuffer + iValDataBitIndex + 3);
							
						ulValCPNValue  = (USHORT)( pfNum * (float)pvCPNValue->Multiple);
						iValDataBitIndex += 4;
						*usLen += 4;	
					break;
					default:break;
				}
				
				switch (pvCPNValue->DataType )
				{
					case uint8:
						if( (ulValCPNValue >= (UCHAR)pvCPNValue->MinValue) && (ulValCPNValue <= (UCHAR)pvCPNValue->MaxValue) )
						{
							if( ulValCPNValue != *(UCHAR*)pvCPNValue->Value )  //更新值
							{
								(*(UCHAR*)pvCPNValue->Value)=(UCHAR)ulValCPNValue ;
							}
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}	
					break;
					case uint16:
						if( (ulValCPNValue >= (USHORT)pvCPNValue->MinValue) && (ulValCPNValue <= (USHORT)pvCPNValue->MaxValue) )
						{
							if( ulValCPNValue != *(USHORT*)pvCPNValue->Value )
							{
								(*(USHORT*)pvCPNValue->Value)=(USHORT)ulValCPNValue;
							}
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}
					break;
					case uint32:
						if( (ulValCPNValue >= (ULONG)pvCPNValue->MinValue) && (ulValCPNValue <= (ULONG)pvCPNValue->MaxValue) )
						{
							if( ulValCPNValue != *(ULONG*)pvCPNValue->Value )
							{
								*(ULONG*)pvCPNValue->Value=(ULONG)ulValCPNValue ;
							}
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}
						
					break;
					case int8:
						cValCPNValue = (int8_t)ulValCPNValue;
					
						if( (cValCPNValue >= (int8_t)pvCPNValue->MinValue) && (cValCPNValue <= (int8_t)pvCPNValue->MaxValue) )
						{
							if( cValCPNValue != *(int8_t*)pvCPNValue->Value )
							{
								*(int8_t*)pvCPNValue->Value=(int8_t)cValCPNValue ;
							}
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}
					break;
					case int16:
						
						sValCPNValue = (SHORT)ulValCPNValue;
					
						 if( (sValCPNValue >= (SHORT)pvCPNValue->MinValue) && (sValCPNValue <= (SHORT)pvCPNValue->MaxValue) )
						{
							if( sValCPNValue != *(SHORT*)pvCPNValue->Value )
							{
								*(SHORT*)pvCPNValue->Value=(SHORT)sValCPNValue;
							}
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}	
					break;
					case int32:
						lValCPNValue = (LONG)ulValCPNValue;
					
						 if( (lValCPNValue >= (LONG)pvCPNValue->MinValue) && (lValCPNValue <= (LONG)pvCPNValue->MaxValue) )
						{
							if( lValCPNValue != *(LONG*)pvCPNValue->Value )
							{
								*(LONG*)pvCPNValue->Value=(LONG)lValCPNValue ;
							}
						}
						else
						{
							eStatus = MB_EINVAL;
							return eStatus;
						}	
					break;
					default: break;
				}	 
			}
			else
			{
				eStatus = MB_ENOREG;
				return eStatus;
			}	
        }
    }
    else
	{
		eStatus = MB_EINVAL;
		return eStatus;
	}
    return eStatus;
}

#endif

#endif
