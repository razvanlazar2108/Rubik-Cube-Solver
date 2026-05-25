#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_init(void);
char uart_receive(void);
void uart_write_str(const char* str);
void uart_read_line(char* buffer, uint8_t max_len);

#endif /* UART_H_ */