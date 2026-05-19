#include "work.h"
#include "message.h"
static void open(uint8_t road)
{
	switch(road)
	{
		case 1:gpio->write(IO_ELC0,1);break;
		case 0:gpio->write(IO_ELC1,1);break;
		default:
			break;
		
	}
}

static void close(uint8_t road)
{
	switch(road)
	{
		case 1:gpio->write(IO_ELC0,0);break;
		case 0:gpio->write(IO_ELC1,0);break;
		default:
			break;
		
	}
}
static void receive_hlw_event(int road)
{
	hlw_pul_t event;
	uint32_t time = 0;
	float s_pul = 0;
	int ret = hlw_opt->get(road,&event);
	if(ret < 0) return;
	
	sock_t * sock = &(wdat->work->sock[road]);
	if( 0 == sock->working ) return; 		//如果不在工作时间 直接丢弃消息
	
	switch(event.type)
	{
		case _type_hlw_sock_out:
			sock->sock_out = 1;
			myprintf("%s(%d) : sock_out\r\n",__func__,road );
			break;
		
		case _type_hlw_cyc_pul:
			//发送消息给网络任务 网络任务吧电流报文发给服务器
			s_pul = event.val * 1000.0f / (float)(event.cycle);
			sock->pwr = (s_pul * wdat->storage->k[road]);
			myprintf("%s(%d) : cyc_pul:%d   pwr:%f\r\n",__func__,road,event.cycle,sock->pwr );
		
			time = get_sys_ticks();
			time = time - sock->stick;
			time = time / 1000 / 60;		//已充分钟时长
			msg_ctn_opt->put_curr_report(sock->ddh , sock->num , road , sock->pwr/220.0 , sock->pwr ,220.0 ,time );
			sock->num++;  			//电流报文x编号
			break;
		
		case _type_hlw_min_pul:
			//增加总电能
			sock->pul += event.val;
			sock->min_e = (event.val * wdat->storage->k[road]);
			myprintf("%s(%d) : min_pul:%d\r\n",__func__,road,event.val);
			break;
		
		default:
			break;
		
		
	}
}
//------------------------------------------------------------------
enum{
	e_osc_init = 0,		//插座初始化
	e_osc_wait_condition,	//等待条件
	e_osc_wait_order,		//等待订单
	e_osc_charging,			//正在充电
	
	//约定msta为0xE0 - 0xEF 时候 为订单结束 ，（msta - 0xE0） 表示订单结束的原因
	
	
};
enum{
	_e_end_by_time = 0,		//订单时间消耗完成，正常结束
	_e_end_curr_to_small,	//电流过小
	_e_end_curr_to_big,		//电流过大
	_e_end_no_ins,			//未插入用电器
	_e_end_fuse_err,		//保险丝异常
	_e_end_manual_stop,		//手动结束订单
	
		
};
static void one_sock_ctrl(uint8_t road)
{
	sock_t* sock = &(wdat->work->sock[road]);
	uint32_t ntick = get_sys_ticks();
	float ekwh = 0;
	uint32_t time = 0;
	uint64_t eval = 0;
	uint64_t sval = 0;
	switch(sock->msta)
	{
		case e_osc_init:
			close(road);
			led_board_opt->set_sock(road,0);
			sock->msta ++;
			sock->ssta = 0;
		
			break;
		
		case e_osc_wait_condition:
			
			receive_hlw_event(road); 
			/*消息队列接收hlw消息 ，并丢弃*/
			/*如果没有网络，则直接下一步*/
			if(wdat->storage->net_mode == Netmode_Not)
			{
				sock->msta ++;
				sock->ssta = 0;
				break;
			}
			/*如果有网络，则判断网络授时是否成功*/
		
			if(1 == cat_get_time_flag())
			{
				sock->msta ++;
				sock->ssta = 0;
			}
				
			
		
			break;
		
		case e_osc_wait_order:
			receive_hlw_event(road); 
			//消息队列接收hlw消息 ，并丢弃
		
			//等待后台订单，work_deal_order订单收到后，会主动修改working
		
			if(sock->working == 0 ) break;
			
		
			open(road);
			led_board_opt->set_sock(road,1);
			sock->stick = get_sys_ticks();
		
		
			sock->msta ++;
			sock->ssta = 0;
		
			sock->sock_out = 0;
			sock->curr_too_small = 0;
			sock->curr_too_big = 0;
			sock->stop_req = 0;
			break;
		
		case e_osc_charging:
			
		
			receive_hlw_event(road);
			//接收hlw消息队列并处理
			//订单开始10s后 ，看是否插入 若没有 则结束订单
			
			if(ntick - sock->stick > (10 * 1000))
			{
				if(sock->sock_out)
				{
					sock->msta = 0xE0 + _e_end_no_ins;
					break;
				}
				
			}
			else 
				sock->sock_out = 0;
		
			//2分钟后，检查电流是否过小
			if(ntick - sock->stick > (2*60*1000))
			{
				if(sock->min_e < (15 * 60)) //15W * 60s 
				{
					sock->msta = 0xE0 + _e_end_curr_to_small;
					break;
				}
					
			}
			//10s后检查电流是否过大
			if(ntick - sock->stick > (10 * 1000) )
			{
				if(sock->pwr > 800)  //周期功率 大于 800 W
				{
					sock->msta = 0xE0 + _e_end_curr_to_big;
					break;
				}
			}
			//检查保险丝是否异常
			if(1 == fuse->cat_err(road))
			{
				sock->msta = 0xE0 + _e_end_fuse_err;
				break;
			}
			//检查是否有手动停止请求
			if(1 == sock->stop_req)
			{
				sock->msta = 0xE0 + _e_end_manual_stop;
				break;
			}
			
#if 1
			//检查订单时间是否消耗完成
			sval = utc_opt->utc_to_timestamp(rtc->get_dt());
			eval = utc_opt->utc_to_timestamp(&(sock->end));
			if(eval < sval)
			{
				sock->msta = 0xE0 + _e_end_by_time;
				break;
			}
			break;
#endif
		
		case 0xE0+_e_end_by_time:
		case 0xE0+_e_end_curr_to_small:
		case 0xE0+_e_end_curr_to_big:
		case 0xE0+_e_end_no_ins:
		case 0xE0+_e_end_fuse_err:
		case 0xE0+_e_end_manual_stop:
			//把结束原因告知网络任务，网络按照报文格式通知服务器
			myprintf("----->:(%d)event:%d,\r\n",road,sock->msta - 0xE0);
			sock->working = 0;
		
			ekwh = (sock->pul * wdat->storage->k[road]) / 1000;	//kwh
		
			time = get_sys_ticks();
			time = time - sock->stick;
			time = time / 1000 / 60;		//已充分钟时长
			
			msg_ctn_opt->put_over_report(sock->ddh , ekwh ,time ,sock->msta - 0xE0 );
			
		
			sock->msta 	= 0; 	
			sock->ssta = 0;
			break;
 		default :
			break;
		
		
		
	}
	
}
void work_sock_ctrl(void)
{
	for(uint8_t road = 0;road < MaxSock;road ++)
	{
		one_sock_ctrl(road);
	}
	
}



