#include "inc/thingspeak.h"

// Variável global que armazena o valor de ruído a ser enviado
float valor_ruido = 0.0;

// Buffer para armazenar a requisição HTTP GET
char request[256];

// Callback quando dados são recebidos via TCP
err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    // Se não houver dados recebidos (p == NULL), fecha a conexão TCP
    if (p == NULL)
    {
        tcp_close(tpcb); // Fecha a conexão
        return ERR_OK;
    }

    // Libera o buffer de dados recebidos
    pbuf_free(p);
    return ERR_OK;
}

// Callback quando a conexão TCP é estabelecida com sucesso
err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    // Se houve algum erro ao estabelecer a conexão, retorna o erro
    if (err != ERR_OK)
    {
        return err;
    }

    // Cria a requisição HTTP GET para enviar ao ThingSpeak
    // A URL é construída com a chave API e o valor de ruído (valor_ruido)
    snprintf(request, sizeof(request),
             "GET /update?api_key=%s&field1=%.2f HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             API_KEY, valor_ruido, THINGSPEAK_HOST);

    // Envia a requisição HTTP ao servidor ThingSpeak via TCP
    tcp_write(tpcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb); // Garantir que os dados são enviados pela rede

    // Define o callback para receber dados da conexão
    tcp_recv(tpcb, tcp_recv_callback);
    return ERR_OK;
}

// Callback de resolução de DNS (para obter o IP do servidor ThingSpeak)
static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    // Se o DNS falhou (ipaddr == NULL), retorna sem fazer nada
    if (ipaddr == NULL)
    {
        return;
    }

    // Estabelece a conexão TCP com o servidor ThingSpeak, usando o IP obtido via DNS
    tcp_connect(tcp_client_pcb, ipaddr, THINGSPEAK_PORT, tcp_connect_callback);
}

// Função para enviar o valor de ruído ao ThingSpeak
void enviar_para_thingspeak(float dB)
{
    // Cria um novo controle de PCB TCP
    tcp_client_pcb = tcp_new();
    if (!tcp_client_pcb)
    {
        return; // Se não conseguir criar o PCB, retorna sem fazer nada
    }

    // Atualiza a variável valor_ruido com o valor recebido como parâmetro
    valor_ruido = dB;

    ip_addr_t server_ip;
    // Realiza a resolução de DNS para obter o IP do servidor ThingSpeak
    err_t err = dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_callback, NULL);

    // Se a resolução de DNS foi bem-sucedida, tenta se conectar ao servidor
    if (err == ERR_OK)
    {
        tcp_connect(tcp_client_pcb, &server_ip, THINGSPEAK_PORT, tcp_connect_callback);
    }
    else if (err != ERR_INPROGRESS)
    {
        // Se houver erro ou resolução não estiver em andamento, fecha a conexão TCP
        tcp_close(tcp_client_pcb);
    }
}
