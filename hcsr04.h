/*
 * hcsr04.h
 *
 * Created: 4/15/2017 8:57:29 PM
 *  Author: Zsolt
 */ 

#ifndef HCSR04_H_
#define HCSR04_H_

#include <stdint.h>

/**
 * Interrupt callback type for measure ready
 * @param[inout] ctx user data for interrupt callback
 */
typedef void (*HCSR04_isr_cb)(void* ctx, uint8_t isr_id);

/**
 * Register callback and context for measurement interrupt
 * @param[in] cb callback for isr; must not be NULL
 * @param[in] ctx user defined callback context; must not be NULL
 */
void regiter_HCSR04_isr_cb(HCSR04_isr_cb cb, void* ctx);

/**
 * Initialize device: set timers and interrupts:
 * timer 1 will generate trigger signals
 * timer 2 will measure elapsed time
 * external interrupt enable at INT0
 * ISR callback must be registered before init is called
*/
void HCSR04_init();

#endif /* HCSR04_H_ */
