/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2013 China Beijing Armink <armink.ztl@gmail.com>
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
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/17 11:42:56 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb_m.h"
#include "mbrtu_m.h"
#include "mbframe.h"
#include "mbconfig.h"

#include "mbdict_m.h"

#if MB_MASTER_RTU_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

/**********************************************************************
 * @brief  RTU模式协议栈初始化
 * @param  *Uart           UART配置
 * @return eMBErrorCode    协议栈错误
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode eMBMasterRTUInit(sMBMasterInfo* psMBMasterInfo)
{
    ULONG           usTimerT35_50us;
    
    eMBErrorCode    eStatus = MB_ENOERR;
    sMBMasterPort* psMBPort = &psMBMasterInfo->sMBPort;
    
	const sUART_Def* psMBMasterUart = psMBPort->psMBMasterUart;
	
    ENTER_CRITICAL_SECTION();

    /* Modbus RTU uses 8 Databits. */
	
    if( xMBMasterPortSerialInit(psMBPort) != TRUE )      //串口初始化
    {
        eStatus = MB_EPORTERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if(psMBMasterUart->UARTCfg.Baud_rate > 19200)
        {
            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = (7UL * 220000UL) / (2UL * psMBMasterUart->UARTCfg.Baud_rate);
        }
        if( xMBsMasterPortTmrsInit(psMBPort, (USHORT)usTimerT35_50us) != TRUE )       //t35超时定时器
        {
            eStatus = MB_EPORTERR;
        }
    }
    EXIT_CRITICAL_SECTION();

    return eStatus;
}

