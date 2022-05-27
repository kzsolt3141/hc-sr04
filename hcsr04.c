/*
 * timer.c
 *
 * Created: 4/15/2017 8:57:43 PM
 *  Author: Zsolt
 */ 

#include "hcsr04.h"

#include <stddef.h>

#include <avr/io.h>
#include <avr/interrupt.h>

static HCSR04_isr_cb  HCSR04_cb_     = NULL;
static void          *HCSR04_cb_ctx_ = NULL;

void HCSR04_init() {
    cli();

// external interrupt config
    DDRD &= ~(1 << PIND2);
    MCUCR |= 0 << ISC01 |
             1 << ISC00;
    GICR |=  1 << INT0;

// set timer 1
    ICR1 = 60000;             // 60 ms period
    TIMSK |= (1 << TOIE1) | (1 << OCIE1A);   // timer overflow interrupt and compare interrupt
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) |  // normal operating mode, no output generated
             (0 << WGM10)  | (1 << WGM11);
    TCCR1B = (1 << WGM12)  | (1 << WGM13) |   // compare to OCR1A
             (2);             // prescaler 8 1us tick
    DDRB |= 1 << PB1;         // set OC1A as output (on port PB1)
    OCR1A = 10000;               // on time

    sei();
}

void regiter_HCSR04_isr_cb(HCSR04_isr_cb cb, void* ctx) {
    HCSR04_cb_ = cb;
    HCSR04_cb_ctx_ = ctx;
}

// external interrupt, the sensor signal indicates the distance
ISR(INT0_vect) {
    // toggle the counter for distance measurement
    static uint8_t start = 0;
    start ^= 0x01;

    if (start) {
        // start the counter:
        // disable PWM generation

        // reset counter
    } else {
        // resend echo for the next measurement:
        // enable PWM gen
        // send measurement to the user application
        // HCSR04_cb_(HCSR04_cb_ctx_, 0);
    }
}
