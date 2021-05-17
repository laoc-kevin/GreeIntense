#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#include "os.h"

/**************************任务配置开关*****************************/
#define DEBUG_ENABLE               0      //调试功能
#define EEPROM_USE_DEFAULT_DATA    0      //eeprom使用默认参数
#define EEPROM_DATA_INIT           0      //eeprom参数复位
#define EEPROM_RUNNING_TIME_INIT   0      //eeprom运行时间复位
#define EEPROM_ENERGY_INIT         0      //eeprom能耗复位

#define LWIP_TASK_EN               1      //LWIP协议栈
#define TCP_SERVER_TASK_EN         0      //TCP服务器功能
#define TCP_CLIENT_TASK_EN         0      //TCP客户端功能
                                   
#define OUTPUT_SET_TASK_EN         1      //IO输出数据功能
#define INPUT_RECEIVE_TASK_EN      1      //IO输入数据功能
                                   
#define SEGMENT_TASK_EN            1      //数码管显示功能
#define EEPROM_DATA_TASK_EN        0      //eeprom参数记忆功能

#define SYSTEM_EVENT_TASK_EN       0      //系统事件功能
#define SYSTEM_MONITOR_TASK_EN     0      //系统数据监控功能
#define SYSTEM_MAIN_CTRL_TASK_EN   1      //系统逻辑控制功能

#define MB_SLAVE_RTU_EN            0      //Modbus RS485 从栈功能 
#define MB_SLAVE_TCP_EN            1      //Modbus RS485 TCP 从栈功能
#define MB_MASTER_RTU_EN           0      //Modbus RS485 主栈功能 
#define MB_MASTER_TCP_EN           0      //Modbus RS485 TCP 主栈功能 
#define CANOPEN_TASK_EN            0      //CANopen通讯功能
                                   
#define WATCHDOG_FEED_TASK_EN      1      //喂狗功能
#define TASK_STACK_WATCH_TASK_EN   1      //内存监控功能

/****************************任务优先级配置********************************/                                                            
#define SYSTEM_EVENT_POLL_TASK_PRIO   4

#define LWIP_CREATE_TASK_PRIO  	      4

#define MB_SLAVE_POLL_TASK_PRIO  	   5
#define MB_SLAVE_TCP_SERVER_TASK_PRIO  5

#define MB_MASTER_POLL_TASK_PRIO  	   6
#define MB_MASTER_SCAN_WRITE_TASK_PRIO 6
#define MB_MASTER_SCAN_READ_TASK_PRIO  7

#define SYSTEM_EVENT_TASK_PRIO        10 
#define SYSTEM_MONITOR_TASK_PRIO      11
#define SYSTEM_POLL_TASK_PRIO         12

#define INPUT_RECEIVE_TASK_PRIO		  13       
#define SEGMENT_TASK_PRIO  			  14

#define EEPROM_DATA_TASK_PRIO         15

#define CANOPEN_CONFIG_TASK_PRIO	  12
#define CANOPEN_SENDSDO_TASK_PRIO  	  13
#define CAN1_SENDDATA_TASK_PRIO  	  14
        
#define TCP_SERVER_TASK_PRIO          16                                                                                                        
#define TCP_CLIENT_TASK_PRIO          17
     
#define WATCHDOG_FEED_TASK_PRIO  	  20
#define TASK_STACK_WATCH_TASK_PRIO    21 
        
/****************************任务堆栈大小**********************************/ 
#define INPUT_RECEIVE_TASK_STK_SIZE       128

#define SEGMENT_TASK_STK_SIZE             128
#define EEPROM_DATA_TASK_STK_SIZE         128

#define SYSTEM_EVENT_TASK_STK_SIZE        160
#define SYSTEM_MONITOR_TASK_STK_SIZE      160

#define SYSTEM_POLL_TASK_STK_SIZE	      128
#define SYSTEM_EVENT_POLL_TASK_STK_SIZE	  256

#define CANOPEN_CONFIG_TASK_STK_SIZE      128
#define CANOPEN_SENDSDO_TASK_STK_SIZE     128
#define CAN1_SENDDATA_TASK_STK_SIZE       128

#define LWIP_CREATE_TASK_STK_SIZE         128
#define TCP_SERVER_TASK_STK_SIZE          128
#define TCP_CLIENT_TASK_STK_SIZE          128

#define WATCHDOG_FEED_TASK_STK_SIZE       128
#define TASK_STACK_WATCH_TASK_STK_SIZE    100 

#endif
