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
#include "hcsr04.h"

/**
 * USART RX interrupt callback handle context
 */
typedef struct USART_RXC_cb_ctx_t {
    uint8_t rx;  // received byte to be seved here
} USART_RXC_cb_ctx;

/**
 * USART RX interrupt callback handle
 * @param[inout] ctx user data for interrupt callback
 * When ISR occurs USART_RXC_cb will be called with ctx as parameter
 * UART RX data (UDR) should be saved in this function
 */
static void USART_RXC_cb_handle(void* ctx) {
    USART_RXC_cb_ctx* t_ctx = (USART_RXC_cb_ctx*)ctx;

    t_ctx->rx = UDR;
    printf("%c\n", t_ctx->rx);
}

/**
 * HCSR04interrupt callback handle context
 */
typedef struct HCSR04_cb_ctx_t {
    int cnt;
    uint8_t isr_id;
} HCSR04_cb_ctx;

static void HCSR04_cb_handle(void* ctx, uint8_t isr_id) {
    HCSR04_cb_ctx* t_ctx = (HCSR04_cb_ctx*)ctx;
    t_ctx->isr_id = isr_id;
    t_ctx->cnt++;
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

    HCSR04_cb_ctx hctx = {};
    regiter_HCSR04_isr_cb(HCSR04_cb_handle, &hctx);
    HCSR04_init();

    printf("HC-SR04 init done\n");


    while (1) {
        printf("%d %d %d\n", hctx.cnt, TCNT1, hctx.isr_id);
        _delay_ms(100);

    }
}