/**********************************************************************
 * @brief  开始RTU模式协议栈
 *         1. 设置接收状态机eRcvState为STATE_RX_INIT；
 *         2. 使能串口接收,禁止串口发送,作为从机,等待主机传送的数据?
 *         3. 开启定时器，3.5T时间后定时器发生第一次中断,此时eRcvState为STATE_RX_INIT,
 *            上报初始化完成事件,然后设置eRcvState为空闲STATE_RX_IDLE?

 *         4. 每次进入3.5T定时器中断,定时器被禁止，等待串口有字节接收后，才使能定时器?
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
void eMBMasterRTUStart(sMBMasterInfo* psMBMasterInfo)
{
	sMBMasterPort* psMBPort = &psMBMasterInfo->sMBPort;
	
    ENTER_CRITICAL_SECTION();     //关全局中断
    /* Initially the receiver is in the state STATE_M_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_M_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    psMBMasterInfo->eRcvState = STATE_M_RX_INIT;
    vMBMasterPortSerialEnable(psMBPort, TRUE, FALSE);    //从栈等待数据，开启串口接收，发送未开启
    vMBsMasterPortTmrsEnable(psMBPort);                  //启动定时器

    EXIT_CRITICAL_SECTION();      //开全局中断
}

/**********************************************************************
 * @brief  关闭RTU模式协议栈
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
void eMBMasterRTUStop(sMBMasterInfo* psMBMasterInfo)
{
	sMBMasterPort* psMBPort = &psMBMasterInfo->sMBPort;
	
    ENTER_CRITICAL_SECTION();
	
    vMBMasterPortSerialEnable(psMBPort,FALSE, FALSE);
    vMBsMasterPortTmrsDisable(psMBPort);
	
    EXIT_CRITICAL_SECTION();
}

/**********************************************************************
 * @brief  报文接收处理
 *         1. eMBPoll函数轮询到EV_FRAME_RECEIVED事件时,调用peMBFrameReceiveCur()
 *         2. 从一帧数据报文中，取得modbus从机地址给pucRcvAddress、PDU报文的长度给pusLength
 *         3. PDU报文的首地址给pucFrame，函数*形参全部为地址传递
 * @param  pucRcvAddress   从站地址
 * @param  pucFrame        MODBUS数据帧指针
 * @param  pusLength       PDU长度
 * @return eMBErrorCode    协议栈错误 
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode
eMBMasterRTUReceive(sMBMasterInfo* psMBMasterInfo, UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength)
{
    /*  eMBRTUReceive函数完成了CRC校验、帧数据地址和长度的赋值，便于给上层进行处理*/
	
	USHORT usBytes = 0;
    eMBErrorCode    eStatus = MB_ENOERR;
	
	ENTER_CRITICAL_SECTION();
    assert_param(usRcvBufferPos < MB_SER_PDU_SIZE_MAX);      //断言宏，判断接收到的字节数<256，如果>256，终止程序
  
	psMBMasterInfo->xFrameErrReceived = TRUE;
	
	*pucRcvAddress = psMBMasterInfo->ucRTURcvBuf[MB_SER_PDU_ADDR_OFF];
	
    if( *pucRcvAddress >= 6 && psMBMasterInfo->usRcvBufferPos == 17)
	{
		if(psMBMasterInfo->usRcvBufferPos >= psMBMasterInfo->usSndBufferPos)
	    {
		    usBytes = psMBMasterInfo->usRcvBufferPos - psMBMasterInfo->usSndBufferPos;
	    }
	    if((usBytes >= MB_SER_PDU_SIZE_MIN)
        && (usMBCRC16( (UCHAR*)psMBMasterInfo->ucRTURcvBuf + psMBMasterInfo->usSndBufferPos, usBytes) == 0) )
        {
            *pusLength = (USHORT)(usBytes - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC);     //PDU的长度为数据帧-从栈地址-CRC校验
            *pucFrame = (UCHAR*) &(psMBMasterInfo->ucRTURcvBuf[MB_SER_PDU_PDU_OFF+psMBMasterInfo->usSndBufferPos]);      //pucFrame指向PDU起始位置
        }
	    else
        {
            eStatus = MB_EIO;
        }
	}
	else
    {
	    /* Length and CRC check */
        if((psMBMasterInfo->usRcvBufferPos >= MB_SER_PDU_SIZE_MIN)
        && (usMBCRC16( (UCHAR*)psMBMasterInfo->ucRTURcvBuf, psMBMasterInfo->usRcvBufferPos ) == 0) )
        {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = psMBMasterInfo->ucRTURcvBuf[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        *pusLength = (USHORT)(psMBMasterInfo->usRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC);     //PDU的长度为数据帧-从栈地址-CRC校验

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = (UCHAR*) &(psMBMasterInfo->ucRTURcvBuf[MB_SER_PDU_PDU_OFF]);      //pucFrame指向PDU起始位置
        }
	    else
        {
            eStatus = MB_EIO;
        }
		if( *pucRcvAddress >= 6)
		{
			if(psMBMasterInfo->usRcvBufferPos == 8)
	        {
     		     eStatus = MB_EIO;		 
	        }
			if(psMBMasterInfo->usRcvBufferPos == 7)
			{
				UART_FIFOReset(psMBMasterInfo->sMBPort.psMBMasterUart->ID, ( UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV2));
			}
		}
        
	}
//	if(*pucRcvAddress == 1)
//	{
//		   myprintf("eMBMasterRTUReceive***pucRcvAddress %d *pusLength  %d usRcvBufferPos %d \n",
//		*pucRcvAddress, *pusLength, psMBMasterInfo->usRcvBufferPos);
//	}
 
	
    psMBMasterInfo->xFrameErrReceived = FALSE;
  
    EXIT_CRITICAL_SECTION();
    return eStatus;
}

