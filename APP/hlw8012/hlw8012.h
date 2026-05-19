#ifndef __hlw8012_h__
#define __hlw8012_h__

#include "framework.h"
typedef struct{
	void (*init)( void );
     int (*get)(uint8_t road , void * dat);
	void (*work)( void );
}hlw_opt_t;

typedef struct{
    uint32_t type;   //记录事件的类型
    uint32_t cycle ; //记录事件的所用的周期
    uint32_t val;    //记录事件的的脉冲数
    
}hlw_pul_t;

enum{
    _type_hlw_sock_out = 0,     //插座拔出事件、
    _type_hlw_cyc_pul,          //周期脉冲
    _type_hlw_min_pul,          //一分钟脉冲
    
};

extern hlw_opt_t* hlw_opt;


#endif

