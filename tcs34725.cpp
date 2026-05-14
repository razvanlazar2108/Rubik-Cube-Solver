#include "tcs34725.h"
#include "i2c_master.h"
#include <util/delay.h>

void select_mux_channel(uint8_t channel) {
    i2c_start();
    i2c_write(MUX_ADDR << 1); // Adresa de scriere
    i2c_write(1 << channel);  // Activează canalul (0-5)
    i2c_stop();
}

void tcs_init(void) {
    // Comandă pornire: Registrul ENABLE (0x00) -> setăm Power ON (0x01) și RGBC (0x02)
    i2c_start();
    i2c_write(TCS_ADDR << 1);
    i2c_write(0x80 | 0x00); // 0x80 e bitul de protocol Command
    i2c_write(0x03);        // PON + AEN
    i2c_stop();
    _delay_ms(3);           // Timp necesar pentru trezire
}

uint16_t read_color_channel(uint8_t reg_low) {
    uint16_t low, high;
    i2c_start();
    i2c_write(TCS_ADDR << 1);
    i2c_write(0x80 | reg_low);
    i2c_start(); // Repeated Start pentru citire
    i2c_write((TCS_ADDR << 1) | 1);
    low = i2c_read_ack();
    high = i2c_read_nack();
    i2c_stop();
    return (high << 8) | low;
}