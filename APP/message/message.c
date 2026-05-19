#include "message.h"

/*
message需要做什么工作
(1)net_cycle
(2)msg_cycle

(3)构建消息队列, 用于网络任务 -> 控制任务
(4)构建消息队列, 用于控制任务 -> 网络任务

(5)对于某些要求可靠传输的请求，需要使用交互池(链表构建)


*/
/*----------------------------------------------------------------------------------*/

//定时同步网络时间
static int get_time_flag = 0;		//=1 说明已经成功获取网络时间

void clr_get_time_flag(void)
{
	get_time_flag = 0;
}

void set_get_time_flag( void )
{
	get_time_flag = 1;
}
int cat_get_time_flag(void)
{
	return get_time_flag; 	
	
}
void get_net_time( void* ndev )
{
	static uint32_t otick = 0;
	net_dev_t* netdev = (net_dev_t*)ndev;
	uint32_t ntick = get_sys_ticks();
	
	uint32_t cyctick = 0; 
	if( 0 == get_time_flag )
	{//说明还没有获取到网络时间
		//3s发送一边获取指令
		cyctick = 3*1000;
	}
	else
	{
		//10分钟发送一边获取指令
		cyctick = 10*60*1000;
	}
	
	if( (ntick-otick) < cyctick ) return;
	otick = ntick;
	
	
	netdev_opt->send_str( netdev, "GETTIME\r\n");
}

/*----------------------------------------------------------------------------------*/
static void ping_serv( void* ndev )
{
	static uint32_t otick = 0;
	
	net_dev_t* netdev = (net_dev_t*)ndev;
	
	uint32_t ntick = get_sys_ticks();
	if( (ntick-otick) < 5000 ) return;
	otick = ntick;
	
	netdev_opt->send_str( netdev, "PING\r\n");
	
	utc_opt->print_utc_time( "PING" , rtc->get_dt() );
}
/*
	net_cycle 借用netdev的ssta，构建状态机
*/
int net_cycle( void* ndev )
{
	int fun_ret = 0;
	net_dev_t* netdev = (net_dev_t*)ndev;
	switch( netdev->ssta )
	{
		//为交互做准备
		case 0:
			//每次登录，执行一遍,说明还没有获取网络时间
			clr_get_time_flag();
			
			netdev->ssta++;
			break;
		
		//循环作业
		case 1: 
			//定时同步网络时间(每次登录必须强制获取网络时间)
			get_net_time( netdev );
			//定时发送心跳给服务器
			ping_serv( netdev );
			
			//处理事件申报
			msg_ctn_opt->work(netdev);
			break;
		
		default:
			netdev->ssta = 0;
			break;
	
	}
	
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/
/*
	msg_cycle，负责解析网络数据，并匹配执行具体的命令回调
*/
extern int get_srv_cmd_list_len(void);
extern net_cmd_t* get_srv_cmd_list(void);

int msg_cycle( void* ndev,int argc,char** argv )
{
	
	int fun_ret = 0;
	
	net_cmd_t* cmdlist;
	
	cmdlist = get_srv_cmd_list();
	//匹配网络命令表,并跳转执行
	for( int index=0;index<get_srv_cmd_list_len();index++ )
	{
		if( 0 == strncmp( argv[0],cmdlist[index].name,strlen(cmdlist[index].name) ) )
		{//成立，说明匹配上了
			//执行对应回调
			fun_ret = cmdlist[index].entry( ndev,argc,argv );
			continue;
		}
	}

	return fun_ret;

}

