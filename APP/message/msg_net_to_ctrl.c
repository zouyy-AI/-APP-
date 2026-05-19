#include "message.h"


static thread_mq_t mq_local;
static uint8_t init_flag = 0;
//初始化
static void init( void )
{
	if(init_flag != 0)  return;
	mq_local = thread_mq_init( "mq_ntc" , MsgLen , 20 );
	
	init_flag = 1;
}
//发送消息
static void put_order(uint32_t type , uint32_t road , uint32_t time , char* ddh)
{
	if(init_flag == 0)	return;
	
	//判断类型长度是否一致
	if(sizeof(order_dat_t) != MsgLen)
	{
		printf("err:%s type len %d ！= MsgLen %d\r\n",
		__func__,sizeof( order_dat_t ) ,MsgLen );
	}
	order_dat_t dat;
	dat.type = 	type;
	dat.road = 	road;
	dat.time = 	time;
	
	memset(dat.ddh , 0, sizeof( dat.ddh ));
	snprintf(dat.ddh , sizeof( dat.ddh ) , "%s" , ddh);
	
	int ret = thread_mq_send( mq_local , &dat , MsgLen , 0 );
    if(ret == pdTRUE )
    {
        
    }
    else
    {//发送超时
        myprintf("%s[%d]",__func__,road);
        
    }
	
}
//接收消息
static int get(void * dat , int len )
{
	
	if(init_flag == 0)	return -1;
	
	if(len < MsgLen )  return -2;
    
    
    int ret = thread_mq_recv( mq_local , dat , MsgLen , 0 );
    if(ret == pdTRUE)
    {
        return 0;
    }
    else 
    {
        return -1;
    }
	
}

static msg_ntc_opt_t do_msg_ntc_opt={
	.init = init,
	.put_order = put_order,
	.get = get,

};

msg_ntc_opt_t* msg_ntc_opt = &do_msg_ntc_opt;
