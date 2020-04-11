#ifndef APP_DTU_H
#define APP_DTU_H

#include "mbconfig.h"

#ifdef MB_MASTER_DTU_ENABLED     //GPRS模块功能支持
#include "mb_m.h"

#define DTU_TIMEOUT_S               30
#define TMR_TICK_PER_SECOND         OS_CFG_TMR_TASK_RATE_HZ

#define DTU247_INIT_START_REG_HOLD_ADDR         0x00
#define INIT_DTU247_REG_HOLD_VALUE              0xF0
#define INITED_DTU247_REG_HOLD_VALUE            0x0F

#define DTU200_SLAVE_ADDR                       200
#define DTU247_SLAVE_ADDR                       247

#define TEST_DTU200_PROTOCOL_REG_HOLD_ADDR      0
#define TEST_DTU200_PROTOCOL_REG_HOLD_VALUE     0x5601

#define TEST_DTU247_PROTOCOL_REG_HOLD_ADDR      1
#define TEST_DTU247_PROTOCOL_REG_HOLD_VALUE     0x0F

#define TEST_DTU247_PROTOCOL_REG_IN_ADDR        12
#define TEST_DTU247_PROTOCOL_REG_IN_VALUE       0xFF

void AppMbScanDevDTU(sMBMasterInfo* psMBMasterInfo);
void AppMbDTUInit(sMBMasterInfo* psMBMasterInfo);
#endif

#endif