#ifndef __framework_H
#define __framework_H

#include "mytask.h"


#define NetRST 	56              /*网络模块复位引脚*/
#define NetUart _e_uart1        /*网络模块串口通信端口*/

#define IO_hc595_sck 	35
#define IO_hc595_dat 	36
#define IO_hc595_ud 	37

#define IO_fuse      	21


#define MaxSock		2		//有2个插座

#define IO_HLW0		22
#define IO_HLW1		20

#define IO_ELC0		26
#define IO_ELC1		3
#include "fk_typedef.h"

#endif
