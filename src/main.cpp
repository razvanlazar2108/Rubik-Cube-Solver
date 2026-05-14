#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

// Definiții pini conform schemei tale
#define DIR_PIN PD2     // Pinul 2 digital [cite: 137]
#define EN_PIN  PD3     // Pinul 3 digital [cite: 143]

// Configurare UART pentru comunicație cu PC (9600 Baud) [cite: 129]
void uart_init() {
    // Calcul UBRR pentru 16MHz și 9600 baud (103) [cite: 193]
    UBRR0H = (unsigned char)(103 >> 8);
    UBRR0L = (unsigned char)103;       
    // Activare recepție (RX) și transmisie (TX) [cite: 115, 189, 191]
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Format cadru: 8 biți date, 1 bit stop [cite: 157, 120]
    UCSR0C = (3 << UCSZ00);              
}

// Funcție primire caracter prin UART [cite: 128]
char uart_receive() {
    while (!(UCSR0A & (1 << RXC0))); // Așteaptă până când bufferul e plin [cite: 106, 161]
    return UDR0;                    
}

// Funcție control motor [cite: 1, 34]
void step_motor(uint8_t motor_idx, uint16_t pulses, uint8_t dir) {
    // Setează direcția pe pinul comun PD2 [cite: 258, 261]
    if (dir) PORTD |= (1 << DIR_PIN);
    else PORTD &= ~(1 << DIR_PIN);   
    _delay_ms(1); // Timp de stabilizare pentru driver [cite: 1]

    for (uint16_t i = 0; i < pulses; i++) {
        // Motoarele 1-4: Pinii 10, 11, 12, 13 (PB2, PB3, PB4, PB5) [cite: 60, 69, 74, 82]
        if (motor_idx < 4) {
            PORTB |= (1 << (motor_idx + 2));
            _delay_us(800); // Impuls STEP [cite: 1]
            PORTB &= ~(1 << (motor_idx + 2));
        } 
        // Motoarele 5-6: Pinii A1, A2 (PC1, PC2) [cite: 152, 146]
        else if (motor_idx < 6) {
            PORTC |= (1 << (motor_idx - 3)); // PC1 pentru motor 5, PC2 pentru motor 6 [cite: 160]
            _delay_us(800);
            PORTC &= ~(1 << (motor_idx - 3));
        }
        _delay_us(800); // Pauză între pași pentru Nema 17 [cite: 1]
    }
}

int main(void) {
    // Configurare direcție pini (Output) [cite: 257, 260]
    DDRD |= (1 << DIR_PIN) | (1 << EN_PIN);
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
    DDRC |= (1 << PC1) | (1 << PC2);

    // Activare drivere (Pinul EN de obicei e activ pe LOW) [cite: 261]
    PORTD &= ~(1 << EN_PIN); 
    
    uart_init();

    while (1) {
        // Așteaptă comanda de tip "MotorAction" (ex: "11")
        char motor_sel = uart_receive();  
        char action_sel = uart_receive(); 

        uint8_t m = motor_sel - '1'; // Convertire caracter în index (0-5)
        uint8_t a = action_sel - '1'; // Convertire caracter în acțiune (0-2)

        if (m >= 0 && m < 6) {
            if (a == 0)      step_motor(m, 50, 1);  // 90 grade Fata [cite: 1]
            else if (a == 1) step_motor(m, 50, 0);  // 90 grade Spate [cite: 1]
            else if (a == 2) step_motor(m, 100, 1); // 180 grade [cite: 1]
        }
    }
    return 0;
}