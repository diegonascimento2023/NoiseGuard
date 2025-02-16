#include "inc/buzzer_led.h"


// Definição do pino do LED e frequência do buzzer.
#define LED_PIN 7
#define BUZZER_FREQUENCY 100 // Frequência do buzzer em Hz.

void npInit(uint pin)
{
    // Cria e inicializa o programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    // Toma posse de uma máquina PIO (máquina de I/O).
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0)
    {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true); // Se não houver máquinas livres, usa a segunda.
    }

    // Inicia o programa na máquina PIO e define o pino de controle de LEDs.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    // Limpa o buffer de LEDs.
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
    // Define as cores RGB para o LED específico.
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void npClear()
{
    // Limpa todos os LEDs (seta cores para 0).
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

void npWrite()
{
    // Escreve os valores de cor de cada LED no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100 microssegundos para finalizar o sinal (RESET do datasheet).
}

// Função para calcular o índice do LED na matriz 5x5 considerando o padrão zig-zag.
int getIndex(int x, int y)
{
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0)
    {
        return 24 - (y * 5 + x); // Linha par (esquerda para direita).
    }
    else
    {
        return 24 - (y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

// Função para configurar a cor de todos os LEDs na matriz.
void led_set_color(int matriz[5][5][3], uint8_t r, uint8_t g, uint8_t b) {
    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, r, g, b); // Define a cor no índice calculado.
        }
    }
    npWrite(); // Atualiza a matriz de LEDs.
}

// Função para acender o LED vermelho.
void led_red() {
    npClear(); // Limpa os LEDs.
    int matriz[5][5][3] = {}; // Matriz de LEDs.
    led_set_color(matriz, 100, 0, 0); // Define a cor vermelha.
}

// Função para acender o LED verde.
void led_green() {
    npClear(); // Limpa os LEDs.
    int matriz[5][5][3] = {}; // Matriz de LEDs.
    led_set_color(matriz, 0, 80, 0); // Define a cor verde.
}

// Função para acender o LED amarelo.
void led_yellow() {
    int matriz[5][5][3] = {}; // Matriz de LEDs.
    led_set_color(matriz, 80, 80, 0); // Define a cor amarela.
}

// Inicializa o PWM no pino do buzzer.
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura o pino como saída PWM.
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o slice do PWM associado ao pino.
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Define o divisor de clock.
    pwm_init(slice_num, &config, true); // Inicializa o PWM.
    pwm_set_gpio_level(pin, 0); // Inicia o PWM com nível baixo.
}

// Desliga o buzzer.
void buzzer_off(uint pin) {
    pwm_set_gpio_level(pin, 0); // Desativa o sinal PWM (duty cycle 0).
}


// Função para iniciar o buzzer com a duração e o tipo de alternância (intermitente ou contínuo).
void iniciar_buzzer(uint pin, uint32_t duracao, bool intermitente) {
    buzzer.ativo = true; // Ativa o buzzer.
    buzzer.tempo_inicio = time_us_64(); // Marca o tempo de início em microssegundos.
    buzzer.duracao_ms = duracao; // Define a duração do buzzer.
    buzzer.intermitente = intermitente; // Define se o buzzer será intermitente.
    buzzer.alternancias = 0; // Inicializa as alternâncias.

    pwm_set_gpio_level(pin, 2048); // Liga o buzzer inicialmente.
}

// Atualiza o estado do buzzer, verificando se ele deve continuar ativado.
void atualizar_buzzer(uint pin) {
    if (!buzzer.ativo) return; // Se o buzzer não estiver ativo, não faz nada.

    uint64_t tempo_atual = time_us_64(); // Obtém o tempo atual em microssegundos.

    // Se o buzzer for intermitente, alterna seu estado a cada 500ms (500000 microssegundos).
    if (buzzer.intermitente) {
        if ((tempo_atual - buzzer.tempo_inicio) >= 500000) {
            buzzer.tempo_inicio = tempo_atual;
            buzzer.alternancias++; // Incrementa a alternância.

            if (buzzer.alternancias % 2 == 0) {
                pwm_set_gpio_level(pin, 2048); // Liga o buzzer.
            } else {
                pwm_set_gpio_level(pin, 0); // Desliga o buzzer.
            }
        }

        // Se o buzzer alcançou a duração, desliga-o.
        if ((buzzer.alternancias * 500) >= buzzer.duracao_ms) {
            buzzer.ativo = false;
            pwm_set_gpio_level(pin, 0);
        }
    } else {
        // Se não for intermitente, desliga após a duração.
        if ((tempo_atual - buzzer.tempo_inicio) >= (buzzer.duracao_ms * 1000)) {
            buzzer.ativo = false;
            pwm_set_gpio_level(pin, 0);
        }
    }
}
