
#include "my_rtt_printf.h"

int myprintf(const char * sFormat, ...)
{  
	va_list ParamList;

	va_start(ParamList, sFormat);
	return SEGGER_RTT_vprintf(0, sFormat, &ParamList);
}

