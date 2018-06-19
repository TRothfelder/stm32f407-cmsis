#include <stm32f4xx.h>
#include <stdint.h>
#include <board.h>

void main_blinky1(void);
void main_blinky2(void);

void main_blinky1(void) {
	while (1) {
		GPIOD->ODR |= (0b1<<LED_ORANGE);
		delay(SYS_TICKS_PER_SEC / 4);
		GPIOD->ODR &= ~(0b1<<LED_ORANGE);
		delay(SYS_TICKS_PER_SEC / 4);
	}
}

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

	SysTick->LOAD = SYS_TICKS_PER_SEC / 8U / 2U - 1;
	SysTick->VAL = 0;
	SysTick->CTRL =  (SysTick->CTRL & ~SysTick_CTRL_CLKSOURCE_Msk) | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	main_blinky1();
	main_blinky2();

}

void sys_tick_handler(void) {
	GPIOD->ODR ^= (0b1<<LED_GREEN);
}
