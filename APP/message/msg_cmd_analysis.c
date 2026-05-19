#include "message.h"
/*----------------------------------------------------------------------------------*/

static int ccb_Logon( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/

static int ccb_unknow( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	for( int i=0;i<argc;i++ )
		NET_LOG("argv[%d]:%s\r\n",i,argv[i] );
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/
extern void set_get_time_flag( void );

/**
 * 计算字符串中所有数字字符的和
 * @param str 输入的字符串指针
 * @return 返回字符串中所有数字字符的和（uint32_t类型）
 */
static uint32_t check_sum_str( char* str )
{
	int i=0;                                    // 循环计数器，用于遍历字符串
	uint32_t ret = 0;                           // 用于存储数字字符的和
	
    // 遍历字符串中的每个字符
	for( i=0;i<strlen( str );i++ )
	{
        // 检查当前字符是否为数字字符（'0'到'9'）
		if( (str[i] >= '0') && (str[i] <= '9') )
		{
            // 如果是数字字符，将其转换为对应的数值并累加到ret中
			ret += str[i] - '0';
		
		}
	}
	
    // 返回计算得到的数字字符之和
	return ret;
}

void dump_rtc_by_str( char* time )
{
	int i=0;
	uint8_t tbuf[20];
	
	utc_t utc;
	
	//年 0123
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<4;i++ )	{tbuf[ i ] = time[ 0 + i ];}
	utc.year = atol( (char*)tbuf );
	
	//月 45
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 4 + i ];}
	utc.month = atol( (char*)tbuf );
	
	//日 67
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 6 + i ];}
	utc.day = atol( (char*)tbuf );
	
	//时 89
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 8 + i ];}
	utc.hour = atol( (char*)tbuf );
	
	//分 10 11
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 10 + i ];}
	utc.minute = atol( (char*)tbuf );
	
	//秒 12 13
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 12 + i ];}
	utc.second = atol( (char*)tbuf );

	
	rtc->stop( );
	rtc->init( &utc );
	rtc->start();
	
}


static int ccb_time( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);

	//判断参数个数是否符合
	if( argc != 3 ) return -1;
	
	//判断时间的合法性
	uint32_t recv_check_sum = atol( argv[2] );
	uint32_t cal_check_sum = check_sum_str( argv[1] );
	
	NET_LOG("recv_check_sum:%d\r\n",recv_check_sum );
	NET_LOG("cal_check_sum :%d\r\n",cal_check_sum  );
	
	if( recv_check_sum != cal_check_sum ) return -2;
	
	//装填到RTC上
	dump_rtc_by_str( argv[1] );
	
	set_get_time_flag();
	
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/
static int ccb_pong( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	
	return fun_ret;
}

/*----------------------------------------------------------------------------------*/
static int ccb_over( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	for( int i=0;i<argc;i++ )
		NET_LOG("argv[%d]:%s\r\n",i,argv[i] );
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/
static int ccb_update( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	for( int i=0;i<argc;i++ )
		NET_LOG("argv[%d]:%s\r\n",i,argv[i] );
	
	cmd_ota(NULL,NULL);
	return fun_ret;
}

/*----------------------------------------------------------------------------------*/
static int ccb_on( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	
	for( int i=0;i<argc;i++ )
		NET_LOG("argv[%d]:%s\r\n",i,argv[i] );
	
	//判断参数个数是否合法
	if(argc != 4)	return -1;
	
	//判断插座号是否合法
	int sock = atoi(argv[1]);
	if(sock > MaxSock) return -2;
	if(sock == 0)		return -2;
	int road = sock - 1;
	
	uint32_t time = atol(argv[2]);
	if(time > 720 || time == 0)
		time = 720;
	
	if(strlen(argv[3]) >= 32) return -3;
	char * ddh = argv[3];
	 
	msg_ntc_opt->put_order(_e_ntc_type_on,road,time,ddh);
	

	return fun_ret;
}
/*----------------------------------------------------------------------------------*/
static int ccb_off( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	for( int i=0;i<argc;i++ )
		NET_LOG("argv[%d]:%s\r\n",i,argv[i] );
	
	//判断参数个数是否合法
	if(argc != 3)	return -1;
	
	//判断插座号是否合法
	int sock = atoi(argv[1]);
	if(sock > MaxSock) return -2;
	if(sock == 0)		return -2;
	int road = sock - 1;
	if(strlen(argv[2]) > 32) return -3;
	char * ddh = argv[2];
	
	msg_ntc_opt->put_order(_e_ntc_type_off,road,0,ddh);
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/
static int ccb_reboot( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	for( int i=0;i<argc;i++ )
		NET_LOG("argv[%d]:%s\r\n",i,argv[i] );
	
	cmd_reboot(NULL,NULL);
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/
static int ccb_cat_data( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	NET_LOG("%s()\r\n",__func__);
	for( int i=0;i<argc;i++ )
		NET_LOG("argv[%d]:%s\r\n",i,argv[i] );
	cmd_cat_data(NULL , NULL);	
	return fun_ret;
}
/*----------------------------------------------------------------------------------*/

net_cmd_t __srv_cmd_list[  ]={
	{"Logon",ccb_Logon},
	{"unknow",ccb_unknow},
	{"time",ccb_time},
	{"pong",ccb_pong},
	{"over",ccb_over},
	{"update",ccb_update},
	{"on",ccb_on},
	{"off",ccb_off},
	{"reboot",ccb_reboot},
	{"cat_data",ccb_cat_data},
	
};

//获取__srv_cmd_list元素个数
int get_srv_cmd_list_len(void)
{
	return ITEM_NUM( __srv_cmd_list );
}

//获取
net_cmd_t* get_srv_cmd_list(void)       //__srv_cmd_list在放回时候 退化为指针 所以要用指针类型的返回值
{
	return __srv_cmd_list;
}



