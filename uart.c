#include "uart.h"
#include "config.h"

void uart_init(void) {
    UBRR0H = (unsigned char)(103 >> 8);
    UBRR0L = (unsigned char)103;       
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);              
}

char uart_receive(void) {
    while (!(UCSR0A & (1 << RXC0))); 
    return UDR0;                    
}

void uart_write_str(const char* str) {
    while (*str) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *str++;
    }
}

void uart_read_line(char* buffer, uint8_t max_len) {
    uint8_t idx = 0;
    while (idx < max_len - 1) {
        char c = uart_receive();
        if (c == '\n' || c == '\r') {
            break;
        }
        buffer[idx++] = c;
    }
    buffer[idx] = '\0';
}