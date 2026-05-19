#include "hlw8012.h"

/*
hlw8012是一个电能计量芯片

这是一个积分芯片，固定周期采样，瞬时电压，瞬时电流,求乘积。
并且对乘积进行累加，加到一定程度(固定值)，则输出一个脉冲。

测量这个固定值的方法：
固定负载功率，策略HLW8012单位时间内输出的脉冲数。

通过对其脉冲计数，就可以知道周期内所消耗的电脑。
而1s钟所消耗的电能就是平均功率。


1,我们要知道插座有没有人插入，需要用到低功率检测
2,我们需要知道插座的平均功率，需要周期采样，求平均
3,我们需要定时1分钟，发出一分钟的电能消耗



*/
//引脚映射表
static uint8_t pin_map[ MaxSock ]={ IO_HLW1,IO_HLW0 };

//采样脉冲数
static uint32_t pulse[ MaxSock ]={ 0,0};

//记录最后一次脉冲计量时间
static uint32_t last_tick[ MaxSock ] = {0,0};

typedef struct{
	uint8_t road;		//第几路HLW芯片
	uint8_t pin;		//对应的具体IO序号
}dat_t;


static dat_t ArgDat[ MaxSock ];

static thread_mq_t my_event[ MaxSock ];

static void do_isr( void *args )		//回调函数
{
	dat_t* pdat = (dat_t*)args;
	
	//判断road 合法性
	if( pdat->road >= MaxSock ) return;
	
	//采样脉冲数++
	pulse[ pdat->road ]++;
	
	//记录最后一次脉冲计量时间
	last_tick[ pdat->road ] = get_sys_tick_irq();
	
}


static void init( void )
{
	
	for( int road=0;road<MaxSock;road++ )
	{
        my_event[road] = thread_mq_init( "hglw_8012" , sizeof(hlw_pul_t) , 5 );
        
		ArgDat[road].pin = pin_map[road];
		ArgDat[road].road = road;
		
		
		//先设置为输入模式
		gpio->set_mode( pin_map[road],PIN_MODE_INPUT );
		//附加中断
		gpio->attach_irq( pin_map[road],PIN_IRQ_MODE_FALLING,do_isr,&( ArgDat[road] ) );
		//使能中断
		gpio->irq_enable( pin_map[road],PIN_IRQ_ENABLE );

	}
	
}
static void put(uint8_t road , uint32_t type , uint32_t cycle , uint32_t val)
{
    if( road >= MaxSock )    return;
        
    hlw_pul_t dat;
    
    dat.cycle = cycle ;
    dat.type = type;
    dat.val = val;
    
    int ret = thread_mq_send( my_event[road] , &dat , sizeof(hlw_pul_t) , 1 );
    if(ret == pdTRUE )
    {
        
    }
    else
    {//发送超时
        myprintf("waring:%s[%d]\r\n",__func__,road);
        
    }
        
}	

static int get(uint8_t road , void * dat)
{
    hlw_pul_t* data = ( hlw_pul_t* )dat;
    
    if( road >= MaxSock )    return -1;
    
    int ret = thread_mq_recv( my_event[road] , data , sizeof(hlw_pul_t) , 1 );
    if(ret == pdTRUE)
    {
        return 0;
    }
    else 
    {
        return -1;
    }
    
}
static void work( void )
{
	static uint32_t otick_cyc =0 ;
	static uint32_t min_pulse[MaxSock] ={0,0};
	static uint32_t min_cntr = 0;
	int road = 0;
	
	uint32_t ntick = get_sys_ticks();
	
	
	//检查用电器拔出事件
	for(  road=0;road<MaxSock;road++ )
	{
		
		if( ( ntick - last_tick[road] ) >( 3000 ) )
		{//成立，则认为没有插入用电器
			//发送用电器拔出事件
            last_tick[road] = ntick;
            put(road , _type_hlw_sock_out ,3000 , 0 );
		}
	}
	
	
	//周期采样脉冲数
	if( (ntick - otick_cyc) >= 5000 )
	{
		otick_cyc = ntick;
		
		//打印一个周期采样的脉冲数
		for(  road=0;road<MaxSock;road++ )
		{
//			myprintf("[%d]pulse = %08d\r\n",road,pulse[road] );
			put(road , _type_hlw_cyc_pul ,5000 , pulse[road]);
			min_pulse[road] += pulse[road];		//累加分钟检测
			
			pulse[road] = 0;
		}
		min_cntr++;
	}
	
	//1分钟发送电能统计
	if( min_cntr >= 12 )
	{
        for(  road=0;road<MaxSock;road++ )
		{
            put(road , _type_hlw_min_pul ,60*1000 , min_pulse[road]);
            min_pulse[road] = 0;
            
        }
		min_cntr = 0;
//		myprintf("[%d]pulse = %08d\r\n",1,pulse[1] );
	}
	
}

static hlw_opt_t do_hlw_opt={
	.init = init,
    .get = get,
	.work = work,
};

hlw_opt_t* hlw_opt = &do_hlw_opt;
