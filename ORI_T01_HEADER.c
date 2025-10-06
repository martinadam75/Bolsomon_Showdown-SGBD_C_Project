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

void copiar_e_converter_para_maiusculo(char *destino, const char *origem);
void reconstruir_treinadores_idx();
int qsort_string_cmp(const void *a, const void *b); 


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
	return strcmp(((bolsomons_index*)a)->id_bolsomon, ((bolsomons_index*)b)->id_bolsomon);
}

/* Função de comparação entre chaves do índice batalhas_idx */
int qsort_batalhas_idx(const void *a, const void *b) {
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
    reconstruir_treinadores_idx();
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
	// Verificação de duplicidade usando o índice.
	treinadores_index key;
	strcpy(key.id_treinador, id_treinador);
	
	// Busca sem exibir o caminho, pois é uma verificação interna.
	int found_idx = busca_binaria(&key, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

	// Se encontrou um registro e ele NÃO está marcado como removido (rrn != -1).
	if (found_idx != -1 && treinadores_idx[found_idx].rrn != -1) {
		printf(ERRO_PK_REPETIDA, id_treinador);
		return;
	}

	// Cria o novo registro.
	Treinador t;
	strcpy(t.id_treinador, id_treinador);
	strcpy(t.apelido, apelido);
	t.bolsobolas = 0.0; 
	current_datetime(t.cadastro); 
	strcpy(t.premio, "000000000000");

	// O novo registro vai para o final do arquivo, no RRN atual.
	int new_rrn = qtd_registros_treinadores;
	escrever_registro_treinador(t, new_rrn);

	// Adiciona a nova entrada de índice.
	strcpy(treinadores_idx[new_rrn].id_treinador, id_treinador);
	treinadores_idx[new_rrn].rrn = new_rrn;
	
	// Incrementa o contador de registros.
	qtd_registros_treinadores++;
	
	// Reordena o índice inteiro com o novo elemento.
	qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	
	printf(SUCESSO);
}

void remover_treinador_menu(char *id_treinador) {
	// Busca o treinador no índice.
	treinadores_index key;
	strcpy(key.id_treinador, id_treinador);

	// Não precisa exibir o caminho da busca.
	int found_idx = busca_binaria(&key, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

	// Valida se o registro existe e não foi removido.
	if (found_idx == -1 || treinadores_idx[found_idx].rrn == -1) {
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Modifica o registro no "arquivo" de dados.
	int rrn_to_remove = treinadores_idx[found_idx].rrn;
	memcpy(ARQUIVO_TREINADORES + rrn_to_remove * TAM_REGISTRO_TREINADOR, "*|", 2);

	// Atualiza o índice primário.
	treinadores_idx[found_idx].rrn = -1;
	
	printf(SUCESSO);
}

void adicionar_bolsobolas_menu(char *id_treinador, double valor) {
	adicionar_bolsobolas(id_treinador, valor, true);
}

void adicionar_bolsobolas(char *id_treinador, double valor, bool flag){
	// Validação do valor a ser adicionado.
	if (valor <= 0) {
		if (flag) printf(ERRO_VALOR_INVALIDO);
		return;
	}

	// Busca pelo treinador no índice.
	treinadores_index key;
	strcpy(key.id_treinador, id_treinador);
	int found_idx = busca_binaria(&key, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

	// Valida se o registro existe e não foi removido.
	if (found_idx == -1 || treinadores_idx[found_idx].rrn == -1) {
		if (flag) printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	// Recupera o registro, atualiza o valor e reescreve no "arquivo".
	int rrn_to_update = treinadores_idx[found_idx].rrn;
	Treinador t = recuperar_registro_treinador(rrn_to_update);
	t.bolsobolas += valor;
	escrever_registro_treinador(t, rrn_to_update);

	if (flag) printf(SUCESSO);
}

void cadastrar_bolsomon_menu(char *nome, char *habilidade, double preco) {
	// Geração do novo ID.
	unsigned new_id_int = qtd_registros_bolsomons;
	char new_id_str[TAM_ID_BOLSOMON];
	sprintf(new_id_str, "%03u", new_id_int); // Formata como string de 3 dí­gitos (ex: "007").

	// Preparação do novo registro de bolsomon.
	Bolsomon b;
	strcpy(b.id_bolsomon, new_id_str);
	strcpy(b.nome, nome);
	strcpy(b.habilidade, habilidade);
	b.preco_bolsobolas = preco;
	
	// Gravação no "arquivo" de dados.
	int new_rrn = qtd_registros_bolsomons;
	escrever_registro_bolsomon(b, new_rrn);

	// Atualização dos índices.
	strcpy(bolsomons_idx[new_rrn].id_bolsomon, new_id_str);
	bolsomons_idx[new_rrn].rrn = new_rrn;

	// Índice secundário (preco_bolsomon_idx)
	strcpy(preco_bolsomon_idx[new_rrn].id_bolsomon, new_id_str);
	preco_bolsomon_idx[new_rrn].preco_bolsobolas = preco;

	// Incrementa o contador.
	qtd_registros_bolsomons++;

	// Reordena ambos os índices.
	qsort(bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx);
	qsort(preco_bolsomon_idx, qtd_registros_bolsomons, sizeof(preco_bolsomon_index), qsort_preco_bolsomon_idx);

	printf(SUCESSO);
}

void comprar_bolsomon_menu(char *id_treinador, char *id_bolsomon) {
	// Buscar e validar o treinador.
	treinadores_index key_t;
	strcpy(key_t.id_treinador, id_treinador);
	int found_t_idx = busca_binaria(&key_t, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

	if (found_t_idx == -1 || treinadores_idx[found_t_idx].rrn == -1) {
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}
	int rrn_treinador = treinadores_idx[found_t_idx].rrn;
	Treinador t = recuperar_registro_treinador(rrn_treinador);

	// Buscar e validar o bolsomon.
	bolsomons_index key_b;
	strcpy(key_b.id_bolsomon, id_bolsomon);
	int found_b_idx = busca_binaria(&key_b, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
	
	if (found_b_idx == -1) {
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}
	int rrn_bolsomon = bolsomons_idx[found_b_idx].rrn;
	Bolsomon b = recuperar_registro_bolsomon(rrn_bolsomon);

	// Verificar se o treinador já possui este bolsomon.
	treinador_possui_bolsomon_index key_tpb;
	strcpy(key_tpb.id_treinador, id_treinador);
	strcpy(key_tpb.id_bolsomon, id_bolsomon);
	int found_tpb_idx = busca_binaria(&key_tpb, treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx, false, 0);

	if (found_tpb_idx != -1) {
		printf(ERRO_BOLSOMON_REPETIDO, id_treinador, id_bolsomon);
		return;
	}

	// Verificar se o treinador tem saldo suficiente.
	if (t.bolsobolas < b.preco_bolsobolas) {
		printf(ERRO_SALDO_NAO_SUFICIENTE);
		return;
	}

	// --- Se todas as validações passaram, efetua a transação ---

	// Deduz o custo do bolsomon do saldo do treinador e atualiza o registro.
	t.bolsobolas -= b.preco_bolsobolas;
	escrever_registro_treinador(t, rrn_treinador);

	// Insere o novo registro de posse.
	TreinadorPossuiBolsomon tpb;
	strcpy(tpb.id_treinador, id_treinador);
	strcpy(tpb.id_bolsomon, id_bolsomon);
	int new_rrn_tpb = qtd_registros_treinador_possui_bolsomon;
	escrever_registro_treinador_possui_bolsomon(tpb, new_rrn_tpb);

	// Atualiza os índices de posse.
	// Índice primário 'treinador_possui_bolsomon_idx'
	strcpy(treinador_possui_bolsomon_idx[new_rrn_tpb].id_treinador, id_treinador);
	strcpy(treinador_possui_bolsomon_idx[new_rrn_tpb].id_bolsomon, id_bolsomon);
	treinador_possui_bolsomon_idx[new_rrn_tpb].rrn = new_rrn_tpb;
	qtd_registros_treinador_possui_bolsomon++;
	qsort(treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx);

	// Lista invertida 'treinador_bolsomons_idx'
	inverted_list_insert(b.nome, id_treinador, &treinador_bolsomons_idx);
	// Como a inserção pode quebrar a ordem do índice secundário, ele precisa ser reordenado.
	qsort(treinador_bolsomons_idx.treinador_bolsomons_secundario_idx, treinador_bolsomons_idx.qtd_registros_secundario, sizeof(treinador_bolsomons_secundario_index), qsort_treinador_bolsomons_secundario_idx);

	printf(SUCESSO);
}

void executar_batalha_menu(char *inicio, char *duracao, char *arena, char *id_treinadores_str, char *bolsomons_treinadores_str, char *duracoes_treinadores_str, char *eliminacoes_treinadores_str, char *danos_causados_treinadores_str) {
	
	// Struct auxiliar para armazenar os dados dos 12 participantes antes de validar e gravar.
	struct BatalhaParticipante {
		char id_treinador[TAM_ID_TREINADOR];
		char id_bolsomon[TAM_ID_BOLSOMON];
		int duracao_segundos;
		int eliminacoes;
		int danos;
	} participantes[QTD_MAX_TREINADORES];

	// --- Parse e pré-validação dos dados dos participantes ---

	for (int i = 0; i < QTD_MAX_TREINADORES; i++) {
		// Extrai os dados de tamanho fixo das strings gigantes
		strncpy(participantes[i].id_treinador, id_treinadores_str + i * (TAM_ID_TREINADOR - 1), TAM_ID_TREINADOR - 1);
		participantes[i].id_treinador[TAM_ID_TREINADOR - 1] = '\0';

		strncpy(participantes[i].id_bolsomon, bolsomons_treinadores_str + i * (TAM_ID_BOLSOMON - 1), TAM_ID_BOLSOMON - 1);
		participantes[i].id_bolsomon[TAM_ID_BOLSOMON - 1] = '\0';

		char duracao_str[TAM_TIME];
		strncpy(duracao_str, duracoes_treinadores_str + i * (TAM_TIME - 1), TAM_TIME - 1);
		duracao_str[TAM_TIME - 1] = '\0';

		char eliminacoes_str[TAM_INT_NUMBER];
		strncpy(eliminacoes_str, eliminacoes_treinadores_str + i * (TAM_INT_NUMBER-1), TAM_INT_NUMBER-1);
		eliminacoes_str[TAM_INT_NUMBER - 1] = '\0';

		char danos_str[TAM_INT_NUMBER];
		strncpy(danos_str, danos_causados_treinadores_str + i * (TAM_INT_NUMBER - 1), TAM_INT_NUMBER - 1);
		danos_str[TAM_INT_NUMBER - 1] = '\0';

		// Converte os valores lidos
		int h, m, s;
		sscanf(duracao_str, "%2d%2d%2d", &h, &m, &s);
		participantes[i].duracao_segundos = h * 3600 + m * 60 + s;
		participantes[i].eliminacoes = atoi(eliminacoes_str);
		participantes[i].danos = atoi(danos_str);

		// Validações de integridade
		// a) O treinador existe e não foi removido?
		treinadores_index key_t;
		strcpy(key_t.id_treinador, participantes[i].id_treinador);
		int found_t_idx = busca_binaria(&key_t, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
		if (found_t_idx == -1 || treinadores_idx[found_t_idx].rrn == -1) {
			printf(ERRO_REGISTRO_NAO_ENCONTRADO);
			return;
		}

		// b) O treinador possui o bolsomon informado?
		treinador_possui_bolsomon_index key_tpb;
		strcpy(key_tpb.id_treinador, participantes[i].id_treinador);
		strcpy(key_tpb.id_bolsomon, participantes[i].id_bolsomon);
		int found_tpb_idx = busca_binaria(&key_tpb, treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx, false, 0);
		if (found_tpb_idx == -1) {
			bolsomons_index key_b;
			strcpy(key_b.id_bolsomon, participantes[i].id_bolsomon);
			int found_b_idx = busca_binaria(&key_b, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
			Bolsomon b = recuperar_registro_bolsomon(bolsomons_idx[found_b_idx].rrn);

			printf(ERRO_TREINADOR_BOLSOMON, participantes[i].id_treinador, b.nome);
			return;
		}

		// Avança para os próximos tokens nas strings.
	}

	// --- Determinação dos vencedores de cada categoria ---
	int idx_maior_duracao = 0;
	int idx_mais_derrotas = 0;
	int idx_mais_dano = 0;

	for (int i = 1; i < QTD_MAX_TREINADORES; i++) {
		// Encontra o índice do participante com a maior duração
		if (participantes[i].duracao_segundos > participantes[idx_maior_duracao].duracao_segundos) {
			idx_maior_duracao = i;
		}
		// Encontra o índice do participante com mais derrotas
		if (participantes[i].eliminacoes > participantes[idx_mais_derrotas].eliminacoes) {
			idx_mais_derrotas = i;
		}
		// Encontra o índice do participante com mais dano
		if (participantes[i].danos > participantes[idx_mais_dano].danos) {
			idx_mais_dano = i;
		}
	}

	// --- Inserção da Batalha ---
	char new_batalha_id[TAM_ID_BATALHA];
	sprintf(new_batalha_id, "%08u", qtd_registros_batalhas);

	Batalha b;
	strcpy(b.id_batalha, new_batalha_id);
	strcpy(b.inicio, inicio);
	strcpy(b.duracao, duracao);
	strcpy(b.arena, arena);
	
	int new_rrn_batalha = qtd_registros_batalhas;
	escrever_registro_batalha(b, new_rrn_batalha);

	// Atualiza índices da batalha
	strcpy(batalhas_idx[new_rrn_batalha].id_batalha, new_batalha_id);
	batalhas_idx[new_rrn_batalha].rrn = new_rrn_batalha;
	strcpy(data_idx[new_rrn_batalha].id_batalha, new_batalha_id);
	strcpy(data_idx[new_rrn_batalha].inicio, inicio);
	qtd_registros_batalhas++;
	qsort(batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx);
	qsort(data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx);

	// --- Inserção dos 12 Resultados ---
	for (int i = 0; i < QTD_MAX_TREINADORES; i++) {
		Resultado r;
		strcpy(r.id_treinador, participantes[i].id_treinador);
		strcpy(r.id_batalha, new_batalha_id);
		strcpy(r.id_bolsomon, participantes[i].id_bolsomon);
		r.foi_maior_duracao = (i == idx_maior_duracao);
		r.foi_mais_derrotas = (i == idx_mais_derrotas);
		r.foi_mais_dano = (i == idx_mais_dano);

		int new_rrn_resultado = qtd_registros_resultados;
		escrever_registro_resultado(r, new_rrn_resultado);

		// Adiciona ao índice de resultados
		strcpy(resultados_idx[new_rrn_resultado].id_treinador, r.id_treinador);
		strcpy(resultados_idx[new_rrn_resultado].id_batalha, r.id_batalha);
		resultados_idx[new_rrn_resultado].rrn = new_rrn_resultado;
		qtd_registros_resultados++;
	}
	// Reordena o índice de resultados UMA VEZ, após adicionar todos os 12.
	qsort(resultados_idx, qtd_registros_resultados, sizeof(resultados_index), qsort_resultados_idx);

	printf(SUCESSO);
}

void recompensar_campeao_menu(char *data_inicio, char *data_fim, double premio) {
	// Encontrar todas as batalhas no período especificado.
	data_index key_data;
	strcpy(key_data.inicio, data_inicio);
	int start_idx = busca_binaria(&key_data, data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx, false, 1);

	char batalhas_periodo[MAX_REGISTROS][TAM_ID_BATALHA];
	int qtd_batalhas_periodo = 0;
	for (unsigned i = start_idx; i < qtd_registros_batalhas; i++) {
		if (strcmp(data_idx[i].inicio, data_fim) > 0) {
			break;
		}
		strcpy(batalhas_periodo[qtd_batalhas_periodo++], data_idx[i].id_batalha);
	}

	if (qtd_batalhas_periodo == 0) {
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
		return;
	}
	// Ordena os IDs de batalha para permitir busca binária neles.
	qsort(batalhas_periodo, qtd_batalhas_periodo, TAM_ID_BATALHA, qsort_string_cmp);

	// Agregar os placares de todos os treinadores que participaram dessas batalhas.
	Info_Treinador placares[MAX_REGISTROS];
	int qtd_placares = 0;

	for (unsigned i = 0; i < qtd_registros_resultados; i++) {
		Resultado r = recuperar_registro_resultado(i);
		
		// Verifica se o resultado pertence a uma das batalhas do período.
		if (bsearch(r.id_batalha, batalhas_periodo, qtd_batalhas_periodo, TAM_ID_BATALHA, qsort_string_cmp)) {
			int p_idx = -1;
			// Procura o treinador na nossa lista de placares.
			for (int j = 0; j < qtd_placares; j++) {
				if (strcmp(placares[j].id_treinador, r.id_treinador) == 0) {
					p_idx = j;
					break;
				}
			}
			// Se não achou, adiciona um novo.
			if (p_idx == -1) {
				p_idx = qtd_placares++;
				strcpy(placares[p_idx].id_treinador, r.id_treinador);
				placares[p_idx].vezes_mais_tempo = 0;
				placares[p_idx].vezes_mais_derrotados = 0;
				placares[p_idx].vezes_mais_dano = 0;
			}
			// Atualiza os placares.
			if (r.foi_maior_duracao) placares[p_idx].vezes_mais_tempo++;
			if (r.foi_mais_derrotas) placares[p_idx].vezes_mais_derrotados++;
			if (r.foi_mais_dano) placares[p_idx].vezes_mais_dano++;
		}
	}
	
	if (qtd_placares == 0) {
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
		return;
	}

	// Ordena os placares para encontrar o campeão (o primeiro da lista).
	qsort(placares, qtd_placares, sizeof(Info_Treinador), qsort_info_treinador);

	// Encontra o primeiro treinador ATIVO na lista ordenada e o premia.
	for (int i = 0; i < qtd_placares; i++) {
		treinadores_index key_t;
		strcpy(key_t.id_treinador, placares[i].id_treinador);
		int found_t_idx = busca_binaria(&key_t, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

		if (found_t_idx != -1 && treinadores_idx[found_t_idx].rrn != -1) {
			// Encontrou um campeão ativo!
			int rrn_vencedor = treinadores_idx[found_t_idx].rrn;
			Treinador t = recuperar_registro_treinador(rrn_vencedor);

			// Se i > 0, significa que o(s) primeiro(s) colocado(s) foram removidos.
			if (i > 0) {
				printf(ERRO_TREINADOR_REMOVIDO, premio, t.apelido, placares[i].vezes_mais_tempo, placares[i].vezes_mais_derrotados, placares[i].vezes_mais_dano);
			} else {
				printf(CONCEDER_PREMIO, t.apelido, placares[i].vezes_mais_tempo, placares[i].vezes_mais_derrotados, placares[i].vezes_mais_dano, premio);
			}
			
			// Atualiza o saldo e a data do prêmio.
			t.bolsobolas += premio;
			strcpy(t.premio, data_fim);
			escrever_registro_treinador(t, rrn_vencedor);
			return; 
		}
	}
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
	bolsomons_index key;
	strcpy(key.id_bolsomon, id_bolsomon);

	// Chama a busca binária no índice de bolsomons, pedindo para exibir o caminho.
	int found_idx = busca_binaria(&key, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, true, 0);

	if (found_idx == -1) {
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	} else {
		// Se encontrou, usa o RRN do índice para exibir o registro completo.
		exibir_bolsomon(bolsomons_idx[found_idx].rrn);
	}
}

void buscar_batalha_id_menu(char *id_batalha) {
	batalhas_index key;
	strcpy(key.id_batalha, id_batalha);
	
	int found_idx = busca_binaria(&key, batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx, true, 0);

	if (found_idx == -1) {
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	} else {
		exibir_batalha(batalhas_idx[found_idx].rrn);
	}
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
	int primeiro_indice_primario;

	char chave_maiuscula[TAM_MAX_NOME_BOLSOMON];
	copiar_e_converter_para_maiusculo(chave_maiuscula, bolsomon);

	// Busca na lista invertida pelo nome do bolsomon
	bool found = inverted_list_secondary_search(&primeiro_indice_primario, true, chave_maiuscula, &treinador_bolsomons_idx);
	
	if (!found) {
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

	// Recupera a lista de IDs de treinadores do índice primário.
	char ids_encontrados[MAX_REGISTROS][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX];
	int qtd_ids = inverted_list_primary_search(ids_encontrados, true, primeiro_indice_primario, NULL, &treinador_bolsomons_idx);

	if (qtd_ids == 0) {
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
		return;
	}

	// Ordena os IDs de treinador recuperados
	qsort(ids_encontrados, qtd_ids, TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX, qsort_string_cmp);

	// Para cada ID ordenado, busca o registro completo do treinador e o exibe.
	for (int i = 0; i < qtd_ids; i++) {
        treinadores_index key_t;
        strcpy(key_t.id_treinador, ids_encontrados[i]);
        int found_t_idx = busca_binaria(&key_t, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

        if (found_t_idx != -1 && treinadores_idx[found_t_idx].rrn != -1) {
            exibir_treinador(treinadores_idx[found_t_idx].rrn);
        }
    }
}

void listar_bolsomons_compra_menu(char *id_treinador) {
	// Encontra o treinador e seu saldo.
	treinadores_index key_t;
	strcpy(key_t.id_treinador, id_treinador);
	int found_t_idx = busca_binaria(&key_t, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);

	if (found_t_idx == -1 || treinadores_idx[found_t_idx].rrn == -1) {
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}
	Treinador t = recuperar_registro_treinador(treinadores_idx[found_t_idx].rrn);
	double saldo = t.bolsobolas;

	// Itera sobre o índice de preços (que está ordenado do mais barato para o mais caro).
	bool encontrou_algum = false;
	for (unsigned i = 0; i < qtd_registros_bolsomons; i++) {
		// Se o preços do bolsomon atual já for maior que o saldo, a busca é interrompida.
		if (preco_bolsomon_idx[i].preco_bolsobolas > saldo) {
			break;
		}

		// O bolsomon é comprável. Precisamos buscar seu RRN para poder exibí-lo.
		bolsomons_index key_b;
		strcpy(key_b.id_bolsomon, preco_bolsomon_idx[i].id_bolsomon);
		int found_b_idx = busca_binaria(&key_b, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);

		if (found_b_idx != -1) {
			exibir_bolsomon(bolsomons_idx[found_b_idx].rrn);
			encontrou_algum = true;
		}
	}

	if (!encontrou_algum) {
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
	}
}

void listar_batalhas_periodo_menu(char *data_inicio, char *data_fim) {
	// Usa a busca binária para achar o primeiro registro >= data_inicio.
	data_index key_data;
	strcpy(key_data.inicio, data_inicio);

	int start_idx = busca_binaria(&key_data, data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx, true, 1);

	// Itera sobre o índice a partir do ponto encontrado.
	bool encontrou_algum = false;
	for (unsigned i = start_idx; i < qtd_registros_batalhas; i++) {
		// Compara a data da batalha atual com a data final do período.
		if (strcmp(data_idx[i].inicio, data_fim) > 0) {
			// Se a data atual já for maior que a data_fim, podemos parar.
			break;
		}

		// A batalha está dentro do período. Recupera seu RRN e exibe.
		batalhas_index key_b;
		strcpy(key_b.id_batalha, data_idx[i].id_batalha);
		int found_b_idx = busca_binaria(&key_b, batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx, true, 0);

		if (found_b_idx != -1) {
			exibir_batalha(batalhas_idx[found_b_idx].rrn);
			encontrou_algum = true;
		}
	}

	if (!encontrou_algum) {
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
	}
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
	if(!qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%.*s\n", qtd_registros_bolsomons * TAM_REGISTRO_BOLSOMON, ARQUIVO_BOLSOMONS);
}

void imprimir_arquivo_batalhas_menu() {
	if(!qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%.*s\n", qtd_registros_batalhas * TAM_REGISTRO_BATALHA, ARQUIVO_BATALHAS);
}

void imprimir_arquivo_resultados_menu() {
	if(!qtd_registros_resultados){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%.*s\n", qtd_registros_resultados * TAM_REGISTRO_RESULTADO, ARQUIVO_RESULTADOS);
}

void imprimir_arquivo_treinador_possui_bolsomon_menu() {
	if(!qtd_registros_treinador_possui_bolsomon){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%.*s\n", qtd_registros_treinador_possui_bolsomon * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON, ARQUIVO_TREINADOR_POSSUI_BOLSOMON);
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
	if(!bolsomons_idx || !qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++)
		printf("%s, %d\n", bolsomons_idx[i].id_bolsomon, bolsomons_idx[i].rrn);
}

void imprimir_batalhas_idx_menu() {
	if(!batalhas_idx || !qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_batalhas; i++)
		printf("%s, %d\n", batalhas_idx[i].id_batalha, batalhas_idx[i].rrn);
}

void imprimir_resultados_idx_menu() {
	if(!resultados_idx || !qtd_registros_resultados){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_resultados; i++)
		printf("%s, %s, %d\n", resultados_idx[i].id_treinador, resultados_idx[i].id_batalha, resultados_idx[i].rrn);
}

void imprimir_treinador_possui_bolsomon_idx_menu() {
	if(!treinador_possui_bolsomon_idx || !qtd_registros_treinador_possui_bolsomon){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++)
		printf("%s, %s, %d\n", treinador_possui_bolsomon_idx[i].id_treinador, treinador_possui_bolsomon_idx[i].id_bolsomon, treinador_possui_bolsomon_idx[i].rrn);
}

void imprimir_preco_bolsomon_idx_menu() {
	if(!preco_bolsomon_idx || !qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++)
		printf("%.2lf, %s\n", preco_bolsomon_idx[i].preco_bolsobolas, preco_bolsomon_idx[i].id_bolsomon);
}

void imprimir_data_idx_menu() {
	if(!data_idx || !qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_batalhas; i++)
		printf("%s, %s\n", data_idx[i].id_batalha, data_idx[i].inicio);
}

void imprimir_treinador_bolsomons_secundario_idx_menu() {
	inverted_list *t = &treinador_bolsomons_idx;
	if(!t->treinador_bolsomons_secundario_idx || t->qtd_registros_secundario == 0){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < t->qtd_registros_secundario; i++)
		printf("%s, %d\n", t->treinador_bolsomons_secundario_idx[i].chave_secundaria, t->treinador_bolsomons_secundario_idx[i].primeiro_indice);
}

void imprimir_treinador_bolsomons_primario_idx_menu() {
	inverted_list *t = &treinador_bolsomons_idx;
	if(!t->treinador_bolsomons_primario_idx || t->qtd_registros_primario == 0){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < t->qtd_registros_primario; i++)
		printf("%s, %d\n", t->treinador_bolsomons_primario_idx[i].chave_primaria, t->treinador_bolsomons_primario_idx[i].proximo_indice);
}

// ---------------- Liberação de espaço e memória ----------------

void liberar_espaco_menu() {
	// Cria um buffer temporário para o novo "arquivo" limpo.
	char temp_arquivo[TAM_ARQUIVO_TREINADORES];
	unsigned novo_qtd = 0;

	// Para manter a ordem original, precisamos varrer o arquivo original.
	for (unsigned i = 0; i < qtd_registros_treinadores; i++) {
		// Pega o registro diretamente do arquivo original.
		char* registro_atual = ARQUIVO_TREINADORES + i * TAM_REGISTRO_TREINADOR;

		// Se o registro não estiver marcado como removido copia ele para o arquivo temporário na nova posição.
		if (strncmp(registro_atual, "*|", 2) != 0) {
			memcpy(temp_arquivo + novo_qtd * TAM_REGISTRO_TREINADOR, registro_atual, TAM_REGISTRO_TREINADOR);
            novo_qtd++;
		}
	}

	// Substitui o arquivo antigo pelo novo.
	memcpy(ARQUIVO_TREINADORES, temp_arquivo, novo_qtd * TAM_REGISTRO_TREINADOR);
	qtd_registros_treinadores = novo_qtd;

	ARQUIVO_TREINADORES[novo_qtd * TAM_REGISTRO_TREINADOR] = '\0';

	// Reconstrói o índice do zero, pois os RRNs mudaram.
	reconstruir_treinadores_idx();

	printf(SUCESSO);
}

void liberar_memoria_menu() {
	if (treinadores_idx) free(treinadores_idx);
	if (bolsomons_idx) free(bolsomons_idx);
	if (batalhas_idx) free(batalhas_idx);
	if (resultados_idx) free(resultados_idx);
	if (treinador_possui_bolsomon_idx) free(treinador_possui_bolsomon_idx);
	if (preco_bolsomon_idx) free(preco_bolsomon_idx);
	if (data_idx) free(data_idx);
	if (treinador_bolsomons_idx.treinador_bolsomons_secundario_idx) free(treinador_bolsomons_idx.treinador_bolsomons_secundario_idx);
	if (treinador_bolsomons_idx.treinador_bolsomons_primario_idx) free(treinador_bolsomons_idx.treinador_bolsomons_primario_idx);
}

// ---------------- Manipulação da lista invertida ----------------

void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t) {

	char chave_sec_maiuscula[TAM_MAX_NOME_BOLSOMON];
	copiar_e_converter_para_maiusculo(chave_sec_maiuscula, chave_secundaria);

	int sec_idx = -1;

	// Busca linear pela chave secundária (nome do bolsomon), pois o índice ainda não está ordenado.
	for (unsigned i = 0; i < t->qtd_registros_secundario; ++i) {
        if (strcmp(t->treinador_bolsomons_secundario_idx[i].chave_secundaria, chave_sec_maiuscula) == 0) {
            sec_idx = i;
            break;
        }
    }


	// --- Caso 1: A chave secundária (nome) não existe. ---
	if (sec_idx == -1) { 

		// Adiciona a chave primária (id_treinador) ao final do índice primário.
		int new_primary_idx = t->qtd_registros_primario;
		strcpy(t->treinador_bolsomons_primario_idx[new_primary_idx].chave_primaria, chave_primaria);
		t->treinador_bolsomons_primario_idx[new_primary_idx].proximo_indice = -1; // É o fim da lista.
		t->qtd_registros_primario++;

		// Adiciona a chave secundária (nome) ao final do í­ndice secundário.
		int new_secondary_idx = t->qtd_registros_secundario;
		strcpy(t->treinador_bolsomons_secundario_idx[new_secondary_idx].chave_secundaria, chave_sec_maiuscula);
		// Aponta para o registro primário que acabamos de criar.
		t->treinador_bolsomons_secundario_idx[new_secondary_idx].primeiro_indice = new_primary_idx;
		t->qtd_registros_secundario++;

	} 

	// --- Caso 2: A chave secundária (nome) Existe. ---
	else {
		
		// Encontra o final da lista encadeada no índice primário.
		int current_primary_idx = t->treinador_bolsomons_secundario_idx[sec_idx].primeiro_indice;
		while (t->treinador_bolsomons_primario_idx[current_primary_idx].proximo_indice != -1) {
			current_primary_idx = t->treinador_bolsomons_primario_idx[current_primary_idx].proximo_indice;
		}
		
		// Adiciona a nova chave primária (id_treinador) no final do índice primário.
		int new_primary_idx = t->qtd_registros_primario;
		strcpy(t->treinador_bolsomons_primario_idx[new_primary_idx].chave_primaria, chave_primaria);
		t->treinador_bolsomons_primario_idx[new_primary_idx].proximo_indice = -1;
		t->qtd_registros_primario++;

		// "Conecta" o antigo último elemento da lista a este novo.
		t->treinador_bolsomons_primario_idx[current_primary_idx].proximo_indice = new_primary_idx;
	}
}

bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t) {
	// Cria uma chave de busca temporária.
	treinador_bolsomons_secundario_index key;
	strcpy(key.chave_secundaria, chave_secundaria);

	// Realiza a busca binária no índice secundário.
	int found_idx = busca_binaria(&key, t->treinador_bolsomons_secundario_idx, t->qtd_registros_secundario, sizeof(treinador_bolsomons_secundario_index), qsort_treinador_bolsomons_secundario_idx, exibir_caminho, 0);

	if (found_idx == -1) {
		// Se não encontrou, retorna false.
		return false;
	} else {
		// Se encontrou, armazena o índice do início da lista primária.
		if (result) {
			*result = t->treinador_bolsomons_secundario_idx[found_idx].primeiro_indice;
		}
		return true;
	}
}

int inverted_list_primary_search(char result[][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t) {
    int count = 0;
    int current_idx = indice;

    // Buffer para o caminho
    char *path_buffer = NULL;
    if (exibir_caminho) {
        path_buffer = malloc(t->qtd_registros_primario * 6 + 1);
        if(path_buffer) path_buffer[0] = '\0'; else exibir_caminho = false;
    }

    // Percorre a "lista encadeada" no índice primário.
	while (current_idx != -1) {
		// Adiciona o índice atual ao caminho, se necessário.
		if (exibir_caminho && path_buffer) {
            char idx_str[7];
            sprintf(idx_str, " %d", current_idx);
            strcat(path_buffer, idx_str);
        }

		if (result) {
        	strcpy(result[count], t->treinador_bolsomons_primario_idx[current_idx].chave_primaria);
		}

		if (indice_final) {
			// Mantém o ponteiro 'indice_final' atualizado com o último índice visitado.
			*indice_final = current_idx;
		}
		
		count++;
        current_idx = t->treinador_bolsomons_primario_idx[current_idx].proximo_indice;
	}

	if (exibir_caminho && path_buffer) {
        printf("%s%s\n", REGS_PERCORRIDOS, path_buffer);
    }
    if (path_buffer) free(path_buffer);

	return count; // Retorna o número de chaves encontradas.
}


// ---------------- Busca binária ----------------

int busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado) {
	
	// Caso especial: se o "arquivo" (vetor) estiver vazio, não há o que buscar.
	if (nmemb == 0) {
		if (exibir_caminho) {
			printf(REGS_PERCORRIDOS "\n");
		}
		// Se pediram sucessor, o sucessor do nada é o índice 0. Senão, não há.
		return (retorno_se_nao_encontrado == 1) ? 0 : -1;
	}

	int low = 0;
	int high = nmemb - 1;
	int mid;
	int found_idx = -1; // -1 indica que não foi encontrado.

	// Buffer para armazenar o caminho percorrido.
	char path_buffer[nmemb * 6]; // Espaço suficiente para os índices.
	path_buffer[0] = '\0';
	
	while (low <= high) {
		// Calcula o índice do meio.
		mid = low + (high - low + 1) / 2;
		
		if (exibir_caminho) {
			// Constrói a string do caminho a ser exibido.
			char mid_str[7];
			sprintf(mid_str, " %d", mid);
			strcat(path_buffer, mid_str);
		}

		// Ponteiro para o elemento do meio do vetor.
		const void *mid_ptr = (char*)base0 + mid * size;
		int cmp = compar(key, mid_ptr);

		if (cmp == 0) {
			// Elemento encontrado!
			found_idx = mid;
			break;
		} else if (cmp < 0) {
			// A chave é menor, busca na metade inferior.
			high = mid - 1;
		} else {
			// A chave é maior, busca na metade superior.
			low = mid + 1;
		}
	}
	
	if (exibir_caminho) {
		printf("%s%s\n", REGS_PERCORRIDOS, path_buffer);
	}

	if (found_idx != -1) {
		// Se encontrou, retorna o índice.
		return found_idx;
	} else {
		// Se não encontrou, decide o que retornar com base no parâmetro.
		switch (retorno_se_nao_encontrado) {
			case -1: // Predecessor
				return high;
			case 1: // Sucessor
				return low;
			case 0: // Nulo (padrão)
			default:
				// Retorna -1 para indicar que não foi encontrado.
				return -1;
		}
	}
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

// Função auxiliar para o qsort de strings (usada em listar_treinadores_bolsomons_menu)
int qsort_string_cmp(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}

/* Copia uma string de origem para um destino e a converte para maiúsculas. */
void copiar_e_converter_para_maiusculo(char *destino, const char *origem) {
    strcpy(destino, origem);
    strupr(destino);
}

//Função para reconstruir o índice de treinadores
void reconstruir_treinadores_idx() {
    if(!treinadores_idx) {
        treinadores_idx = malloc(MAX_REGISTROS * sizeof(treinadores_index));
        if(!treinadores_idx) {
            printf(ERRO_MEMORIA_INSUFICIENTE);
            exit(1);
        }
    }

    for(unsigned i = 0; i < qtd_registros_treinadores; i++) {
        Treinador t = recuperar_registro_treinador(i);
        treinadores_idx[i].rrn = (strncmp(t.id_treinador, "*|", 2)) ? i : -1;
        strcpy(treinadores_idx[i].id_treinador, t.id_treinador);
    }
    qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
}