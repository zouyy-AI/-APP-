#include "mytask.h"
#include "work.h"


static TaskHandle_t listen_task_Handler;


void listen_task(void* arg)
{
    drv_time->init(0, 1);	//使用定时器模式，周期为50
    drv_time->start();
    myprintf("%s\r\n",__func__); 
    static uint32_t ntick = 0,otick =0;
    while(1)
    {
        char statsBuf[512] = {0};
//        UBaseType_t valu = uxTaskGetStackHighWaterMark(listen_task_Handler);
//        myprintf("%s,%d\r\n",__func__,valu);
        fuse->work();
        hlw_opt->work();
        ntick = get_sys_ticks();
        //每3秒打印一次当前最小剩余堆内存
        if((ntick-otick)>=3000)
        {
            otick = ntick;
            
            vTaskGetRunTimeStats((char *)statsBuf); 

            myprintf("%s\r\n", statsBuf);
            myprintf("min ever free heap: %u\r\n", xPortGetMinimumEverFreeHeapSize());
        }
        vTaskDelay(5);
    }
}
void listen_task_create(void)
{
    myprintf("%s\r\n",__func__); 
    xTaskCreate(listen_task,(const char *)"listen_task", 1024, NULL, 3, &listen_task_Handler);
}




