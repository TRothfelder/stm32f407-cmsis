/*
 * board.c
 *
 *  Created on: 19.06.2018
 *      Author: kt
 */

#include <board.h>

void delay(uint32_t ticks) {
	uint32_t i;
	for (i=0; i<ticks; i++) {
		__asm("nop");
	}
}

