#include "framework.h"


static void init( uint32_t tick )
{
    //判断tick的合法性
    if( tick >= 4294967295 )
    {
        tick = 4294967295;
        myprintf(" tick is to big\r\n");
    }
    //初始化WDT
    WDT_Init( WDT, tick, WDT_MODE_RESET );

}

static void feed(void)
{
    WDT_Feed( WDT );
}

static void start(void)
{
    WDT_Start( WDT );
}

static void stop(void)
{
    WDT_Stop( WDT );
}

static drvp_wdt_t do_drvp_wdt = {
    .init = init,
    .feed = feed,
    .start = start,
    .stop = stop,
};


drvp_wdt_t* drvp_wdt = &do_drvp_wdt;
