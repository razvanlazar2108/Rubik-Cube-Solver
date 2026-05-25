#include "motoare.h"
#include "config.h"
#include <util/delay.h>
#include <string.h>

void motoare_init_pini(void) {
    DDRD |= (1 << DIR_M1) | (1 << DIR_M2) | (1 << DIR_M3) | (1 << DIR_M4);
    DDRB |= (1 << STEP_M1) | (1 << STEP_M2) | (1 << STEP_M3) | (1 << STEP_M4) | (1 << EN_PIN);
    DDRC |= (1 << DIR_M5) | (1 << DIR_M6) | (1 << STEP_M5) | (1 << STEP_M6);

    PORTD &= ~((1 << DIR_M1) | (1 << DIR_M2) | (1 << DIR_M3) | (1 << DIR_M4));
    PORTB &= ~((1 << STEP_M1) | (1 << STEP_M2) | (1 << STEP_M3) | (1 << STEP_M4));
    PORTC &= ~((1 << DIR_M5) | (1 << DIR_M6) | (1 << STEP_M5) | (1 << STEP_M6));

    PORTB |= (1 << EN_PIN); // Dezactivate implicit (Liniște în repaus)
}

static void set_direction_exclusive(uint8_t motor_idx, uint8_t dir) {
    PORTD &= ~((1 << DIR_M1) | (1 << DIR_M2) | (1 << DIR_M3) | (1 << DIR_M4));
    PORTC &= ~((1 << DIR_M5) | (1 << DIR_M6));

    if (dir) {
        switch(motor_idx) {
            case 0: PORTD |= (1 << DIR_M1); break;
            case 1: PORTD |= (1 << DIR_M2); break;
            case 2: PORTD |= (1 << DIR_M3); break;
            case 3: PORTD |= (1 << DIR_M4); break;
            case 4: PORTC |= (1 << DIR_M5); break;
            case 5: PORTC |= (1 << DIR_M6); break;
        }
    }
}

static void pulse_step_exclusive(uint8_t motor_idx) {
    PORTB &= ~((1 << STEP_M1) | (1 << STEP_M2) | (1 << STEP_M3) | (1 << STEP_M4));
    PORTC &= ~((1 << STEP_M5) | (1 << STEP_M6));

    switch(motor_idx) {
        case 0: PORTB |= (1 << STEP_M1); break;
        case 1: PORTB |= (1 << STEP_M2); break;
        case 2: PORTB |= (1 << STEP_M3); break;
        case 3: PORTB |= (1 << STEP_M4); break;
        case 4: PORTC |= (1 << STEP_M5); break;
        case 5: PORTC |= (1 << STEP_M6); break;
    }
    
    _delay_us(800); // Durată impuls HIGH
    
    switch(motor_idx) {
        case 0: PORTB &= ~(1 << STEP_M1); break;
        case 1: PORTB &= ~(1 << STEP_M2); break;
        case 2: PORTB &= ~(1 << STEP_M3); break;
        case 3: PORTB &= ~(1 << STEP_M4); break;
        case 4: PORTC &= ~(1 << STEP_M5); break;
        case 5: PORTC &= ~(1 << STEP_M6); break;
    }
}

void step_motor(uint8_t motor_idx, uint16_t pulses, uint8_t dir) {
    PORTB &= ~(1 << EN_PIN); // Activare drivere CNC
    _delay_ms(5);

    set_direction_exclusive(motor_idx, dir);
    _delay_us(20); 

    for (uint16_t i = 0; i < pulses; i++) {
        pulse_step_exclusive(motor_idx);
        _delay_us(800); // Pauză între pași
    }

    _delay_ms(5);
    PORTB |= (1 << EN_PIN); // Stop curent în repaus
}

static int8_t obtine_index_motor(char c) {
    switch(c) {
        case 'U': return 0; case 'D': return 1; case 'L': return 2;
        case 'R': return 3; case 'F': return 4; case 'B': return 5;
        default: return -1;
    }
}

static void executa_mutare_string(const char* mutare) {
    int8_t motor = obtine_index_motor(mutare[0]);
    if (motor == -1) return;

    if (mutare[1] == '\0') {
        step_motor(motor, 50, 1);  // Sens orar, 90°
    } else if (mutare[1] == '\'') {
        step_motor(motor, 50, 0);  // Sens anti-orar, 90°
    } else if (mutare[1] == '2') {
        step_motor(motor, 100, 1); // Mișcare dublă, 180°
    }
}

void executa_lista_miscari(char* lista) {
    char* token = strtok(lista, " ");
    while (token != NULL) {
        executa_mutare_string(token);
        _delay_ms(400); // Timp de stabilizare mecanică între mutări
        token = strtok(NULL, " ");
    }
}