#include "senzor_culoare.h"
#include "i2c.h"
#include "config.h"
#include <util/delay.h>

void tcs34725_init_sensor(void) {
    i2c_start();
    i2c_write(TCS34725_ADDR << 1);
    i2c_write(0x80 | 0x00); // Registrul ENABLE (0x00) + Bitul de Comandă (0x80)
    i2c_write(0x01 | 0x02); // 0x01 (Power ON) | 0x02 (RGBC ADC Enable)
    i2c_stop();
    _delay_ms(5);
    
    // Setăm timpul de integrare (gătitul datelor). ATIME = 0xC0 înseamnă ~154ms, rezoluție bună.
    i2c_start();
    i2c_write(TCS34725_ADDR << 1);
    i2c_write(0x80 | 0x01); // Registrul ATIME
    i2c_write(0xC0);        
    i2c_stop();
}

uint8_t determina_cod_culoare(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
    if (c == 0) return 1; // Evităm împărțirea la zero, fallback pe Alb

    // Calculăm rapoartele cromatice (frecvența ponderată a fiecărui canal din lumina totală)
    float r_ratio = (float)r / (float)c;
    float g_ratio = (float)g / (float)c;
    float b_ratio = (float)b / (float)c;

    // Logica de decizie bazată pe profilele cromatice ale culorilor Cubului Rubik
    // Galben -> 0, Alb -> 1, Verde -> 2, Albastru -> 3, Rosu -> 4, Portocaliu -> 5

    // ALBASTRU: Canalul de albastru domină clar raportul față de roșu
    if (b_ratio > r_ratio && b_ratio > 0.33) {
        return 3; 
    }
    
    // VERDE: Canalul de verde e mai mare decât roșu și albastru
    if (g_ratio > r_ratio && g_ratio > b_ratio && g_ratio > 0.35) {
        return 2;
    }

    // ROȘU și PORTOCALIU au ambele componenta R puternică. Le diferențiem prin componenta G:
    if (r_ratio > g_ratio && r_ratio > b_ratio) {
        // Dacă are și destul de mult Verde, se duce spre Portocaliu / Galben
        if (g_ratio > 0.28) {
            if (g_ratio > 0.36) return 0; // Galben (Mult Roșu + Mult Verde)
            return 5;                    // Portocaliu
        }
        return 4; // Roșu pur
    }

    // ALB: Toate canalele au ponderi strânse și egale, iar intensitatea totală (Clear) e mare
    if (r_ratio > 0.28 && g_ratio > 0.32 && b_ratio > 0.25) {
        return 1; 
    }

    return 1; // Fallback implicit la Alb în caz de incertitudine fizică
}

uint8_t citeste_culoare_senzor(uint8_t senzor_idx) {
    // În arhitectura ta, pinii fizici SD0-SD5/SC0-SC5 citesc datele direct
    tcs34725_init_sensor();

    uint16_t r, g, b, c;
    
    // Solicităm citirea datelor brute începând cu registrul 0x14
    i2c_start();
    i2c_write(TCS34725_ADDR << 1);
    i2c_write(0x80 | 0x14); 
    i2c_stop();

    i2c_start();
    i2c_write((TCS34725_ADDR << 1) | 1); // Direcție READ
    c = i2c_read_ack() | (i2c_read_ack() << 8);
    r = i2c_read_ack() | (i2c_read_ack() << 8);
    g = i2c_read_ack() | (i2c_read_ack() << 8);
    b = i2c_read_ack() | (i2c_read_nack() << 8);
    i2c_stop();

    return determina_cod_culoare(r, g, b, c);
}