/**********************************************************************
 * @brief  报文发送处理
 *         1. 对响应报文PDU前面加上从机地址
 *         2. 对响应报文PDU后加上CRC校
 *         3. 使能发送，启动传输
 * @param  ucSlaveAddr   从站地址
 * @param  pucFrame        MODBUS数据帧指针
 * @param  pusLength       PDU长度
 * @return eMBErrorCode    协议栈错误  
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
eMBErrorCode
eMBMasterRTUSend(sMBMasterInfo* psMBMasterInfo, UCHAR ucSlaveAddr, const UCHAR* pucFrame, USHORT usLength)
{
    /* 在 eMBRTUSend函数中会调用串口发送数据，在进入串口发送中断后会调用xMBRTUTransmitFSM
     * 发送状态机函数发送应答报文。*/
    USHORT          usCRC16;
    eMBErrorCode    eStatus = MB_ENOERR;

    sMBMasterPort*     psMBPort     = &psMBMasterInfo->sMBPort;
	sMBMasterDevsInfo* psMBDevsInfo = &psMBMasterInfo->sMBDevsInfo;          //从设备状态

    if( (ucSlaveAddr < psMBDevsInfo->ucSlaveDevMinAddr) || (ucSlaveAddr > psMBDevsInfo->ucSlaveDevMaxAddr) ) 
	{
		return MB_EINVAL;
	}
    
    ENTER_CRITICAL_SECTION();

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if(psMBMasterInfo->eRcvState == STATE_M_RX_IDLE)
    {
        /* First byte before the Modbus-PDU is the slave address. */
        psMBMasterInfo->pucSndBufferCur = (UCHAR*)(pucFrame - 1);
        psMBMasterInfo->usSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        *( psMBMasterInfo->pucSndBufferCur + MB_SER_PDU_ADDR_OFF ) = ucSlaveAddr;        //在协议数据单元前加从机地址
        psMBMasterInfo->usSndBufferCount += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( (UCHAR*) psMBMasterInfo->pucSndBufferCur, psMBMasterInfo->usSndBufferCount );
        psMBMasterInfo->ucRTUSndBuf[psMBMasterInfo->usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        psMBMasterInfo->ucRTUSndBuf[psMBMasterInfo->usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        psMBMasterInfo->eSndState = STATE_M_TX_XMIT;         //发送状态
        vMBMasterPortSerialEnable( psMBPort, FALSE, TRUE );  //使能发送，禁止接收	

		//启动第一次发送
        (void)xMBMasterPortSerialPutByte( psMBPort, (CHAR)(*psMBMasterInfo->pucSndBufferCur) );
        psMBMasterInfo->usSndBufferPos = psMBMasterInfo->usSndBufferCount;
		psMBMasterInfo->pucSndBufferCur++;
        psMBMasterInfo->usSndBufferCount--;	
		
		psMBMasterInfo->xFrameErrReceived = TRUE;
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION();
    return eStatus;
}

/**********************************************************************
 * @brief  串口接收数据
 *         1. 将接收到的数据存入ucRTUBuf[]中
 *         2. usRcvBufferPos为全局变量，表示接收数据的个数
 *         3. 每接收到一个字节的数据，3.5T定时器清0
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL xMBMasterRTUReceiveFSM(sMBMasterInfo* psMBMasterInfo)
{
    /*在串口中断前，状态机为eRcvState=STATE_RX_IDLE，接收状态机开始后，读取uart串口缓存中的数据，并进入STATE_RX_IDLE分支中存储一次数据后开启定时器，
    然后进入STATE_RX_RCV分支继续接收后续的数据，直至定时器超时！如果没有超时的话，状态不会转换，将还可以继续接收数据。超时之后，
    在T3.5超时函数xMBRTUTimerT35Expired 中将发送EV_FRAME_RECEIVED事件。然后eMBPoll函数将会调用eMBRTUReceive函数。*/
    UCHAR ucByte;
    BOOL  xTaskNeedSwitch = FALSE;
 
    sMBMasterPort* psMBPort = &psMBMasterInfo->sMBPort;
	
    assert_param(( eSndState == STATE_M_TX_IDLE ) || ( eSndState == STATE_M_TX_XFWR ));   //确保没有数据在发送或者主栈没有在等待从栈响应

    /* Always read the character. */
    (void)xMBMasterPortSerialGetByte(psMBPort, (CHAR*) &ucByte);

    switch(psMBMasterInfo->eRcvState)
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_M_RX_INIT:
        vMBsMasterPortTmrsEnable(psMBPort);
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_M_RX_ERROR:     //数据帧被损坏，重启定时器，不保存串口接收的数据
        vMBsMasterPortTmrsEnable(psMBPort);
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE and disable early
         * the timer of respond timeout .
         */
    case STATE_M_RX_IDLE:                                     // 接收器空闲，开始接收，进入STATE_RX_RCV状态
    	/* In time of respond timeout,the receiver receive a frame.
    	 * Disable timer of respond timeout and change the transmiter state to idle.
    	 */
    	vMBsMasterPortTmrsDisable(psMBPort);
    	psMBMasterInfo->eSndState = STATE_M_TX_IDLE;

        psMBMasterInfo->usRcvBufferPos = 0;
        psMBMasterInfo->ucRTURcvBuf[psMBMasterInfo->usRcvBufferPos++] = ucByte;
        psMBMasterInfo->eRcvState = STATE_M_RX_RCV;

        /* Enable t3.5 timers. */
        vMBsMasterPortTmrsEnable(psMBPort);              //重启3.5T定时器
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_M_RX_RCV:
        if(psMBMasterInfo->usRcvBufferPos < MB_SER_PDU_SIZE_MAX)  //一帧报文的字节数大于最大PDU长度，忽略超出的数据
        {
            psMBMasterInfo->ucRTURcvBuf[psMBMasterInfo->usRcvBufferPos++] = ucByte;
        }
        else
        {
            psMBMasterInfo->eRcvState = STATE_M_RX_ERROR;
        }
        vMBsMasterPortTmrsEnable(psMBPort);   //每收到一个字节，都重启3.5T定时器
        break;
	default: break;
    }
    return xTaskNeedSwitch;
}

