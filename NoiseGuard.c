/*AVISOS PARA A EXECUÇÃO DO PROJETO
- Substituir o usuário e senha no arquivo wifi.h que está dentro da pas inc
- Ao rodar o codigo lembrar de abrir o serial
- O link para ver os dados no ThingSpeak é: https://thingspeak.mathworks.com/channels/2840125
- O link para ver a página web é: https://diegonascimento2023.github.io/NoiseGuard/ */ 

#include "inc/adc_dma.h"    // Inclusão de cabeçalhos para configuração do ADC com DMA
#include "inc/buzzer_led.c" // Inclusão de funções de controle de LED e buzzer
#include "inc/display.h"    // Inclusão de funções para controle do display OLED
#include "inc/wifi.h"       // Inclusão de funções para conectar ao Wi-Fi
#include "inc/thingspeak.h" // Inclusão de funções para enviar dados ao ThingSpeak

#define NUM_LEITURAS 15       // Definindo o número de leituras que serão feitas antes de enviar a média para o ThingSpeak
float leituras[NUM_LEITURAS]; // Array para armazenar as leituras de ruído
int contagem = 0;             // Variável para contar o número de leituras realizadas

int main()
{
    stdio_init_all(); // Inicializa todas as funções básicas de entrada e saída, incluindo USB e UART

    // Espera até que a conexão serial seja estabelecida
    // A PLACA IRÁ INCIAR APENAS QUANDO O SERIAL FOR CONECTADO
    printf("Aguardando conexão serial...\n");
    while (!stdio_usb_connected())
    {
        sleep_ms(100); // Aguarda 100ms antes de tentar novamente
    }
    printf("Serial conectado! Iniciando servidor HTTP\n");

    // Inicializa a conexão Wi-Fi
    wifi_init();

    // Inicializa os LEDs e o buzzer
    npInit(LED_PIN); // Inicializa o controle dos LEDs
    npClear();       // Limpa todos os LEDs (desliga todos)

    pwm_init_buzzer(BUZZER_PIN); // Inicializa o buzzer com controle PWM

    // Inicializa o display OLED
    init_display();

    // Configura o ADC para ler os dados do microfone (entrada de áudio)
    adc_gpio_init(MIC_PIN);        // Inicializa o pino do microfone
    adc_init();                    // Inicializa o ADC
    adc_select_input(MIC_CHANNEL); // Seleciona o canal do ADC para o microfone

    // Configura a fila do ADC para capturar dados com DMA
    adc_fifo_setup(true, true, 1, false, false);
    adc_set_clkdiv(96.0f); // Define a divisão de clock para o ADC (ajustando a velocidade de amostragem)

    // Configuração do DMA para transferência dos dados do ADC
    dma_channel = dma_claim_unused_channel(true);                 // Solicita um canal de DMA disponível
    dma_cfg = dma_channel_get_default_config(dma_channel);        // Obtém a configuração padrão do DMA
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Define o tamanho da transferência como 16 bits
    channel_config_set_read_increment(&dma_cfg, false);           // Não incrementa o ponteiro de leitura no buffer
    channel_config_set_write_increment(&dma_cfg, true);           // Incrementa o ponteiro de escrita no buffer
    channel_config_set_dreq(&dma_cfg, DREQ_ADC);                  // Configura a requisição do ADC como fonte de dados para o DMA

    printf("Sistema de Monitoramento de Ruído Iniciado!\n");

    while (true)
    {                                // Loop principal do sistema
        sample_mic();                // Captura os dados do microfone
        float rms = mic_power_rms(); // Calcula a potência RMS do sinal de áudio
        float dB = rms_to_db(rms);   // Converte a potência RMS para decibéis (dB)

        // Armazena a leitura de dB na matriz de leituras
        leituras[contagem] = dB;
        contagem++; // Incrementa o contador de leituras

        // Classifica o nível de ruído com base nos valores de dB e exibe o resultado
        if (dB <= 50)
        {
            printf("Ruído: %.2f dB - Ambiente Silencioso\n", dB);
        }
        else if (dB > 50 && dB < 70)
        {
            printf("Ruído: %.2f dB - Ruído Moderado\n", dB);
        }
        else
        {
            printf("Ruído: %.2f dB - ALTO! ⚠️\n", dB);
        }

        // Quando atingir o número de leituras especificado (15), calcula a média e envia ao ThingSpeak
        if (contagem >= NUM_LEITURAS)
        {
            float soma = 0.0;
            // Soma os valores das leituras armazenadas
            for (int i = 0; i < NUM_LEITURAS; i++)
            {
                soma += leituras[i];
            }
            // Calcula a média das leituras
            float media_dB = soma / NUM_LEITURAS;

            // Envia a média para o ThingSpeak
            enviar_para_thingspeak(media_dB);
            printf("Enviado para ThingSpeak: %.2f dB\n", media_dB);

            // Atualiza o display, LEDs e buzzer com base na média de ruído calculada
            if (media_dB <= 52)
            {
                zera_display();         // Limpa o display
                ambiente_silencioso();  // Exibe mensagem de "Ambiente Silencioso" no display
                led_green();            // Acende o LED verde
                buzzer_off(BUZZER_PIN); // Desliga o buzzer
                npClear();              // Limpa os LEDs de NeoPixel
            }
            else if (media_dB > 52 && media_dB < 70)
            {
                zera_display();                          // Limpa o display
                led_yellow();                            // Acende o LED amarelo
                pedindo_silencio();                      // Exibe mensagem de "Fazer Silêncio" no display
                iniciar_buzzer(BUZZER_PIN, 1000, false); // Liga o buzzer por 1 segundo
                npClear();                               // Limpa os LEDs de NeoPixel
            }
            else
            {
                zera_display();                          // Limpa o display
                led_red();                               // Acende o LED vermelho
                pedindo_silencio();                      // Exibe mensagem de "Fazer Silêncio" no display
                iniciar_buzzer(BUZZER_PIN, 13000, true); // Liga o buzzer intermitente por 13 segundos
                npClear();                               // Limpa os LEDs de NeoPixel
            }

            contagem = 0; // Reseta a contagem de leituras
        }

        // Atualiza o buzzer, ajustando o estado conforme a necessidade
        atualizar_buzzer(BUZZER_PIN);

        sleep_ms(1000); // Aguarda 1 segundo antes de medir novamente
    }
}
