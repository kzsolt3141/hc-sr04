/*
 * timer.c
 *
 * Created: 12/22/2019 3:00:52 PM
 * Author : Zsolt
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"

/**
 * USART RX interrupt callback handle context
 */
struct USART_RXC_cb_ctx_t {
    uint8_t rx;  // received byte to be seved here
};

/**
 * USART RX interrupt callback handle
 * @param[inout] ctx user data for interrupt callback
 * When ISR occurs USART_RXC_cb will be called with ctx as parameter
 * UART RX data (UDR) should be saved in this function
 */
static void USART_RXC_cb_handle(void* ctx) {
    struct USART_RXC_cb_ctx_t* t_ctx = (struct USART_RXC_cb_ctx_t*)ctx;

    t_ctx->rx = UDR;
    printf("%c\n", t_ctx->rx);
}

/**
 * TIMER 0 interrupt callback handle context
 */
struct Timer0_cb_ctx_t {
    int tmr0_int_cnt;
};

static void TIMER0_OVF_cb_handle(void* ctx) {
    struct Timer0_cb_ctx_t* t_ctx = (struct Timer0_cb_ctx_t*)ctx;

    t_ctx->tmr0_int_cnt++;
}

/**
 * TIMER 1 interrupt callback handle context
 */
struct Timer1_cb_ctx_t {
    uint16_t tmr1_init_val;
    int tmr1_int_cnt;
};

// static void TIMER1_OVF_cb_handle(void* ctx) {
//     struct Timer1_cb_ctx_t* t_ctx = (struct Timer1_cb_ctx_t*)ctx;

//     TCNT1 = t_ctx->tmr1_init_val;
//     t_ctx->tmr1_int_cnt++;
// }

// static void TIMER1_CMP_cb_handle(void* ctx) {
//     struct Timer1_cb_ctx_t* t_ctx = (struct Timer1_cb_ctx_t*)ctx;

//     t_ctx->tmr1_int_cnt++;
// }

static void TIMER1_PWM_SWEEP_cb_handle(void* ctx) {
    struct Timer1_cb_ctx_t* t_ctx = (struct Timer1_cb_ctx_t*)ctx;

    t_ctx->tmr1_int_cnt++;
    OCR1A = t_ctx->tmr1_int_cnt;

}

/**
 * TIMER 1 interrupt callback handle context
 */
struct Timer2_cb_ctx_t {
    uint8_t tmr2_init_val;
    int tmr2_int_cnt;
};

// static void TIMER2_OVF_cb_handle(void* ctx) {
//     struct Timer2_cb_ctx_t* t_ctx = (struct Timer2_cb_ctx_t*)ctx;

//     TCNT2 = t_ctx->tmr2_init_val;
//     t_ctx->tmr2_int_cnt++;
// }

// static void TIMER2_CMP_cb_handle(void* ctx) {
//     struct Timer2_cb_ctx_t* t_ctx = (struct Timer2_cb_ctx_t*)ctx;

//     t_ctx->tmr2_int_cnt++;
// }

static void TIMER2_PWM_SWEEP_cb_handle(void* ctx) {
    struct Timer2_cb_ctx_t* t_ctx = (struct Timer2_cb_ctx_t*)ctx;

    t_ctx->tmr2_int_cnt++;
    OCR2 = t_ctx->tmr2_int_cnt;

}

int main(void)
{
    // UART INIT
    //-------------------------------
    const uint16_t baud_rate = 38400;

    struct USART_RXC_cb_ctx_t USART_RXC_ctx = {};

    regiter_USART_RXC_cb(USART_RXC_cb_handle, &USART_RXC_ctx);

    USART_init(baud_rate);

    printf("Init Done UART baud: %u\n", (uint16_t)baud_rate);
    //-------------------------------

    // TIMER0 init
    //-------------------------------
    struct Timer0_cb_ctx_t timer0_ctx = {0};

    regiter_TIMER0_isr_cb(TIMER0_OVF_cb_handle, &timer0_ctx);

    TIMER0_init(TIMER0_PS_PRESCALE_1024);

    printf("Init Done TIMER0\n");
    //-------------------------------

    // TIMER1 init
    //-------------------------------
    const uint16_t tmr1_init_val = 15625;
    struct Timer1_cb_ctx_t timer1_ctx = {0};
    timer1_ctx.tmr1_init_val = tmr1_init_val;

    // regiter_TIMER1_isr_cb(TIMER1_OVF_cb_handle, &timer1_ctx);
    // regiter_TIMER1_isr_cb(TIMER1_CMP_cb_handle, &timer1_ctx);
    regiter_TIMER1_isr_cb(TIMER1_PWM_SWEEP_cb_handle, &timer1_ctx);

    // TIMER1_init(tmr1_init_val, TIMER1_PS_PRESCALE_1024);
    // TIMER1_compare_init(tmr1_init_val, TIMER1_PS_PRESCALE_1024);
    TIMER1_PWM_init(0, TIMER1_PS_PRESCALE_256);

    printf("Init Done TIMER1\n");
    //-------------------------------

    // TIMER2 init
    //-------------------------------
    const uint8_t tmr2_init_val = 0xFF;
    struct Timer2_cb_ctx_t timer2_ctx = {0};
    timer2_ctx.tmr2_init_val = tmr2_init_val;

    // regiter_TIMER2_isr_cb(TIMER2_OVF_cb_handle, &timer2_ctx);
    // regiter_TIMER2_isr_cb(TIMER2_CMP_cb_handle, &timer2_ctx);
    regiter_TIMER2_isr_cb(TIMER2_PWM_SWEEP_cb_handle, &timer2_ctx);

    // TIMER2_init(tmr2_init_val, TIMER2_PS_PRESCALE_1024);
    // TIMER2_compare_init(tmr2_init_val, TIMER2_PS_PRESCALE_1024);
    TIMER2_PWM_init(0, TIMER2_PS_PRESCALE_128);

    printf("Init Done TIMER2\n");
    //-------------------------------

    while (1) {
        timer0_ctx.tmr0_int_cnt = 0;
        TIMER0_start();
        _delay_ms(10);
        const uint8_t t0_cnt = TIMER0_stop();
        TIMER0_reset();

        printf("T0 ovf:%d cnt%d; T1:%d; T2:%d\n", timer0_ctx.tmr0_int_cnt, t0_cnt, timer1_ctx.tmr1_int_cnt, timer2_ctx.tmr2_int_cnt);

    }
}
