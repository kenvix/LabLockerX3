#ifndef __INC_GUARD_WATCHDOG

#define WATCHDOG_OVERFLOW_SCALE 256



sfr WDT_CONTR = 0xE1;
//sbit EN_WDT = WDT_CONTR ^ 5;
#include "impl/watchdog.c"
#endif

void WatchDog_Init(int scale);
void WatchDog_Enable();
void WatchDog_EnableIdleState();
void WatchDog_QuitIdleState();