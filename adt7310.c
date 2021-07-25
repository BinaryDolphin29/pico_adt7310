#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/types.h"
#include "hardware/spi.h"

#define VERBOSE
#define SCK  6
#define MISO 4
#define MOSI 7
#define CS 5


void setup() {
    stdio_init_all();
    gpio_init(CS);
    gpio_set_dir(CS, GPIO_OUT);

    spi_init(spi0, 8000000);
    gpio_set_function(MISO, GPIO_FUNC_SPI);
    gpio_set_function(SCK, GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);
    spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST); // Can it be up to 16 bit bus...?

    gpio_put(CS, 1);
}

void write_register(uint8_t r) {
    gpio_put(CS, 0);
    spi_write_blocking(spi0, &r, sizeof(r));
    gpio_put(CS, 1);
}

void read_register(uint8_t r, uint8_t* data) {
    gpio_put(CS, 0);
    spi_read_blocking(spi0, r, data, sizeof(r));
    gpio_put(CS, 1);
}

void software_reset() {
    const uint8_t sendSeries = 0xFF;
    uint8_t i = 0;

    gpio_put(CS, 0);
    while (i++ < 4) spi_write_blocking(spi0, &sendSeries, sizeof(sendSeries));
    gpio_put(CS, 1);

    sleep_us(500);
}

int main() {
    setup();
    software_reset();

    write_register(0b00001100);
    write_register(0b10000000);
    write_register(0b01010100);
    sleep_ms(240);

    while (1) {
        uint8_t most;
        uint8_t least;
        read_register(0x00, &most);
        read_register(0x00, &least);

        uint16_t retBits = (most << 8) | least;
        double temperature = retBits / 128.0;

        #ifdef VERBOSE
        printf("%lf\n", temperature);
        #else
        printf("%.2lf\n", temperature);
        #endif

        sleep_ms(1000);
    }

    spi_deinit(spi0);
    return 0;
}

// cmake -G "MinGW Makefiles" ..
// make -j 12
// cp ./ips_lcd.uf2 F:\\