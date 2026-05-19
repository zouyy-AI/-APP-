#include "message.h"
#include "work.h"

enum{
	_e_type_curr = 0,		//电流报文事件
	_e_type_over,			//结束事件报文
	
};
typedef struct{
	uint32_t type;		//消息类型
	uint32_t num;		//电流报文序号
	uint32_t sock;		//左边插座1  右边插座2 
	float curr;			//电流值
	float pwr;			//功率值
	float vol;			//电压值
	uint32_t usetime;	//已充时间值
	uint32_t rec;		//保留 一共64位
	
	
	char ddh[32];
	
}curr_rp_t;		//	电流报文结构体


static thread_mq_t mq_local;
static uint8_t init_flag = 0;
//初始化
static void init( void )
{
	if(init_flag != 0)  return;
	mq_local = thread_mq_init( "mq_ctn" , MsgLen , 20 );
	
	init_flag = 1;
}
//发送电流报文
static void put_curr_report( char* ddh , int num , int road ,float curr,float pwr,float vol , uint32_t usetime)
{
	if(init_flag == 0)	return;
	
	if(wdat->storage->net_mode == Netmode_Not)		return;
	//判断类型长度是否一致
	if(sizeof(order_dat_t) != MsgLen)
	{
		printf("err:%s type len %d ！= MsgLen %d\r\n",
		__func__,sizeof( order_dat_t ) ,MsgLen );
	}
	curr_rp_t dat;
	dat.type = 	_e_type_curr;
	
	memset(dat.ddh , 0, sizeof( dat.ddh ));
	snprintf(dat.ddh , sizeof( dat.ddh ) , "%s" , ddh);
	
	dat.num = 	num;
	dat.sock = 	road + 1;
	dat.curr = 	curr;
	dat.pwr = 	pwr;
	dat.vol = 	vol;
	dat.usetime = 	usetime;
	
	
	 
	int ret = thread_mq_send( mq_local , &dat , MsgLen , 1 );
    if(ret == pdTRUE )
    {
        
    }
    else
    {//发送超时
        myprintf("%s[%d]",__func__,road);
        
    }
	
}

typedef struct{
	uint32_t type;		//消息类型
	float ekwh;		//充电所消耗的电能
	uint32_t time;		//充电时长 分钟 
	uint32_t event;		//订单结束原因
	uint32_t rec[4];	//保留
	
	
	char ddh[32];
	
}over_rp_t;		//	结束报文结构体
//发送结束事件报文
static void put_over_report( char* ddh , float ekwh , uint32_t time , uint32_t event)
{
	
	if(init_flag == 0)	return;
	
	if(wdat->storage->net_mode == Netmode_Not)		return;
	
	//判断类型长度是否一致
	if(sizeof(over_rp_t) != MsgLen)
	{
		printf("err:%s type len %d ！= MsgLen %d\r\n",
		__func__,sizeof( over_rp_t ) ,MsgLen );
	}
	over_rp_t dat;
	dat.type = 	_e_type_over;
	
	memset(dat.ddh , 0, sizeof( dat.ddh ));
	snprintf(dat.ddh , sizeof( dat.ddh ) , "%s" , ddh);
	
	dat.ekwh= 	ekwh;
	dat.time = 	time;
	dat.event = event;
	 
	int ret = thread_mq_send( mq_local , &dat , MsgLen , 1 );
    if(ret == pdTRUE )
    {
        
    }
    else
    {//发送超时
        myprintf("warning: %s",__func__);
        
    }
	
}
//接收消息
static int get(void * dat , int len )
{
	
	if(init_flag == 0)	return -1;
	
	if(len < MsgLen )  return -2;
    
    
    int ret = thread_mq_recv( mq_local , dat , MsgLen , 1 );
    if(ret == pdTRUE)
    {
        return 0;
    }
    else 
    {
        return -1;
    }
	
}
static char obuf[256];
static void work(void* ndev)
{
	
	if(init_flag == 0)	return;
	
	
	uint8_t buf[MsgLen];
	
	int ret = get(buf , sizeof( buf ) );
	if(ret < 0)	return;
	
	curr_rp_t* curr_rp = (curr_rp_t*) buf;
	over_rp_t* over_rp = (over_rp_t*) buf;
	
	net_dev_t * netdev = (net_dev_t*)ndev;
	switch(over_rp->type)
	{
		case _e_type_curr:
			//log data ddh num sock curr pwr vol usetime
			memset(obuf , 0 , sizeof( obuf ));
			snprintf(obuf , sizeof( obuf ) ,"log data %s %d %d %f %f %f %d\r\n",
					curr_rp->ddh,
					curr_rp->num,
					curr_rp->sock,
					curr_rp->curr,
					curr_rp->pwr,
					curr_rp->vol,
					curr_rp->usetime
					);
			netdev_opt->send_str(netdev,obuf);
			myprintf("----->send:%s\r\n",obuf);
			break;
		
		case _e_type_over:
			//log over ddh ekwh time event  
			memset(obuf , 0 , sizeof( obuf ));
			snprintf(obuf , sizeof( obuf ) ,"log over %s %f %d %d\r\n",
					over_rp->ddh,
					over_rp->ekwh,
					over_rp->time,
					over_rp->event
					);
			netdev_opt->send_str(netdev,obuf);
			myprintf("----->send:%s\r\n",obuf);
			break;
		default:
			break;
		
		
	}
	
}
static msg_ctn_opt_t do_msg_ctn_opt={
	.init = init,
	.put_curr_report = put_curr_report,
	.put_over_report = put_over_report,
	.work = work,
	
};

msg_ctn_opt_t* msg_ctn_opt = &do_msg_ctn_opt;
