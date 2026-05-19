#include "work.h"

enum{
	get_tim = 0,
	feed_wdt,

};
void work_cycle_reboot(void)
{
	static uint8_t flag = 0;
	static uint32_t ntick = 0;
	uint32_t otick = 0;
	if(wdat->storage->app_mode != (APPA_attemp_APPB || APPB_attemp_APPA))	
		return;
	switch(flag)
	{
		case get_tim:
			ntick = get_sys_ticks();
			flag ++;
			myprintf("%s,%d,ntick:%d\r\n",__FILE__,__LINE__,ntick);
			break;
		case feed_wdt:
			wdt->feed();
			otick = get_sys_ticks();
				
			if(otick - ntick > 1000*10)	//	防止变砖
			{
				myprintf("%s,%d,ntick:%d\r\n",__FILE__,__LINE__,otick);
				uint32_t current_addr = (uint32_t)work_cycle_reboot;
				if(current_addr > _f_appb_start )
				{
					wdat->storage->app_mode = APPB_running;
					myprintf("----------------%s,APPB_running",__FILE__,__LINE__);
				}
				else
				{
					wdat->storage->app_mode = APPA_running;
					myprintf("----------------%s,APPA_running",__FILE__,__LINE__);
				}
				
				wdt->stop();
				wdat->storage->f_attemp_updata = 0;		//	跟新成功，清楚标志位
				work_data_write();
				myprintf("-------------------------------------------------------------系统正常运行成功 不启用回滚 \r\n");
			}
			break;

		default:
			break;
	}
	
	
	
}



