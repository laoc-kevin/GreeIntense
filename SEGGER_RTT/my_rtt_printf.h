//基于SEGGER_RTT自定义的printf函数
#ifndef __MY_RTT_PRINTF_H__
#define __MY_RTT_PRINTF_H__


#include "stdio.h"
#include "stdarg.h"
#include "SEGGER_RTT.h"

int myprintf(const char * sFormat, ...);

#endif



