#ifndef __led_board_H
#define __led_board_H

#include "framework.h"



typedef struct{
	void (*init)( void );
	
	//外部修改灯的显示，调用以下3个函数，修改状态，work函数，会刷新显示
	void (*set_net)( uint8_t pwr );
	void (*set_fuse)( uint8_t pwr );
	void (*set_sock)( uint8_t road,uint8_t sta );
	void (*work)( uint32_t tick );
	
	
}led_board_opt_t;


extern led_board_opt_t* led_board_opt;

#endif
