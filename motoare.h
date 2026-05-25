#ifndef MOTOARE_H_
#define MOTOARE_H_

#include <stdint.h>

void motoare_init_pini(void);
void step_motor(uint8_t motor_idx, uint16_t pulses, uint8_t dir);
void executa_lista_miscari(char* lista);

#endif /* MOTOARE_H_ */