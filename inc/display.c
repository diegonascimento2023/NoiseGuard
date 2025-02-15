#include "inc/display.h"

// Definição dos pinos SDA e SCL para comunicação I2C
const uint I2C_SDA = 14; // Pino SDA (dados)
const uint I2C_SCL = 15; // Pino SCL (relógio)

// Estrutura que define a área de renderização no display
struct render_area frame_area;

// Buffer para armazenar os dados do display SSD1306
uint8_t ssd[ssd1306_buffer_length];

// Função para renderizar a primeira mensagem no display
void renderMessage1(char *line1, char *line2)
{
    // Limpa o buffer do display (reseta todos os pixels para 0)
    memset(ssd, 0, ssd1306_buffer_length);

    // Define o tamanho da fonte (1.5x o tamanho padrão)
    float font_size = 1.5;

    // Desenha as duas linhas no display com o tamanho de fonte especificado
    int y = 15; // Posição Y para a primeira linha
    ssd1306_draw_string_with_size(ssd, 18, y, line1, font_size);
    y += 25; // Ajusta a posição Y para a segunda linha
    ssd1306_draw_string_with_size(ssd, 5, y, line2, font_size);

    // Atualiza o display para exibir as mensagens desenhadas
    render_on_display(ssd, &frame_area);
}

// Função para renderizar a segunda mensagem no display
void renderMessage2(char *line1, char *line2, char *line3)
{
    // Limpa o buffer do display
    memset(ssd, 0, ssd1306_buffer_length);

    // Define o tamanho da fonte (1.5x o tamanho padrão)
    float font_size = 1.5;

    // Desenha as três linhas no display
    int y = 10; // Posição Y para a primeira linha
    ssd1306_draw_string_with_size(ssd, 16, y, line1, font_size);
    y += 18; // Ajusta a posição Y para a segunda linha
    ssd1306_draw_string_with_size(ssd, 35, y, line2, font_size);
    y += 18; // Ajusta a posição Y para a terceira linha
    ssd1306_draw_string_with_size(ssd, 18, y, line3, font_size);

    // Atualiza o display para exibir as mensagens desenhadas
    render_on_display(ssd, &frame_area);
}

// Função que exibe a mensagem "AMBIENTE SILENCIOSO" no display
void ambiente_silencioso()
{
    // Chama a função renderMessage1 para exibir a mensagem no display
    renderMessage1("AMBIENTE", "SILENCIOSO");
}

// Função que exibe a mensagem "POR FAVOR FAZER SILENCIO" no display
void pedindo_silencio()
{
    // Chama a função renderMessage2 para exibir a mensagem no display
    renderMessage2("POR FAVOR", "FAZER", "SILENCIO");
}

// Função para inicializar o display e configurar o I2C
void init_display()
{
    // Inicializa a comunicação I2C com a frequência definida
    i2c_init(i2c1, 400000); // 400kHz de frequência no barramento I2C

    // Configura os pinos SDA e SCL para a função I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Ativa o pull-up para os pinos SDA e SCL
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display SSD1306
    ssd1306_init();

    // Configura a área de renderização do display
    frame_area.start_column = 0;               // Início da coluna (X)
    frame_area.end_column = ssd1306_width - 1; // Fim da coluna (X)
    frame_area.start_page = 0;                 // Início da página (Y)
    frame_area.end_page = ssd1306_n_pages - 1; // Fim da página (Y)

    // Calcula o comprimento do buffer de renderização com base na área definida
    calculate_render_area_buffer_length(&frame_area);
}

// Função para limpar o display
void zera_display()
{
    // Limpa o buffer do display
    memset(ssd, 0, ssd1306_buffer_length);

    // Atualiza o display para mostrar a tela limpa
    render_on_display(ssd, &frame_area);
}
