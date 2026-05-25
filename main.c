#include "config.h"
#include "uart.h"
#include "i2c.h"
#include "senzor_culoare.h"
#include "motoare.h"
#include <util/delay.h>
#include <stdio.h>

uint8_t cub_culori[6][9];

void scanare_completa_cub(void) {
    char raspuns[16];
    
    for (uint8_t f = 0; f < 6; f++) {
        uint8_t pozitie_fata = 0;

        for (uint8_t i = 0; i < 4; i++) {
            cub_culori[f][pozitie_fata++] = citeste_culoare_senzor(f);
            
            if (pozitie_fata < 9) {
                cub_culori[f][pozitie_fata++] = citeste_culoare_senzor(f);
                cub_culori[f][pozitie_fata++] = citeste_culoare_senzor(f);
            }

            if (i < 3) {
                step_motor(f, 50, 1); 
                _delay_ms(200); 
            }
        }
        step_motor(f, 50, 0); // Resetare rotație hardware la starea inițială
        _delay_ms(100);
    }

    // Returnarea rezultatului pur prin UART
    for (uint8_t i = 0; i < 6; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            snprintf(raspuns, sizeof(raspuns), "%d ", cub_culori[i][j]);
            uart_write_str(raspuns);
        }
        uart_write_str("\n");
    }
}

int main(void) {
    motoare_init_pini();
    uart_init();
    i2c_init();

    char buffer_miscari[64];

    while (1) {
        char cod_actiune = uart_receive();

        if (cod_actiune == '1') {
            scanare_completa_cub();
        } 
        else if (cod_actiune == '2') {
            uart_read_line(buffer_miscari, 64);
            executa_lista_miscari(buffer_miscari);
        }
    }
    return 0;
}