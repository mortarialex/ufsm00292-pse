/**
 * @file transmissionProtocol.c
 * @brief Implementação de um decodificador de protocolo de comunicação usando uma Máquina de Estados Finitos (FSM).
 *
 * Este arquivo contém a implementação de um decodificador de protocolo de comunicação usando uma FSM.
 * O formato do protocolo é o seguinte: (STX (1 B)|QTD (1 B)|DADOS (N B)|CHK (1 B)|ETX (1 B)).
 * A FSM é implementada usando ponteiros de função e uma tabela de estados.
 *
 * A FSM transita pelos seguintes estados:
 * - STATE_WAIT_STX: Aguardando o byte de Início de Texto (STX).
 * - STATE_READ_QTD: Lendo o byte de quantidade (QTD).
 * - STATE_READ_DATA: Lendo os bytes de dados.
 * - STATE_READ_CHK: Lendo o byte de checksum (CHK).
 * - STATE_WAIT_ETX: Aguardando o byte de Fim de Texto (ETX).
 * - STATE_COMPLETE: Mensagem completa recebida com sucesso.
 * - STATE_ERROR: Ocorreu um erro durante a recepção da mensagem.
 *
 * A FSM é resetada para o estado inicial (STATE_WAIT_STX) após processar uma mensagem completa ou encontrar um erro.
 *
 * As seguintes funções lidam com os diferentes estados da FSM:
 * - handleWaitSTX: Lida com o estado STATE_WAIT_STX.
 * - handleReadQTD: Lida com o estado STATE_READ_QTD.
 * - handleReadData: Lida com o estado STATE_READ_DATA.
 * - handleReadCHK: Lida com o estado STATE_READ_CHK.
 * - handleWaitETX: Lida com o estado STATE_WAIT_ETX.
 *
 * A função processByte processa cada byte da mensagem de entrada e atualiza o estado da FSM de acordo.
 * A função testFSM testa a FSM com uma mensagem de exemplo.
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
    State currentState;
    uint8_t qtd;
    uint8_t data[256];
    uint8_t chk;
    uint8_t dataIndex;
} FSM;

typedef bool (*StateHandler)(FSM *fsm, uint8_t byte);

bool handleWaitSTX(FSM *fsm, uint8_t byte);
bool handleReadQTD(FSM *fsm, uint8_t byte);
bool handleReadData(FSM *fsm, uint8_t byte);
bool handleReadCHK(FSM *fsm, uint8_t byte);
bool handleWaitETX(FSM *fsm, uint8_t byte);

StateHandler stateTable[] = {
    handleWaitSTX,
    handleReadQTD,
    handleReadData,
    handleReadCHK,
    handleWaitETX
};

void resetFSM(FSM *fsm) {
    fsm->currentState = STATE_WAIT_STX;
    fsm->qtd = 0;
    fsm->chk = 0;
    fsm->dataIndex = 0;
}

bool handleWaitSTX(FSM *fsm, uint8_t byte) {
    if (byte == 0x02) { // STX
        fsm->currentState = STATE_READ_QTD;
    }
    return false;
}

bool handleReadQTD(FSM *fsm, uint8_t byte) {
    fsm->qtd = byte;
    fsm->currentState = STATE_READ_DATA;
    return false;
}

bool handleReadData(FSM *fsm, uint8_t byte) {
    fsm->data[fsm->dataIndex++] = byte;
    if (fsm->dataIndex == fsm->qtd) {
        fsm->currentState = STATE_READ_CHK;
    }
    return false;
}

bool handleReadCHK(FSM *fsm, uint8_t byte) {
    fsm->chk = byte;
    fsm->currentState = STATE_WAIT_ETX;
    return false;
}

bool handleWaitETX(FSM *fsm, uint8_t byte) {
    if (byte == 0x03) { // ETX
        fsm->currentState = STATE_COMPLETE;
        return true;
    } else {
        fsm->currentState = STATE_ERROR;
    }
    return false;
}

bool processByte(FSM *fsm, uint8_t byte) {
    if (fsm->currentState < STATE_COMPLETE) {
        return stateTable[fsm->currentState](fsm, byte);
    }
    resetFSM(fsm);
    return false;
}

void testFSM() {
    FSM fsm;
    resetFSM(&fsm);

    uint8_t message[] = {0x02, 0x03, 'A', 'B', 'C', 0x05, 0x03};
    bool result = false;

    for (int i = 0; i < sizeof(message); i++) {
        result = processByte(&fsm, message[i]);
        if (result) {
            printf("Sucesso!\n");
            break;
        }
    }

    if (!result) {
        printf("Falha!\n");
    }
}

int main() {
    testFSM();
    return 0;
}