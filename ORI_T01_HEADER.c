/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. Almeida
 *
 * Trabalho 01 - Indexação
 *
 * ========================================================================== *
 *   Nome: Martin Henrique Viana Adam
 *   RA: 759526
 *   Data: 28/09/2025
 * ========================================================================== */

// Bibliotecas
#include "ORI_T01_HEADER.h"
#include "utils.h"


// Defina aqui protótipos de funções auxiliares e macros (se quiser)


// Arquivos de dados
char ARQUIVO_TREINADORES[TAM_ARQUIVO_TREINADORES];
char ARQUIVO_BOLSOMONS[TAM_ARQUIVO_BOLSOMONS];
char ARQUIVO_BATALHAS[TAM_ARQUIVO_BATALHAS];
char ARQUIVO_RESULTADOS[TAM_ARQUIVO_RESULTADOS];
char ARQUIVO_TREINADOR_POSSUI_BOLSOMON[TAM_ARQUIVO_TREINADOR_POSSUI_BOLSOMON];


// Índices
treinadores_index *treinadores_idx = NULL;
bolsomons_index *bolsomons_idx = NULL;
batalhas_index *batalhas_idx = NULL;
resultados_index* resultados_idx = NULL;
treinador_possui_bolsomon_index *treinador_possui_bolsomon_idx = NULL;
preco_bolsomon_index *preco_bolsomon_idx = NULL;
data_index *data_idx = NULL;
inverted_list treinador_bolsomons_idx = {
        .treinador_bolsomons_secundario_idx = NULL,
        .treinador_bolsomons_primario_idx = NULL,
        .qtd_registros_secundario = 0,
        .qtd_registros_primario = 0,
        .tam_chave_secundaria = TAM_CHAVE_TREINADOR_BOLSOMON_SECUNDARIO_IDX,
        .tam_chave_primaria = TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX,
};


// Contadores
unsigned qtd_registros_treinadores = 0;
unsigned qtd_registros_bolsomons = 0;
unsigned qtd_registros_batalhas = 0;
unsigned qtd_registros_resultados = 0;
unsigned qtd_registros_treinador_possui_bolsomon = 0;

// ---------------- Funções de comparação ----------------

/* Função de comparação entre chaves do índice treinadores_idx */
int qsort_treinadores_idx(const void *a, const void *b) {
	return strcmp(((treinadores_index*)a)->id_treinador, ((treinadores_index*)b)->id_treinador);
}

/* Função de comparação entre chaves do índice bolsomons_idx */
int qsort_bolsomons_idx(const void *a, const void *b) {
	// Compara dois índices de bolsomon pelo id_bolsomon.
	return strcmp(((bolsomons_index*)a)->id_bolsomon, ((bolsomons_index*)b)->id_bolsomon);
}

/* Função de comparação entre chaves do índice batalhas_idx */
int qsort_batalhas_idx(const void *a, const void *b) {
	// Compara dois índices de batalha pelo id_batalha.
	return strcmp(((batalhas_index*)a)->id_batalha, ((batalhas_index*)b)->id_batalha);
}

/* Função de comparação entre chaves do índice resultados_idx */
int qsort_resultados_idx(const void *a, const void *b) {
	int cmp = strcmp(((resultados_index*)a)->id_treinador, ((resultados_index*)b)->id_treinador);
	if (cmp != 0) {
		return cmp; // Se forem diferentes, já temos o resultado.
	}
	// Se os IDs de treinador forem iguais, o desempate é pelo id_batalha.
	return strcmp(((resultados_index*)a)->id_batalha, ((resultados_index*)b)->id_batalha);
}

/* Função de comparação entre chaves do índice treinador_possui_bolsomon_idx */
int qsort_treinador_possui_bolsomon_idx(const void *a, const void *b) {
	int cmp = strcmp(((treinador_possui_bolsomon_index*)a)->id_treinador, ((treinador_possui_bolsomon_index*)b)->id_treinador);
	if (cmp != 0) {
		return cmp;
	}
	// Se os IDs de treinador forem iguais, o desempate é pelo id_bolsomon.
	return strcmp(((treinador_possui_bolsomon_index*)a)->id_bolsomon, ((treinador_possui_bolsomon_index*)b)->id_bolsomon);
}

