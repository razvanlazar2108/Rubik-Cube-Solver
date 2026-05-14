#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "i2c_master.h"
#include "tcs34725.h"

void uart_init(void) {
    UBRR0L = 103; // 9600 baud
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}

void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char* s) {
    while (*s) uart_send_char(*s++);
}

char uart_receive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

int main(void) {
    uart_init();
    i2c_init();
    char msg[64];

    uart_send_string("Sistem Testare Senzori Pregatit.\r\n");

    while (1) {
        uart_send_string("Alege senzor (1-6): ");
        char input = uart_receive();
        uint8_t idx = input - '1';

        if (idx < 6) {
            select_mux_channel(idx);
            tcs_init();
            _delay_ms(700); // Timp de integrare pentru citire precisă

            uint16_t r = read_color_channel(0x16);
            uint16_t g = read_color_channel(0x18);
            uint16_t b = read_color_channel(0x1A);

            sprintf(msg, "\r\nSenzor %d -> R:%u G:%u B:%u\r\n", idx + 1, r, g, b);
            uart_send_string(msg);
        } else {
            uart_send_string("\r\nIndex invalid!\r\n");
        }
    }
}