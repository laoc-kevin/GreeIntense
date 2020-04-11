#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#include "os.h"

/**************************任务配置开关*****************************/

//系统控制主任务
#define SYS_MAIN_CONTROL_TASK_EN               0

//LWIP协议栈初始化任务
#define LWIP_CREATE_TASK_EN                    1

//TCP服务器任务
#define TCP_SERVER_TASK_EN                     0

//TCP客户端任务
#define TCP_CLIENT_TASK_EN                     1

//IO输出数据接收任务
#define OUTPUT_SET_TASK_EN                     1

//IO输入数据接收任务
#define INPUT_RECEIVE_TASK_EN                  1

//数码管显示任务
#define SEGMENT_TASK_EN                        1

//Modbus RS485 从栈状态机轮询任务
#define MB_SLAVE_POLL_TASK_EN                  0

//Modbus RS485 主栈状态机轮询任务
#define MB_MASTER_POLL_TASK_EN                 0

//Modbus RS485 主栈轮询从设备任务
#define MB_SCAN_SLAVE_TASK_EN                  0

//CANopen通讯任务
#define CANOPEN_TASK_EN                        0

//喂狗任务
#define WATCHDOG_FEED_TASK_EN                  1

//内存监控任务
#define TASK_STACK_WATCH_ENABLE                0



//****************************任务配置信息**********************************
#if SYS_MAIN_CONTROL_TASK_EN > 0

//系统控制主任务
#define     APP_SYSCRTL_MAIN_TASK_PRIO 		       	4
#define     APP_SYSCRTL_MAIN_TASK_STK_SIZE	       	160

#endif

//主板初始化任务
#if BOARD_INIT_TASK_EN > 0

#define  	APP_BOARD_INIT_TASK_PRIO  	3
#define     APP_BOARD_INIT_TASK_STK_SIZE    128

#endif

//LWIP协议栈初始化任务
#if LWIP_CREATE_TASK_EN > 0 

#define  	APP_LWIP_CREATE_TASK_PRIO  	4
#define     APP_LWIP_CREATE_TASK_STK_SIZE    128

#endif

#if TCP_SERVER_TASK_EN > 0

#define  	APP_TCP_SERVER_TASK_PRIO  	5
#define     APP_TCP_SERVER_TASK_STK_SIZE    128

#endif

#if TCP_CLIENT_TASK_EN > 0

#define  	APP_TCP_CLIENT_TASK_PRIO  	7
#define     APP_TCP_CLIENT_TASK_STK_SIZE    128

#endif

#if OUTPUT_SET_TASK_EN > 0

#define  	APP_OUTPUT_SET_TASK_PRIO		12
#define     APP_OUTPUT_SET_TASK_STK_SIZE     128

#endif

#if INPUT_RECEIVE_TASK_EN > 0

#define  	APP_INPUT_RECEIVE_TASK_PRIO		13
#define     APP_INPUT_RECEIVE_TASK_STK_SIZE     128

#endif

#if SEGMENT_TASK_EN > 0

#define  	APP_SEGMENT_TASK_PRIO  			14
#define     APP_SEGMENT_TASK_STK_SIZE         	128

#endif

#if MB_SLAVE_POLL_TASK_EN > 0

#define  	APP_MB_SLAVE_POLL_TASK_PRIO  	16
#define     APP_MB_SLAVE_POLL_TASK_STK_SIZE   160

#endif

#if MB_MASTER_POLL_TASK_EN > 0

#define  	APP_MB_MASTER_POLL_TASK_PRIO  	17
#define     APP_MB_MASTER_POLL_TASK_STK_SIZE    160

#endif

#if MB_SCAN_SLAVE_TASK_EN > 0

#define  	APP_MB_SCAN_SLAVE_TASK_PRIO  	18
#define     APP_MB_SCAN_SLAVE_TASK_STK_SIZE    160

#endif

#if CANOPEN_TASK_EN > 0

#define		APP_CANOPEN_CONFIG_TASK_PRIO	15
#define     APP_CANOPEN_CONFIG_TASK_STK_SIZE    128

#define  	APP_CANOPEN_SENDSDO_TASK_PRIO  	21
#define     APP_CANOPEN_SENDSDO_TASK_STK_SIZE    128

#define  	APP_CAN1_SENDDATA_TASK_PRIO  	22
#define     APP_CAN1_SENDDATA_TASK_STK_SIZE    128

#endif

#if WATCHDOG_FEED_TASK_EN > 0

#define  	APP_WATCHDOG_FEED_TASK_PRIO  	23
#define     APP_WATCHDOG_FEED_TASK_STK_SIZE    128

#endif

#if SYSTEM_DATAS_BROADCAST_TASK_EN > 0

#define  SYSTEM_DATAS_BROADCAST_TASK_PRIO           24 
#define  SYSTEM_DATAS_BROADCAST_TASK_STK_SIZE       100 

#endif



#endif