/* Função de comparação entre chaves do índice preco_bolsomon_idx */
int qsort_preco_bolsomon_idx(const void *a, const void *b){
	preco_bolsomon_index *idx_a = (preco_bolsomon_index*)a;
	preco_bolsomon_index *idx_b = (preco_bolsomon_index*)b;

	// Compara os preços dos bolsomons.
	if (idx_a->preco_bolsobolas < idx_b->preco_bolsobolas) {
		return -1;
	}
	if (idx_a->preco_bolsobolas > idx_b->preco_bolsobolas) {
		return 1;
	}
	// Se os preços forem iguais, o desempate é pelo id_bolsomon.
	return strcmp(idx_a->id_bolsomon, idx_b->id_bolsomon);
}

/* Função usada na comparação entre as chaves do índice data_idx */
int qsort_data_idx(const void *a, const void *b) {
	int cmp = strcmp(((data_index*)a)->inicio, ((data_index*)b)->inicio);
	if (cmp != 0) {
		return cmp;
	}
	// Se as datas forem iguais, o desempate é pelo id_batalha.
	return strcmp(((data_index*)a)->id_batalha, ((data_index*)b)->id_batalha);
}

/* Função de comparação para a struct Info_Treinador, usada para determinar o campeão. */
int qsort_info_treinador(const void *a, const void *b) {
	Info_Treinador *treinador_a = (Info_Treinador*)a;
	Info_Treinador *treinador_b = (Info_Treinador*)b;

	// A comparação é decrescente para as vitórias, então invertemos a lógica (b - a).
	
	// (a) Maior placar de tempo
	if (treinador_b->vezes_mais_tempo != treinador_a->vezes_mais_tempo) {
		return treinador_b->vezes_mais_tempo - treinador_a->vezes_mais_tempo;
	}
	// (b) Maior placar de derrotas
	if (treinador_b->vezes_mais_derrotados != treinador_a->vezes_mais_derrotados) {
		return treinador_b->vezes_mais_derrotados - treinador_a->vezes_mais_derrotados;
	}
	// (c) Maior placar de dano
	if (treinador_b->vezes_mais_dano != treinador_a->vezes_mais_dano) {
		return treinador_b->vezes_mais_dano - treinador_a->vezes_mais_dano;
	}
	// (d) Menor valor de id_treinador
	// A comparação é crescente para o ID (a vs b).
	return strcmp(treinador_a->id_treinador, treinador_b->id_treinador);
}

/* Função de comparação entre chaves do índice secundário de treinador_bolsomons_secundario_idx */
int qsort_treinador_bolsomons_secundario_idx(const void *a, const void *b) {
	return strcmp(((treinador_bolsomons_secundario_index*)a)->chave_secundaria, ((treinador_bolsomons_secundario_index*)b)->chave_secundaria);
}

/* Função de comparação entre chaves do índice primário da lista invertida */
int qsort_inverted_list_primary_search(const void *a, const void *b) {
	return strcmp(((treinador_bolsomons_primario_index*)a)->chave_primaria, ((treinador_bolsomons_primario_index*)b)->chave_primaria);
}

// ---------------- Criação do índice respectivo ----------------

void criar_treinadores_idx() {
	if(!treinadores_idx)
		treinadores_idx = malloc(MAX_REGISTROS * sizeof(treinadores_index));

	if(!treinadores_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_treinadores; i++) {
		Treinador t = recuperar_registro_treinador(i);

		treinadores_idx[i].rrn = (strncmp(t.id_treinador, "*|", 2)) ? i : -1;

		strcpy(treinadores_idx[i].id_treinador, t.id_treinador);
	}

	qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	printf(INDICE_CRIADO, "treinadores_idx");
}

