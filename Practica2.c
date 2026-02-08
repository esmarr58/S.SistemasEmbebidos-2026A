#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"

// =============================
// Pines botones
// =============================
#define BTN_UP_PIN     18
#define BTN_DOWN_PIN   19

// =============================
// Pines display 7 segmentos
// =============================
#define SEG_A    2
#define SEG_B    3
#define SEG_C    4
#define SEG_D    5
#define SEG_E    6
#define SEG_F    7
#define SEG_G    8

#define POLL_MS        10
#define DEBOUNCE_MS    30

// =======================================================
// TABLA CÁTODO COMÚN (1 = encendido)
// =======================================================
const uint8_t digitos[10] = {
    0x3F, // 0: A B C D E F
    0x06, // 1: B C
    0x5B, // 2: A B D E G
    0x4F, // 3: A B C D G
    0x66, // 4: B C F G
    0x6D, // 5: A C D F G
    0x7D, // 6: A C D E F G
    0x07, // 7: A B C
    0x7F, // 8: A B C D E F G
    0x6F  // 9: A B C D F G
};

void delay_ms(int ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

// =============================
// Inicializar display
// =============================
void inicializar_display() {
    int segmentos[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

    for (int i = 0; i < 7; i++) {
        gpio_reset_pin(segmentos[i]);
        gpio_set_direction(segmentos[i], GPIO_MODE_OUTPUT);
        gpio_set_level(segmentos[i], 0);
    }
}

// =============================
// Mostrar dígito
// =============================
void mostrar_digito(uint8_t numero) {
    if (numero > 9) return;

    uint8_t m = digitos[numero];

    gpio_set_level(SEG_A, (m >> 0) & 1);
    gpio_set_level(SEG_B, (m >> 1) & 1);
    gpio_set_level(SEG_C, (m >> 2) & 1);
    gpio_set_level(SEG_D, (m >> 3) & 1);
    gpio_set_level(SEG_E, (m >> 4) & 1);
    gpio_set_level(SEG_F, (m >> 5) & 1);
    gpio_set_level(SEG_G, (m >> 6) & 1);
}

void app_main(void) {
    esp_task_wdt_deinit();

    printf("\n===== CONTADOR MANUAL (POLLING) =====\n");

    // Inicializar display
    inicializar_display();

    // Inicializar botones
    gpio_reset_pin(BTN_UP_PIN);
    gpio_set_direction(BTN_UP_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_UP_PIN, GPIO_PULLDOWN_ONLY);

    gpio_reset_pin(BTN_DOWN_PIN);
    gpio_set_direction(BTN_DOWN_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_DOWN_PIN, GPIO_PULLDOWN_ONLY);

    uint8_t contador = 0;
    mostrar_digito(contador);
    printf("Contador inicial: %d\n", contador);

    int up_prev = 0;
    int down_prev = 0;
    
    // Variables para debounce
    int up_debounce_counter = 0;
    int down_debounce_counter = 0;
    
    // Estados para evitar repetición
    int up_pressed = 0;   // 0 = no presionado, 1 = presionado
    int down_pressed = 0;

    while (true) {
        int up_now = gpio_get_level(BTN_UP_PIN);
        int down_now = gpio_get_level(BTN_DOWN_PIN);

        // =====================
        // Botón UP - Debounce y detección
        // =====================
        if (up_now != up_prev) {
            up_debounce_counter = 0;  // Cambio detectado
        } else {
            up_debounce_counter++;    // Mismo estado
        }
        
        // Si el estado es estable por tiempo suficiente
        if (up_debounce_counter >= (DEBOUNCE_MS / POLL_MS)) {
            if (up_now == 1 && up_pressed == 0) {
                // Botón presionado (flanco ascendente confirmado)
                contador++;
                if (contador > 9) contador = 0;
                
                mostrar_digito(contador);
                printf("UP → Contador = %d\n", contador);
                
                up_pressed = 1;  // Marcar como presionado
            } 
            else if (up_now == 0) {
                up_pressed = 0;  // Botón liberado
            }
        }

        // =====================
        // Botón DOWN - Debounce y detección
        // =====================
        if (down_now != down_prev) {
            down_debounce_counter = 0;
        } else {
            down_debounce_counter++;
        }
        
        if (down_debounce_counter >= (DEBOUNCE_MS / POLL_MS)) {
            if (down_now == 1 && down_pressed == 0) {
                // Botón presionado (flanco ascendente confirmado)
                if (contador == 0) {
                    contador = 9;
                } else {
                    contador--;
                }
                
                mostrar_digito(contador);
                printf("DOWN → Contador = %d\n", contador);
                
                down_pressed = 1;  // Marcar como presionado
            } 
            else if (down_now == 0) {
                down_pressed = 0;  // Botón liberado
            }
        }

        // Guardar estados para la próxima iteración
        up_prev = up_now;
        down_prev = down_now;

        delay_ms(POLL_MS);
    }
}
