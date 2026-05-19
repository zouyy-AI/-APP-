#include "SWM320.h"

#include "FreeRTOS.h"
#include "mytask.h"

void SerialInit(void);



extern void start_taks_create(void);
void HardFault_Handler_C(uint32_t *stack_frame);

__asm void HardFault_Handler(void)
{
    IMPORT HardFault_Handler_C
    TST LR, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B HardFault_Handler_C
}

void HardFault_Handler_C(uint32_t *stack_frame)
{
    /*
     * HardFault 定位说明：
     * 1. PC = 崩溃指令地址，最关键。去 Keil Debug -> Disassembly 或 Map 文件中查找该地址对应的指令和函数。
     *    注意 Cortex-M 的 Thumb 地址最低位为 1，查找时可去掉最低位。
     * 2. LR = 链接寄存器，表示异常发生前的返回地址。它可以帮助判断错误是由哪个函数调用路径触发的。
     * 3. CFSR/HFSR 表示具体错误类型，若 HFSR.FORCED 置位，说明是由 MemManage/BusFault/UsageFault 升级而来。
     * 4. MMFAR/BFAR 是访问出错地址，只有在对应 Fault 有效时才有意义。
     * 5. 先定位 PC，再结合 LR 和 CFSR 进一步分析调用关系和错误类型。
     */
    /* 从堆栈帧取出异常时的寄存器现场 */
    uint32_t r0 = stack_frame[0];      /* 通用寄存器 R0 */
    uint32_t r1 = stack_frame[1];      /* 通用寄存器 R1 */
    uint32_t r2 = stack_frame[2];      /* 通用寄存器 R2 */
    uint32_t r3 = stack_frame[3];      /* 通用寄存器 R3 */
    uint32_t r12 = stack_frame[4];     /* 临时寄存器 R12 / IP */
    uint32_t lr = stack_frame[5];      /* 链接寄存器 LR，返回地址 */
    uint32_t pc = stack_frame[6];      /* 程序计数器 PC，崩溃指令地址 */
    uint32_t psr = stack_frame[7];     /* 程序状态寄存器 PSR */

    /* SCB 错误状态寄存器，用于判断具体故障类型 */
    uint32_t cfsr = SCB->CFSR;         /* 综合故障状态寄存器 */
    uint32_t hfsr = SCB->HFSR;         /* HardFault 状态寄存器 */
    uint32_t dfsr = SCB->DFSR;         /* 调试故障状态寄存器 */
    uint32_t afsr = SCB->AFSR;         /* 辅助故障状态寄存器 */
    uint32_t mmfar = SCB->MMFAR;       /* MemManage/BusFault 访问地址 */
    uint32_t bfar = SCB->BFAR;         /* BusFault 访问地址 */

    /* 打印崩溃现场，重点查看 PC 和 LR */
    printf("\r\nHardFault detected!\r\n");
    printf("R0 = %08X\r\n", r0);
    printf("R1 = %08X\r\n", r1);
    printf("R2 = %08X\r\n", r2);
    printf("R3 = %08X\r\n", r3);
    printf("R12= %08X\r\n", r12);
    printf("LR = %08X\r\n", lr);           /* 异常前返回地址，用于判断调用路径 */
    printf("PC = %08X\r\n", pc);           /* 崩溃指令地址，定位出错位置 */
    printf("PSR= %08X\r\n", psr);
    printf("CFSR=%08X HFSR=%08X DFSR=%08X AFSR=%08X\r\n", cfsr, hfsr, dfsr, afsr); /* 故障类型寄存器 */
    printf("MMFAR=%08X BFAR=%08X\r\n", mmfar, bfar); /* 访问错误地址 */

    while (1);
}

// 2. 内存管理错误（空指针、非法地址最常见）
void MemManage_Handler(void)
{
    myprintf("MemManage Fault\r\n");
    while(1);
}

// 3. 总线错误（读不存在的寄存器，比如 RTC 没时钟就去读）
void BusFault_Handler(void)
{
    myprintf("BusFault\r\n");
    while(1);
}

// 4. 使用错误（指令错误、除零等）
void UsageFault_Handler(void)
{
    myprintf("UsageFault\r\n");
    while(1);
}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // 在这里死循环+打印任务名
    printf("StackOverflow: %s\r\n", pcTaskName);
    while(1);
}int main(void)
{ 	
 	SystemInit();
	
	SerialInit();
	printf("----------------------\r\n");
    printf("App_____A system run...\r\n");
    
    start_taks_create();
	
	vTaskStartScheduler();
} 

/****************************************************************************************************************************************** 
* 函数名称:	TaskADC()
* 功能说明: 启动ADC采集任务
* 输    入: void *arg
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/

void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
 	UART_initStruct.Baudrate = 115200;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
 	while(UART_IsTXFIFOFull(UART0));
	
	UART_WriteByte(UART0, ch);
 	
	return ch;
}
