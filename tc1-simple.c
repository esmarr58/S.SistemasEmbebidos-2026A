#include <unistd.h>
#include "sdkconfig.h"
#include "driver/gpio.h"

#define pinLED 4
#define ALTO 1
#define BAJO 0






void app_main(void) {
    gpio_reset_pin(pinLED);
    gpio_set_direction(pinLED, GPIO_MODE_OUTPUT);

 
    while (true) {
        gpio_set_level(pinLED, ALTO);
        usleep(8043); // Convertir ms a microsegundos

        gpio_set_level(pinLED, BAJO);
        usleep(3447); // Convertir ms a microsegundos
    }
}
