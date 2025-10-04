/* ==========================================================================
Universidade Federal de São Carlos - Campus Sorocaba
Disciplina: Organização de Recuperação da Informação
Prof. Tiago A. Almeida
Trabalho 01 - Indexação
========================================================================== *
<<< NÃO ALTERAR >>>
========================================================================== */
#ifndef ORI_T01_H
#define ORI_T01_H

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h> 

// Tamanhos de campos de tamanho fixo
#define TAM_DATE 9
#define TAM_TIME 7
#define TAM_DATETIME 13
#define TAM_INT_NUMBER 5
#define TAM_FLOAT_NUMBER 14
#define TAM_ID_TREINADOR 12
#define TAM_ID_BOLSOMON 4
#define TAM_ID_BATALHA 9
#define TAM_ARENA 5
#define QTD_MAX_BOLSOMONS 10
#define QTD_MAX_TREINADORES 12

// Tamanhos de campos multivalorados fixos
#define TAM_ID_TREINADORES (TAM_ID_TREINADOR * QTD_MAX_TREINADORES - (QTD_MAX_TREINADORES-1))
#define TAM_BOLSOMONS_DERROTADOS_TREINADORES (TAM_INT_NUMBER * QTD_MAX_TREINADORES - (QTD_MAX_TREINADORES - 1))
#define TAM_BOLSOMONS_TREINADORES (TAM_ID_BOLSOMON * QTD_MAX_TREINADORES - (QTD_MAX_TREINADORES-1))
#define TAM_DURACAO_TREINADORES (TAM_TIME * QTD_MAX_TREINADORES - (QTD_MAX_TREINADORES-1))

// Tamanhos máximos de campos de tamanho variável
#define TAM_MAX_APELIDO 44
#define TAM_MAX_NOME_BOLSOMON 21
#define TAM_MAX_HABILIDADE_BOLSOMON 61

#define MAX_REGISTROS 1000

// Tamanhos de cada registro no arquivo
#define TAM_REGISTRO_TREINADOR 96
#define TAM_REGISTRO_BOLSOMON 100
#define TAM_REGISTRO_BATALHA 30
#define TAM_REGISTRO_RESULTADO 25
#define TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON 14

// Tamanhos dos arquivos
#define TAM_ARQUIVO_TREINADORES (TAM_REGISTRO_TREINADOR * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_BOLSOMONS (TAM_REGISTRO_BOLSOMON * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_BATALHAS (TAM_REGISTRO_BATALHA * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_RESULTADOS (TAM_REGISTRO_RESULTADO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_TREINADOR_POSSUI_BOLSOMON (TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON * MAX_REGISTROS + 1)


// Tamanhos das chaves
#define TAM_RRN_REGISTRO 4
#define TAM_CHAVE_TREINADORES_IDX ((TAM_ID_TREINADOR - 1) + TAM_RRN_REGISTRO)
#define TAM_CHAVE_BOLSOMONS_IDX ((TAM_ID_BOLSOMON - 1) + TAM_RRN_REGISTRO)
#define TAM_CHAVE_BATALHAS_IDX ((TAM_ID_BATALHA - 1) + TAM_RRN_REGISTRO)
#define TAM_CHAVE_RESULTADOS_IDX ((TAM_ID_TREINADOR - 1) + (TAM_ID_BATALHA - 1) + TAM_RRN_REGISTRO)
#define TAM_CHAVE_PRECO_BOLSOMON_IDX ((TAM_FLOAT_NUMBER - 1) + (TAM_ID_BOLSOMON - 1))
#define TAM_CHAVE_BATALHA_DATA_IDX ((TAM_DATETIME - 1) + (TAM_ID_BATALHA - 1))
#define TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX (TAM_ID_TREINADOR - 1)
#define TAM_CHAVE_TREINADOR_BOLSOMON_SECUNDARIO_IDX (TAM_MAX_NOME_BOLSOMON - 1)

