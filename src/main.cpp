#include <avr/io.h>
#include <util/delay.h>

// Definiții pini conform schemei noastre
#define LCD_PORT_D PORTD
#define LCD_DDR_D  DDRD
#define LCD_PORT_B PORTB
#define LCD_DDR_B  DDRB

// RS: PB0 (8), EN: PB1 (9)
#define RS PB0
#define EN PB1

// Date: PD4, PD5, PD6, PD7 (Pinii 4, 5, 6, 7)
#define D4 PD4
#define D5 PD5
#define D6 PD6
#define D7 PD7

void lcd_trigger_en() {
    LCD_PORT_B |= (1 << EN);
    _delay_us(1);
    LCD_PORT_B &= ~(1 << EN);
    _delay_us(100);
}

void lcd_send_nibble(uint8_t nibble) {
    // Curățăm pinii de date PD4-PD7 și trimitem cei 4 biți [cite: 258, 260]
    LCD_PORT_D &= ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7));
    if (nibble & 0x01) LCD_PORT_D |= (1 << D4);
    if (nibble & 0x02) LCD_PORT_D |= (1 << D5);
    if (nibble & 0x04) LCD_PORT_D |= (1 << D6);
    if (nibble & 0x08) LCD_PORT_D |= (1 << D7);
    lcd_trigger_en();
}

void lcd_send(uint8_t data, uint8_t is_cmd) {
    if (is_cmd) LCD_PORT_B &= ~(1 << RS); // RS = 0 pentru comandă
    else LCD_PORT_B |= (1 << RS);        // RS = 1 pentru date

    lcd_send_nibble(data >> 4);   // Trimitem cei mai semnificativi 4 biți
    lcd_send_nibble(data & 0x0F); // Trimitem cei mai puțin semnificativi 4 biți
    _delay_ms(2);
}

void lcd_init() {
    LCD_DDR_D |= (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7);
    LCD_DDR_B |= (1 << RS) | (1 << EN);

    _delay_ms(50); // Așteptăm stabilizarea tensiunii
    lcd_send_nibble(0x03); _delay_ms(5);
    lcd_send_nibble(0x03); _delay_us(150);
    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02); // Setăm modul 4 biți

    lcd_send(0x28, 1); // 2 linii, font 5x8
    lcd_send(0x0C, 1); // Display ON, Cursor OFF
    lcd_send(0x01, 1); // Clear Display
    _delay_ms(2);
}

void lcd_print(const char* str) {
    while (*str) lcd_send(*str++, 0);
}

int main(void) {
    lcd_init();
    lcd_print("Hello World!");

    while (1) {
        // Loop infinit
    }
    return 0;
}