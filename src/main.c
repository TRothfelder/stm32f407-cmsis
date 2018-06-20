#include <stm32f4xx.h>
#include <stdint.h>
#include <board.h>

void main_blinky1(void);
void main_blinky2(void);
void sys_tick_handler(void);

uint32_t stack_blinky1[40];
uint32_t *sp_blinky1 = &stack_blinky1[40];
void main_blinky1(void) {
	while (1) {
		GPIOD->ODR |= (0b1<<LED_ORANGE);
		delay(SYS_TICKS_PER_SEC / 4);
		GPIOD->ODR &= ~(0b1<<LED_ORANGE);
		delay(SYS_TICKS_PER_SEC / 4);
	}
}

uint32_t stack_blinky2[40];
uint32_t *sp_blinky2 = &stack_blinky2[40];
void main_blinky2(void) {
	while (1) {
		GPIOD->ODR |= (0b1<<LED_BLUE);
		delay(SYS_TICKS_PER_SEC / 2);
		GPIOD->ODR &= ~(0b1<<LED_BLUE);
		delay(SYS_TICKS_PER_SEC / 2);
	}
}

int main(void) {
	//enable clock for GPIOD
	RCC->AHB1ENR |= (0b1<<3);

	//set all pins to output
	GPIOD->MODER |= (0b1<<(LED_GREEN*2)) | (0b1<<(LED_ORANGE*2)) | (0b1<<(LED_RED*2)) | (0b1<<(LED_BLUE*2));

	//set systick interrupt
	SysTick->LOAD = SYS_TICKS_PER_SEC / 8U / 2U - 1;
	SysTick->VAL = 0;
	SysTick->CTRL =  (SysTick->CTRL & ~SysTick_CTRL_CLKSOURCE_Msk) | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	/* cortex-m isr stack frame for main_blinky1 */
	*(--sp_blinky1) = 0b1<<24; /* xPSR */
	*(--sp_blinky1) = (uint32_t)&main_blinky1; /* PC */
	*(--sp_blinky1) = 0x0000000EU; /* LR */
	*(--sp_blinky1) = 0x0000000CU; /* R12 */
	*(--sp_blinky1) = 0x00000003U; /* R3 */
	*(--sp_blinky1) = 0x00000002U; /* R2 */
	*(--sp_blinky1) = 0x00000001U; /* R1 */
	*(--sp_blinky1) = 0x00000000U; /* R0 */
	*(--sp_blinky1) = 0x0000000BU; /* R11 */
	*(--sp_blinky1) = 0x0000000AU; /* R10 */
	*(--sp_blinky1) = 0x00000009U; /* R09 */
	*(--sp_blinky1) = 0x00000008U; /* R08 */
	*(--sp_blinky1) = 0x00000007U; /* R07 */
	*(--sp_blinky1) = 0x00000006U; /* R06 */
	*(--sp_blinky1) = 0x00000005U; /* R05 */
	*(--sp_blinky1) = 0x00000004U; /* R04 */


	/* cortex-m isr stack frame for main_blinky2 */
	*(--sp_blinky2) = 0b1<<24; /* xPSR */
	*(--sp_blinky2) = (uint32_t)&main_blinky2; /* PC */
	*(--sp_blinky2) = 0x0000000EU; /* LR */
	*(--sp_blinky2) = 0x0000000CU; /* R12 */
	*(--sp_blinky2) = 0x00000003U; /* R3 */
	*(--sp_blinky2) = 0x00000002U; /* R2 */
	*(--sp_blinky2) = 0x00000001U; /* R1 */
	*(--sp_blinky2) = 0x00000000U; /* R0 */
	*(--sp_blinky2) = 0x0000000BU; /* R11 */
	*(--sp_blinky2) = 0x0000000AU; /* R10 */
	*(--sp_blinky2) = 0x00000009U; /* R09 */
	*(--sp_blinky2) = 0x00000008U; /* R08 */
	*(--sp_blinky2) = 0x00000007U; /* R07 */
	*(--sp_blinky2) = 0x00000006U; /* R06 */
	*(--sp_blinky2) = 0x00000005U; /* R05 */
	*(--sp_blinky2) = 0x00000004U; /* R04 */

	while(1) {

	}

	return 0;
}

void sys_tick_handler(void) {
	GPIOD->ODR ^= (0b1<<LED_GREEN);
}
