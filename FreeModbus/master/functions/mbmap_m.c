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
                                      USHORT usRegAddr, sMasterRegInData** pvRegInValue)
{
	USHORT usIndex;

    eMBMasterReqErrCode        eStatus  = MB_MRE_NO_ERR;
    sMBSlaveDev*        psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*     psMBRegInTable = &psMBSlaveDevCur->psDevCurData->sMBRegInTable;
	*pvRegInValue = NULL;
	
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
    
    if(psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex == NULL || psMBRegInTable == NULL || psMBRegInTable->pvDataBuf == NULL)
    {
         return MB_MRE_EILLSTATE;
    }
    if(psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex(RegInputData, psMBSlaveDevCur->ucProtocolID, usRegAddr, &usIndex))  //字典映射函数
    {
        *pvRegInValue = (sMasterRegInData*)(psMBRegInTable->pvDataBuf) + usIndex; //指针赋值，这里传递的是个地址，指向目标寄存器所在数组位置
    }
	else
	{
        return MB_MRE_NO_REG;
    }	
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
                                           USHORT usRegAddr, sMasterRegHoldData** pvRegHoldValue)
{
	USHORT usIndex;
    
	eMBMasterReqErrCode        eStatus  = MB_MRE_NO_ERR;
    sMBSlaveDev*        psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*   psMBRegHoldTable = &psMBSlaveDevCur->psDevCurData->sMBRegHoldTable;
	*pvRegHoldValue = NULL;
	
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
    if(psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex == NULL || psMBRegHoldTable == NULL || psMBRegHoldTable->pvDataBuf == NULL)
    {
         return MB_MRE_EILLSTATE;
    }
	if( psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex(RegHoldData, psMBSlaveDevCur->ucProtocolID, usRegAddr, &usIndex))  //字典映射函数
    {
        *pvRegHoldValue = (sMasterRegHoldData*)(psMBRegHoldTable->pvDataBuf) + usIndex;
    }
	else
	{
        return MB_MRE_NO_REG;
    }
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
                                     USHORT usCoilAddr, sMasterBitCoilData** pvCoilValue)
{
	USHORT usIndex, iRegIndex, iRegBitIndex, iBit;

	eMBMasterReqErrCode          eStatus = MB_MRE_NO_ERR;
    sMBSlaveDev*         psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*       psMBCoilTable = &psMBSlaveDevCur->psDevCurData->sMBCoilTable;
	*pvCoilValue = NULL;
	
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
    if(psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex == NULL || psMBCoilTable == NULL || psMBCoilTable->pvDataBuf == NULL)
    {
         return MB_MRE_EILLSTATE;
    }    
	if( psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex(CoilData, psMBSlaveDevCur->ucProtocolID, usCoilAddr, &usIndex) )  //字典映射函数
	{
        *pvCoilValue = (sMasterBitCoilData*)(psMBCoilTable->pvDataBuf) + usIndex;
    }
	else
	{
        return MB_MRE_NO_REG;
    }
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
                                         USHORT usDiscreteAddr, sMasterBitDiscData** pvDiscreteValue)
{
	USHORT usIndex, iRegIndex, iRegBitIndex,iBit;

	eMBMasterReqErrCode          eStatus = MB_MRE_NO_ERR;
    sMBSlaveDev*         psMBSlaveDevCur = psMBMasterInfo->sMBDevsInfo.psMBSlaveDevCur;     //当前从设备
    sMBDevDataTable*     psMBDiscInTable = &psMBSlaveDevCur->psDevCurData->sMBDiscInTable;
	*pvDiscreteValue = NULL;
	
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
    if(psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex == NULL || psMBDiscInTable == NULL || psMBDiscInTable->pvDataBuf == NULL)
    {
         return MB_MRE_EILLSTATE;
    }    
	if( psMBSlaveDevCur->psDevCurData->pxDevDataMapIndex(DiscInData, psMBSlaveDevCur->ucProtocolID, usDiscreteAddr, &usIndex))  //字典映射函数
    {
        *pvDiscreteValue = (sMasterBitDiscData*)(psMBDiscInTable->pvDataBuf)  + usIndex;
    }
    else
	{
        return MB_MRE_NO_REG;
    }
    return eStatus;		
}    

#endif

/***********************************************************************************
 * @brief  保持寄存器数据点初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void vMBMasterDevRegHoldDataInit(sMasterRegHoldData* pData, USHORT usAddr, UCHAR ucDataType, LONG lMinVal, LONG lMaxVal, 
                                 USHORT usPreVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue)                                  

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
void vMBMasterDevRegInDataInit(sMasterRegInData* pData, USHORT usAddr, UCHAR ucDataType, LONG lMinVal, 
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
void vMBMasterDevCoilDataInit(sMasterBitCoilData* pData, USHORT usAddr, UCHAR ucPreVal, UCHAR ucAccessMode, void* pvValue)                                  
{
    pData->usAddr        = usAddr;           //地址
    pData->ucPreVal      = ucPreVal;         //先前值
    pData->ucAccessMode  = ucAccessMode;     //访问权限
    pData->pvValue       = pvValue;          //变量指针
    
//    if(usAddr ==0)
//    {
//         myprintf("vMBMasterDevCoilDataInit %d  pvValue %d \n", pData, pvValue);
//    }
}                                                        

/***********************************************************************************
 * @brief  离散量数据点初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void vMBMasterDevDiscDataInit(sMasterBitDiscData* pData, USHORT usAddr, UCHAR ucAccessMode, void* pvValue)                                  
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
void vMBMasterDevTestCmdInit(sMBTestDevCmd* pCmd, USHORT usAddr, eMasterCmdMode eCmdMode ,USHORT usValue, BOOL xCheckVal)                                  
{
    pCmd->usValue   = usValue;       //数值
    pCmd->eCmdMode  = eCmdMode;      //测试模式
    pCmd->usAddr    = usAddr;        //测试点位通讯地址
    pCmd->xCheckVal = xCheckVal;     //测试时比较数值
}

/***********************************************************************************
 * @brief  心跳帧初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void vMBMasterDevHeartBeatInit(sMBDevHeartBeat* psDevHeartBeat, USHORT usAddr, eMasterCmdMode eCmdMode, 
                               USHORT usValue, USHORT usHeartBeatPeriod, BOOL xHeartBeatEnable)                                  
{
    psDevHeartBeat->usValue           = usValue;           //数值
    psDevHeartBeat->eCmdMode          = eCmdMode;          //测试模式
    psDevHeartBeat->usAddr            = usAddr;            //测试点位通讯地址
    psDevHeartBeat->usHeartBeatPeriod = usHeartBeatPeriod; //心跳间隔(s)
    psDevHeartBeat->xHeartBeatEnable  = xHeartBeatEnable;  //使能
}

/***********************************************************************************
 * @brief 数据表初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void vMBMasterDevDataTableInit(sMBDevDataTable* pDataTable, void* pvDataBuf, USHORT usStartAddr, USHORT usEndAddr, USHORT usDataCount)                                  
{
    pDataTable->pvDataBuf   = pvDataBuf;        //协议数据域
    pDataTable->usStartAddr = usStartAddr;      //起始地址
    pDataTable->usEndAddr   = usEndAddr;        //末尾地址
    pDataTable->usDataCount = usDataCount;      //协议点位总数
    
}
