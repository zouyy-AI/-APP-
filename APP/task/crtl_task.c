#include "mytask.h"
#include "work.h"

static TaskHandle_t crtl_task_Handler;
void crtl_task(void* arg)
{
    myprintf("%s\r\n",__func__); 
    while(1)
    {
//		uint32_t tick = get_sys_ticks();
//		static uint32_t ntick123 = 0;
//		if(tick - ntick123 >= 1000)
//		{
//			ntick123  = tick;
//			UBaseType_t valu = uxTaskGetStackHighWaterMark(crtl_task_Handler);
//			myprintf("%s,%d\r\n",__func__,valu);
//		}
		work_cycle_entry();
        vTaskDelay(5);
		
		
    }
}
void crtl_task_create(void)
{
    myprintf("%s\r\n",__func__); 
    xTaskCreate(crtl_task,(const char *)"crtl_task", 1024, NULL, 3, &crtl_task_Handler);
}
