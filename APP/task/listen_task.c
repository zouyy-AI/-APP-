#include "mytask.h"
#include "work.h"


static TaskHandle_t listen_task_Handler;
void listen_task(void* arg)
{
    myprintf("%s\r\n",__func__); 
    while(1)
    {
//        UBaseType_t valu = uxTaskGetStackHighWaterMark(listen_task_Handler);
//        myprintf("%s,%d\r\n",__func__,valu);
        fuse->work();
        hlw_opt->work();
        vTaskDelay(5);
    }
}
void listen_task_create(void)
{
    myprintf("%s\r\n",__func__); 
    xTaskCreate(listen_task,(const char *)"listen_task", 1024, NULL, 3, &listen_task_Handler);
}
