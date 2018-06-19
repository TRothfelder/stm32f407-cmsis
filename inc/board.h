/*
 * board.h
 *
 *  Created on: 19.06.2018
 *      Author: kt
 */

#ifndef INC_BOARD_H_
#define INC_BOARD_H_

#include <stdint.h>

/*
 * PD12: green LED
 * PD13: orange LED
 * PD14: red LED
 * PD15: blue LED
 */

#define LED_GREEN (12)
#define LED_ORANGE (13)
#define LED_RED (14)
#define LED_BLUE (15)

#define SYS_TICKS_PER_SEC ((uint32_t)168000000)

void delay(uint32_t ticks);

#endif /* INC_BOARD_H_ */