/**********************************************************************
 * @brief  串口发送数据
 *         eMBRTUSend函数中会调用串口发送数据，在进入串口发送中断后会调用xMBRTUTransmitFSM
 *         发送状态机函数发送应答报文。
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL xMBMasterRTUTransmitFSM(sMBMasterInfo* psMBMasterInfo)
{
    BOOL           xNeedPoll = FALSE;
    sMBMasterPort*  psMBPort = &psMBMasterInfo->sMBPort;
	
    assert_param(eRcvState == STATE_M_RX_IDLE);

    switch(psMBMasterInfo->eSndState)
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_M_TX_IDLE:
        /* enable receiver/disable transmitter. */
	    psMBMasterInfo->usRcvBufferPos = 0;
        vMBMasterPortSerialEnable(psMBPort, TRUE, FALSE); //发送器处于空闲状态，使能接收，禁止发送
        break;

    case STATE_M_TX_XMIT:   //发送器处于发送状态,在从机发送函数eMBRTUSend中赋值STATE_TX_XMIT
        /* check if we are finished. */
        if(psMBMasterInfo->usSndBufferCount != 0)
        {
            (void)xMBMasterPortSerialPutByte( psMBPort, (CHAR)(*psMBMasterInfo->pucSndBufferCur) );          //发送数据
            psMBMasterInfo->pucSndBufferCur++;  /* next byte in sendbuffer. */
            psMBMasterInfo->usSndBufferCount--;
        }
        else
        {
            psMBMasterInfo->xFrameIsBroadcast = ( psMBMasterInfo->ucRTUSndBuf[MB_SER_PDU_ADDR_OFF] == MB_ADDRESS_BROADCAST ) ? TRUE : FALSE;
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
			
            vMBMasterPortSerialEnable(psMBPort, TRUE, FALSE);
            psMBMasterInfo->eSndState = STATE_M_TX_XFWR;
            /* If the frame is broadcast ,master will enable timer of convert delay,
             * else master will enable timer of respond timeout. */
            if ( psMBMasterInfo->xFrameIsBroadcast == TRUE )
            {
            	vMBsMasterPortTmrsConvertDelayEnable(psMBPort);
            }
            else
            {
            	vMBsMasterPortTmrsRespondTimeoutEnable(psMBPort);
//                myprintf("vMBsMasterPortTmrsRespondTimeoutEnable\n");
            }
        }
        break;
	default: break;
    }
    return xNeedPoll;
}

/**********************************************************************
 * @brief  超时响应函数
 * @return BOOL   
 * @author laoc
 * @date 2019.01.22
 *********************************************************************/
