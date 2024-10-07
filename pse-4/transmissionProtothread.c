/**
 * @file transmissionProtothread.c
 * @brief Implementação de uma Máquina de Estados Finitos (FSM) para decodificação de um protocolo de comunicação.
 * 
 * O protocolo de comunicação é composto pelos seguintes campos:
 * - STX (1 byte): Início da transmissão.
 * - QTD (1 byte): Quantidade de bytes de dados.
 * - DADOS (N bytes): Dados da mensagem.
 * - CHK (1 byte): Checksum dos dados.
 * - ETX (1 byte): Fim da transmissão.
 * 
 * A FSM é implementada utilizando uma enumeração para os estados e uma estrutura para armazenar o estado atual e os dados recebidos.
 * 
 * Estados da FSM:
 * - STATE_WAIT_STX: Aguardando o byte de início da transmissão (STX).
 * - STATE_READ_QTD: Lendo o byte que indica a quantidade de dados (QTD).
 * - STATE_READ_DATA: Lendo os bytes de dados.
 * - STATE_READ_CHK: Lendo o byte de checksum (CHK).
 * - STATE_WAIT_ETX: Aguardando o byte de fim da transmissão (ETX).
 * - STATE_COMPLETE: Transmissão completa.
 * - STATE_ERROR: Erro na transmissão.
 * 
 * Funções principais:
 * - fsm_init: Inicializa a FSM.
 * - fsm_process: Processa um byte de entrada e atualiza o estado da FSM.
 * - fsm_is_complete: Verifica se a transmissão foi completada com sucesso.
 * - fsm_is_error: Verifica se ocorreu um erro na transmissão.
 * 
 * Função de teste:
 * - test_fsm: Testa a FSM com uma mensagem de exemplo.
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STATE_WAIT_STX,
    STATE_READ_QTD,
    STATE_READ_DATA,
    STATE_READ_CHK,
    STATE_WAIT_ETX,
    STATE_COMPLETE,
    STATE_ERROR
} State;

typedef struct {
    State current_state;
    uint8_t qtd;
    uint8_t data[256];
    uint8_t chk;
    uint8_t data_index;
} FSM;

void fsm_init(FSM *fsm) {
    fsm->current_state = STATE_WAIT_STX;
    fsm->qtd = 0;
    fsm->chk = 0;
    fsm->data_index = 0;
}

bool fsm_process(FSM *fsm, uint8_t byte) {
    switch (fsm->current_state) {
        case STATE_WAIT_STX:
            if (byte == 0x02) { // STX
                fsm->current_state = STATE_READ_QTD;
            }
            break;
        case STATE_READ_QTD:
            fsm->qtd = byte;
            fsm->current_state = STATE_READ_DATA;
            break;
        case STATE_READ_DATA:
            fsm->data[fsm->data_index++] = byte;
            if (fsm->data_index == fsm->qtd) {
                fsm->current_state = STATE_READ_CHK;
            }
            break;
        case STATE_READ_CHK:
            fsm->chk = byte;
            fsm->current_state = STATE_WAIT_ETX;
            break;
        case STATE_WAIT_ETX:
            if (byte == 0x03) { // ETX
                fsm->current_state = STATE_COMPLETE;
                return true;
            } else {
                fsm->current_state = STATE_ERROR;
            }
            break;
        case STATE_COMPLETE:
        case STATE_ERROR:
            break;
    }
    return false;
}

bool fsm_is_complete(FSM *fsm) {
    return fsm->current_state == STATE_COMPLETE;
}

bool fsm_is_error(FSM *fsm) {
    return fsm->current_state == STATE_ERROR;
}

// Test function
void test_fsm() {
    FSM fsm;
    fsm_init(&fsm);

    uint8_t message[] = {0x02, 0x03, 'A', 'B', 'C', 0x00, 0x03};
    bool complete = false;

    for (int i = 0; i < sizeof(message); i++) {
        complete = fsm_process(&fsm, message[i]);
        if (fsm_is_error(&fsm)) {
            printf("FSM Error\n");
            return;
        }
    }

    if (complete && fsm_is_complete(&fsm)) {
        printf("FSM Complete\n");
        printf("QTD: %d\n", fsm.qtd);
        printf("Data: ");
        for (int i = 0; i < fsm.qtd; i++) {
            printf("%c", fsm.data[i]);
        }
        printf("\nCHK: %d\n", fsm.chk);
    } else {
        printf("FSM Incomplete\n");
    }
}

int main() {
    test_fsm();
    return 0;
}