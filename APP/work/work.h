#ifndef __work_h__
#define __work_h__



#include "framework.h"
#define BootCode 0xA5


#define _e_boot_mode_ota 0
#define _e_boot_mode_app 1

enum{
	APPA_running = 0,
	APPA_attemp_APPB,
	APPB_running,
	APPB_attemp_APPA,
};
enum{
	Netmode_Not = 0,
	Netmode_air724,
	Netmode_air780,
	Netmode_ML307
	
};


//16字节对齐(4个字)
typedef struct{
	
	//-----------------------------
	uint32_t bootcode;		//启动码
	
	uint8_t boot_app;			//启动哪一个APP
	uint8_t ota_req;			//是否存在ota请求
	uint8_t net_mode;			//联网方式
	uint8_t download_upset;		//异常时（不等于0时） 记录下载包数偏移

	uint32_t download_addr;		//记录下载地址偏移
	
	uint8_t f_attemp_updata;	//ota跟新完成标志位
	uint8_t app_mode;
	uint8_t reserve[2];			//预留5字节对齐

	//-----------------------------	
	//HLW8012的线性拟合校准值
	float k[2];
	float b[2];
	
	//-----------------------------	
	char user_name[ 32 ];	//用户名
	char user_pwd [ 32 ];	//用户密码
	
}storage_t;




typedef struct{
	uint8_t working;					//描述是否正在工作 =0,非工作
	uint8_t msta;							//主状态
	uint8_t ssta;							//子状态
	uint8_t rec;
	
	uint8_t sock_out;					//描述插座拔出
	uint8_t curr_too_small;		//描述电流过小 
	uint8_t curr_too_big;			//描述电流过大
	uint8_t stop_req;					//手动结束请求
	
	uint32_t pul;							//本订单的电能脉冲数
	uint32_t num;							//本订单电流报文的编号
\
	
	char ddh[32];							//存储本订单的编号
	
	uint32_t stick;						//记录订单开始时候的tick(加速运算)
	
	utc_t start;							//存储订单开始的时间
	utc_t end;								//存储订单计划结束的时间
	
	float pwr;								//周期平均功率
	
	uint32_t min_e;						//分钟电能
	
	
}sock_t;


typedef struct{

	uint32_t rp_serv_first_boot;		//描述是否需要向服务器汇报启动
	
	sock_t sock[ MaxSock ];					//插座的工作数据
	
	
}work_t;

typedef struct{
	
	storage_t* storage;	//指向存储的内容
	
	work_t* work;				//指向不需要存储的工作数据 用指针可以节省内存、灵活管理
	
	
}wdat_t;


typedef struct{
    void (*init)(void);
	void (*work)(void);
    uint8_t (*cat_err)(uint8_t road);
	
}fuse_t;


extern void work_data_init(void);
extern void work_cycle_entry(void);
extern void work_data_write(void );

extern fuse_t* fuse ;
extern wdat_t* wdat;						//描述所有工作的数据vs


#endif

