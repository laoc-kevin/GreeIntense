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
 * @brief  保持寄存器数据点初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBMasterRegHoldDataInit(sMasterRegHoldData* pData, USHORT usAddr, UCHAR ucDataType, USHORT usPreVal,
                              LONG lMinVal, LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue)                                  

{
    pData->usAddr            = usAddr;              //地址
    pData->ucDataType        = ucDataType;          //数据类型
    pData->usPreVal          = usPreVal;            //先前值
    pData->lMinVal           = lMinVal;             //最小值
    pData->lMaxVal           = lMaxVal;             //最大值
    pData->ucAccessMode      = ucAccessMode;        //访问权限
    pData->fTransmitMultiple = fTransmitMultiple;   //传输因子
    pData->pvValue           = pvValue;             //变量指针 	
}

/***********************************************************************************
 * @brief  输入寄存器数据点初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBMasterRegInDataInit(sMasterRegInData* pData, USHORT usAddr, UCHAR ucDataType, LONG lMinVal, 
                            LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue)                                  
{
    pData->usAddr            = usAddr;              //地址  
    pData->ucDataType        = ucDataType;          //数据类型         
    pData->lMinVal           = lMinVal;             //最小值
    pData->lMaxVal           = lMaxVal;             //最大值
    pData->ucAccessMode      = ucAccessMode;        //访问权限
    pData->fTransmitMultiple = fTransmitMultiple;   //传输因子
    pData->pvValue           = pvValue;             //变量指针	
}

/***********************************************************************************
 * @brief  线圈数据点初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBMasterCoilDataInit(sMasterBitCoilData* pData, USHORT usAddr, UCHAR ucPreVal, UCHAR ucAccessMode, void* pvValue)                                  
{
    pData->usAddr        = usAddr;           //地址
    pData->ucPreVal      = ucPreVal;         //先前值
    pData->ucAccessMode  = ucAccessMode;     //访问权限
    pData->pvValue       = pvValue;          //变量指针
}                                                        

/***********************************************************************************
 * @brief  离散量数据点初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBMasterDiscDataInit(sMasterBitDiscData* pData, USHORT usAddr, UCHAR ucAccessMode, void* pvValue)                                  
{
    pData->usAddr       = usAddr;            //地址        
    pData->ucAccessMode = ucAccessMode;      //访问权限
    pData->pvValue      = pvValue;           //变量指针 	
}

/***********************************************************************************
 * @brief  测试命令点初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBMasterTestCmdDaInit(sMBTestDevCmd* pCmd, USHORT usValue, eMasterCmdMode eCmdMode, UCHAR ucAddr)                                  
{
    pCmd->usValue   = usValue;       //数值
    pCmd->eCmdMode  = eCmdMode;      //测试模式
    pCmd->ucAddr    = ucAddr;        //测试点位通讯地址
}

/***********************************************************************************
 * @brief 数据表初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBMasterDevDataTableInit(sMBDevDataTable* pDataTable, void* pvDataBuf, USHORT usStartAddr, USHORT usEndAddr, USHORT usDataCount)                                  
{
    pDataTable->pvDataBuf   = pvDataBuf;        //协议数据域
    pDataTable->usStartAddr = usStartAddr;      //起始地址
    pDataTable->usEndAddr   = usEndAddr;        //末尾地址
    pDataTable->usDataCount = usDataCount;      //协议点位总数
    
}
