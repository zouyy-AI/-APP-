#include "framework.h"
#include "work.h"


/*bat脚本路径.\build_ota_file.bat*/
#define NetReset_Pin  56

void cslock_init(void);
void swap_rx0_to_rx1(void);
void swap_rx1_to_rx0(void);


static TaskHandle_t start_task_Handler;



void start_task(void* arg)
{
    
    
    printf("%s\r\n",__func__); 
    
    
    
    /*初始化串口 生成缓冲区*/
    uart->close(_e_uart0);
    uart->init(_e_uart0,115200);
    uart->open(_e_uart0);
    
    cslock_init();
    
    work_data_init();
	
    host_task_create();
    listen_task_create();
    crtl_task_create();
    
    hlw_opt->init();
    fuse->init();
    
    led_board_opt->init();
    led_board_opt->set_fuse( __OFF );
    led_board_opt->set_net( __OFF );
    led_board_opt->set_sock( 0 , 0 );
    led_board_opt->set_sock( 1 , 0 );
    
    /*开机延时 防止后面内存溢出 导致死机 下载不了程序*/
    vTaskDelay(1000);
    
    //测试  继电器打开
	gpio->set_mode( IO_ELC0,PIN_MODE_OUTPUT );
	gpio->set_mode( IO_ELC1,PIN_MODE_OUTPUT );
	gpio->write( IO_ELC0,PIN_LOW );
	gpio->write( IO_ELC1,PIN_LOW );

	msg_ntc_opt->init();
	msg_ctn_opt->init();
	
	utc_t utc={ 2025,1,1,0,0,0 };
	rtc->stop( );
	rtc->init( &utc );
	rtc->start();
	
	
	 
    while(1)
    {
        network_entry(); 
		
        vTaskDelay(10);
        
    }
}
void start_taks_create(void)
{
    printf("%s\r\n",__func__); 
    xTaskCreate(start_task,(const char *)"start_task", 1024, NULL, 2, &start_task_Handler);
}

/*----------------串口发送互斥锁--------------------*/
static SemaphoreHandle_t cslock;    //保存锁对象

static int flag_init = 0;            //描述锁是否已经初始化了

//初始化锁
void cslock_init(void)
{
    if( flag_init == 1 ) return;
    cslock = xSemaphoreCreateMutex();
    if( cslock == NULL )
    {//说明不成功
        printf( "err:%s\r\n",__func__ );
        while(1);
    }
    flag_init = 1;
}

//获取锁
void cslock_get(void)
{
    if( flag_init == 0 ) return;

    //获取cslock，最多等待100个tick，超时，则返回错误；成功，返回TRUE
    int ret = xSemaphoreTake( cslock,0xFFFFFFFF );
    if( ret == pdTRUE )
    {//说明，成功获取了

    }
    else
    {//失败

    }
}

//释放锁
void cslock_free(void)
{
    if( flag_init == 0 ) return;
    xSemaphoreGive( cslock );
}

//实现自己的打印函数
void myprintf( char* format, ... )
{
    if( flag_init == 0 ) return;

    char buf[256];        //最大可以打印的字符串长度

    va_list v_args;
    va_start( v_args,format );
    (void)vsnprintf( buf,sizeof(buf),format,v_args );

    va_end( v_args );

    //获取锁
    cslock_get();
    //使用互斥资源,因为printf最终调用到uart,指向同一个硬件资源，好几个任务同时使用，是不行的。
    printf("%s",buf);
    //释放锁
    cslock_free();
}


/*
AT

OK
------
AT+RESET
OK
^boot.rom.'v....'!\n
RDY

^MODE: 17,17

+E_UTRAN Service

+CGEV: ME PDN ACT 1

------
读取模块厂家信息
AT+CGMI

+CGMI: "AirM2M"

OK

------
读取模块的固件信息
AT+CGMR

+CGMR: "AirM2M_780EP_V1010_LTE_AT"

OK
------
查询物联网卡/SIM卡有没有插好
AT+CPIN?

+CPIN: READY

OK
------
查询信号质量
AT+CSQ

+CSQ: 22,0

OK
------
查询网络的注册状态
AT+CREG?

+CREG: 0,1

OK
------
查询附着GPRS网络
AT+CGATT?

+CGATT: 1

OK

------
设置透明传输
AT+CIPMODE=1


OK
------
设置模块为单链接
AT+CIPMUX=0

OK
------
自动获取APN

AT+CSTT="","",""

OK
------
激活移动场景,激活之后，可以去获取IP
AT+CIICR

OK
------
查询IP
AT+CIFSR

10.191.209.135
------
连接TCP服务器
AT+CIPSTART="TCP","www.armsoc.cn","9002"

AT+CIPSTART="TCP","www.armsoc.cn","9002"

OK

CONNECT




*/

