#include "mytask.h"



static TaskHandle_t host_task_Handler;
void host_task(void* arg)
{
    myprintf("%s\r\n",__func__); 
    while(1)
    {
        cmd_line_work();
        led_board_opt->work(200);
//        UBaseType_t valu = uxTaskGetStackHighWaterMark(host_task_Handler);
//        myprintf("%s,%d\r\n",__func__,valu);
        vTaskDelay(5);
    }
}
void host_task_create(void)
{
    myprintf("%s\r\n",__func__); 
    xTaskCreate(host_task,(const char *)"host_task", 1024, NULL, 2, &host_task_Handler);
}
