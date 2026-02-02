#include <unistd.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"

// Definición de pines para cada segmento (AJUSTA SEGÚN TU CONEXIÓN)
#define SEG_A    2
#define SEG_B    3
#define SEG_C    4
#define SEG_D    5
#define SEG_E    6
#define SEG_F    7
#define SEG_G    8

// =======================================================
// DISPLAY DE 7 SEGMENTOS — TABLAS DE DECODIFICACIÓN
// Convención de bits:
// bit0 = a, bit1 = b, bit2 = c, bit3 = d,
// bit4 = e, bit5 = f, bit6 = g
// =======================================================


/* ======================================================
 * TABLA PARA DISPLAY DE CÁTODO COMÚN (CC)
 * - El pin común va a GND
 * - 1 = segmento ENCENDIDO
 * ====================================================== */
const uint8_t digitos_cc[10] = {
    0x3F, // 0: 00111111 → a b c d e f
    0x06, // 1: 00000110 → b c
    0x5B, // 2: 01011011 → a b d e g
    0x4F, // 3: 01001111 → a b c d g
    0x66, // 4: 01100110 → b c f g
    0x6D, // 5: 01101101 → a c d f g
    0x7D, // 6: 01111101 → a c d e f g
    0x07, // 7: 00000111 → a b c
    0x7F, // 8: 01111111 → a b c d e f g
    0x6F  // 9: 01101111 → a b c d f g
};


/* ======================================================
 * TABLA PARA DISPLAY DE ÁNODO COMÚN (CA)
 * - El pin común va a VCC
 * - 0 = segmento ENCENDIDO (lógica invertida)
 * ====================================================== */
const uint8_t digitos_ca[10] = {
    0x40, // 0: 1000000 → a b c d e f
    0x79, // 1: 1111001 → b c
    0x24, // 2: 0100100 → a b d e g
    0x30, // 3: 0110000 → a b c d g
    0x19, // 4: 0011001 → b c f g
    0x12, // 5: 0010010 → a c d f g
    0x02, // 6: 0000010 → a c d e f g
    0x78, // 7: 1111000 → a b c
    0x00, // 8: 0000000 → a b c d e f g
    0x10  // 9: 0010000 → a b c d f g
};


/* ======================================================
 * 👉 INSTRUCCIÓN PARA EL ALUMNO
 * ======================================================
 * SOLO CAMBIA ESTA LÍNEA SEGÚN TU DISPLAY
 * NO CAMBIES EL RESTO DEL PROGRAMA
 */

// Para display de CÁTODO COMÚN:
// const uint8_t *digitos = digitos_cc;

// Para display de ÁNODO COMÚN:
const uint8_t *digitos = digitos_ca;

// Función para inicializar todos los pines del display
void inicializar_display() {
    // Inicializar cada pin individualmente
    gpio_reset_pin(SEG_A);
    gpio_set_direction(SEG_A, GPIO_MODE_OUTPUT);
    
    gpio_reset_pin(SEG_B);
    gpio_set_direction(SEG_B, GPIO_MODE_OUTPUT);
    
    gpio_reset_pin(SEG_C);
    gpio_set_direction(SEG_C, GPIO_MODE_OUTPUT);
    
    gpio_reset_pin(SEG_D);
    gpio_set_direction(SEG_D, GPIO_MODE_OUTPUT);
    
    gpio_reset_pin(SEG_E);
    gpio_set_direction(SEG_E, GPIO_MODE_OUTPUT);
    
    gpio_reset_pin(SEG_F);
    gpio_set_direction(SEG_F, GPIO_MODE_OUTPUT);
    
    gpio_reset_pin(SEG_G);
    gpio_set_direction(SEG_G, GPIO_MODE_OUTPUT);
}

// Función para mostrar un dígito en el display
void mostrar_digito(uint8_t numero) {
    if (numero > 9) return; // Solo dígitos 0-9
    
    uint8_t mascara = digitos[numero];
    
    // Controlar cada segmento según la máscara
    // bit0 = segmento A, bit1 = segmento B, ..., bit6 = segmento G
    gpio_set_level(SEG_A, (mascara >> 0) & 0x01);
    gpio_set_level(SEG_B, (mascara >> 1) & 0x01);
    gpio_set_level(SEG_C, (mascara >> 2) & 0x01);
    gpio_set_level(SEG_D, (mascara >> 3) & 0x01);
    gpio_set_level(SEG_E, (mascara >> 4) & 0x01);
    gpio_set_level(SEG_F, (mascara >> 5) & 0x01);
    gpio_set_level(SEG_G, (mascara >> 6) & 0x01);
}

// Función para apagar todos los segmentos
void apagar_display() {
    gpio_set_level(SEG_A, 0);
    gpio_set_level(SEG_B, 0);
    gpio_set_level(SEG_C, 0);
    gpio_set_level(SEG_D, 0);
    gpio_set_level(SEG_E, 0);
    gpio_set_level(SEG_F, 0);
    gpio_set_level(SEG_G, 0);
}

void app_main(void) {
      esp_task_wdt_deinit();

    // Inicializar el display
    inicializar_display();
    
    // Contador para el ejemplo
    uint8_t contador = 0;
    
    while (true) {
        // Mostrar el dígito actual
        mostrar_digito(contador);
        
        // Esperar 1 segundo
        usleep(1000000);
        
        // Incrementar contador (0-9)
        contador++;
        if (contador > 9) {
            contador = 0;
        }
    }
}
