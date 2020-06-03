#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#include "os.h"

/**************************任务配置开关*****************************/
#define DEBUG_ENABLE               1      //调试功能

#define LWIP_CREATE_TASK_EN        0      //LWIP协议栈
#define TCP_SERVER_TASK_EN         0      //TCP服务器功能
#define TCP_CLIENT_TASK_EN         0      //TCP客户端功能
                                   
#define OUTPUT_SET_TASK_EN         0      //IO输出数据功能
#define INPUT_RECEIVE_TASK_EN      1      //IO输入数据功能
                                   
#define SEGMENT_TASK_EN            1      //数码管显示功能
#define EEPROM_DATA_TASK_EN        0      //eeprom参数记忆功能

#define SYSTEM_EVENT_TASK_EN       0      //系统事件功能
#define SYSTEM_MONITOR_TASK_EN     0      //系统数据监控功能
#define SYSTEM_MAIN_CTRL_TASK_EN   0      //系统逻辑控制功能

#define MB_SLAVE_TASK_EN           1      //Modbus RS485 从栈功能          
#define MB_MASTER_TASK_EN          1      //Modbus RS485 主栈功能                                              
#define CANOPEN_TASK_EN            0      //CANopen通讯功能
                                   
#define WATCHDOG_FEED_TASK_EN      1      //喂狗功能
#define TASK_STACK_WATCH_TASK_EN   0      //内存监控功能


/****************************任务优先级配置********************************/                                                            
#define INPUT_RECEIVE_TASK_PRIO		  4
        
#define SEGMENT_TASK_PRIO  			  5
#define EEPROM_DATA_TASK_PRIO         6
        
#define SYSTEM_MONITOR_TASK_PRIO      7
#define SYSTEM_EVENT_TASK_PRIO        8
        
#define MB_SLAVE_POLL_TASK_PRIO  	  10
#define MB_MASTER_POLL_TASK_PRIO  	  11
#define MB_MASTER_SCAN_TASK_PRIO  	  12
        
#define CANOPEN_CONFIG_TASK_PRIO	  12
#define CANOPEN_SENDSDO_TASK_PRIO  	  13
#define CAN1_SENDDATA_TASK_PRIO  	  14
        
#define LWIP_CREATE_TASK_PRIO  	      15
#define TCP_SERVER_TASK_PRIO          16                                                                                                        
#define TCP_CLIENT_TASK_PRIO          17
        
#define SYSTEM_MAIN_CTRL_TASK_PRIO    9
        
#define WATCHDOG_FEED_TASK_PRIO  	  20
#define TASK_STACK_WATCH_TASK_PRIO    21 
        

/****************************任务堆栈大小**********************************/ 
#define INPUT_RECEIVE_TASK_STK_SIZE       128

#define SEGMENT_TASK_STK_SIZE             128
#define EEPROM_DATA_TASK_STK_SIZE         128

#define SYSTEM_EVENT_TASK_STK_SIZE        128
#define SYSTEM_MONITOR_TASK_STK_SIZE      128
#define SYSTEM_MAIN_CTRL_TASK_STK_SIZE	  160

#define CANOPEN_CONFIG_TASK_STK_SIZE      128
#define CANOPEN_SENDSDO_TASK_STK_SIZE     128
#define CAN1_SENDDATA_TASK_STK_SIZE       128

#define LWIP_CREATE_TASK_STK_SIZE         128
#define TCP_SERVER_TASK_STK_SIZE          128
#define TCP_CLIENT_TASK_STK_SIZE          128

#define WATCHDOG_FEED_TASK_STK_SIZE       128
#define TASK_STACK_WATCH_TASK_STK_SIZE    100 

#endif
