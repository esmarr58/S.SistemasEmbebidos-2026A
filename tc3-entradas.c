#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"

// =====================
// Pines (AJUSTA AQUÍ)
// =====================
#define LED_PIN        4      // opcional: muestra parpadeo o estado
#define BTN_UP_PIN     18     // botón ascendente
#define BTN_DOWN_PIN   19     // botón descendente

// Ajustes de polling / debounce
#define POLL_MS        10
#define DEBOUNCE_MS    30

static inline void delay_ms(int ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void app_main(void) {
    esp_task_wdt_deinit();

    // LED opcional
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    // Botón UP: entrada con pull-down
    gpio_reset_pin(BTN_UP_PIN);
    gpio_set_direction(BTN_UP_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_UP_PIN, GPIO_PULLDOWN_ONLY);

    // Botón DOWN: entrada con pull-down
    gpio_reset_pin(BTN_DOWN_PIN);
    gpio_set_direction(BTN_DOWN_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_DOWN_PIN, GPIO_PULLDOWN_ONLY);

    int contador = 0;

    // Estados previos para detectar flanco
    int up_prev = 0;
    int down_prev = 0;

    while (true) {
        int up_now = gpio_get_level(BTN_UP_PIN);
        int down_now = gpio_get_level(BTN_DOWN_PIN);

        // =========================
        // UP: detectar 0 -> 1
        // =========================
        if (up_prev == 0 && up_now == 1) {
            // debounce simple: espera y confirma que sigue presionado
            delay_ms(DEBOUNCE_MS);
            if (gpio_get_level(BTN_UP_PIN) == 1) {
                contador++;
                // Aquí podrías imprimir por serial si lo deseas
                // printf("Contador: %d\n", contador);

                // opcional: blink rápido para feedback
                gpio_set_level(LED_PIN, 1);
                delay_ms(30);
                gpio_set_level(LED_PIN, 0);

                // espera a que suelte para evitar auto-repetición por mantener presionado
                while (gpio_get_level(BTN_UP_PIN) == 1) {
                    delay_ms(POLL_MS);
                }
            }
        }

        // =========================
        // DOWN: detectar 0 -> 1
        // =========================
        if (down_prev == 0 && down_now == 1) {
            delay_ms(DEBOUNCE_MS);
            if (gpio_get_level(BTN_DOWN_PIN) == 1) {
                contador--;
                // printf("Contador: %d\n", contador);

                gpio_set_level(LED_PIN, 1);
                delay_ms(30);
                gpio_set_level(LED_PIN, 0);

                while (gpio_get_level(BTN_DOWN_PIN) == 1) {
                    delay_ms(POLL_MS);
                }
            }
        }

        // actualizar previos
        up_prev = up_now;
        down_prev = down_now;

        delay_ms(POLL_MS);
    }
}