// Mensagens padrão
#define SUCESSO                          "OK\n"
#define REGS_PERCORRIDOS                 "Registros percorridos:"
#define INDICE_CRIADO                    "Indice %s criado com sucesso!\n"
#define AVISO_NENHUM_REGISTRO_ENCONTRADO "AVISO: Nenhum registro encontrado\n"
#define ERRO_OPCAO_INVALIDA              "ERRO: Opcao invalida\n"
#define ERRO_MEMORIA_INSUFICIENTE        "ERRO: Memoria insuficiente\n"
#define ERRO_PK_REPETIDA                 "ERRO: Ja existe um registro com a chave %s\n"
#define ERRO_REGISTRO_NAO_ENCONTRADO     "ERRO: Registro nao encontrado\n"
#define ERRO_SALDO_NAO_SUFICIENTE        "ERRO: Saldo insuficiente\n"
#define ERRO_VALOR_INVALIDO              "ERRO: Valor invalido\n"
#define ERRO_ARQUIVO_VAZIO               "ERRO: Arquivo vazio\n"
#define ERRO_NAO_IMPLEMENTADO            "ERRO: Funcao %s nao implementada\n"
#define ERRO_BOLSOMON_REPETIDO                "ERRO: O treinador %s já possui o bolsomon %s\n"
#define ERRO_TREINADOR_BOLSOMON	             "ERRO: O treinador %s nao possui o bolsomon %s\n"
#define CONCEDER_PREMIO 				 "O treinador %s com o melhor desempenho (Vitórias em Duração: %d, Vitórias em Derrotas: %d, Vitórias em Dano: %d) foi recompensado com %.2lf de bolsobolas.\n"
#define ERRO_TREINADOR_REMOVIDO 		 "O treinador com melhor desempenho foi removido. O prêmio de %.2lf será concedido ao treinador %s (Vitórias em Duração: %d, Vitórias em Derrotas: %d,  Vitórias em Dano: %d).\n"

// ---------------------- Registros principais ----------------------

// Registro de Treinador
typedef struct {
	char id_treinador[TAM_ID_TREINADOR];
	char apelido[TAM_MAX_APELIDO];
	char cadastro[TAM_DATETIME];
	char premio[TAM_DATETIME];
	double bolsobolas;
} Treinador;

// Registro de Bolsomon
typedef struct {
	char id_bolsomon[TAM_ID_BOLSOMON];
	char nome[TAM_MAX_NOME_BOLSOMON];
	char habilidade[TAM_MAX_HABILIDADE_BOLSOMON];
	double preco_bolsobolas;
} Bolsomon;

// Registro de Batalha
typedef struct {
	char id_batalha[TAM_ID_BATALHA];
	char inicio[TAM_DATETIME];
	char duracao[TAM_TIME];
	char arena[TAM_ARENA];
} Batalha;

// Registro de Resultado
typedef struct {
	char id_treinador[TAM_ID_TREINADOR];
	char id_batalha[TAM_ID_BATALHA];
	char id_bolsomon[TAM_ID_BOLSOMON];
	bool foi_maior_duracao;
	bool foi_mais_derrotas;
	bool foi_mais_dano;
} Resultado;

// Registro da relação treinador-bolsomon
typedef struct {
    char id_treinador[TAM_ID_TREINADOR];
    char id_bolsomon[TAM_ID_BOLSOMON];
} TreinadorPossuiBolsomon;


// Registro auxiliar para a função recompensar_campeao_menu()
typedef struct {
	char id_treinador[TAM_ID_TREINADOR];
	int vezes_mais_tempo;
    int vezes_mais_dano;
    int vezes_mais_derrotados;
} Info_Treinador;

// ---------------------- Registros dos índices ----------------------

// Índice primário dos treinadores
typedef struct {
	char id_treinador[TAM_ID_TREINADOR];
	int rrn;
} treinadores_index;

// Índice primário dos bolsomons
typedef struct {
	char id_bolsomon[TAM_ID_BOLSOMON];
	int rrn;
} bolsomons_index;

// Índice primário das batalhas
typedef struct {
	char id_batalha[TAM_ID_BATALHA];
	int rrn;
} batalhas_index;

