#include "led_board.h"


/*
16个led 

1.修改状态 网络指示灯
2.修改状态 保险丝状态指示
3.修改状态 插座状态
    空闲状态
    充电状态

*/

hc595_pin_t LED_BUF = {IO_hc595_dat,IO_hc595_sck,IO_hc595_ud,0};

typedef struct{
    uint8_t net;        //网络连接状态
    uint8_t fuse;       //保险丝状态
    uint8_t sock[2];    //插座状态
    
}lsta_t;
static lsta_t lsta = {0,0,0,0};

static void set_net(uint8_t pwr)
{
    lsta.net = pwr;
}
static void set_fuse(uint8_t pwr)
{
    lsta.fuse = pwr;
}

static void set_sock(uint8_t road , uint8_t sta)
{
   lsta.sock[road] = sta;
}

static uint8_t disp[2];     //显存

#define NetLED	0			//网络指示灯
#define FuseLED	15			//保险丝指示灯

#define MaxSockLed  7

static uint8_t led_map[MaxSock][MaxSockLed] = {
    {8,9,10,11,12,13,14},
    {7,6, 5, 4, 3, 2, 1}
};


static void init()
{
    hc595_opt->init( &LED_BUF );
    for(uint8_t i = 0 ; i < sizeof( disp ) ;i ++)
    {
        disp[i] = 0xFF;     //清零缓存 led 共阳极
    }
    
    hc595_opt->write( &LED_BUF , disp , sizeof(disp) );
    hc595_opt->update( &LED_BUF );
    
}
static void set_one_bit(uint8_t ibit , uint8_t pwr)
{
    uint8_t index = ibit / 8 ;  //获取数组下标
    uint8_t offset = ibit % 8 ;//获取数组元素偏移量
    if( pwr == __ON)    //亮
    {
        disp[index] &= ~(1 << offset);
    }
    else if(pwr == __OFF) //灭
    {
        disp[index] |= (1 << offset);
    }
    
}
static void show()
{
    hc595_opt->write( &LED_BUF , disp , sizeof(disp) );
    hc595_opt->update( &LED_BUF );
}
static void sock_led( int road,int sta )
{
	static uint8_t counter[ MaxSock ] ={0,0};
	
	//在缓冲区上灭掉所有灯
	for( int i=0;i< MaxSockLed;i++ )
			set_one_bit( led_map[road][i],__OFF );
	
	if( sta == 0 )
	{//空闲状态
		counter[road] = 0;
		
	}
	else
	{//充电状态，跑灯
		counter[road]++;	//每次进来亮多一颗灯
		if( counter[road]> MaxSockLed )
			counter[road] = 0;
		
		for( int i=0;i< counter[road];i++ )
			set_one_bit( led_map[road][i],__ON );
		
	}
	
	
}


static void work(uint32_t tick)
{
    static uint32_t o_tick = 0;
    uint32_t n_tick = get_sys_ticks();
    
    //访问频率控制
    if(n_tick - o_tick < tick) return;
    o_tick = n_tick;
    
    set_one_bit( NetLED , lsta.net);
    set_one_bit( FuseLED , lsta.fuse);
    
    for( int road=0;road<MaxSock;road++ )
		sock_led( road,lsta.sock[road] );
    
    show();
}

static led_board_opt_t do_led_board_opt ={
	.init = init,
	.set_net = set_net,
	.set_fuse = set_fuse,
	.set_sock = set_sock,
	.work = work,

};

led_board_opt_t* led_board_opt = &do_led_board_opt;

