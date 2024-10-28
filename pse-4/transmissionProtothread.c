#include <stdio.h>
#include <time.h>
#include "pt-1.4/pt.h"

#define TIMEOUT 5    // Tempo máximo de espera em segundos
#define DATA_SIZE 10 // Tamanho dos dados a serem enviados

// Definição da macro PT_SLEEP
#define PT_SLEEP(pt, seconds)                                     \
    do {                                                          \
        static time_t sleep_start;                                \
        sleep_start = time(NULL);                                 \
        PT_WAIT_UNTIL(pt, time(NULL) - sleep_start >= (seconds)); \
    } while (0)

static struct pt pt_sender, pt_receiver;
static int data_buffer[DATA_SIZE];
static int ack_flag = 0;

// Protothread Sender
static PT_THREAD(sender(struct pt *pt))
{
    static int index;
    static int timeout_counter;

    PT_BEGIN(pt);

    // Preparar dados para envio
    for (index = 0; index < DATA_SIZE; index++)
    {
        data_buffer[index] = index;
    }

    printf("Transmissor: Enviando dados...\n");

    // Enviar dados para o receptor (simulação)
    ack_flag = 0; // Resetar o ACK

    // Esperar pelo ACK ou timeout
    timeout_counter = 0;
    while (ack_flag == 0 && timeout_counter < TIMEOUT)
    {
        PT_SLEEP(pt, 1); // Espera de 1 segundo
        timeout_counter++;
    }

    if (ack_flag)
    {
        printf("Transmissor: ACK recebido.\n");
    }
    else
    {
        printf("Transmissor: Timeout. Reenviando dados...\n");
        // Reenviar dados
    }

    PT_END(pt);
}

// Protothread Receiver
static PT_THREAD(receiver(struct pt *pt))
{
    static int index;
    static int received_data[DATA_SIZE];
    static int data_valid = 1;

    PT_BEGIN(pt);

    // Simular recepção de dados
    PT_SLEEP(pt, 2); // Simula o tempo de chegada dos dados

    // Receber dados do transmissor
    for (index = 0; index < DATA_SIZE; index++)
    {
        received_data[index] = data_buffer[index];
    }

    // Verificar se os dados estão corretos
    data_valid = 1;
    for (index = 0; index < DATA_SIZE; index++)
    {
        if (received_data[index] != index)
        {
            data_valid = 0;
            break;
        }
    }

    if (data_valid)
    {
        printf("Receptor: Dados corretos. Enviando ACK...\n");
        ack_flag = 1; // Envia ACK
    }
    else
    {
        printf("Receptor: Dados incorretos. Aguardando novos dados...\n");
    }

    PT_END(pt);
}

// Função principal
int main()
{
    PT_INIT(&pt_sender);
    PT_INIT(&pt_receiver);

    sender(&pt_sender);
    receiver(&pt_receiver);

    return 0;
}
