#ifndef ADC_DMA_H
#define ADC_DMA_H

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

// Definições do ADC e Microfone
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)
#define SAMPLES 1000  // Número de amostras do ADC 
#define ADC_MAX 4095 // Valor máximo do ADC (12 bits)
#define REF_VOLTAGE 3.3f // Tensão de referência
#define OFFSET_VOLTAGE 1.65f // Offset do microfone

extern uint dma_channel;
extern dma_channel_config dma_cfg;
extern uint16_t adc_buffer[SAMPLES];


// Funções para captura de áudio e conversão
void sample_mic();
float mic_power_rms();
float rms_to_db(float rms);

#endif