BOOL xMBMasterRTUTimerT35Expired(sMBMasterInfo* psMBMasterInfo)
{
	BOOL xNeedPoll = FALSE;
    BOOL xSndStateNeedChange = TRUE;
	sMBMasterPort* psMBPort = &psMBMasterInfo->sMBPort;
	
	switch (psMBMasterInfo->eRcvState)
	{
		/* Timer t35 expired. Startup phase is finished. */
	case STATE_M_RX_INIT:
		 xNeedPoll = xMBMasterPortEventPost(psMBPort, EV_MASTER_READY);
		break;

		/* A frame was received and t35 expired. Notify the listener that
		 * a new frame was received. */
	case STATE_M_RX_RCV:
		
	    if( psMBMasterInfo->usRcvBufferPos >= 5)   //防止错误数据而导致激发接收事件,该芯片存在bug，发送完数据后会自动接收上次发送的数据
		{
			xNeedPoll = xMBMasterPortEventPost(psMBPort, EV_MASTER_FRAME_RECEIVED);   //一帧数据接收完成，上报协议栈事件,接收到一帧完整的数据
//			myprintf("EV_MASTER_FRAME_RECEIVED****************** usRcvBufferPos %d \n", psMBMasterInfo->usRcvBufferPos);
		}
		else if(psMBMasterInfo->ucMBDestAddr == 1 || psMBMasterInfo->ucMBDestAddr == 2 )
		{
			psMBMasterInfo->eSndState = STATE_M_TX_XFWR;
			vMBsMasterPortTmrsRespondTimeoutEnable(psMBPort);      //接收数据不完整，重启定时器
	
			xSndStateNeedChange = FALSE;
			xNeedPoll = FALSE;
//			myprintf("EV_MASTER_FRAME_RECEIVED_ERROR****************** usRcvBufferPos %d \n", psMBMasterInfo->usRcvBufferPos);
//		
		    psMBMasterInfo->usRcvBufferPos = 0;
		}
		break;

		/* An error occured while receiving the frame. */
	case STATE_M_RX_ERROR:
		vMBMasterSetErrorType(psMBMasterInfo, EV_ERROR_RECEIVE_DATA);
		xNeedPoll = xMBMasterPortEventPost( psMBPort, EV_MASTER_ERROR_PROCESS );
		break;

		/* Function called in an illegal state. */
	default:
		assert_param( (eRcvState == STATE_M_RX_INIT)  || (eRcvState == STATE_M_RX_RCV) ||
				      (eRcvState == STATE_M_RX_ERRO ) || (eRcvState == STATE_M_RX_IDLE) );
		break;
	}
	psMBMasterInfo->eRcvState = STATE_M_RX_IDLE;   //处理完数据，接收器状态为空闲

	if(xSndStateNeedChange)
	{
		switch (psMBMasterInfo->eSndState)
		{
			/* A frame was send finish and convert delay or respond timeout expired.
			 * If the frame is broadcast,The master will idle,and if the frame is not
			 * broadcast.Notify the listener process error.*/
		case STATE_M_TX_XFWR:                       //接收超时
			if (psMBMasterInfo->xFrameIsBroadcast == FALSE) 
			{
				vMBMasterSetErrorType(psMBMasterInfo, EV_ERROR_RESPOND_TIMEOUT);
				xNeedPoll = xMBMasterPortEventPost(psMBPort, EV_MASTER_ERROR_PROCESS);   //上报接收数据超时
			}
			break;
			/* Function called in an illegal state. */
		default:
			assert_param( (eSndState == STATE_M_TX_XFWR) || (eSndState == STATE_M_TX_IDLE) );
			break;
		}
		psMBMasterInfo->eSndState = STATE_M_TX_IDLE;

        vMBsMasterPortTmrsDisable(psMBPort);                                 //当接收到一帧数据后，禁止3.5T定时器，直到接受下一帧数据开始，开始计时
	/* If timer mode is convert delay, the master event then turns EV_MASTER_EXECUTE status. */
        if (psMBPort->eCurTimerMode == MB_TMODE_CONVERT_DELAY) {
            xNeedPoll = xMBMasterPortEventPost(psMBPort, EV_MASTER_EXECUTE);
        }
	}
	return xNeedPoll;
}

/* Get Modbus Master send RTU's buffer address pointer.*/
void vMBMasterGetRTUSndBuf(const sMBMasterInfo* psMBMasterInfo, UCHAR** pucFrame)
{
	*pucFrame = (UCHAR*)psMBMasterInfo->ucRTUSndBuf;
}

/* Get Modbus Master send PDU's buffer address pointer.*/
void vMBMasterGetPDUSndBuf(const sMBMasterInfo* psMBMasterInfo, UCHAR** pucFrame)
{
	*pucFrame = (UCHAR*) &(psMBMasterInfo->ucRTUSndBuf[MB_SER_PDU_PDU_OFF]);
}

/* Set Modbus Master send PDU's buffer length.*/
void vMBMasterSetPDUSndLength(sMBMasterInfo* psMBMasterInfo, USHORT SendPDULength)
{
	psMBMasterInfo->usSndPDULength = SendPDULength;
}

/* Get Modbus Master send PDU's buffer length.*/
USHORT usMBMasterGetPDUSndLength(const sMBMasterInfo* psMBMasterInfo)
{
	return psMBMasterInfo->usSndPDULength;
}

/* The master request is broadcast? */
BOOL xMBMasterRequestIsBroadcast(const sMBMasterInfo* psMBMasterInfo)
{
	return psMBMasterInfo->xFrameIsBroadcast;
}

/* Get Modbus Master Receive State.*/
eMBMasterRcvState usMBMasterGetRcvState(const sMBMasterInfo* psMBMasterInfo)
{
	return psMBMasterInfo->eRcvState;
}

/* Get Modbus Master send State.*/
eMBMasterSndState usMBMasterGetSndState(const sMBMasterInfo* psMBMasterInfo)
{
	return psMBMasterInfo->eSndState;
}


#endif