// Índice primário dos resultados do treinador por batalha
typedef struct {
	char id_treinador[TAM_ID_TREINADOR];
	char id_batalha[TAM_ID_BATALHA];
	int rrn;
} resultados_index;

// Índice primário da relação treinador-bolsomon
typedef struct {
	char id_treinador[TAM_ID_TREINADOR];
	char id_bolsomon[TAM_ID_BOLSOMON];
	int rrn;
} treinador_possui_bolsomon_index;

// Índice secundário dos preços dos bolsomons
typedef struct {
	char id_bolsomon[TAM_ID_BOLSOMON];
	double preco_bolsobolas;
} preco_bolsomon_index;

// Índice secundário das datas das batalhas
typedef struct {
	char inicio[TAM_DATETIME];
	char id_batalha[TAM_ID_BATALHA];
} data_index;

// Índice secundário dos nomes de bolsomon (lista invertida)
typedef struct {
	char chave_secundaria[TAM_MAX_NOME_BOLSOMON];   //string com o nome do bolsomon
	int primeiro_indice;
} treinador_bolsomons_secundario_index;

// Índice primário dos nomes de bolsomon (lista invertida)
typedef struct {
	char chave_primaria[TAM_ID_TREINADOR];   //string com o ID do treinador
	int proximo_indice;
} treinador_bolsomons_primario_index;

// Struct para os parâmetros de uma lista invertida
typedef struct {
	treinador_bolsomons_secundario_index *treinador_bolsomons_secundario_idx; // Ponteiro para o índice secundário
	treinador_bolsomons_primario_index *treinador_bolsomons_primario_idx; // Ponteiro para o arquivo de índice primário
	unsigned qtd_registros_secundario; // Quantidade de registros de índice secundário
	unsigned qtd_registros_primario; // Quantidade de registros de índice primário
	unsigned tam_chave_secundaria; // Tamanho de uma chave secundária nesse índice
	unsigned tam_chave_primaria; // Tamanho de uma chave primária nesse índice
	int (*compar)(const void *key, const void *elem); // Função utilizada para comparar as chaves do índice secundário.
} inverted_list;

// ---------------------- Funções ----------------------

// Tem algumas funções auxiliares em utils.h

// Funções de comparação para o qsort
// Devem retornar:
//   um número maior que 0 se a > b.
//   0 se a == b
//   um número menor que 0 se b > a
int qsort_treinadores_idx(const void *a, const void *b);
int qsort_bolsomons_idx(const void *a, const void *b);
int qsort_batalhas_idx(const void *a, const void *b);
int qsort_resultados_idx(const void *a, const void *b);
int qsort_preco_bolsomon_idx(const void *a, const void *b);
int qsort_data_idx(const void *a, const void *b);
int qsort_info_treinador(const void *a, const void *b);
int qsort_treinador_bolsomons_secundario_idx(const void *a, const void *b);
int qsort_inverted_list_primary_search(const void *a, const void *b);
int qsort_treinador_possui_bolsomon_idx(const void *a, const void *b);

// Cria o índice respectivo
void criar_treinadores_idx();
void criar_bolsomons_idx();
void criar_batalhas_idx();
void criar_resultados_idx();
void criar_preco_bolsomon_idx();
void criar_treinador_bolsomons_idx();
void criar_treinador_possui_bolsomon_idx();
void criar_data_idx();

// Recupera do arquivo o registro com o RRN informado e o retorna
Treinador recuperar_registro_treinador(int rrn);
Bolsomon recuperar_registro_bolsomon(int rrn);
Batalha recuperar_registro_batalha(int rrn);
Resultado recuperar_registro_resultado(int rrn);
TreinadorPossuiBolsomon recuperar_registro_treinador_possui_bolsomon(int rrn);

// Escreve o registro em seu respectivo arquivo no RRN informado
void escrever_registro_treinador(Treinador j, int rrn);
void escrever_registro_bolsomon(Bolsomon k, int rrn);
void escrever_registro_batalha(Batalha p, int rrn);
void escrever_registro_resultado (Resultado jp, int rrn);

// Exibe um registro com base no RRN
bool exibir_treinador(int rrn);
bool exibir_bolsomon(int rrn);
bool exibir_batalha(int rrn);

