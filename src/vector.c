#include "vector.h"
#include <stm32f4xx.h>


/* Less common symbols exported by the linker script(s): */
typedef void (*funcp_t) (void);
extern funcp_t __preinit_array_start, __preinit_array_end;
extern funcp_t __init_array_start, __init_array_end;
extern funcp_t __fini_array_start, __fini_array_end;


int main(void);
void blocking_handler(void);
void null_handler(void);
void reset_handler(void);

void nmi_handler(void) __attribute__((weak, alias("null_handler")));
void hard_fault_handler(void) __attribute__((weak, alias("blocking_handler")));
void sv_call_handler(void) __attribute__((weak, alias("null_handler")));
void pend_sv_handler(void) __attribute__((weak, alias("null_handler")));
void sys_tick_handler(void) __attribute__((weak, alias("null_handler")));

void mem_manage_handler(void) __attribute__((weak, alias("blocking_handler")));
void bus_fault_handler(void) __attribute__((weak, alias("blocking_handler")));
void usage_fault_handler(void) __attribute__((weak, alias("blocking_handler")));
void debug_monitor_handler(void) __attribute__((weak, alias("null_handler")));


__attribute__ ((section(".vectors")))
vector_table_t vector_table = {
	.initial_sp_value = &_stack,
	.reset = reset_handler,
	.nmi = nmi_handler,
	.hard_fault = hard_fault_handler,
	.memory_manage_fault = mem_manage_handler,
	.bus_fault = bus_fault_handler,
	.usage_fault = usage_fault_handler,
	.debug_monitor = debug_monitor_handler,
	.sv_call = sv_call_handler,
	.pend_sv = pend_sv_handler,
	.systick = sys_tick_handler,
	.irq = {
	}
};

static void pre_main(void)
{
	//set hse on
	RCC->CR &= ~RCC_CR_HSEON;
	RCC->CR |= RCC_CR_HSEON;

	//wait for hse to be ready
	while ( !(RCC->CR & RCC_CR_HSERDY) );

	//set hse as sysclk
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSE;
	//wait for switch
	while ( !(RCC->CFGR & RCC_CFGR_SWS_HSE) );
	//set wait states for flash
	FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_0WS;

	//disable internal oscillator
	RCC->CR &= ~RCC_CR_HSION;

	//set PLLM=8, PLLN=336, PLLP=2, PLLQ=7
	//HSE_VALUE=8MHz
	//PLL_VCO = PLL_N * (HSE_VALUE / PLL_M) = 336MHz
	//SYSCLK = PLL_VCO / PLL_P = 336MHz / 2 = 168MHz
	RCC->PLLCFGR = (	7 << 24)	| // Set Q to 7
				   (	1 << 22)	| // Use HSE as PLL source
				   (	0 << 16)	| // Set P tp 0x00 (=> Div = 2)
				   (  336 <<  6)	| // Set N to 336
				   (	8 <<  0);	  // Set M to 8
	RCC->CR |= RCC_CR_PLLON; //enable Pll circuit
	while( !(RCC->CR & RCC_CR_PLLRDY) ); //wait for PLL lock
	uint32_t iReg = RCC->CFGR & ~( RCC_CFGR_HPRE   |  // Clear bits  4 -  7 (HPRE)  (AHB domain)
								   RCC_CFGR_PPRE1  |  // Clear bits 10 - 12 (PPRE1) (APB Low Speed domain)
								   RCC_CFGR_PPRE2);   // Clear bits 13 - 15 (PPRE2) (APB High Speed domain)

	RCC->CFGR = iReg | (0x00 <<  4) | // No prescaler for AHB domain
			           (0x05 << 10) | // Pre-scaler 4 for APB1 domain (APB low speed)
					   (0x04 << 13);  // Pre-scaler 2 for APB2 domain (APB high speed)

	//Information whether and how many wait-states are required can be
	//obtained from the reference manual "DM00031020.pdf", Chapter "3.5.1 Relation between CPU clock frequency and Flash
	//memory read time".
	FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS; // Set flash wait-states to 5

	RCC->CFGR = (RCC->CFGR  & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;        // Set SysClock to PLL
	while( ( (RCC->CFGR >> 2) & 0x03) != 2) ;// Wait for SysClk to become ready


	/* Enable access to Floating-Point coprocessor. */
	SCB->CPACR = ((0b11 << 10*2) |             /* set CP10 Full Access               */
				  (0b11 << 11*2));             /* set CP11 Full Access               */
}

void __attribute__ ((weak, naked)) reset_handler(void)
{
	volatile unsigned *src, *dest;
	funcp_t *fp;

	for (src = &_data_loadaddr, dest = &_data;
		dest < &_edata;
		src++, dest++) {
		*dest = *src;
	}

	while (dest < &_ebss) {
		*dest++ = 0;
	}

	/* Ensure 8-byte alignment of stack pointer on interrupts */
	/* Enabled by default on most Cortex-M parts, but not M3 r1 */
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	/* might be provided by platform specific vector.c */
	pre_main();

	/* Constructors. */
	for (fp = &__preinit_array_start; fp < &__preinit_array_end; fp++) {
		(*fp)();
	}
	for (fp = &__init_array_start; fp < &__init_array_end; fp++) {
		(*fp)();
	}

	/* Call the application's entry point. */
	main();

	/* Destructors. */
	for (fp = &__fini_array_start; fp < &__fini_array_end; fp++) {
		(*fp)();
	}

}

void blocking_handler(void)
{
	while (1);
}

void null_handler(void)
{
	/* Do nothing. */
}
