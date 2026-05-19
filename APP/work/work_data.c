#include "work.h"

static void storage_data_load(void);
static void logic_data_init(void);

static storage_t m_storage;
static work_t m_work;

static wdat_t m_wdat;

wdat_t* wdat;						//描述所有工作的数据vs

static const struct fal_partition * ptt_conf = NULL;
static int init_flag = 0;
void work_data_init( void )
{
	
	if(init_flag != 0) return;
	
	wdat = &m_wdat;
	wdat->storage = &m_storage;
	wdat->work = &m_work;
	
	//fal初始化
	fal_init();
	
	storage_data_load();
	logic_data_init();
	init_flag = 1;
}


static void storage_data_load(void)
{
		
	int write_flag = 0;	//描述是否需要执行写入操作
	
	
	
	//获取想要操作的fal分区
	ptt_conf = fal_partition_find( "conf" );
	if( NULL == ptt_conf )
	{//成立，说明获取失败
		myprintf("err:%s,%d\r\n",__func__,__LINE__);
		while(1);
	}
	
	//从这一个分区里面读取数据
	int len = fal_partition_read( ptt_conf, 0, (uint8_t*)(&m_storage), sizeof( storage_t ) );
	if( len != sizeof( storage_t ) )
	{//说明读取出错
		myprintf("err:%s,%d\r\n",__func__,__LINE__);
		while(1);
	}
	
		
	//判断是否为第一次开机
	if( wdat->storage->bootcode != BootCode )
	{//成立，说明是第一次开机
		//初始化一些参数
		myprintf("Sys is first boot\r\n");
		wdat->storage->bootcode = BootCode;
		wdat->storage->boot_app = _e_boot_mode_app;
		wdat->storage->ota_req = 0;
		wdat->storage->net_mode = Netmode_air780;
		wdat->storage->app_mode = 0;
		wdat->storage->f_attemp_updata = 0;

		wdat->storage->download_upset = 0;
		wdat->storage->download_addr = 0;
		
		wdat->storage->k[0] = 10.0;
		wdat->storage->k[1] = 10.0;
		
		wdat->storage->b[0] = 0;
		wdat->storage->b[1] = 0;
		
		
		
		write_flag ++;
	}
	else
	{//否则，说明不是第一次开机
		myprintf("Sys is not first boot\r\n");
	}
	
	for( int i=0;i<MaxSock;i++ )
	{
		myprintf("k[%d] = %f\r\n",i,wdat->storage->k[i]);
		
	}
	//如果有数据变化，则执行写入操作
	if( write_flag != 0 )
	{
		//擦除
		fal_partition_erase( ptt_conf, 0,  sizeof( storage_t ) );
		//写入
		fal_partition_write( ptt_conf, 0, (uint8_t*)(wdat->storage), sizeof( storage_t ) );
		myprintf("write data to flash \r\n");
	}

}

static void logic_data_init(void)
{
	wdat->work->rp_serv_first_boot = 0;
	
	sock_t* sock;
	for( int road=0;road<MaxSock;road++ )
	{
		sock = &( wdat->work->sock[road] );
		
		memset( sock,0,sizeof( sock_t ) );
		
	}
	
}

void work_data_write(void )
{
	if(init_flag == 0) return;
	
	//擦除
	fal_partition_erase( ptt_conf, 0,  sizeof( storage_t ) );
	//写入
	fal_partition_write( ptt_conf, 0, (uint8_t*)(wdat->storage), sizeof( storage_t ) );
	myprintf("write data to flash \r\n");

}




