#include "ssd1306.h"


#define SSD1306_MEMORYMODE 0x20          ///< See datasheet
#define SSD1306_COLUMNADDR 0x21          ///< See datasheet
#define SSD1306_PAGEADDR 0x22            ///< See datasheet
#define SSD1306_SETCONTRAST 0x81         ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D          ///< See datasheet
#define SSD1306_SEGREMAP 0xA0            ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5        ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6       ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7       ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8        ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE          ///< See datasheet
#define SSD1306_DISPLAYON 0xAF           ///< See datasheet
#define SSD1306_COMSCANINC 0xC0          ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8          ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3    ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5  ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9        ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA          ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB       ///< See datasheet

#define SSD1306_SETLOWCOLUMN 0x00  ///< Not currently used
#define SSD1306_SETHIGHCOLUMN 0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet

#define SSD1306_EXTERNALVCC 0x01  ///< External display voltage source
#define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL 0x2E                    ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL 0x2F                      ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3             ///< Set scroll range

uint8_t SSD1306_buffer[SSD1306_BUFFER_LENGTH];

void SSD1306_init() 
{
    // init all gpio pins (enable I/O and set func to GPIO_FUNC_SIO)
    gpio_init_mask(1 << SSD1306_SCK_PIN | 1 << SSD1306_SDA_PIN | 1 << SSD1306_CS_PIN | 1 << SSD1306_DC_PIN | 1 << SSD1306_RES_PIN);

    // rst, leave it low (reset) first
    gpio_set_dir(SSD1306_RES_PIN, GPIO_OUT);
    gpio_put(SSD1306_RES_PIN, 0);

    // data/command pin (dc), used to switch between data and command output
    gpio_set_dir(SSD1306_DC_PIN, GPIO_OUT);
    gpio_put(SSD1306_DC_PIN, 0);

    // configure spi peripheral
    spi_init(SSD1306_SPI, SSD1306_SPI_BAUDRATE);
    // spi_init calls this internally
    // spi_set_format(spi_, /*data_bits*/ 8u, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    // spi_init also sets master mode by default

    // configure mosi and sclk pins to pull from spi peripheral
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_SPI);
    // gpio_set_function(cs_pin_,   GPIO_FUNC_SIO);  // done in gpio_init
    gpio_set_function(SSD1306_SCK_PIN, GPIO_FUNC_SPI);

    gpio_set_function(SSD1306_CS_PIN, GPIO_FUNC_SPI);
    /*
    // cs, following example. might be able to connect this to spi peripheral...
    gpio_set_dir(SSD1306_CS_PIN, GPIO_OUT);
    // just always set chip select to low, which means we're using the device
    gpio_put(SSD1306_CS_PIN, 0);
    */

    sleep_ms(1);
    SSD1306_send_init_commands();
}

void SSD1306_send_command(uint8_t command) 
{
    gpio_put(SSD1306_DC_PIN, 0);  // low for command mode
    spi_write_blocking(SSD1306_SPI, &command, 1);
}

void SSD1306_send_commands(const uint8_t* commands, size_t len) 
{
    gpio_put(SSD1306_DC_PIN, 0);  // low for command mode
    spi_write_blocking(SSD1306_SPI, commands, len);
}

void SSD1306_send_init_commands()
{
    // reset the display by pulling rst low then high
    gpio_put(SSD1306_RES_PIN, 0);
    sleep_ms(10);
    gpio_put(SSD1306_RES_PIN, 1);

    // it's dangerous to go alone, take this:
    // https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

    // SSD1306_SWITCHCAPVCC to generate from a 3.3v source
    // or SSD1306_EXTERNALVCC for external vcc
    uint8_t vccstate = SSD1306_SWITCHCAPVCC;

    SSD1306_send_command(SSD1306_DISPLAYOFF);

    SSD1306_send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_send_command(0xF0); // the suggested ratio 0x80

    SSD1306_send_command(SSD1306_SETMULTIPLEX);
    SSD1306_send_command(SSD1306_SCREEN_HEIGTH - 1);

    static const uint8_t init2[] = {
        SSD1306_SETDISPLAYOFFSET,  // 0xD3
        0x0,  // no offset
        SSD1306_SETSTARTLINE | 0x0, // line #0
        SSD1306_CHARGEPUMP
    };
    SSD1306_send_commands(init2, sizeof(init2) / sizeof(init2[0]));
    SSD1306_send_command((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

    static const uint8_t init3[] = {
        SSD1306_MEMORYMODE,  // 0x20
        0x00,  // 0x0 act like ks0108
        SSD1306_SEGREMAP | 0x1,
        SSD1306_COMSCANDEC
    };
    SSD1306_send_commands(init3, sizeof(init3) / sizeof(init3[0]));

    uint8_t comPins = 0x02;
    uint8_t contrast = 0x8F;

    if ((SSD1306_SCREEN_WIDTH == 128) && (SSD1306_SCREEN_HEIGTH == 64)) {
        comPins = 0x12;
        contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;
    } else {
        // Other screen varieties -- TBD
    }

    SSD1306_send_command(SSD1306_SETCOMPINS);
    SSD1306_send_command(comPins);
    SSD1306_send_command(SSD1306_SETCONTRAST);
    SSD1306_send_command(contrast);

    // This command is used to set the duration of the pre-charge period. The interval is counted in number of DCLK, where RESET equals 2 DCLKs.
    SSD1306_send_command(SSD1306_SETPRECHARGE); // 0xd9
    SSD1306_send_command((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);

    static const uint8_t init5[] = {
        SSD1306_SETVCOMDETECT,  // 0xDB
        0x40,
        SSD1306_DISPLAYALLON_RESUME,  // 0xA4 - start using gddram
        //SSD1306_DISPLAYALLON,  // 0xA5 - ignore gddram, set everything on
        SSD1306_NORMALDISPLAY,  // 0xA6
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_DISPLAYON  // Main screen turn on
    };
    SSD1306_send_commands(init5, sizeof(init5) / sizeof(init5[0]));

}

void SSD1306_screen_update() 
{
    static const uint8_t dlist1[] = {
        SSD1306_PAGEADDR,
        0x0,                    // Page start address
        0xFF,                   // Page end (not really, but works here)
        SSD1306_COLUMNADDR,
        0x0
    };  // Column start address

    SSD1306_send_commands(dlist1, sizeof(dlist1) / sizeof(dlist1[0]));
    SSD1306_send_command(SSD1306_SCREEN_WIDTH - 1);  // Column end address

    gpio_put(SSD1306_DC_PIN, 1);  // bring data/command high since we're sending data now
    spi_write_blocking(SSD1306_SPI, SSD1306_buffer, SSD1306_BUFFER_LENGTH);
}

void SSD1306_draw_pixel(int16_t x, int16_t y, uint8_t color)
{
    switch (color) 
    {
        case SSD1306_COLOR_ON:
            SSD1306_buffer[x + (y / 8) * SSD1306_SCREEN_WIDTH] |= (1 << (y & 7));
            break;
        case SSD1306_COLOR_OFF:
            SSD1306_buffer[x + (y / 8) * SSD1306_SCREEN_WIDTH] &= ~(1 << (y & 7));
            break;
        case SSD1306_COLOR_INVERTED:
            SSD1306_buffer[x + (y / 8) * SSD1306_SCREEN_WIDTH] ^= (1 << (y & 7));
            break;
        default:
            break;
    }
}

void SSD1306_buffer_clear()
{
    for(int i=0; i<SSD1306_BUFFER_LENGTH; i++)
        SSD1306_buffer[i] = 0;
}