#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"
#include "esp_log.h"   // <-- agregado

// =====================
// Pines (AJUSTA AQUÍ)
// =====================
#define LED_PIN        4
#define BTN_UP_PIN     18
#define BTN_DOWN_PIN   19

#define POLL_MS        10
#define DEBOUNCE_MS    30

static const char *TAG = "CONTADOR";   // <-- TAG para logging

static inline void delay_ms(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));   // pequeña mejora recomendada
}

void app_main(void) {
    esp_task_wdt_deinit();

    ESP_LOGI(TAG, "Sistema iniciado");

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    gpio_reset_pin(BTN_UP_PIN);
    gpio_set_direction(BTN_UP_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_UP_PIN, GPIO_PULLDOWN_ONLY);

    gpio_reset_pin(BTN_DOWN_PIN);
    gpio_set_direction(BTN_DOWN_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_DOWN_PIN, GPIO_PULLDOWN_ONLY);

    int contador = 0;
    int up_prev = 0;
    int down_prev = 0;

    while (true) {
        int up_now = gpio_get_level(BTN_UP_PIN);
        int down_now = gpio_get_level(BTN_DOWN_PIN);

        // =========================
        // UP
        // =========================
        if (up_prev == 0 && up_now == 1) {
            delay_ms(DEBOUNCE_MS);
            if (gpio_get_level(BTN_UP_PIN) == 1) {
                contador++;
                ESP_LOGI(TAG, "UP presionado -> contador = %d", contador);

                gpio_set_level(LED_PIN, 1);
                delay_ms(30);
                gpio_set_level(LED_PIN, 0);

                while (gpio_get_level(BTN_UP_PIN) == 1) {
                    delay_ms(POLL_MS);
                }
            }
        }

        // =========================
        // DOWN
        // =========================
        if (down_prev == 0 && down_now == 1) {
            delay_ms(DEBOUNCE_MS);
            if (gpio_get_level(BTN_DOWN_PIN) == 1) {
                contador--;
                ESP_LOGI(TAG, "DOWN presionado -> contador = %d", contador);

                gpio_set_level(LED_PIN, 1);
                delay_ms(30);
                gpio_set_level(LED_PIN, 0);

                while (gpio_get_level(BTN_DOWN_PIN) == 1) {
                    delay_ms(POLL_MS);
                }
            }
        }

        up_prev = up_now;
        down_prev = down_now;

        delay_ms(POLL_MS);
    }
}
