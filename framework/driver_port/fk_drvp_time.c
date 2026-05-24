#include "framework.h"


/**********************************************************
 * FreeRTOSConfig.h中添加以下代码
 * 
 * extern volatile uint32_t CPU_RunTime;
 * 
 * #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()     (CPU_RunTime = 0ul)
 * #define portGET_RUN_TIME_COUNTER_VALUE()             CPU_RunTime
 * 
 **********************************************************/

volatile uint32_t CPU_RunTime = 0ul;



void TIMR0_Handler(void)
{
    CPU_RunTime++;
    TIMR_INTClr(TIMR0);
}
void tim_CPU_Init(uint32_t mode, uint32_t period)
{
    switch (mode)
    {
        case 0:
            mode = TIMR_MODE_TIMER;
            break;
        case 1:
            mode = TIMR_MODE_COUNTER;
            break;
        default:
            mode = TIMR_MODE_TIMER;
            break;
    }
    period = ((configCPU_CLOCK_HZ / 1000000) * period ) - 1; // 1us计数一次
    // NVIC_SetPriority(TIMR0_IRQn, 15);  
    // NVIC_EnableIRQ(TIMR0_IRQn);
    TIMR_Init(TIMR0, mode, period, 1);
    
}
void tim_CPU_Start(void)
{
    TIMR_Start(TIMR0);
}
void tim_CPU_Stop(void)
{
    TIMR_Stop(TIMR0);
}
static drvp_time_t do_drvp_time = {
    .init = tim_CPU_Init,
    .start = tim_CPU_Start,
    .stop = tim_CPU_Stop,
};

drvp_time_t *drv_time = &do_drvp_time;


