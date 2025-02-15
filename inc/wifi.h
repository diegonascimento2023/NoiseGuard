#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"


// Defina o nome e senha do Wi-Fi
#define WIFI_SSID "Temperatura" // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "ufersa1234"  // Substitua pela senha da sua rede Wi-Fi

// Função para conectar ao Wi-Fi
void wifi_init();

#endif
