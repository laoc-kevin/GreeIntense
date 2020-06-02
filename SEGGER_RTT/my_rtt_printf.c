
#include "my_rtt_printf.h"
#include "app_config.h"

int myprintf(const char * sFormat, ...)
{
#if DEBUG_ENABLE  > 0    
	va_list ParamList;

	va_start(ParamList, sFormat);
	return SEGGER_RTT_vprintf(0, sFormat, &ParamList);
#else
    return 0;
#endif
}