void criar_bolsomons_idx() {
	// Aloca memória para o índice se ainda não foi alocado.
	if(!bolsomons_idx)
		bolsomons_idx = malloc(MAX_REGISTROS * sizeof(bolsomons_index));

	if(!bolsomons_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	// Itera sobre todos os registros de bolsomons.
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++) {
		Bolsomon b = recuperar_registro_bolsomon(i);
		
		// Assume-se que bolsomons não podem ser removidos, então RRN é sempre o índice 'i'.
		bolsomons_idx[i].rrn = i;
		strcpy(bolsomons_idx[i].id_bolsomon, b.id_bolsomon);
	}

	// Ordena o índice.
	qsort(bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx);
	printf(INDICE_CRIADO, "bolsomons_idx");
}

void criar_batalhas_idx() {
	if(!batalhas_idx)
		batalhas_idx = malloc(MAX_REGISTROS * sizeof(batalhas_index));

	if(!batalhas_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_batalhas; i++) {
		Batalha b = recuperar_registro_batalha(i);
		batalhas_idx[i].rrn = i;
		strcpy(batalhas_idx[i].id_batalha, b.id_batalha);
	}

	qsort(batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx);
	printf(INDICE_CRIADO, "batalhas_idx");
}

void criar_resultados_idx(){
	if(!resultados_idx)
		resultados_idx = malloc(MAX_REGISTROS * sizeof(resultados_index));

	if(!resultados_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_resultados; i++) {
		Resultado r = recuperar_registro_resultado(i);
		resultados_idx[i].rrn = i;
		strcpy(resultados_idx[i].id_treinador, r.id_treinador);
		strcpy(resultados_idx[i].id_batalha, r.id_batalha);
	}

	qsort(resultados_idx, qtd_registros_resultados, sizeof(resultados_index), qsort_resultados_idx);
	printf(INDICE_CRIADO, "resultados_idx");
}

void criar_treinador_possui_bolsomon_idx(){
	if(!treinador_possui_bolsomon_idx)
		treinador_possui_bolsomon_idx = malloc(MAX_REGISTROS * sizeof(treinador_possui_bolsomon_index));

	if(!treinador_possui_bolsomon_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}
	
	for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++) {
		TreinadorPossuiBolsomon tpb = recuperar_registro_treinador_possui_bolsomon(i);
		treinador_possui_bolsomon_idx[i].rrn = i;
		strcpy(treinador_possui_bolsomon_idx[i].id_treinador, tpb.id_treinador);
		strcpy(treinador_possui_bolsomon_idx[i].id_bolsomon, tpb.id_bolsomon);
	}

	qsort(treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx);
	printf(INDICE_CRIADO, "treinador_possui_bolsomon_idx");
}

void criar_preco_bolsomon_idx() {
	if(!preco_bolsomon_idx)
		preco_bolsomon_idx = malloc(MAX_REGISTROS * sizeof(preco_bolsomon_index));

	if(!preco_bolsomon_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_bolsomons; i++) {
		Bolsomon b = recuperar_registro_bolsomon(i);
		preco_bolsomon_idx[i].preco_bolsobolas = b.preco_bolsobolas;
		strcpy(preco_bolsomon_idx[i].id_bolsomon, b.id_bolsomon);
	}

	qsort(preco_bolsomon_idx, qtd_registros_bolsomons, sizeof(preco_bolsomon_index), qsort_preco_bolsomon_idx);
	printf(INDICE_CRIADO, "preco_bolsomon_idx");
}

void criar_data_idx() {
	if(!data_idx)
		data_idx = malloc(MAX_REGISTROS * sizeof(data_index));
	
	if(!data_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_batalhas; i++) {
		Batalha b = recuperar_registro_batalha(i);
		strcpy(data_idx[i].inicio, b.inicio);
		strcpy(data_idx[i].id_batalha, b.id_batalha);
	}

	qsort(data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx);
	printf(INDICE_CRIADO, "data_idx");
}

