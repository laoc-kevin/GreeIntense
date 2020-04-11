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
 * File: $Id: mb.c,v 1.27 2007/02/18 23:45:41 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include <LPC407x_8x_177x_8x.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbport.h"
#include "user_mb_map.h"
#include "app_led.h"

#if MB_SLAVE_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_SLAVE_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_SLAVE_TCP_ENABLED == 1
#include "mbtcp.h"
#endif
#if MB_SLAVE_CPN_ENABLED == 1
#include "mbcpn.h"
#endif



#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0 || MB_SLAVE_CPN_ENABLED > 0

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

#define MB_CPN_FUNC_WRITE_CODE 0x05



/* ----------------------- Static variables ---------------------------------*/ 

static pvMBFrameStart    pvMBFrameStartCur;
static pvMBFrameStop     pvMBFrameStopCur;
static pvMBFrameClose    pvMBFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *pxMBFrameCBByteReceived ) ( void );
BOOL( *pxMBFrameCBTransmitterEmpty ) ( void );
BOOL( *pxMBPortCBTimerExpired ) ( void );
BOOL( *pxMBFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBFrameCBTransmitFSMCur ) ( void );


#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
static UCHAR    ucMBAddress;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
static peMBFrameSend     peMBFrameSendCur;
static peMBFrameReceive  peMBFrameReceiveCur;

#endif

#if MB_SLAVE_CPN_ENABLED > 0
static UCHAR    ucMBAddress;
static UCHAR    ucSourAddress;
static UCHAR    ucDestAddress;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (CPN) the are set to the correct implementations.
 */
static peMBCPNFrameSend    peMBCPNFrameSendCur;
static peMBCPNFrameReceive peMBCPNFrameReceiveCur;

#endif

static eMBMode  eMBCurrentMode;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;


/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0                     
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0              //读输入寄存器(0x04)
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0            //读保持寄存器(0x03)
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0  //写多个保持寄存器(0x10)
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0           //写单个保持寄存器(0x06)
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0       //读多个保持寄存器(0x17)
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0              //读线圈(0x01)
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0              //写单个线圈(0x05)
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0    //写多个线圈(0x0F)
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0    //读离散量(0x02)
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
#if MB_FUNC_CPN_READ_ENABLED > 0    //读CPN量（0x106）
    {MB_FUNC_CPN_READ, eMBFuncReadCPNValue},
#endif		
#if MB_FUNC_CPN_WRITE_ENABLED > 0    //写CPN变量（0x105）
    {MB_FUNC_CPN_WRITE, eMBFuncWriteCPNValue},
#endif
};

/**********************************************************************
 * @brief  MODBUS协议栈初始化
 * @param  eMode           MODBUS模式:    RTU模式   ASCII模式   TCP模式  
 * @param  ucSlaveAddress  从站地址
 * @param  *Uart           UART配置
 * @return eMBErrorCode    错误码
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode
eMBInit( eMBMode eMode, UCHAR ucSlaveAddress, UART_Def *Uart )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    /* check preconditions */
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        ucMBAddress = ucSlaveAddress;

        switch ( eMode )
        {
#if MB_SLAVE_RTU_ENABLED > 0
        case MB_RTU:

    /* 协议栈核心函数指针的赋值，包括Modbus协议栈的使能和禁止、报文的接收和响应、3.5T定时器中断回调函数、串口发送和接收中断回调函数 */
            pvMBFrameStartCur = eMBRTUStart;                                        // 开启协议栈 
            pvMBFrameStopCur = eMBRTUStop;                                          // 终止协议栈 
            peMBFrameSendCur = eMBRTUSend;                                          // 发送消息帧 
            peMBFrameReceiveCur = eMBRTUReceive;                                    // 接收消息帧 
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;            // 关闭协议栈串口
            pxMBFrameCBByteReceived = xMBRTUReceiveFSM;                             //接收状态机，串口接受中断最终调用此函数接收数据
            pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;                        //发送状态机，串口发送中断最终调用此函数发送数据
            pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;                         //T35超时， 报文到达间隔检查，定时器中断函数最终调用次函数完成定时器中断

            eStatus = eMBRTUInit( Uart );
            break;
#endif
#if MB_SLAVE_ASCII_ENABLED > 0
        case MB_ASCII:
            pvMBFrameStartCur = eMBASCIIStart;
            pvMBFrameStopCur = eMBASCIIStop;
            peMBFrameSendCur = eMBASCIISend;
            peMBFrameReceiveCur = eMBASCIIReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBASCIIReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBASCIITransmitFSM;
            pxMBPortCBTimerExpired = xMBASCIITimerT1SExpired;

            eStatus = eMBASCIIInit(ucMBAddress, ucPort, ulBaudRate, eParity);
            break;
#endif
#if MB_SLAVE_CPN_ENABLED > 0
        case MB_CPN:
            pvMBFrameStartCur = eMBCPNStart;
            pvMBFrameStopCur = eMBCPNStop;
		
            peMBCPNFrameSendCur = eMBCPNSend;
            peMBCPNFrameReceiveCur = eMBCPNReceive;
		
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBCPNReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBCPNTransmitFSM;
            pxMBPortCBTimerExpired = xMBCPNTimerT35Expired;

            eStatus = eMBCPNInit( Uart );
		    (void)eMBSlaveTableInit();
            break;
#endif
        default:
            eStatus = MB_EINVAL;
		    break;
        }

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit( ) )
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR;
            }
            else
            {
                eMBCurrentMode = eMode;                       //设定模式
                eMBState = STATE_DISABLED;                    //modbus协议栈初始化状态,在此初始化为禁止
#if MB_SLAVE_USE_TABLE > 0				
				(void)eMBScanTableBind();
#endif
            }
        }
    }
    return eStatus;
}

