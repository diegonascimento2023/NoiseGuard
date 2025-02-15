/* adc_dma.c - Captura de áudio usando ADC e DMA */
#include "inc/adc_dma.h"

// Configuração do DMA
uint dma_channel;
dma_channel_config dma_cfg;
uint16_t adc_buffer[SAMPLES];

// Captura os dados do microfone utilizando DMA
void sample_mic()
{
    adc_fifo_drain(); // Esvazia FIFO do ADC antes de iniciar
    adc_run(false);   // Para o ADC temporariamente

    dma_channel_configure(dma_channel, &dma_cfg,
                          adc_buffer,      // Buffer onde os dados serão armazenados
                          &(adc_hw->fifo), // Origem: FIFO do ADC
                          SAMPLES,         // Quantidade de amostras
                          true             // Inicia a captura automaticamente
    );

    adc_run(true);                                     // Inicia a captura de dados pelo ADC
    dma_channel_wait_for_finish_blocking(dma_channel); // Aguarda a finalização do DMA
    adc_run(false);                                    // Para o ADC
}

// Aplica um filtro média móvel para suavizar os dados do microfone
void apply_moving_average_filter(uint16_t *data, uint16_t *filtered_data, uint size, uint window_size)
{
    for (uint i = 0; i < size; i++)
    {
        float sum = 0;
        uint count = 0;
        for (int j = i - window_size / 2; j <= (int)(i + window_size / 2); j++)
        {
            if (j >= 0 && j < size)
            {
                sum += data[j];
                count++;
            }
        }
        filtered_data[i] = sum / count; // Média dos valores na janela
    }
}

// Calcula a potência média (RMS) do som capturado
float mic_power_rms()
{
    uint16_t filtered_buffer[SAMPLES];
    apply_moving_average_filter(adc_buffer, filtered_buffer, SAMPLES, 10); // Aplica filtro com janela de 10

    float sum = 0.0f;
    for (uint i = 0; i < SAMPLES; i++)
    {
        float voltage = (filtered_buffer[i] * REF_VOLTAGE / ADC_MAX) - OFFSET_VOLTAGE; // Converte ADC para Volts
        sum += voltage * voltage;
    }

    return sqrt(sum / SAMPLES); // Retorna o valor RMS
}

// Converte RMS para decibéis (dB)
float rms_to_db(float rms)
{
    if (rms <= 0)
        return 0;                     // Evita erro log(0)
    return 20 * log10(rms / 0.00002); // Conversão para dB (nível SPL)
}
