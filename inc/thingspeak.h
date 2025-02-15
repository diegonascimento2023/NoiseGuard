#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

// Config para o ThingSpeak
#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_PORT 80
#define API_KEY "TBG4DXDHKN58KQ06" // API Key do ThingSpeak

#define NUM_LEITURAS 15

// Global
static struct tcp_pcb *tcp_client_pcb;
extern char request[]; // Buffer para o HTTP GET

extern float valor_ruido;


// Função para enviar dados ao ThingSpeak
void enviar_para_thingspeak(float dB);

#endif
