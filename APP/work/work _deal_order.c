#include "work.h"
#include "message.h"

void work_deal_order(void)
{
	//接收订单消息
	order_dat_t order;
	
	int ret = msg_ntc_opt->get(&order ,sizeof( order_dat_t ) );
	if(ret < 0)	return; 
	
	//判断插座号是否合法
	if(order.road >= MaxSock) return;
	
	sock_t * sock = &(wdat->work->sock[order.road]);
	switch(order.type)
	{
		case _e_ntc_type_on:
			//判断是否存在工作中的订单 如果成立那就不处理 丢弃本消息
			if(0 != sock->working)
			{
				myprintf("err:sock is working ,%s\r\n" , sock->ddh);
				
			}
			else//否则处理这条新的订单
			{
				myprintf("no working order,start charge\r\n");
				//复制订单号
				memset(sock->ddh,0,32);
				snprintf(sock->ddh,32,"%s",order.ddh);
				 
				sock->pul = 0;
				sock->num = 1;//第一条电流报文
				
				//计算计划结束的时间
				sock->start = *(rtc->get_dt());
				uint64_t timestamp = utc_opt->utc_to_timestamp(&(sock->start));
				timestamp = timestamp + (order.time * 60);
				utc_opt->timestamp_to_utc(timestamp,&(sock->end ));
				
				utc_opt->print_utc_time( "start" , &( sock->start ));
				utc_opt->print_utc_time("end" , &( sock->end ));
				
				msg_ctn_opt->put_curr_report(sock->ddh , sock->num , order.road , 0 , 0 ,220.0 ,0 );
				sock->num ++;
				sock->working = 1;
				
			}
			break;
		case _e_ntc_type_off:
			//记录订单存在结束请求
			sock->stop_req = 1;
			break;
		default:
			break;
		
		
	}
		
}



