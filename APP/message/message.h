#ifndef __message_h__
#define __message_h__

#include "framework.h"
#include "stdlib.h"
#include "network.h"
#include "cmdline.h"


typedef int (*cmd_cb_t)( void* ndev,int argc,char** argv );

typedef struct{
	char* name;				//字符串
	cmd_cb_t entry;		//操作函数

}net_cmd_t;

extern int net_cycle( void* ndev );
extern int msg_cycle( void* ndev,int argc,char** argv );
extern int cat_get_time_flag(void);

//msg_ntc

#define MsgLen  64			//消息长度固定大小
enum{
	_e_ntc_type_on = 0,		//启动订单消息
	_e_ntc_type_off,		//停止订单消息
	
};
typedef struct{
	uint32_t type;		//描述消息的类型
	uint32_t road;		//第几路插座
	uint32_t time;		//充电时长
	uint32_t rec[5];	//保留20字节 一共64字节
	
	char ddh[32];		//订单号
}order_dat_t;


typedef struct{
	void (*init)( void );
	void (*put_order)(uint32_t type , uint32_t road , uint32_t time , char* ddh);	
	int (*get)(void * dat , int len );
}msg_ntc_opt_t;


extern msg_ntc_opt_t* msg_ntc_opt ;

//------------------------------------------------------
typedef struct{
	void (*init)( void );
	void (*put_curr_report)( char* ddh , int num , int road ,float curr,float pwr,float vol , uint32_t usetime);
	void (*put_over_report)( char* ddh , float ekwh , uint32_t time , uint32_t event);
	void (*work)(void* ndev);
}msg_ctn_opt_t;
extern msg_ctn_opt_t* msg_ctn_opt;


extern int cmd_reboot( int argc,char** argv );

#endif


