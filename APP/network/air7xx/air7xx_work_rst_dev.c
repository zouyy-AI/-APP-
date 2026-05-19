#include "network.h"


enum{
    //--------------------------------------------
	_e_service_init = 0,			//服务初始化
	//--------------------------------------------	
	_e_rst_pin_ctrl_0,				//复位引脚控制步骤0
	_e_rst_pin_ctrl_1,				//复位引脚控制步骤1
	_e_rst_pin_ctrl_2,				//复位引脚控制步骤2
	//--------------------------------------------
	_e_AT_RESET,							//软件命令复位
	_e_wait_some_tick,				//等待一些时间
	_e_wait_ready,						//等待就绪
	//--------------------------------------------
	_e_wait_some_time,				//等待一段时间
    
};

uint8_t wait_ready(net_dev_t* netdev);


int air_work_rst_dev( net_dev_t* netdev )
{
    uint8_t ret = 0;
    switch(netdev->msta)
    {
        case _e_service_init:
            NET_LINE();
            netdev->msta++;
            netdev->ssta=0;
            break;
        case _e_rst_pin_ctrl_0:
            NET_LINE();
            //网络指示灯熄灭
			//TODO...
        
            //清除环形缓冲区 和 接收缓冲区
            uart->clr_rxbuf( netdev->dev );
            netdev_opt->clr_rxbuf(netdev);
        
            //复位引脚输出低电平
			gpio->write( netdev->RstPin,0 );
        
            //记录当前的滴答到otick
			netdev->otick = get_sys_ticks();
            
            //切换下一个状态
			netdev->msta++;
			netdev->ssta=0;
            break;
        case _e_rst_pin_ctrl_1:
            //等待100ms
			netdev->ntick = get_sys_ticks();
            if( ( netdev->ntick - netdev->otick )< 100)
                break;
            NET_LINE();
            
            //复位引脚输出高电平
			gpio->write( netdev->RstPin,1 );
            //记录当前的滴答到otick
            netdev->otick = netdev->ntick;
            
            //切换下一个状态
			netdev->msta++;
			netdev->ssta=0;
            
            break;
        case _e_rst_pin_ctrl_2:
            netdev->ntick = get_sys_ticks();
            if( ( netdev->ntick - netdev->otick )< 100)
                break;
            NET_LINE();
            
            //复位引脚输出低电平
			gpio->write( netdev->RstPin,0 );
            //记录当前的滴答到otick
            netdev->otick = netdev->ntick;
            
            //切换下一个状态
			netdev->msta++;
			netdev->ssta=0;
            
            break;
        case _e_AT_RESET:       //软件指令复位
            netdev->ntick = get_sys_ticks();
            if( ( netdev->ntick - netdev->otick )< 100)
                break;
            NET_LINE();
            
            netdev_opt->send_str(netdev , "AT+RESET\r\n");
            
            //记录当前的滴答到otick
			netdev->otick = netdev->ntick;
            
            //切换下一个状态
			netdev->msta++;
			netdev->ssta=0;
            break;
        case _e_wait_some_tick:
            netdev->ntick = get_sys_ticks();
            if( ( netdev->ntick - netdev->otick )< 100)
                break;
            NET_LINE();
            netdev->otick = netdev->ntick;
            //切换下一个状态
			netdev->msta++;
			netdev->ssta=0;
            break;
        case _e_wait_ready:     //等待接收是否收到复位
            if( wait_ready( netdev ) != 0 )
            {
                //打印收到的数据
				netdev_opt->ptf_rxbuf( netdev,__func__,__LINE__ );
                //记录当前的滴答到otick
				netdev->otick = netdev->ntick;
                //切换下一个状态
				netdev->msta++;
				netdev->ssta=0;
				
				NET_LINE();
				//清空rxbuf
				netdev_opt->clr_rxbuf( netdev );
                
            }
            //处理超时 2s超时, 4次尝试,如果失败，则返回 _e_AT_RESET ，超过4次代表发生重大错误，自动进入_e_dev_err_msta
            netdev_opt->deal_cmd_timeout(netdev,2000,4,_e_AT_RESET,__func__,__LINE__);
            
            break;
        case _e_wait_some_time:
            //等待10s,确保4G模块连接上移动网络
            netdev->ntick = get_sys_ticks();
            if((netdev->ntick - netdev->otick) < 500)
                break;
            netdev->otick = netdev->ntick;
            //借助ssta作为计数器，每次访问++
			netdev->ssta++;
            NET_LOG(".");
            if(netdev->ssta >= 20)
            {
                NET_LINE();
				netdev->msta = _e_dev_done_msta; 
				netdev->ssta = 0;
            }
        break;
        case _e_dev_done_msta:
            ret = 1;        //复位工作完毕 后续在net_work 继续配置剩下的
        break;
		
		case _e_dev_err_msta://清除所有状态 重新开始
			netdev_opt->clr_sta( netdev );
        break;
		
		default:
			break;
        
    }
    
    
    
    return ret;
    
}
static uint16_t tlen;
uint8_t wait_ready(net_dev_t* netdev)
{
    uint8_t ret = 0;
    char* buf = netdev->rxbuf;
    char* str = NULL;
    
    tlen = uart->read( netdev->dev,&(buf[netdev->rx_len ]),64 );
	netdev->rx_len += tlen;
    
    str = strstr(buf,"RDY");
    if(str != NULL)
        return 1;
    str = strstr(buf,"boot.rom");
    if(str != NULL)
        return 1;
    
    return ret;
    
}
