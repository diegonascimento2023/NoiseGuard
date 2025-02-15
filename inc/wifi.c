#include "inc/wifi.h"

// Função para inicializar e conectar ao Wi-Fi
void wifi_init()
{
    // Tenta inicializar a arquitetura Wi-Fi do dispositivo
    if (cyw43_arch_init())
    {
        // Se falhar na inicialização, imprime uma mensagem de erro
        printf("Falha ao inicializar Wi-Fi!\n");
        return; // Retorna para a função chamadora sem tentar se conectar
    }

    // Habilita o modo STA (Station), o que significa que o dispositivo vai se conectar a um ponto de acesso Wi-Fi
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    // Tenta se conectar ao ponto de acesso Wi-Fi com o SSID e a senha fornecidos, e um tempo limite de 10 segundos
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        // Se a conexão falhar, imprime uma mensagem de erro
        printf("Conexão Wi-Fi falhou!\n");
    }
    else
    {
        // Se a conexão for bem-sucedida, imprime uma mensagem de sucesso
        printf("Conectado ao Wi-Fi!\n");
    }
}