#if MB_SLAVE_TCP_ENABLED > 0
eMBErrorCode
eMBTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = eMBTCPDoInit( ucTCPPort ) ) != MB_ENOERR )
    {
        eMBState = STATE_DISABLED;
    }
    else if( !xMBPortEventInit(  ) )
    {
        /* Port dependent event module initalization failed. */
        eStatus = MB_EPORTERR;
    }
    else
    {
        pvMBFrameStartCur = eMBTCPStart;
        pvMBFrameStopCur = eMBTCPStop;
        peMBFrameReceiveCur = eMBTCPReceive;
        peMBFrameSendCur = eMBTCPSend;
        pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBTCPPortClose : NULL;
        ucMBAddress = MB_TCP_PSEUDO_ADDRESS;
        eMBCurrentMode = MB_TCP;
        eMBState = STATE_DISABLED;
    }
    return eStatus;
}
#endif

/**********************************************************************
 * @brief  功能码和相应功能的处理函数捆绑在一起
 * @return eMBErrorCode    错误码
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode
eMBRegisterCB( UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler )
{
    int             i;
    eMBErrorCode    eStatus;

    if( ( 0 < ucFunctionCode ) && ( ucFunctionCode <= 127 ) )
    {
        ENTER_CRITICAL_SECTION(  );
        if( pxHandler != NULL )
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( ( xFuncHandlers[i].pxHandler == NULL ) ||
                    ( xFuncHandlers[i].pxHandler == pxHandler ) )
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode;
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = ( i != MB_FUNC_HANDLERS_MAX ) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
        EXIT_CRITICAL_SECTION(  );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}

/**********************************************************************
 * @brief  MODBUS协议栈关闭
 * @return eMBErrorCode    错误码
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode
eMBClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBFrameCloseCur != NULL )
        {
            pvMBFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

/**********************************************************************
 * @brief  MODBUS协议栈使能
 * @return eMBErrorCode    协议栈错误
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode
eMBEnable( void )
{

    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur(  );                   //激活协议栈
        eMBState = STATE_ENABLED;                //设置Modbus协议栈工作状态eMBState为STATE_ENABLED
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

/**********************************************************************
 * @brief  MODBUS协议栈禁止
 * @return eMBErrorCode    协议栈错误
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode
eMBDisable( void )
{
    eMBErrorCode    eStatus;

    if( eMBState == STATE_ENABLED )
    {
        pvMBFrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

/**********************************************************************
 * @brief  MODBUS协议栈轮询
 *          1. 检查协议栈状态是否使能，eMBState初值为STATE_NOT_INITIALIZED，
 *             在eMBInit()函数中被赋值为STATE_DISABLED,在eMBEnable函数中被赋值为STATE_ENABLE;
 *          2. 轮询EV_FRAME_RECEIVED事件发生，若EV_FRAME_RECEIVED事件发生，
 *             接收一帧报文数据，上报EV_EXECUTE事件，解析一帧报文，响应(发送)一帧数据给主机;
 * @return eMBErrorCode    协议栈错误
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode eMBPoll( void )
{
    static UCHAR   *ucMBFrame;            //接收和发送报文数据缓存区
    static UCHAR    ucRcvAddress;         //modbus从机地址
    static UCHAR    ucFunctionCode;       //功能码
    static USHORT   usLength;             //报文长度
    static eMBException eException;       //错误码响应 枚举
    int             i;

	eMBEventType    eEvent;              //错误码
	CPU_SR_ALLOC();
	
    eMBErrorCode    eStatus = MB_ENOERR;
   
    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED )      //检查协议栈是否使能
    {
        return MB_EILLSTATE;             //协议栈未使能，返回协议栈无效错误码
    }

     /* 检查是否有事件发生。 若没有事件发生，将控制权交还主调函数. 否则，将处理该事件 */
    if( xMBPortEventGet( &eEvent ) == TRUE )              
    {

        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:                    //接收到一帧数据，此事件发生

			AppLedOn( &LedModbus2 );
		
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0	
            /*CRC校验、提取地址、有效数据指针和有效数据长度*/
            eStatus = peMBFrameReceiveCur( &ucRcvAddress, &ucMBFrame, &usLength ); //ucRcvAddress 主站要读取的从站的地址，ucMBFrame 指向PDU的头部，usLength PDU的长度    
          	
		    if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucRcvAddress == ucMBAddress ) || ( ucRcvAddress == MB_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost( EV_EXECUTE );      //修改事件标志为EV_EXECUTE执行事件
                }
            }