void criar_treinador_bolsomons_idx() {
	// Apelido para a struct da lista invertida para facilitar a leitura.
	inverted_list *t = &treinador_bolsomons_idx;

	// Aloca memória para os dois vetores da lista invertida.
	if(!t->treinador_bolsomons_secundario_idx)
		t->treinador_bolsomons_secundario_idx = malloc(MAX_REGISTROS * sizeof(treinador_bolsomons_secundario_index));
	if(!t->treinador_bolsomons_primario_idx)
		t->treinador_bolsomons_primario_idx = malloc(MAX_REGISTROS * sizeof(treinador_bolsomons_primario_index));

	// Itera sobre a tabela de associação 'treinador_possui_bolsomon'.
	for (unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; ++i) {
		TreinadorPossuiBolsomon tpb = recuperar_registro_treinador_possui_bolsomon(i);

		// Para cada associação, precisamos do nome do bolsomon.
		// Vamos buscá-lo usando o índice de bolsomons.
		bolsomons_index key_b;
		strcpy(key_b.id_bolsomon, tpb.id_bolsomon);
		bolsomons_index *found_b = bsearch(&key_b, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx);
		
		// Se o bolsomon existir no índice...
		if (found_b) {
			Bolsomon b = recuperar_registro_bolsomon(found_b->rrn);
			// ...insere a chave secundária (nome) e a primária (id_treinador) na lista.
			inverted_list_insert(b.nome, tpb.id_treinador, t);
		}
	}
	
	// Ordena o índice secundário (o que contém os nomes) para permitir busca binária.
	qsort(t->treinador_bolsomons_secundario_idx, t->qtd_registros_secundario, sizeof(treinador_bolsomons_secundario_index), qsort_treinador_bolsomons_secundario_idx);
	printf(INDICE_CRIADO, "treinador_bolsomons_idx");
}

// ---------------- Recuperação do registro ----------------

Treinador recuperar_registro_treinador(int rrn) {
	Treinador t;
	char buffer[TAM_REGISTRO_TREINADOR + 1], *p;

	strncpy(buffer, ARQUIVO_TREINADORES + (rrn * TAM_REGISTRO_TREINADOR), TAM_REGISTRO_TREINADOR);
	buffer[TAM_REGISTRO_TREINADOR] = '\0';

	p = strtok(buffer, ";");
	strcpy(t.id_treinador, p);

	p = strtok(NULL, ";");
	strcpy(t.apelido, p);

	p = strtok(NULL, ";");
	strcpy(t.cadastro, p);

	p = strtok(NULL, ";");
	strcpy(t.premio, p);

	p = strtok(NULL, ";");
	t.bolsobolas = atof(p);

	return t;
}

Bolsomon recuperar_registro_bolsomon(int rrn) {
	Bolsomon b;
	char buffer[TAM_REGISTRO_BOLSOMON + 1], *p;
	
	strncpy(buffer, ARQUIVO_BOLSOMONS + (rrn * TAM_REGISTRO_BOLSOMON), TAM_REGISTRO_BOLSOMON);
	buffer[TAM_REGISTRO_BOLSOMON] = '\0';
	
	p = strtok(buffer, ";");
	strcpy(b.id_bolsomon, p);
	
	p = strtok(NULL, ";");
	strcpy(b.nome, p);
	
	p = strtok(NULL, ";");
	strcpy(b.habilidade, p);
	
	p = strtok(NULL, ";");
	b.preco_bolsobolas = atof(p);
	
	return b;
}

Batalha recuperar_registro_batalha(int rrn) {
	Batalha b;
	// Usa sscanf para ler os campos de tamanho fixo.
	sscanf(ARQUIVO_BATALHAS + rrn * TAM_REGISTRO_BATALHA, "%8s%12s%6s%4s", 
		   b.id_batalha, b.inicio, b.duracao, b.arena);
	
	// Adiciona terminadores nulos manualmente.
	b.id_batalha[8] = '\0';
	b.inicio[12] = '\0';
	b.duracao[6] = '\0';
	b.arena[4] = '\0';
	
	return b;
}

