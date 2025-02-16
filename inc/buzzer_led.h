#ifndef BUZZER_LED_RGB_H
#define BUZZER_LED_RGB_H

#include <stdint.h>
#include "hardware/pio.h"
#include "inc/display.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

#define LED_COUNT 25

// Configuração do pino do buzzer
#define BUZZER_PIN 21

typedef struct {
    bool ativo;
    uint32_t tempo_inicio;
    uint32_t duracao_ms;
    bool intermitente;
    int alternancias;
} BuzzerState;

BuzzerState buzzer = {false, 0, 0, false, 0};

// Definição de pixel GRB
struct pixel_t
{
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

//Inicializa a máquina PIO para controle da matriz de LEDs.
void npInit(uint pin);

//Atribui uma cor RGB a um LED.
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b);

//Limpa o buffer de pixels.
void npClear();

//Escreve os dados do buffer nos LEDs.
void npWrite();

// Função para converter a posição do matriz para uma posição do vetor.
int getIndex(int x, int y);

// Função genérica para configurar as cores.
void led_set_color(int matriz[5][5][3], uint8_t r, uint8_t g, uint8_t b);

// Função para cor Vermelha
void led_red();

// Função para cor Verde
void led_green();

// Função para cor Amarela (Vermelho + Verde)
void led_yellow();

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin);

// Função para desligar o buzzer
void buzzer_off(uint pin);

// Função para ligar o buzzer por 3 segundos
void buzzer_on_for_3_seconds(uint pin);

// Função para ligar e desligar o buzzer intermitentemente
void buzzer_toggle_intermittent(uint pin);

// Função para iniciar o buzzer com a duração e o tipo de alternância (intermitente ou contínuo).
void iniciar_buzzer(uint pin, uint32_t duracao, bool intermitente);

// Atualiza o estado do buzzer, verificando se ele deve continuar ativado.
void atualizar_buzzer(uint pin);


#endif
