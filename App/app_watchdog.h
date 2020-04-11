#ifndef APP_WATCHDOG_H
#define APP_WATCHDOG_H
#include "includes.h"

#define WATCHDOG_ENABLE 1


void AppWatchDogInit(void);
void AppWatchDogFeedTask(void * p_arg);


#endif
