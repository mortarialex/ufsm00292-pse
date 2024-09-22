#include <stdio.h>
#include <stdlib.h>

/* macros de testes - baseado em minUnit: www.jera.com/techinfo/jtns/jtn002.html */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                if (mensagem) return mensagem; } while (0)


/*--------------------------------------------------------------------------

ATIVIDADE 1 - Buffer circular com método TDD e testes 

Test-Driven Development (TDD) é uma abordagem de desenvolvimento 
de software onde os testes são escritos antes do código funcional. 
O ciclo básico do TDD é composto por três etapas principais:

# Red: Escrever um teste que falha. 

# Green: Escrever o código mínimo necessário para fazer o teste passar. 

# Refactor: Refatorar o código para melhorar sua estrutura e qualidade,
sem alterar seu comportamento externo. 

Este ciclo é repetido continuamente, garantindo que o código é constantemente 
testado e melhorado. TDD ajuda a criar um design de código mais limpo 
e robusto, além de fornecer uma suite de testes que pode ser usada para 
verificar a integridade do sistema a cada mudança.

----------------------------------------------------------------------------*/

/* Estrutura que representa o buffer circular */
typedef struct {
    int *buffer; // Ponteiro para o array que armazena os elementos do buffer
    int head;    // Índice da cabeça do buffer (onde novos elementos são inseridos)
    int tail;    // Índice da cauda do buffer (de onde elementos são removidos)
    int max;     // Capacidade máxima do buffer
    int count;   // Número atual de elementos no buffer
} CircularBuffer;

/* Função para criar um buffer circular */
CircularBuffer* createBuffer(int size) {
    CircularBuffer *cb = (CircularBuffer *)malloc(sizeof(CircularBuffer));
    cb->buffer = (int *)malloc(size * sizeof(int));
    cb->max = size;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    return cb;
}

/* Função para liberar a memória alocada para o buffer circular */
void freeBuffer(CircularBuffer *cb) {
    free(cb->buffer);
    free(cb);
}

/* Função para verificar se o buffer está cheio */
int isFull(CircularBuffer *cb) {
    return cb->count == cb->max;
}

/* Função para verificar se o buffer está vazio */
int isEmpty(CircularBuffer *cb) {
    return cb->count == 0;
}

/* Função para enfileirar (inserir) um item no buffer */
void enqueue(CircularBuffer *cb, int item) {
    if (!isFull(cb)) {
        cb->buffer[cb->head] = item;
        cb->head = (cb->head + 1) % cb->max;
        cb->count++;
    } else {
        printf("Buffer is full\n");
    }
}

/* Função para desenfileirar (remover) um item do buffer */
int dequeue(CircularBuffer *cb) {
    if (!isEmpty(cb)) {
        int item = cb->buffer[cb->tail];
        cb->tail = (cb->tail + 1) % cb->max;
        cb->count--;
        return item;
    } else {
        printf("Buffer is empty\n");
        return -1; // Retorna um valor de erro
    }
}

int testes_executados = 0;

/* Função que executa todos os testes */
static char * executa_testes(void);

/* Teste para criar o buffer */
static char * teste_criaBuffer(void) {
    CircularBuffer *cb = createBuffer(5);
    verifica("erro: buffer não foi criado corretamente", cb != NULL);
    verifica("erro: tamanho máximo do buffer incorreto", cb->max == 5);
    verifica("erro: head inicial incorreto", cb->head == 0);
    verifica("erro: tail inicial incorreto", cb->tail == 0);
    verifica("erro: count inicial incorreto", cb->count == 0);
    freeBuffer(cb);
    return 0;
}

/* Teste para enfileirar um item no buffer */
static char * teste_enqueue(void) {
    CircularBuffer *cb = createBuffer(3);
    enqueue(cb, 1);
    verifica("erro: valor não foi enfileirado corretamente", cb->buffer[0] == 1);
    verifica("erro: head não foi atualizado corretamente", cb->head == 1);
    verifica("erro: count não foi atualizado corretamente", cb->count == 1);
    freeBuffer(cb);
    return 0;
}

/* Teste para desenfileirar um item do buffer */
static char * teste_dequeue(void) {
    CircularBuffer *cb = createBuffer(3);
    enqueue(cb, 1);
    int item = dequeue(cb);
    verifica("erro: valor não foi desenfileirado corretamente", item == 1);
    verifica("erro: tail não foi atualizado corretamente", cb->tail == 1);
    verifica("erro: count não foi atualizado corretamente", cb->count == 0);
    freeBuffer(cb);
    return 0;
}

/* Teste para verificar se o buffer está cheio */
static char * teste_isFull(void) {
    CircularBuffer *cb = createBuffer(2);
    enqueue(cb, 1);
    enqueue(cb, 2);
    verifica("erro: buffer deveria estar cheio", isFull(cb) == 1);
    freeBuffer(cb);
    return 0;
}

/* Teste para verificar se o buffer está vazio */
static char * teste_isEmpty(void) {
    CircularBuffer *cb = createBuffer(2);
    verifica("erro: buffer deveria estar vazio", isEmpty(cb) == 1);
    enqueue(cb, 1);
    verifica("erro: buffer não deveria estar vazio", isEmpty(cb) == 0);
    freeBuffer(cb);
    return 0;
}

/* Função que executa todos os testes */
static char * executa_testes(void) {
    executa_teste(teste_criaBuffer);
    executa_teste(teste_enqueue);
    executa_teste(teste_dequeue);
    executa_teste(teste_isFull);
    executa_teste(teste_isEmpty);
    return 0;
}

/* Função principal que executa os testes e imprime os resultados */
int main() {
    char *resultado = executa_testes();
    if (resultado != 0) {
        printf("%s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    return resultado != 0;
}