Resultado recuperar_registro_resultado(int rrn) {
	Resultado r;
	char foi_maior_duracao_str[2], foi_mais_derrotas_str[2], foi_mais_dano_str[2];

	// Usa sscanf para ler os campos de tamanho fixo.
	sscanf(ARQUIVO_RESULTADOS + rrn * TAM_REGISTRO_RESULTADO, "%11s%8s%3s%1s%1s%1s",
		   r.id_treinador, r.id_batalha, r.id_bolsomon, 
		   foi_maior_duracao_str, foi_mais_derrotas_str, foi_mais_dano_str);

	// Adiciona terminadores nulos manualmente.
	r.id_treinador[11] = '\0';
	r.id_batalha[8] = '\0';
	r.id_bolsomon[3] = '\0';
	foi_maior_duracao_str[1] = '\0';
	foi_mais_derrotas_str[1] = '\0';
	foi_mais_dano_str[1] = '\0';

	// Converte '0' ou '1' para os booleanos (false/true).
	r.foi_maior_duracao = atoi(foi_maior_duracao_str);
	r.foi_mais_derrotas = atoi(foi_mais_derrotas_str);
	r.foi_mais_dano = atoi(foi_mais_dano_str);
	
	return r;
}

TreinadorPossuiBolsomon recuperar_registro_treinador_possui_bolsomon(int rrn) {
	TreinadorPossuiBolsomon tpb;
	
	// Lê os dois campos de tamanho fixo diretamente da string global.
	sscanf(ARQUIVO_TREINADOR_POSSUI_BOLSOMON + rrn * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON, "%11s%3s",
		   tpb.id_treinador, tpb.id_bolsomon);

	// Adiciona terminadores nulos.
	tpb.id_treinador[11] = '\0';
	tpb.id_bolsomon[3] = '\0';

	return tpb;
}

// ---------------- Gravação nos arquivos de dados ----------------

void escrever_registro_treinador(Treinador t, int rrn) {
	char buffer[TAM_REGISTRO_TREINADOR + 1], double_str[100];
	buffer[0] = '\0';
	double_str[0] = '\0';

	strcat(buffer, t.id_treinador); strcat(buffer, ";");
	strcat(buffer, t.apelido); strcat(buffer, ";");
	strcat(buffer, t.cadastro); strcat(buffer, ";");
    strcat(buffer, t.premio); strcat(buffer, ";");
	sprintf(double_str, "%013.2lf", t.bolsobolas);
	strcat(buffer, double_str);	strcat(buffer, ";");
    
	strpadright(buffer, '#', TAM_REGISTRO_TREINADOR);
    
	strncpy(ARQUIVO_TREINADORES + rrn * TAM_REGISTRO_TREINADOR, buffer, TAM_REGISTRO_TREINADOR);
}

void escrever_registro_bolsomon(Bolsomon b, int rrn) {
	char buffer[TAM_REGISTRO_BOLSOMON + 1];
	buffer[0] = '\0';
	
	sprintf(buffer, "%s;%s;%s;%013.2lf;",
		b.id_bolsomon, b.nome, b.habilidade, b.preco_bolsobolas);
	
	strpadright(buffer, '#', TAM_REGISTRO_BOLSOMON);
	
	strncpy(ARQUIVO_BOLSOMONS + rrn * TAM_REGISTRO_BOLSOMON, buffer, TAM_REGISTRO_BOLSOMON);
}

void escrever_registro_batalha(Batalha b, int rrn) {
	char buffer[TAM_REGISTRO_BATALHA + 1];
	
	// Formata a string com os campos de tamanho fixo concatenados.
	sprintf(buffer, "%-8s%-12s%-6s%-4s", 
		b.id_batalha, b.inicio, b.duracao, b.arena);
	
	strncpy(ARQUIVO_BATALHAS + rrn * TAM_REGISTRO_BATALHA, buffer, TAM_REGISTRO_BATALHA);
}

