#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_task_wdt.h"

// =====================
// Pines (AJUSTA AQUÍ)
// =====================
#define LED_PIN        4
#define BTN_UP_PIN     18
#define BTN_DOWN_PIN   19

#define POLL_MS        10
#define DEBOUNCE_MS    30

#define UART_PORT      UART_NUM_0
#define UART_TX_PIN    1
#define UART_RX_PIN    3
#define UART_BAUDRATE  115200

static inline void delay_ms(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

// =====================
// Inicialización UART0
// =====================
void uart_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_PORT, 1024, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void app_main(void) {

    esp_task_wdt_deinit();

    uart_init();

    printf("\n=============================\n");
    printf(" Sistema iniciado\n");
    printf("=============================\n");

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

                printf("UP presionado -> contador = %d\n", contador);

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

                printf("DOWN presionado -> contador = %d\n", contador);

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
