#include "work.h"


extern void work_deal_order(void);
extern void work_cycle_reboot(void);
extern void work_sock_ctrl(void);

void work_cycle_entry(void)
{
	//管理插座的作业 
	
	work_sock_ctrl();
	//处理网络过来的订单
	
	work_deal_order();
	//周期重启设备
	work_cycle_reboot();
	


}



