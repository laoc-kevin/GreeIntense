#include "mbmap.h"
#include "mbdict.h"

#if MB_FUNC_READ_INPUT_ENABLED > 0
/***********************************************************************************
 * @brief  输入寄存器映射
 * @param  usRegInAddr    寄存器地址
 * @param  pvRegInValue   寄存器指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveRegInMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usRegInAddr, sMBSlaveRegData** pvRegInValue)
{
	eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveDataInfo*        psCurData  = psMBSlaveInfo->psMBCommInfo->psSlaveCurData;
	const sMBSlaveDataTable* psRegInputBuf = psCurData->psMBRegInTable;
	
    UCHAR i = psCurData->psMBSlaveDataMap(RegInputData, usRegInAddr);    //从栈字典映射函数
	
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
    eMBErrorCode    eStatus = MB_ENOERR;
    sMBSlaveDataInfo*          psCurData  = psMBSlaveInfo->psMBCommInfo->psSlaveCurData;
	const sMBSlaveDataTable* psRegHoldBuf = psCurData->psMBRegHoldTable;

	UCHAR i = psCurData->psMBSlaveDataMap(RegInputData, usRegHoldAddr);    //从栈字典映射函数

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
    eMBErrorCode    eStatus = MB_ENOERR;
    
	sMBSlaveDataInfo*        psCurData = psMBSlaveInfo->psMBCommInfo->psSlaveCurData;
	const sMBSlaveDataTable* psCoilBuf = psCurData->psMBCoilTable;
    
	UCHAR i = psCurData->psMBSlaveDataMap(RegInputData, usCoilAddr);    //从栈字典映射函数
    
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
    eMBErrorCode    eStatus = MB_ENOERR;

	sMBSlaveDataInfo*         psCurData  = psMBSlaveInfo->psMBCommInfo->psSlaveCurData;
	const sMBSlaveDataTable* psDiscInBuf = psCurData->psMBDiscInTable;

	UCHAR i = psCurData->psMBSlaveDataMap(RegInputData, usDiscreteAddr);    //从栈字典映射函数
    
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
	eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveDataInfo*        psCurData = psMBSlaveInfo->psMBCommInfo->psSlaveCurData;
	const sMBSlaveDataTable* psCPNBuf = psCurData->psMBCPNTable;

	UCHAR i = psCurData->psMBSlaveDataMap(ValCPNData, usCpnName);    //从栈字典映射函数
    
	*pvCPNValue = (sMBSlaveCPNData*)psCPNBuf->pvDataBuf + i;
	return eStatus;
}

#endif

