#include "network.h"
#include "work.h"
static wk_net_t m_wk_net;
wk_net_t * wk_net = &m_wk_net;

static net_dev_t m_net_dev;
net_dev_t * air7xx = &m_net_dev;


enum{
    _e_port_init = 0,   //初始化端口
    _e_rst_net_dev,     //复位网络设备
    _e_init_net_dev,    //初始化网络设备
    _e_connect_serv,    //连接服务器,并数据交互
};


void network_entry()
{
   int8_t val = 0;
	
	if(wdat->storage->net_mode == Netmode_Not) return;
    
    switch(wk_net->msta)
    {
        case _e_port_init:          //初始化端口
            /*创建一个网络设备，并指定网络接口,指定复位引脚*/
            netdev_opt->create_dev(air7xx,NetUart,NetRST);
        
            //安装轮询作业函数
			netdev_opt->install_net_cycle( net_cycle );
			netdev_opt->install_msg_cycle( msg_cycle ); 
			
            /*初始化串口 和 gpio*/
            uart->init( air7xx->dev , 460800 );
            uart->open( air7xx->dev );
            gpio->set_mode( air7xx->RstPin , PIN_MODE_OUTPUT );
            gpio->write( air7xx->RstPin , 0 );
            
            wk_net->msta ++;
            break;
        case _e_rst_net_dev:        //复位网络设备
            val = netdev_opt->work_rst_dev( air7xx );
            if(val != 1 )
                break;
            netdev_opt->clr_sta( air7xx );
            
            wk_net->msta++;
			wk_net->ssta=0;
                
            break;
        case _e_init_net_dev:       //初始化网络设备
            val = netdev_opt->work_init_dev( air7xx );
            if( val == 0 ) break;
            netdev_opt->clr_sta( air7xx );
			if( val == -1 )
			{//发生了异常
				wk_net->msta = _e_rst_net_dev;
				wk_net->ssta=0;
			}
			else if( val == 1 )
			{//说明成功
				wk_net->msta++;
				wk_net->ssta=0;
				
			}
            break;
        case _e_connect_serv:        //连接服务器并通信
            val = netdev_opt->work_connect_serv( air7xx,wdat->storage->user_name,wdat->storage->user_pwd,"01" );
            if(val == 0 ) break;
            
            netdev_opt->clr_sta( air7xx );
        
            if( val == -1 )
			{//发生了不可预料的错误，直接重启网络 
				wk_net->msta = _e_rst_net_dev;
				wk_net->ssta=0;
			}
            else if( val == -2 )
			{//发生了不可预料的错误，直接重启网络
				wk_net->msta = _e_connect_serv;
				wk_net->ssta= 0;
                
                air7xx->msta = 0;
                air7xx->ssta = 0;
			}
            
            break;
            //循环工作
        default:
            
			break;
        
    }
    
    
}

