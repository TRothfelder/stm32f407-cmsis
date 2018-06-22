/*
 * os.c
 *
 *  Created on: 20.06.2018
 *      Author: kt
 */

#include <stdint.h>
#include "os/os.h"
#include <stm32f4xx.h>

OSThread * volatile OS_curr;
OSThread * volatile OS_next;

extern OSThread blinky1,blinky2;

void OS_init(void)
{
	// set pendsv to lowest priority
	NVIC_SetPriority(PendSV_IRQn, 0xFF);
	NVIC_EnableIRQ(PendSV_IRQn);
	//set systick priority to highest
	NVIC_SetPriority(SysTick_IRQn, 0U);
	NVIC_EnableIRQ(SysTick_IRQn);
	__enable_irq();
}

void OS_sched(void)
{
	if ((OS_curr == (OSThread*)0) || OS_curr == &blinky2) {
		OS_next = &blinky1;
	} else {
		OS_next = &blinky2;
	}
	if (OS_next != OS_curr) {
		//NVIC_SetPendingIRQ(PendSV_IRQn);
		*(uint32_t volatile *)0xE000ED04 = (1<<28);
	}
}

void OSThread_start(
		OSThread *me,
		OSThreadHandler threadHandler,
		void *stkSto, uint32_t stkSize)
{
	/* round down the stack top to the 8-byte boundary
	 * NOTE: ARM Cortex-M stack grows down from hi -> low memory
	 */
	uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
	uint32_t *stk_limit;

	*(--sp) = 1U<<24; /* xPSR */
	*(--sp) = (uint32_t)threadHandler; /* PC */
	*(--sp) = 0x0000000EU; /* LR */
	*(--sp) = 0x0000000CU; /* R12 */
	*(--sp) = 0x00000003U; /* R3 */
	*(--sp) = 0x00000002U; /* R2 */
	*(--sp) = 0x00000001U; /* R1 */
	*(--sp) = 0x00000000U; /* R0 */
	/* additionally, fake registers R4-R11 */
	*(--sp) = 0x0000000BU; /* R11 */
	*(--sp) = 0x0000000AU; /* R10 */
	*(--sp) = 0x00000009U; /* R09 */
	*(--sp) = 0x00000008U; /* R08 */
	*(--sp) = 0x00000007U; /* R07 */
	*(--sp) = 0x00000006U; /* R06 */
	*(--sp) = 0x00000005U; /* R05 */
	*(--sp) = 0x00000004U; /* R04 */

	/* save the top of the stack in thread's attribute */
	me->sp = sp;

	/* round the bottom of the stack to the 8-byte boundary */
	stk_limit = (uint32_t *)(((((uint32_t)stkSto - 1U) / 8) + 1U) * 8);

	/* fill rest of stack with known pattern */
	for (sp = sp - 1U; sp >= stk_limit; --sp) {
		*sp = 0xDEADBEEFU;
	}
}


__attribute__((naked))
void pend_sv_handler(void)
{
	///* works but not supported by compiler */
	//__disable_irq();
	//
	//if (OS_curr != (OSThread *)0) {
	//	/* push registers r4-r11 on the stack */
	//	__asm__ volatile ("PUSH	{r4-r11}" : : : "memory");
	//	OS_curr->sp = (uint32_t *)__get_MSP();
	//}
	//__set_MSP((uint32_t)OS_next->sp);
	//OS_curr = OS_next;
	///* pop registers r4-r11 on the stack */
	//__asm__ volatile ("POP	{r4-r11}" : : : "memory");
	//
	//__enable_irq();
	//__asm__ ("bx lr");
	__asm__ volatile (
			"LDR	r0,=OS_curr	\n" /* load ram address from rom of OS_curr */
			"LDR	r0,[r0]	\n" /* load pointer OS_curr */
			"CBZ	r0,PendSV_restore	\n" /* if there is no OS_curr skip next steps */
			"PUSH	{r4-r11}	\n" /* push unsaved registers to stack */
			"MRS	r1,msp	\n" /* load main stack pointer to r1 */
			"STR	r1,[r0]	\n" /* save msp to OS_curr */
			"PendSV_restore:	\n"
			"LDR	r0,=OS_curr	\n"	/* load pointer to OS_curr (ram address of OS_curr) */
			"LDR	r1,=OS_next	\n" /* load ram address from rom of OS_next */
			"LDR	r1,[r1]	\n"	/* load pointer OS_next */
			"STR	r1,[r0]	\n" /* store OS_next to OS_curr (OS_curr=0S_next) */
			"LDR	r1,[r1]	\n" /* load msp to r1 */
			"MSR	msp,r1	\n" /* restore the msp */
			"POP	{r4-r11}	\n" /* pop saved registers */
			"BX	LR"
	);

}
