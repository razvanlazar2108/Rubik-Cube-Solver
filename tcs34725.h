#ifndef TCS34725_H
#define TCS34725_H

#include <avr/io.h>

#define MUX_ADDR 0x70  // Adresa TCA9548A
#define TCS_ADDR 0x29  // Adresa senzorilor TCS34725

void select_mux_channel(uint8_t channel);
void tcs_init(void);
uint16_t read_color_channel(uint8_t reg_low);

#endif