void escrever_registro_resultado(Resultado r, int rrn) {
	char buffer[TAM_REGISTRO_RESULTADO + 1];
	
	sprintf(buffer, "%-11s%-8s%-3s%d%d%d",
		r.id_treinador, r.id_batalha, r.id_bolsomon,
		r.foi_maior_duracao, r.foi_mais_derrotas, r.foi_mais_dano);

	strncpy(ARQUIVO_RESULTADOS + rrn * TAM_REGISTRO_RESULTADO, buffer, TAM_REGISTRO_RESULTADO);
}

void escrever_registro_treinador_possui_bolsomon(TreinadorPossuiBolsomon tpb, int rrn) {
	char buffer[TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON + 1];
	
	sprintf(buffer, "%-11s%-3s", 
		tpb.id_treinador, tpb.id_bolsomon);
	
	strncpy(ARQUIVO_TREINADOR_POSSUI_BOLSOMON + rrn * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON, buffer, TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON);
}

// ---------------- Exibição dos registros ----------------

bool exibir_treinador(int rrn) {
	if(rrn < 0) return false;
    
	Treinador t = recuperar_registro_treinador(rrn);
    
	printf("%s, %s, %s, %s, %.2lf\n", t.id_treinador, t.apelido, t.cadastro, t.premio, t.bolsobolas);
    
	return true;
}

bool exibir_bolsomon(int rrn) {
	// Se o RRN for inválido (como -1 para registros removidos), não exibe nada.
	if(rrn < 0) return false;

	// Recupera o registro do bolsomon.
	Bolsomon b = recuperar_registro_bolsomon(rrn);

	// Imprime os dados de forma formatada.
	printf("%s, %s, %s, %.2lf\n", b.id_bolsomon, b.nome, b.habilidade, b.preco_bolsobolas);

	return true;
}

bool exibir_batalha(int rrn) {
	if(rrn < 0) return false;

	Batalha b = recuperar_registro_batalha(rrn);
	
	printf("%s, %s, %s, %s\n", b.id_batalha, b.inicio, b.duracao, b.arena);

	return true;
}

// ---------------- Funções principais ----------------

void cadastrar_treinador_menu(char *id_treinador, char *apelido) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "cadastrar_treinador_menu()");
}

void remover_treinador_menu(char *id_treinador) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "remover_treinador_menu()");
}

void adicionar_bolsobolas_menu(char *id_treinador, double valor) {
	adicionar_bolsobolas(id_treinador, valor, true);
}

void adicionar_bolsobolas(char *id_treinador, double valor, bool flag){
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "adicionar_bolsobolas()");
}

void cadastrar_bolsomon_menu(char *nome, char *habilidade, double preco) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "cadastrar_bolsomon_menu()");
}

void comprar_bolsomon_menu(char *id_treinador, char *id_bolsomon) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "comprar_bolsomon_menu()");
}

void executar_batalha_menu(char *inicio, char *duracao, char *arena, char *id_treinadores, char *bolsomons_treinadores, char *duracoes_treinadores, char *eliminacoes_treinadores, char *danos_causados_treinadores) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "executar_batalha_menu()");
}

void recompensar_campeao_menu(char *data_inicio, char *data_fim, double premio) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "recompensar_campeao_menu()");
}

// ---------------- Busca pelo índice ----------------

void buscar_treinador_id_menu(char *id_treinador) {
	treinadores_index index;
	strcpy(index.id_treinador, id_treinador);
	int found = busca_binaria((void*)&index, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, true, 0);
	if(found == -1 || treinadores_idx[found].rrn < 0)
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	else
		exibir_treinador(treinadores_idx[found].rrn);
}

