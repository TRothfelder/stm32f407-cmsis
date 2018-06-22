#include <stm32f4xx.h>
#include <stdint.h>
#include <board.h>
#include "os/os.h"

void main_blinky1(void);
void main_blinky2(void);
void sys_tick_handler(void);

uint32_t stack_blinky1[40];
OSThread blinky1;
void main_blinky1(void) {
	while (1) {
		GPIOD->ODR |= (0b1<<LED_ORANGE);
		delay(10000000);
		GPIOD->ODR &= ~(0b1<<LED_ORANGE);
		delay(10000000);
	}
}

uint32_t stack_blinky2[40];
OSThread blinky2;
void main_blinky2(void) {
	while (1) {
		GPIOD->ODR |= (0b1<<LED_BLUE);
		delay(1000000);
		GPIOD->ODR &= ~(0b1<<LED_BLUE);
		delay(1000000);
	}
}

int main(void) {
	//enable clock for GPIOD
	RCC->AHB1ENR |= (0b1<<3);

	//set all pins to output
	GPIOD->MODER |= (0b1<<(LED_GREEN*2)) | (0b1<<(LED_ORANGE*2)) | (0b1<<(LED_RED*2)) | (0b1<<(LED_BLUE*2));

	//set systick interrupt
	SysTick->LOAD = SystemCoreClock / 8U / 1000U - 1;
	SysTick->VAL = 0;
	SysTick->CTRL =  (SysTick->CTRL & ~SysTick_CTRL_CLKSOURCE_Msk) | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;


	OS_init();
	OSThread_start(&blinky1, &main_blinky1, stack_blinky1, sizeof(stack_blinky1));
	OSThread_start(&blinky2, &main_blinky2, stack_blinky2, sizeof(stack_blinky2));

	while(1) {

	}

	return 0;
}

void sys_tick_handler(void) {
	GPIOD->ODR ^= (0b1<<LED_GREEN);

	__disable_irq();
	OS_sched();
	__enable_irq();
}
