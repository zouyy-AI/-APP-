#include "work.h"

/*
	命令行代码
	(1)config
	config zd_user zjy_001 zjy_123456
	config net 0
	config net air780
	config kb 0 10.1 0
	config default			重置数据
	
	
	(2)ota
	(3)ctrl
	没有插入网卡的时候 想要调试引脚，校准 就用这个命令
	ctrl 0 open 10		打开插座010分钟
	ctrl 1 close 0		关闭插座0
	
	(4)cat_data
	打印工作参数命令
	
*/
/*---------------------------------------------------------*/

extern int cmd_reboot( int argc,char** argv );

int cmd_cat_data( int argc,char** argv ) 
{
	myprintf( "%s\r\n",__func__ );
	
	myprintf("bootcode:%08x\r\n",wdat->storage->bootcode );
	myprintf("boot_app:%08x\r\n",wdat->storage->boot_app );
	myprintf("ota_req:%08x\r\n",wdat->storage->ota_req ); 
	myprintf("net_mode:%08x\r\n",wdat->storage->net_mode );
	myprintf("f_attemp_updata:%d\r\n",wdat->storage->f_attemp_updata );
	myprintf("app_mode:%d\r\n",wdat->storage->app_mode );
	myprintf("download_upset:%d\r\n",wdat->storage->download_upset );
	myprintf("download_addr:%d\r\n",wdat->storage->download_addr );

	for(uint8_t road = 0;road < MaxSock;road ++)
	{
		myprintf("[%d] y = %f*x + (%f)\r\n",road,
		wdat->storage->k[road],wdat->storage->b[road]);
		
	}
	
	myprintf("user_name:%s\r\n",wdat->storage->user_name );
	myprintf("user_pwd:%s\r\n",wdat->storage->user_pwd );
		
	
	return 0;
	
}
#define _cmd_work_labal(cmd)	__##cmd##_work

int cmd_config( int argc,char** argv ) 
{
	myprintf("%s\r\n",__func__);
		
	int road = 0;
	float k = 0,b = 0;
	
	if(0 == strcmp(argv[1],"kb"))
	{
		goto _cmd_work_labal(kb);
	}
	if(0 == strcmp(argv[1],"rst_kb"))
	{
		goto _cmd_work_labal(rst_kb);
	}
	if(0 == strcmp(argv[1],"net"))
	{
		goto _cmd_work_labal(net);
	}
	if(0 == strcmp(argv[1],"zd_user"))
	{
		goto _cmd_work_labal(zd_user);
	}
	if(0 == strcmp(argv[1],"default"))
	{
		goto _cmd_work_labal(default);
	}
	return 0;
	
	
_cmd_work_labal(kb):
	/* config kb 0 10.1 0 */
	if(argc != 5) { myprintf("usage err!\r\n");	return -2;}
	
	road = atoi(argv[2]);
	if(road >= MaxSock)	
	{
		myprintf("err:sock num \r\n");
	}
	
	k = atof(argv[3]);
	b = atof(argv[4]);
	
	wdat->storage->k[road] = k;
	wdat->storage->b[road] = b;
	
	work_data_write(); 	
	
	return 0;
_cmd_work_labal(rst_kb):
	
	/* config kb 0  */
	if(argc != 3) { myprintf("usage err!\r\n");	return -2;}
	
	road = atoi(argv[2]);
	if(road >= MaxSock)	
	{
		myprintf("err:sock num \r\n");
	}
	
	
	wdat->storage->k[road] = 10.0;
	wdat->storage->b[road] = 0;
	
	work_data_write(); 	
	
	return 0;
	
_cmd_work_labal(net):
	/*config net air780*/
	if(argc != 3) { myprintf("usage err!\r\n");	return -2;}
	
	if(0 == strcmp(argv[2],"air780"))
		wdat->storage->net_mode = Netmode_air780;
	else if(0 == strcmp(argv[2],"air724"))
		wdat->storage->net_mode = Netmode_air724;
	else 
		wdat->storage->net_mode = Netmode_Not;
		
	work_data_write(); 
	return 0;

_cmd_work_labal(zd_user):
	
	if(argc != 4) { myprintf("usage err!\r\n");	return -2;}
	
	memset(wdat->storage->user_name,0,32);
	memset(wdat->storage->user_pwd,0,32);
	
	snprintf(wdat->storage->user_name,32,	"%s",argv[2]);
	snprintf(wdat->storage->user_pwd,32,"%s",argv[3]);
	
	myprintf("config zd_user:%s %s\r\n",
			wdat->storage->user_name,wdat->storage->user_pwd);
	work_data_write(); 
	return 0;
_cmd_work_labal(default):
	
	if(argc != 2) { myprintf("usage err!\r\n");	return -2;}
	wdat->storage->bootcode = 0;
	work_data_write(); 
	myprintf("config default set success\r\n");
	/*重启设备*/
	cmd_reboot(NULL,NULL);
	return 0;	
}

int cmd_ota( int argc,char** argv ) 
{
	myprintf("%s",__func__);
	
	wdat->storage->ota_req = 1;
	work_data_write(); 
	
	myprintf("ota_req = 1\r\n");
	myprintf("system reboot after 1s \r\n");
	
	
	cmd_reboot(NULL,NULL);
	
	return 0;
	
}



int cmd_ctrl( int argc,char** argv ) 
{
	/*			road 	do		time
		ctrl 	0 		close 	1
		ctrl 	0 		open 	10
	*/
	
	myprintf("%s\r\n",__func__);
	uint32_t type = 0;
	uint32_t road = 0;
	uint32_t time = 0;
	char ddh[32] ;
	/*只有在网络未连接 才可以执行一下命令*/
	if(wdat->storage->net_mode != Netmode_Not)		return -1;
	
	if(argc != 4) { myprintf("usage err!\r\n");	return -2;}
	
	/*获取订单号*/
	utc_t* ndt = rtc->get_dt();
	
	if(ndt->year == 0)
	{
		myprintf("rtc init fail\r\n");
		return -2;
	}
	snprintf(ddh,sizeof(ddh),"%04d%02d%02d%02d%02d%02d",
			ndt->year,ndt->month,ndt->day,
			ndt->hour,ndt->minute,ndt->second);
	
	
	road = atoi(argv[1]);
	if(road >= MaxSock)		return -3;
	if(0 == strcmp(argv[2],"open"))
	{
		type = _e_ntc_type_on;
		time = atoi(argv[3]);
	}
	else if(0 == strcmp(argv[2],"close"))
	{
		type = _e_ntc_type_off;
		time = 0;
	}
	
	msg_ntc_opt->put_order(type,road,time,ddh);

	myprintf("ctrl->sock[%d] %s->%d min  ddh:%s\r\n",road,argv[2],time,ddh);
	
	return 0;
	
}



