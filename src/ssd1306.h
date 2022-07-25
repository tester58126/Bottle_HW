#ifndef _SSD1306_H_
#define _SSD1306_H_

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SSD1306_SCREEN_WIDTH        128
#define SSD1306_SCREEN_HEIGTH       64
#define SSD1306_BUFFER_LENGTH       SSD1306_SCREEN_WIDTH*((SSD1306_SCREEN_HEIGTH+7)/8)

#define SSD1306_DC_PIN          0 // SSD1306 DATA/COMMAND SELECT PIN
#define SSD1306_CS_PIN          1 // SPI_CS ACTIVE LOW
#define SSD1306_SCK_PIN         2 // SPI_SCK
#define SSD1306_SDA_PIN         3 // SPI_MOSI
#define SSD1306_RES_PIN         4 // SSD1306 RESET PIN ACTIVE LOW

#define SSD1306_SPI             spi0 
#define SSD1306_SPI_BAUDRATE    10 * 1000 * 1000    // 10Mhz

#define SSD1306_COLOR_OFF            0
#define SSD1306_COLOR_ON             1
#define SSD1306_COLOR_INVERTED       2

void SSD1306_init();
void SSD1306_send_command(uint8_t command);
void SSD1306_send_commands(const uint8_t* commands, size_t len);
void SSD1306_send_init_commands();
void SSD1306_screen_update();
void SSD1306_buffer_clear();

void SSD1306_draw_pixel(int16_t x, int16_t y, uint8_t color);

#endif