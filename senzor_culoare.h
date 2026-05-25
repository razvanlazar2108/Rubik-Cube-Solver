#ifndef SENZOR_CULOARE_H_
#define SENZOR_CULOARE_H_

#include <stdint.h>

void tcs34725_init_sensor(void);
uint8_t citeste_culoare_senzor(uint8_t senzor_idx);
uint8_t determina_cod_culoare(uint16_t r, uint16_t g, uint16_t b, uint16_t c);

#endif /* SENZOR_CULOARE_H_ */