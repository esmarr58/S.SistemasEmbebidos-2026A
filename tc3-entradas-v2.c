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
    
    // Contadores para debounce
    int up_debounce_counter = 0;
    int down_debounce_counter = 0;
    
    // Estados de botones (1 = presionado confirmado, 0 = no presionado)
    int up_confirmed = 0;
    int down_confirmed = 0;

    while (true) {
        int up_now = gpio_get_level(BTN_UP_PIN);
        int down_now = gpio_get_level(BTN_DOWN_PIN);

        // =========================
        // UP: Lógica de debounce
        // =========================
        if (up_now != up_prev) {
            // Cambio detectado, reiniciar contador debounce
            up_debounce_counter = 0;
        } else {
            // Mismo estado, incrementar contador
            up_debounce_counter++;
        }
        
        // Si el estado ha sido estable por DEBOUNCE_MS/POLL_MS ciclos
        if (up_debounce_counter >= (DEBOUNCE_MS / POLL_MS)) {
            // Estado estable confirmado
            if (up_now == 1 && up_confirmed == 0) {
                // Flanco ascendente confirmado
                contador++;
                printf("Contador: %d\n", contador);
                
                // Feedback LED
                gpio_set_level(LED_PIN, 1);
                delay_ms(30);
                gpio_set_level(LED_PIN, 0);
                
                up_confirmed = 1; // Marcar como presionado confirmado
            }
            else if (up_now == 0) {
                up_confirmed = 0; // Botón liberado
            }
        }

        // =========================
        // DOWN: Lógica de debounce
        // =========================
        if (down_now != down_prev) {
            down_debounce_counter = 0;
        } else {
            down_debounce_counter++;
        }
        
        if (down_debounce_counter >= (DEBOUNCE_MS / POLL_MS)) {
            if (down_now == 1 && down_confirmed == 0) {
                // Flanco ascendente confirmado
                contador--;
                printf("Contador: %d\n", contador);
                
                // Feedback LED
                gpio_set_level(LED_PIN, 1);
                delay_ms(30);
                gpio_set_level(LED_PIN, 0);
                
                down_confirmed = 1; // Marcar como presionado confirmado
            }
            else if (down_now == 0) {
                down_confirmed = 0; // Botón liberado
            }
        }

        // actualizar estados previos
        up_prev = up_now;
        down_prev = down_now;

        delay_ms(POLL_MS);
    }
}