#endif  
				
#if MB_SLAVE_CPN_ENABLED > 0		
			eStatus = peMBCPNFrameReceiveCur( &ucSourAddress, &ucDestAddress, &ucMBFrame, &usLength );
			
		    if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucDestAddress == ucMBAddress ) || ( ucDestAddress == MB_CPN_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost( EV_EXECUTE );      //修改事件标志为EV_EXECUTE执行事件
                }
            }     
#endif  
//			AppLedOff( &LedModbus2 );
		break;
			
        case EV_EXECUTE:
			
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0	
           
		    ucFunctionCode = *( ucMBFrame + MB_PDU_FUNC_OFF );              //提取功能码
            eException = MB_EX_ILLEGAL_FUNCTION;
		
		for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );  //xFuncHandlers数组的成员为xMBFunctionHandler结构体，
                    AppLedOn( &LedModbus2 );
					break;                                                            //该结构体将功能码和相应功能的处理函数捆绑在一起。
                }
            }
			
            /*若不是广播命令，则需要发出响应。*/
            if( ucRcvAddress != MB_ADDRESS_BROADCAST )     
            {
                if( eException != MB_EX_NONE )
                {
                    /*发生异常，建立一个错误报告帧*/
                    usLength = 0;
                    *(ucMBFrame + (usLength++)) = (UCHAR)( ucFunctionCode | MB_FUNC_ERROR );  //响应发送数据帧的第二个字节，功能码最高位置1
                    *(ucMBFrame + (usLength++)) = eException;                                   //响应发送数据帧的第三个字节为错误码标识
                }
                 /* eMBRTUSend()进行必要的发送预设后，禁用RX，使能TX。发送操作由USART_DATA（UDR空）中断实现。*/			
                eStatus = peMBFrameSendCur( ucMBAddress, ucMBFrame, usLength );     //modbus从机响应函数,发送响应给主机
			}
#endif    

#if MB_SLAVE_CPN_ENABLED > 0	
			
			ucFunctionCode = *(ucMBFrame + MB_CPN_PDU_FUNC_OFF) + MB_CPN_FUNC_CODE_OFF_TO_REAL;  //提取功能码，并加上偏移
			eException = MB_EX_ILLEGAL_FUNCTION;
			
			for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
					AppLedOn( &LedModbus2 );
                    eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );  //xFuncHandlers数组的成员为xMBFunctionHandler结构体，
                    break;                                                            //该结构体将功能码和相应功能的处理函数捆绑在一起。
                }
            }

            if( ucFunctionCode == MB_FUNC_CPN_READ )     
            {
                if( eException == MB_EX_NONE )
                {
                    /* eMBRTUSend()进行必要的发送预设后，禁用RX，使能TX。发送操作由USART_DATA（UDR空）中断实现。*/			
					eStatus = peMBCPNFrameSendCur( ucMBAddress, ucSourAddress, ucMBFrame, usLength );     //modbus从机响应函数,发送响应给主机,注意地址对换
			    }    
			}			
#endif 			
        break;

        case EV_FRAME_SENT:
			vMBPortSerialEnable( TRUE, FALSE );      //使能接收，禁止发送
		    AppLedOff( &LedModbus2 );
        break;
		
		default: break;
        }
    }
   	
    return MB_ENOERR;
}

#endif
