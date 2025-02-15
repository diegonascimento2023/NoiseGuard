#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "inc/ssd1306.h"
#include "hardware/i2c.h"


extern const uint I2C_SDA;
extern const uint I2C_SCL;

extern struct render_area frame_area;
extern uint8_t ssd[ssd1306_buffer_length];

// Função para renderizar as mensagens no display
void renderMessage1(char *line1, char *line2);

// Função para renderizar as mensagens no display
void renderMessage2(char *line1, char *line2, char *line3);

// Função para mostrar mensagem de "Capitando som"
void ambiente_silencioso();

// Função para mostrar mensagem de "Pedindo silencio"
void pedindo_silencio();

// Função de inicialização do display
void init_display();

// Função para zerar o display
void zera_display();
#endif
