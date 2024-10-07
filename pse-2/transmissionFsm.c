#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Definição dos estados da FSM
typedef enum {
    STATE_WAIT_STX,    // Aguardando STX
    STATE_READ_QTD,    // Lendo QTD
    STATE_READ_DATA,   // Lendo DADOS
    STATE_READ_CHK,    // Lendo CHK
    STATE_WAIT_ETX,    // Aguardando ETX
    STATE_COMPLETE,    // Completo
    STATE_ERROR        // Erro
} State;

// Estrutura da FSM
typedef struct {
    State currentState;  // Estado atual
    uint8_t qtd;         // Quantidade de dados
    uint8_t data[256];   // Dados
    uint8_t chk;         // Checksum
    uint8_t dataIndex;   // Índice dos dados
} FSM;

// Inicializa a FSM
void fsm_init(FSM *fsm) {
    fsm->currentState = STATE_WAIT_STX;
    fsm->qtd = 0;
    fsm->chk = 0;
    fsm->dataIndex = 0;
}

// Processa um byte na FSM
bool fsm_process(FSM *fsm, uint8_t byte) {
    switch (fsm->currentState) {
        case STATE_WAIT_STX:
            if (byte == 0x02) { // STX
                fsm->currentState = STATE_READ_QTD;
            }
            break;
        case STATE_READ_QTD:
            fsm->qtd = byte;
            fsm->currentState = STATE_READ_DATA;
            break;
        case STATE_READ_DATA:
            fsm->data[fsm->dataIndex++] = byte;
            if (fsm->dataIndex == fsm->qtd) {
                fsm->currentState = STATE_READ_CHK;
            }
            break;
        case STATE_READ_CHK:
            fsm->chk = byte;
            fsm->currentState = STATE_WAIT_ETX;
            break;
        case STATE_WAIT_ETX:
            if (byte == 0x03) { // ETX
                fsm->currentState = STATE_COMPLETE;
                return true;
            } else {
                fsm->currentState = STATE_ERROR;
            }
            break;
        case STATE_COMPLETE:
        case STATE_ERROR:
            break;
    }
    return false;
}

// Testes usando TDD
void test_fsm() {
    FSM fsm;
    fsm_init(&fsm);

    uint8_t message[] = {0x02, 0x03, 'A', 'B', 'C', 0x05, 0x03};
    bool result = false;

    for (int i = 0; i < sizeof(message); i++) {
        result = fsm_process(&fsm, message[i]);
        if (result) {
            break;
        }
    }

    if (result && fsm.currentState == STATE_COMPLETE) {
        printf("FSM completou com sucesso.\n");
    } else {
        printf("FSM falhou.\n");
    }
}

int main() {
    test_fsm();
    return 0;
}