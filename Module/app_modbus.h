#ifndef _APP_MODBUS_H_
#define _APP_MODBUS_H_

#define MODBUS_DEBUG                            0 
#define MODBUS_ACCESS_ADDR                      1     //自动寻址
#define MODBUS_ACCESS_ADDR_INTERVAL_TIMES       10     //自动寻址轮询间隔次数

#define MB_SCAN_READ_SLAVE_INTERVAL_MS          20
#define MB_SCAN_WRITE_SLAVE_INTERVAL_MS         20

//extern uint8_t ControllerID;

void AppMbSlavePollTask(void *p_arg);
void AppMbSlaveUartTxTask(void *p_arg);

void AppMbMasterPollTask(void *p_arg);
void AppMbScanSlaveTask(void *p_arg);
void AppMbScanWriteSlaveTask(void *p_arg);

#endif
