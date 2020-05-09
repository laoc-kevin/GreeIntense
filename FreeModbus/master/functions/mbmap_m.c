#include "mbmap_m.h"
#include "mbdict_m.h"

#if MB_FUNC_READ_INPUT_ENABLED > 0

/***********************************************************************************
 * @brief  输入寄存器字典映射
* @param   ucSndAddr             从栈地址
 * @param  usRegAddr            寄存器地址
 * @param  pvRegInValue         寄存器指针
 * @return eMBMasterReqErrCode  错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterRegInMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                      USHORT usRegAddr, sMasterRegInData ** pvRegInValue)
{
	USHORT i;
	
    eMBMasterReqErrCode        eStatus  = MB_MRE_NO_ERR;
    sMBSlaveDev*        psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*     psMBRegInTable = &psMBSlaveDevCur->psDevCurData->sMBRegInTable;

    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBRegInTable = &psMBSlaveDevCur->psDevCurData->sMBRegInTable;
    } 
	if( (psMBRegInTable->pvDataBuf == NULL) || (psMBRegInTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
    i = psMBSlaveDevCur->psDevCurData->psMBDevDataMapIndex(RegInputData, psMBSlaveDevCur->ucProtocolID, usRegAddr);  //字典映射函数
    
	*pvRegInValue = (sMasterRegInData*)(psMBRegInTable->pvDataBuf) + i; //指针赋值，这里传递的是个地址，指向目标寄存器所在数组位置
	return eStatus;
}
#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

/***********************************************************************************
 * @brief  保持寄存器字典映射
 * @param  ucSndAddr      从栈地址
 * @param  usRegAddr      寄存器地址
 * @param  pvRegInValue   寄存器指针
 * @return eMBErrorCode   错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterRegHoldingMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                           USHORT usRegAddr, sMasterRegHoldData ** pvRegHoldValue)
{
	USHORT i;
    
	eMBMasterReqErrCode        eStatus  = MB_MRE_NO_ERR;
    sMBSlaveDev*        psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*   psMBRegHoldTable = &psMBSlaveDevCur->psDevCurData->sMBRegHoldTable;
    
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBRegHoldTable = &psMBSlaveDevCur->psDevCurData->sMBRegHoldTable;
    } 
	if( (psMBRegHoldTable->pvDataBuf == NULL) || (psMBRegHoldTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	i = psMBSlaveDevCur->psDevCurData->psMBDevDataMapIndex(RegHoldData, psMBSlaveDevCur->ucProtocolID, usRegAddr);  //字典映射函数
    
	*pvRegHoldValue = (sMasterRegHoldData*)(psMBRegHoldTable->pvDataBuf) + i ;
	return eStatus;
} 
#endif

#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0

/***********************************************************************************
 * @brief  线圈字典映射
 * @param   ucSndAddr      从栈地址
 * @param  usCoilAddr      线圈地址
 * @param  pvCoilValue     线圈指针
 * @return eMBErrorCode    错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterCoilMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                     USHORT usCoilAddr, sMasterBitCoilData ** pvCoilValue)
{
	UCHAR i;
	USHORT iRegIndex, iRegBitIndex, iBit;
	
	eMBMasterReqErrCode          eStatus = MB_MRE_NO_ERR;
    sMBSlaveDev*         psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*       psMBCoilTable = &psMBSlaveDevCur->psDevCurData->sMBCoilTable;
	
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBCoilTable = &psMBSlaveDevCur->psDevCurData->sMBCoilTable;
    } 
	if( (psMBCoilTable->pvDataBuf == NULL) || (psMBCoilTable->pvDataBuf == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	i = psMBSlaveDevCur->psDevCurData->psMBDevDataMapIndex(CoilData, psMBSlaveDevCur->ucProtocolID, usCoilAddr);  //字典映射函数
	
	*pvCoilValue = (sMasterBitCoilData*)(psMBCoilTable->pvDataBuf) + i ; 	
	return eStatus;
}    
#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
/***********************************************************************************
 * @brief  离散量字典映射
 * @param  ucSndAddr       从栈地址
 * @param  usCoilAddr      线圈地址
 * @param  pvCoilValue     线圈指针
 * @return eMBErrorCode    错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBMasterReqErrCode eMBMasterDiscreteMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                         USHORT usDiscreteAddr, sMasterBitDiscData ** pvDiscreteValue)
{
	UCHAR i;
	USHORT iRegIndex, iRegBitIndex,iBit;
	
	eMBMasterReqErrCode          eStatus = MB_MRE_NO_ERR;
    sMBSlaveDev*         psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*     psMBDiscInTable = &psMBSlaveDevCur->psDevCurData->sMBDiscInTable;
	
    if(psMBSlaveDevCur->ucDevAddr != ucSndAddr) //如果当前从设备地址与要轮询从设备地址不一致，则更新从设备
    {
        psMBSlaveDevCur = psMBMasterGetDev(psMBMasterInfo, ucSndAddr);
        psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur = psMBSlaveDevCur;
        psMBDiscInTable = &psMBSlaveDevCur->psDevCurData->sMBDiscInTable;
    } 
	if( (psMBDiscInTable->pvDataBuf == NULL) || (psMBDiscInTable->usDataCount == 0)) //非空且数据点不为0
	{
		return MB_MRE_ILL_ARG;
	}
	i = psMBSlaveDevCur->psDevCurData->psMBDevDataMapIndex(DiscInData, psMBSlaveDevCur->ucProtocolID, usDiscreteAddr);  //字典映射函数
    
	*pvDiscreteValue = (sMasterBitDiscData*)(psMBDiscInTable->pvDataBuf)  + i;
	return eStatus;
}    

#endif

/***********************************************************************************
 * @brief  字典初始化,初始化各点位的原始值
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBMasterTableInit(const sMBMasterInfo* psMBMasterInfo)
{
//	USHORT iIndex, iBit, iTable;
//	eMBMasterReqErrCode    eStatus = MB_MRE_NO_ERR;
//	
//	const sMBDevDataTable*  psRegHoldBuf;
//	sMasterRegHoldData*     psRegHoldValue;
//	
//	const sMBDevDataTable*  psCoilBuf;
//	sMasterBitCoilData*     psCoilValue;

//	sMBMasterDictInfo* psMBDictInfo = psMBMasterInfo->psMBMasterDictInfo;          //通讯字典
//	sMBMasterDevsInfo* psMBDevsInfo = psMBMasterInfo->psMBDevsInfo;          //从设备状态

//	USHORT nSlaveTypes = psMBDevsInfo->ucSlaveDevTypes;                            //从设备类型数
//	
//#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
//    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0
//	
//	for(iTable = 0; iTable < nSlaveTypes; iTable++)
//	{
//		psRegHoldBuf = &(psMBDictInfo->psMBRegHoldTable[iTable]);
//		
//		if( (psRegHoldBuf->pvDataBuf != NULL) && (psRegHoldBuf->usDataCount !=0) )
//		{
//			for(iIndex = 0; iIndex < psRegHoldBuf->usDataCount; iIndex++)
//		    {
//		    	psRegHoldValue = (sMasterRegHoldData*)(psRegHoldBuf->pvDataBuf) + iIndex;
//		    	if(psRegHoldValue->Value != NULL)
//		    	{
//		    		if (psRegHoldValue->DataType == uint16)
//		    		{
//		    			psRegHoldValue->PreValue = *(USHORT*)psRegHoldValue->Value;
//		    		}
//		    		else if(psRegHoldValue->DataType == uint8)
//		    		{
//		    			psRegHoldValue->PreValue = (USHORT)(*(UCHAR*)psRegHoldValue->Value);
//		    		}
//		    		else if (psRegHoldValue->DataType == int16)
//		    		{
//		    			psRegHoldValue->PreValue = (USHORT)(*(SHORT*)psRegHoldValue->Value);
//		    		}
//		    		else if(psRegHoldValue->DataType == int8)
//		    		{
//		    			psRegHoldValue->PreValue = (USHORT)(*(int8_t*)psRegHoldValue->Value);
//		    		}  
//		    	}
//		    }
//		}	
//	}	
//#endif
//	
//#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
//	
//	for(iTable = 0; iTable < nSlaveTypes; iTable++)
//	{
//		psCoilBuf = &(psMBDictInfo->psMBCoilTable[iTable]);
//		
//		if( (psCoilBuf->pvDataBuf != NULL) || (psCoilBuf->usDataCount !=0) )
//		{
//			for(iIndex = 0; iIndex < psCoilBuf->usDataCount; iIndex++)
//		    {
//		    	psCoilValue = (sMasterBitCoilData*)(psCoilBuf->pvDataBuf) + iIndex;
//				
//                if( psCoilValue->Value  != NULL )
//		    	{
//		    		psCoilValue->PreValue  = *(UCHAR*)psCoilValue->Value;
//		    	}
//		    }
//		}
//	}
//#endif	
		
}