void buscar_bolsomon_id_menu(char *id_bolsomon) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "buscar_bolsomon_id_menu()");
}

void buscar_batalha_id_menu(char *id_batalha) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "buscar_batalha_id_menu()");
}

// ---------------- Listagem dos registros ----------------

void listar_treinadores_id_menu() {
	if(!qtd_registros_treinadores){
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }
	for(unsigned int i = 0; i < qtd_registros_treinadores; i++)
		exibir_treinador(treinadores_idx[i].rrn);
}

void listar_treinadores_bolsomons_menu(char *bolsomon){
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "listar_treinadores_bolsomons_menu()");
}

void listar_bolsomons_compra_menu(char *id_treinador) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "listar_bolsomons_compra_menu()");
}

void listar_batalhas_periodo_menu(char *data_inicio, char *data_fim) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "listar_batalhas_periodo_menu()");
}

// ---------------- Exibição dos arquivos de dados ----------------

void imprimir_arquivo_treinadores_menu() {
	if(!qtd_registros_treinadores){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%s\n", ARQUIVO_TREINADORES);
}

void imprimir_arquivo_bolsomons_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_bolsomons_menu()");
}

void imprimir_arquivo_batalhas_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_batalhas_menu()");
}

void imprimir_arquivo_resultados_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_resultados_menu()");
}

void imprimir_arquivo_treinador_possui_bolsomon_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_arquivo_treinador_possui_bolsomon_menu()");
}

// ---------------- Exibição dos índices ----------------

void imprimir_treinadores_idx_menu() {
	if(!treinadores_idx || !qtd_registros_treinadores){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_treinadores; i++)
		printf("%s, %d\n", treinadores_idx[i].id_treinador, treinadores_idx[i].rrn);
}

void imprimir_bolsomons_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_bolsomons_idx_menu()");
}

void imprimir_batalhas_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_batalhas_idx_menu()");
}

void imprimir_resultados_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_resultados_idx_menu()");
}

void imprimir_treinador_possui_bolsomon_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_possui_bolsomon_idx_menu()");
}

void imprimir_preco_bolsomon_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_preco_bolsomon_idx_menu()");
}

void imprimir_data_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_data_idx_menu()");
}

void imprimir_treinador_bolsomons_secundario_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_bolsomons_secundario_idx_menu()");
}

void imprimir_treinador_bolsomons_primario_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_bolsomons_primario_idx_menu()");
}

// ---------------- Liberação de espaço e memória ----------------

void liberar_espaco_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "liberar_espaco_menu()");
}

void liberar_memoria_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "liberar_memoria_menu()");
}

// ---------------- Manipulação da lista invertida ----------------

void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_insert()");
}

bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_secondary_search()");
	return false;
}

int inverted_list_primary_search(char result[][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_primary_search()");
	return 0;
}

// ---------------- Busca binária ----------------

int busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "busca_binaria()");
	return -1;
}

// ---------------- Funções SET de arquivos ----------------

void set_arquivo_treinadores(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_TREINADORES, novos_dados, tamanho);
    qtd_registros_treinadores = tamanho / TAM_REGISTRO_TREINADOR;
}

void set_arquivo_bolsomons(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_BOLSOMONS, novos_dados, tamanho);
    qtd_registros_bolsomons = tamanho / TAM_REGISTRO_BOLSOMON;
}

void set_arquivo_batalhas(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_BATALHAS, novos_dados, tamanho);
    qtd_registros_batalhas = tamanho / TAM_REGISTRO_BATALHA;
}

void set_arquivo_resultados(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_RESULTADOS, novos_dados, tamanho);
    qtd_registros_resultados = tamanho / TAM_REGISTRO_RESULTADO;
}

void set_arquivo_treinador_possui_bolsomon(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_TREINADOR_POSSUI_BOLSOMON, novos_dados, tamanho);
    qtd_registros_treinador_possui_bolsomon = tamanho / TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON;
}

// ---------------- Implementação das suas próprias funções auxiliares ----------------

