#include <stdbool.h>

/* Diferenças entre os modos preemptivo e cooperativo:
 * 
 * Modo Preemptivo:
 * - O sistema operacional pode interromper uma tarefa em execução para dar lugar a outra tarefa de maior prioridade.
 * - Permite uma resposta mais rápida a eventos de alta prioridade.
 * - Requer um mecanismo de temporização e interrupções.
 * 
 * Modo Cooperativo:
 * - As tarefas devem ceder o controle explicitamente para permitir que outras tarefas sejam executadas.
 * - Simplicidade na implementação, pois não requer interrupções complexas.
 * - Pode resultar em menor responsividade se uma tarefa não ceder o controle frequentemente.
 */

/* Variável global para armazenar o modo de operação */
bool modo_preemptivo = true;

/* Prototipo da tarefa periódica */
void tarefa_periodica(void);

/*
 * Configuracao dos tamanhos das pilhas
 */
#define TAM_PILHA_PERIODICA (TAM_MINIMO_PILHA + 24)
#define TAM_PILHA_OCIOSA    (TAM_MINIMO_PILHA + 24)

/*
 * Declaracao das pilhas das tarefas
 */
uint32_t PILHA_TAREFA_PERIODICA[TAM_PILHA_PERIODICA];
uint32_t PILHA_TAREFA_OCIOSA[TAM_PILHA_OCIOSA];

/*
 * Funcao principal de entrada do sistema
 */
int main(void)
{
	system_init();
	
	/* Criacao das tarefas */
	/* Parametros: ponteiro, nome, ponteiro da pilha, tamanho da pilha, prioridade da tarefa */
	CriaTarefa(tarefa_periodica, "Tarefa Periodica", PILHA_TAREFA_PERIODICA, TAM_PILHA_PERIODICA, 1);  // Cria a nova tarefa
	
	/* Cria tarefa ociosa do sistema */
	CriaTarefa(tarefa_ociosa,"Tarefa ociosa", PILHA_TAREFA_OCIOSA, TAM_PILHA_OCIOSA, 0);
	
	/* Configura marca de tempo */
	ConfiguraMarcaTempo();   
	
	/* Inicia sistema multitarefas */
	IniciaMultitarefas();
	
	/* Nunca chega aqui */
	while (1)
	{
	}
}

/* Nova tarefa periodica */
void tarefa_periodica(void)
{
	for(;;)
	{
		// Código da tarefa periódica
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);  // Liga LED
		TarefaEspera(100);  // Espera 100 ms
		port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);  // Desliga LED
		TarefaEspera(100);  // Espera 100 ms
		
		if (!modo_preemptivo)
		{
			// Se estiver no modo cooperativo, cede o controle
			CedeControle();
		}
	}
}

/* Configura a tarefa periódica para ser executada a cada 100 ms */
void ConfiguraTarefaPeriodica(void)
{
	// Configura o temporizador para gerar uma interrupção a cada 100 ms
	ConfiguraTemporizador(100, tarefa_periodica);
}

/* Função para definir o modo de operação */
void DefineModoOperacao(bool preemptivo)
{
	modo_preemptivo = preemptivo;
}
