/*
 * os.h
 *
 *  Created on: 20.06.2018
 *      Author: kt
 */

#ifndef INC_OS_OS_H_
#define INC_OS_OS_H_

/* Thread Control Block (TCB) */
typedef struct {
	void *sp; /* stack pointer */
} OSThread;

typedef void (*OSThreadHandler)(void);

void OS_init(void);
void OS_sched(void);

void OSThread_start(
		OSThread *me,
		OSThreadHandler threadHandler,
		void *stkSto, uint32_t stkSize);

__attribute__((naked))
void pend_sv_handler(void);


#endif /* INC_OS_OS_H_ */
