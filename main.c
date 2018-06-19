#include "stm32f4xx.h"
#include <stdint.h>

void delay(void) {
  uint32_t i;
  for (i=0; i<1000000; i++);
}

int main(void) {
  RCC->AHB1ENR |= (0b1<<3);
  GPIOD->MODER |= (0b1<<(12*2)) | (0b1<<(13*2)) | (0b1<<(14*2)) | (0b1<<(15*2));

  while (1) {
    delay();
    GPIOD->ODR ^= (0b1<<12) | (0b1<<13) | (0b1<<14) | (0b1<<15);
  }
}
