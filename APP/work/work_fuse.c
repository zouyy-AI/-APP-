#include "work.h"


/* 
    保险丝检测原理 1s内 如果脉冲小于10 正常50HZ 说明不合理
*/

static uint8_t fuse_err = 0; // 0 - 正常 1 - 保险丝异常


static uint8_t cat_err(uint8_t road)
{
   road = road ; 
   return fuse_err;
    
}

void init()
{
    gpio->set_mode(IO_fuse , PIN_MODE_INPUT);
}
static uint32_t otick = 0;
static uint32_t otick_sec = 0;
static uint8_t pulse = 50;
void work()
{
    static uint8_t sta = 0;
    uint32_t ntick = get_sys_ticks();
    if( ntick < 2000 ) return;
    if(( ntick - otick ) < 5 ) return;
    otick = ntick ;
    
    uint8_t val = gpio->read( IO_fuse );
    switch(sta)
    {
        case 0:
            if( val == 0)
                sta ++;
            break;
        case 1:
            if( val )
            {
                sta = 0;
                pulse ++;
            }
            break;
        default:
            sta = 0;
            break;
        
    }
    if(( ntick - otick_sec ) < 1000 ) return;
    otick_sec = ntick;
    
    if( pulse < 10)     //保险丝异常
    {
        fuse_err = 1;
        led_board_opt->set_fuse( __OFF );
		
    }
    else
    {
        fuse_err = 0;
        led_board_opt->set_fuse( __ON );
    }
//    myprintf("fuse_pulse:%d\r\n",pulse);
    pulse = 0;
    sta = 0;
}

static fuse_t do_fuse ={
    .init = init,
    .work = work,
    .cat_err = cat_err,
};

fuse_t* fuse = &do_fuse;