// Funções principais
void cadastrar_treinador_menu(char *id_treinador, char *apelido);
void remover_treinador_menu(char *id_treinador);
void adicionar_bolsobolas_menu(char *id_treinador, double valor);
void adicionar_bolsobolas(char *id_treinador, double valor, bool flag);
void cadastrar_bolsomon_menu(char *nome, char *habilidade, double preco);
void comprar_bolsomon_menu(char *id_treinador, char *id_bolsomon);
void executar_batalha_menu(char *inicio, char *duracao, char *arena, char *id_treinadores, char *bolsomons_treinadores, char *duracoes_treinadores, char *eliminacoes_treinadores, char *danos_causados_treinadores);
void recompensar_campeao_menu (char *data_inicio, char *data_fim, double premio);

// Funções de busca
void buscar_treinador_id_menu(char *id_treinador);
void buscar_bolsomon_id_menu(char *id_bolsomon);
void buscar_batalha_id_menu(char *id_batalha);

// Funções de listagem
void listar_treinadores_id_menu();
void listar_treinadores_bolsomons_menu(char *bolsomon);
void listar_bolsomons_compra_menu(char *id_treinador);
void listar_batalhas_periodo_menu(char *data_inicio, char *data_fim);

// Funções de exibição dos arquivos de dados
void imprimir_arquivo_treinadores_menu();
void imprimir_arquivo_bolsomons_menu();
void imprimir_arquivo_batalhas_menu();
void imprimir_arquivo_resultados_menu();
void imprimir_arquivo_treinador_possui_bolsomon_menu();

// Funções de exibição de índice
void imprimir_treinadores_idx_menu();
void imprimir_bolsomons_idx_menu();
void imprimir_batalhas_idx_menu();
void imprimir_resultados_idx_menu();
void imprimir_treinador_possui_bolsomon_idx_menu();
void imprimir_preco_bolsomon_idx_menu();
void imprimir_data_idx_menu();
void imprimir_treinador_bolsomons_secundario_idx_menu();
void imprimir_treinador_bolsomons_primario_idx_menu();

// Funções de alteração direta dos arquivos
void set_arquivo_treinadores(char* novos_dados, size_t tamanho);
void set_arquivo_bolsomons(char* novos_dados, size_t tamanho);
void set_arquivo_batalhas(char* novos_dados, size_t tamanho);
void set_arquivo_resultados(char* novos_dados, size_t tamanho);
void set_arquivo_treinador_possui_bolsomon(char* novos_dados, size_t tamanho);

// Funções de liberação de espaço
void liberar_espaco_menu();
void liberar_memoria_menu();

// Funções de manipulação de Lista Invertida

/*
Responsável por inserir duas chaves (chave_secundaria e chave_primaria) em uma
Lista Invertida (t).

Atualiza os parâmetros dos índices primário e secundário conforme necessário.
As chaves a serem inseridas devem estar no formato correto e com tamanho t->tam_chave_primario e t->tam_chave_secundario.

O funcionamento deve ser genérico para qualquer Lista Invertida, adaptando-se para os diferentes parâmetros presentes em seus structs.

@param chave_secundaria Chave a ser buscada (caso exista) ou inserida (caso não exista) no registro secundário da Lista Invertida.
@param chave_primaria Chave a ser inserida no registro primário da Lista Invertida.
@param t Ponteiro para a Lista Invertida na qual serão inseridas as chaves.
*/
void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t);

