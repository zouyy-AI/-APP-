#ifndef __mytask_H
#define __mytask_H

#include "SWM320.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "task.h"
#include "queue.h"
#include "semphr.h" 
//#include "work.h"
#include "network.h"
#include "led_board.h"
#include "hlw8012.h"
#include "message.h"


extern void listen_task_create(void);
extern void host_task_create(void);
extern void crtl_task_create(void);

void myprintf( char* format, ... );

#endif
