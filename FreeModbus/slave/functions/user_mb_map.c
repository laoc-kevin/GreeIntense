#include "user_mb_map.h"
#include "user_mb_dict.h"

#if MB_FUNC_READ_INPUT_ENABLED > 0
/***********************************************************************************
 * @brief  输入寄存器映射
 * @param  usAddress      寄存器地址
 * @param  pvRegInValue   寄存器指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveRegInMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usAddress, sMBSlaveRegData** pvRegInValue)
{
	UCHAR i;
	eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveCommInfo*        psMBCommInfo  = psMBSlaveInfo->psMBCommInfo;
	const sMBSlaveDataTable* psRegInputBuf = psMBCommInfo->psSlaveCurData->psMBRegInTable;
	
	switch( SLAVE_PROTOCOL_TYPE_ID )
	{
		case 0:
			switch( usAddress )
	       {
			    case 0: i= 0;break;
			    default:
					return MB_ENOREG;
				break;
		   }
		break;
		   
		default: break;
	}
	*pvRegInValue = (sMBSlaveRegData*)psRegInputBuf->pvDataBuf + i;
    return eStatus;	
}

#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

/***********************************************************************************
 * @brief  保持寄存器映射
 * @param  usRegHoldAddr    寄存器地址
 * @param  pvRegHoldValue   寄存器指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveRegHoldMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usRegHoldAddr, sMBSlaveRegData** pvRegHoldValue)
{
	UCHAR i;
    eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveCommInfo*        psMBCommInfo  = psMBSlaveInfo->psMBCommInfo;
	const sMBSlaveDataTable* psRegHoldBuf = psMBCommInfo->psSlaveCurData->psMBRegHoldTable;

	switch (SLAVE_PROTOCOL_TYPE_ID)
	{
		case 0:                             //字典各寄存器的映射地址
			switch(usRegHoldAddr)
			{
				case 0: i= 0;break;
				case 1: i= 1;break;
				case 3: i= 2;break;
				case 4: i= 3;break;
				case 5: i= 4;break;
				
				case   9: i= 5;break;			
				case  23: i= 6;break;
				case  27: i= 7;break;
				case  46: i= 8;break;
				case 180: i= 9;break;
				
				case 181: i= 10;break;
				case 182: i= 11;break;
				case 183: i= 12;break;
				case 186: i= 13;break;
				case 187: i= 14;break;
				
				case 192: i= 15;break;
				case 194: i= 16;break;
				case 195: i= 17;break;
				case 196: i= 18;break;
				case 197: i= 19;break;
				
				case 202: i= 20;break;
				case 204: i= 21;break;
				case 212: i= 22;break;
				case 220: i= 23;break;
				case 240: i= 24;break;
			
				case 246: i= 25;break;
				case 247: i= 26;break;
				case 248: i= 27;break;
				case 316: i= 28;break;
				case 319: i= 29;break;
				
				case 342: i= 30;break;
				case 343: i= 31;break;
				case 800: i= 32;break;
									
				default:
					return MB_ENOREG;
				break;
			}
		break;	
        default: break;
	}
	*pvRegHoldValue = (sMBSlaveRegData*)psRegHoldBuf->pvDataBuf + i;
	return eStatus;
}
#endif

#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0

/***********************************************************************************
 * @brief  线圈字典映射
 * @param  usCoilAddr    线圈地址
 * @param  pvCoilValue   线圈指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveCoilsMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usCoilAddr, sMBSlaveBitData** pvCoilValue)
{
	UCHAR i;
    USHORT iRegIndex, iRegBitIndex,iBit ;
    
	eMBErrorCode    eStatus = MB_ENOERR;
	
	sMBSlaveCommInfo*        psMBCommInfo  = psMBSlaveInfo->psMBCommInfo;
	const sMBSlaveDataTable* psCoilBuf = psMBCommInfo->psSlaveCurData->psMBCoilTable;
    
	switch (SLAVE_PROTOCOL_TYPE_ID)
	{
		case 0:
			switch(usCoilAddr)        //映射地址
			{
				case 17: i= 0;break;
				case 33: i= 1;break;
				case 37: i= 2;break;
				case 38: i= 3;break;
				case 43: i= 4;break;
				
				case 44: i= 5;break;
				case 47: i= 6;break;
				case 76: i= 7;break;
				default:
					return MB_ENOREG;
				break;
			}
		break;
			
		default: break;
	}
	*pvCoilValue = (sMBSlaveBitData*)psCoilBuf->pvDataBuf + i ;
	return eStatus;
} 

#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0

/***********************************************************************************
 * @brief  离散量字典映射
 * @param  usCoilAddr    线圈地址
 * @param  pvCoilValue   线圈指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/

eMBErrorCode 
eMBSlaveDiscreteMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usDiscreteAddr, sMBSlaveBitData** pvDiscreteValue)
{
	UCHAR i;
    USHORT iRegIndex, iRegBitIndex,iBit;
    
	eMBErrorCode    eStatus = MB_ENOERR;

	sMBSlaveCommInfo*        psMBCommInfo  = psMBSlaveInfo->psMBCommInfo;
	const sMBSlaveDataTable* psDiscInBuf = psMBCommInfo->psSlaveCurData->psMBDiscInTable;

	switch (SLAVE_PROTOCOL_TYPE_ID)
	{
		case 0:
			switch(usDiscreteAddr)
			{
				case 0: i= 0;break;
				default:
					return MB_ENOREG;
				break;
			}
		break;
	}
	*pvDiscreteValue = (sMBSlaveBitData*)psDiscInBuf->pvDataBuf + i ;
	 return eStatus;
}
#endif


#if MB_SLAVE_CPN_ENABLED > 0 

/***********************************************************************************
 * @brief  CPN变量字典映射
 * @param  usCpnName    变量名称
 * @param  pvCPNValue   变量指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveCPNMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usCpnName, sMBSlaveCPNData ** pvCPNValue)
{
	UCHAR i;
	eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveCommInfo*        psMBCommInfo  = psMBSlaveInfo->psMBCommInfo;
	const sMBSlaveDataTable* psCPNBuf = psMBCommInfo->psSlaveCurData->psMBCPNTable;

	switch (SLAVE_PROTOCOL_TYPE_ID)
	{
		case 0:
			switch(usCpnName)
			{
				case 0x200: i= 0;break;
				case 0x202: i= 1;break;
				case 0x206: i= 2;break;
				case 0x207: i= 3;break;
				case 0x208: i= 4;break;
				
				case 0x210: i= 5;break;
				case 0x211: i= 6;break;
				case 0x214: i= 7;break;
				case 0x215: i= 8;break;
				case 0x216: i= 9;break;
				
				case 0x217: i= 10;break;
				case 0x218: i= 11;break;
				case 0x219: i= 12;break;
				case 0x21A: i= 13;break;
				case 0x21B: i= 14;break;
				
				case 0x21C: i= 15;break;
				case 0x220: i= 16;break;
				case 0x221: i= 17;break;
				case 0x224: i= 18;break;
				case 0x228: i= 19;break;
				
				case 0x230: i= 20;break;
				case 0x242: i= 21;break;
				case 0x243: i= 22;break;
				case 0x244: i= 23;break;
				case 0x245: i= 24;break;
				
				case 0x246: i= 25;break;
				case 0x247: i= 26;break;
				case 0x24A: i= 27;break;
				case 0x24B: i= 28;break;
				case 0x270: i= 29;break;
				
				case 0x271: i= 30;break;
				case 0x280: i= 31;break;
				case 0x281: i= 32;break;
				case 0x283: i= 33;break;
				case 0x284: i= 34;break;
				
				case 0x286: i= 35;break;
				case 0x288: i= 36;break;
				case 0x289: i= 37;break;
				case 0x28A: i= 38;break;
				case 0x28B: i= 39;break;
				
				case 0x311: i= 40;break;
				case 0x313: i= 41;break;
				case 0x319: i= 42;break;
			    case 0x31A: i= 43;break;
				case 0x31D: i= 44;break;
				
//				case 544: i= 36;break;
				default:
					return MB_ENOREG;
				break;
			}
		break;
		default: break;
	}
	*pvCPNValue = (sMBSlaveCPNData*)psCPNBuf->pvDataBuf + i;
	return eStatus;
}

#endif

