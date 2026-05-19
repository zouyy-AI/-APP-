

#define __ON    0
#define __OFF   1


//求数组的元素个数
#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
//内存管理
#define mem_get_free() 					xPortGetFreeHeapSize()			//例如 int vfree = mem_get_free();
#define mem_alloc(size_bytes) 	pvPortMalloc(size_bytes)		//eg. u8* buf = (u8*)mem_alloc(100);
#define mem_free(pv) 						vPortFree(pv)								//eg. mem_free(buf);

//队列 
#define thread_mq_t                                 QueueHandle_t
#define thread_mq_init( mq_name , Onesize , MqNum)  xQueueCreate( MqNum , Onesize )
#define thread_mq_send( mq , sbuf , size , time)    xQueueSend( mq , sbuf , time)
#define thread_mq_recv( mq , sbuf , size , time)    xQueueReceive( mq , sbuf , time)

#define get_sys_ticks() ( (uint32_t)(xTaskGetTickCount() * ( 1000.0/configTICK_RATE_HZ ) ) )
#define get_sys_tick_irq() ( (uint32_t)(xTaskGetTickCountFromISR() * ( 1000.0/configTICK_RATE_HZ ) ) )


#define thread_delay_ms( ms ) do{ vTaskDelay( pdMS_TO_TICKS( ms ) ); }while(0);

#include "./lib_utc/utc.h"

#include "./lib_hc595/lib_hc595.h"
#include "./lib_stropt/stropt.h"
#include "./lib_loopbuf/loopbuf.h"
#include "./lib_fal/fal/inc/fal.h"

#include "./driver_port/fk_drvp_typedef.h"
#include "./driver/fk_drv_typedef.h"