/*
Responsável por buscar uma chave no índice secundário de uma Lista invertida (T). O valor de retorno indica se a chave foi encontrada ou não.

O ponteiro para o int result pode ser fornecido opcionalmente, e conterá o índice inicial das chaves no registro primário.

Exemplos de uso:

Exemplo 1. A chave encontrada deverá ser retornada e o caminho não deve ser informado.<br />

int result;
bool found = inverted_list_secondary_search(&result, false, modelo, &treinador_bolsomons_idx);

Exemplo 2. Não há interesse na chave encontrada, apenas se ela existe, e o caminho não deve ser informado.

bool found = inverted_list_secondary_search(NULL, false, modelo, &treinador_bolsomons_idx);

Exemplo 3. Há interesse no caminho feito para encontrar a chave.

int result;
bool found = inverted_list_secondary_search(&result, true, modelo, &treinador_bolsomons_idx);

@param result Ponteiro para ser escrito o índice inicial (primeira ocorrência) das chaves do registro primário. É ignorado caso NULL.
@param exibir_caminho Indica se o caminho percorrido deve ser impresso.
@param chave_secundaria Chave a ser buscada.
@param t Ponteiro para o índice do tipo Lista invertida no qual será buscada a chave.

@return Indica se a chave foi encontrada.
*/
bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t);

/*
Responsável por percorrer o índice primário de uma Lista invertida (T). O valor de retorno indica a quantidade de chaves encontradas.
O ponteiro para o vetor de strings result pode ser fornecido opcionalmente, e será populado com a lista de todas as chaves encontradas.
O ponteiro para o inteiro indice_final também pode ser fornecido opcionalmente, e deve conter o índice do último campo da lista encadeada
da chave primaria fornecida (isso é útil na inserção de um novo registro).

Exemplos de uso:

Exemplo 1. As chaves encontradas deverão ser retornadas e tanto o caminho quanto o indice_final não devem ser informados.

char chaves[TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX][MAX_REGISTROS];
int qtd = inverted_list_primary_search(chaves, false, indice, NULL, &treinador_bolsomons_idx);

Exemplo 2. Não há interesse nas chaves encontradas, apenas no indice_final, e o caminho não deve ser informado.

int indice_final;
int qtd = inverted_list_primary_search(NULL, false, indice, &indice_final, &treinador_bolsomons_idx);

Exemplo 3. Há interesse nas chaves encontradas e no caminho feito.

char chaves[TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX][MAX_REGISTROS];
int qtd = inverted_list_primary_search(chaves, true, indice, NULL, &treinador_bolsomons_idx);

@param result Ponteiro para serem escritas as chaves encontradas. É ignorado caso NULL.
@param exibir_caminho Indica se o caminho percorrido deve ser impresso.
@param indice Índice do primeiro registro da lista encadeada a ser procurado.
@param indice_final Ponteiro para ser escrito o índice do último registro encontrado (cujo campo indice é -1). É ignorado caso NULL.
@param t Ponteiro para o índice do tipo Lista invertida no qual será buscada a chave.

@return Indica a quantidade de chaves encontradas.
*/
int inverted_list_primary_search(char result[][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t);

/*
Função Genérica de busca binária, que aceita parâmetros genéricos (assinatura baseada na função bsearch da biblioteca C).

@param key Chave de busca genérica.
@param base0 Base onde ocorrerá a busca, por exemplo, um ponteiro para um vetor.
@param nmemb Número de elementos na base.
@param size Tamanho do tipo do elemento na base, dica: utilize a função sizeof().
@param compar Ponteiro para a função que será utilizada nas comparações.
@param exibir_caminho Indica se o caminho percorrido deve ser impresso. Se true, imprime as posições avaliadas durante a busca, até que todas sejam visitadas ou o elemento seja encontrado (pela 1a vez).
Caso o número de elementos seja par (p.ex, 10 elementos), então há 2 (duas) possibilidades para a posição da mediana dos elementos (p.ex., 5a ou 6a posição se o total fosse 10).
Neste caso, SEMPRE escolha a posição mais à direita (p.ex., a posição 6 caso o total for 10).
@param retorno_se_nao_encontrado Caso o elemento NÃO seja encontrado, indica qual valor deve ser retornado. As opções são:
-1 = predecessor : retorna o elemento PREDECESSOR (o que apareceria imediatamente antes do elemento procurado, caso fosse encontrado).
0 = nulo : retorna NULL. [modo padrão]
+1 = sucessor : retorna o elemento SUCESSOR (o que apareceria imediatamente depois do elemento procurado, caso fosse encontrado).

@return Retorna o elemento encontrado ou NULL se não encontrou.
*/

int busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado);

#endif
