/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. Almeida
 *
 * Trabalho 01 - Indexação
 *
 * ========================================================================== *
 *                             <<< NÃO ALTERAR >>>
 * ========================================================================== */


/* Bibliotecas */
#include "ORI_T01_HEADER.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <string.h>


int main() {
    // variáveis utilizadas pelo interpretador de comandos
    char input[500];
    uint64_t seed = 2;
    char datetime[TAM_DATETIME] = "202103181430"; // UTC 18/03/2021 14:30:00

    char id_treinador[TAM_ID_TREINADOR];
    char apelido_treinador[TAM_MAX_APELIDO];
    
    // Variáveis para conversão de string para double, conforme PDF
    char valor_str[100];
    char preco_str[100];
    char premio_str[100];
    double valor;

    char id_bolsomon[TAM_ID_BOLSOMON];
    char nome_bolsomon[TAM_MAX_NOME_BOLSOMON];
    char poder_bolsomon[TAM_MAX_HABILIDADE_BOLSOMON];
    double preco_bolsomon;

    char id_batalha[TAM_ID_BATALHA];
    char horario_inicio[TAM_DATETIME];
    char horario_fim[TAM_DATETIME];
    char duracao[TAM_TIME];
    char cenario[TAM_ARENA];
    char treinadores[TAM_ID_TREINADORES];
    char bolsomons_treinadores[TAM_BOLSOMONS_TREINADORES];
    char duracoes_treinadores[TAM_DURACAO_TREINADORES];
    char eliminacoes[TAM_BOLSOMONS_DERROTADOS_TREINADORES];
    char danos_causados[TAM_BOLSOMONS_DERROTADOS_TREINADORES];
	double premio;

    char file_buffer[TAM_ARQUIVO_BATALHAS];

    if (scanf("SET ARQUIVO_TREINADORES TO '%[^']';\n", file_buffer) == 1) {
        int temp_len = strlen(file_buffer);
        set_arquivo_treinadores(file_buffer, temp_len);
    }

    if (scanf("SET ARQUIVO_BOLSOMONS TO '%[^']';\n", file_buffer) == 1) {
        int temp_len = strlen(file_buffer);
        set_arquivo_bolsomons(file_buffer, temp_len);
    }

    if (scanf("SET ARQUIVO_TREINADOR_POSSUI_BOLSOMON TO '%[^']';\n", file_buffer) == 1) {
        int temp_len = strlen(file_buffer);
        set_arquivo_treinador_possui_bolsomon(file_buffer, temp_len);
    }

    if (scanf("SET ARQUIVO_BATALHAS TO '%[^']';\n", file_buffer) == 1) {
        int temp_len = strlen(file_buffer);
        set_arquivo_batalhas(file_buffer, temp_len);
    }

    if (scanf("SET ARQUIVO_RESULTADOS TO '%[^']';\n", file_buffer) == 1) {
        int temp_len = strlen(file_buffer);
        set_arquivo_resultados(file_buffer, temp_len);
    }

    // inicialização do gerador de números aleatórios e função de datas
    prng_srand(seed);
    putenv("TZ=UTC");
    set_time(datetime);

    criar_treinadores_idx();
    criar_bolsomons_idx();
    criar_batalhas_idx();
    criar_resultados_idx();
    criar_preco_bolsomon_idx();
    criar_treinador_bolsomons_idx();
    criar_treinador_possui_bolsomon_idx();
    criar_data_idx();


    while (1) {

        fgets(input, 500, stdin);
        printf("%s", input);
        clear_input(input);

        if (strcmp("", input) == 0)
            continue; // não avança o tempo nem imprime o comando caso esteja em branco

        /* Funções principais */
        if (sscanf(input, "INSERT INTO treinadores VALUES ('%[^']', '%[^']');", id_treinador, apelido_treinador) == 2)
            cadastrar_treinador_menu(id_treinador, apelido_treinador);
        else if (sscanf(input, "DELETE FROM treinadores WHERE id_treinador = '%[^']';", id_treinador) == 1)
            remover_treinador_menu(id_treinador);
        else if (sscanf(input, "UPDATE treinadores SET bolsobolas = bolsobolas + '%[^']' WHERE id_treinador = '%[^']';", valor_str, id_treinador) == 2) {
            valor = atof(valor_str);
            adicionar_bolsobolas_menu(id_treinador, valor);
        }
        else if (sscanf(input, "INSERT INTO treinador_possui_bolsomon VALUES ('%[^']', '%[^']');", id_treinador, id_bolsomon) == 2)
            comprar_bolsomon_menu(id_treinador, id_bolsomon);
        else if (sscanf(input, "INSERT INTO bolsomons VALUES ('%[^']', '%[^']', '%[^']');", nome_bolsomon, poder_bolsomon, preco_str) == 3) {
            preco_bolsomon = atof(preco_str);
            cadastrar_bolsomon_menu(nome_bolsomon, poder_bolsomon, preco_bolsomon);
        }
        else if(sscanf(input, "INSERT INTO batalhas VALUES ('%[^']', '%[^']', '%[^']', '%[^']', '%[^']', '%[^']', '%[^']', '%[^']');", horario_inicio, duracao, cenario, treinadores, bolsomons_treinadores, duracoes_treinadores, eliminacoes, danos_causados) == 8)
            executar_batalha_menu(horario_inicio, duracao, cenario, treinadores, bolsomons_treinadores, duracoes_treinadores, eliminacoes, danos_causados);
        else if (sscanf(input, "RECOMPENSAR_CAMPEAO('%[^']', '%[^']', '%[^']');", horario_inicio, horario_fim, premio_str) == 3) {
            premio = atof(premio_str);
            recompensar_campeao_menu(horario_inicio, horario_fim, premio);
        }
		
        /* Busca */
        else if (sscanf(input, "SELECT * FROM treinadores WHERE id_treinador = '%[^']';", id_treinador) == 1)
            buscar_treinador_id_menu(id_treinador);
        else if (sscanf(input, "SELECT * FROM bolsomons WHERE id_bolsomon = '%[^']';", id_bolsomon) == 1)
            buscar_bolsomon_id_menu(id_bolsomon);
        else if (sscanf(input, "SELECT * FROM batalhas WHERE id_batalha = '%[^']';", id_batalha) == 1)
            buscar_batalha_id_menu(id_batalha);


        /* Listagem */
        else if (strcmp("SELECT * FROM treinadores ORDER BY id_treinador ASC;", input) == 0)
            listar_treinadores_id_menu();
        // CORRIGIDO: Comando alterado para a sintaxe com INNER JOIN do PDF.
        else if (sscanf(input, "SELECT T.* FROM treinadores T INNER JOIN treinador_possui_bolsomon TPB ON T.id_treinador = TPB.id_treinador INNER JOIN bolsomons B ON TPB.id_bolsomon = B.id_bolsomon WHERE B.nome = '%[^']' ORDER BY T.id_treinador;", nome_bolsomon) == 1)
            listar_treinadores_bolsomons_menu(nome_bolsomon);
        // CORRIGIDO: Alterado 'preco' para 'preco_bolsobolas' conforme PDF.
        else if (sscanf(input, "SELECT * FROM bolsomons WHERE preco_bolsobolas <= ('SELECT bolsobolas FROM treinadores WHERE id_treinador = '%[^']' ');", id_treinador) == 1)
            listar_bolsomons_compra_menu(id_treinador);
        else if (sscanf(input, "SELECT * FROM batalhas WHERE inicio BETWEEN '%[^']' AND '%[^']' ORDER BY inicio ASC;", horario_inicio, horario_fim) == 2)
            listar_batalhas_periodo_menu(horario_inicio, horario_fim);


        /* Liberar espaço */
        else if (strcmp("VACUUM treinadores;", input) == 0)
            liberar_espaco_menu();


        /* Imprimir arquivos de dados - CORRIGIDO: Removido prefixo '\' */
        else if (strcmp("echo file ARQUIVO_TREINADORES", input) == 0)
            imprimir_arquivo_treinadores_menu();
        else if (strcmp("echo file ARQUIVO_BOLSOMONS", input) == 0)
            imprimir_arquivo_bolsomons_menu();
        else if (strcmp("echo file ARQUIVO_BATALHAS", input) == 0)
            imprimir_arquivo_batalhas_menu();
        else if (strcmp("echo file ARQUIVO_RESULTADOS", input) == 0)
            imprimir_arquivo_resultados_menu();
        else if (strcmp("echo file ARQUIVO_TREINADOR_POSSUI_BOLSOMON", input) == 0)
            imprimir_arquivo_treinador_possui_bolsomon_menu();


        /* Imprimir índices primários - CORRIGIDO: Removido prefixo '\' */
        else if (strcmp("echo index treinadores_idx", input) == 0)
            imprimir_treinadores_idx_menu();
        else if (strcmp("echo index bolsomons_idx", input) == 0)
            imprimir_bolsomons_idx_menu();
        else if (strcmp("echo index batalhas_idx", input) == 0)
            imprimir_batalhas_idx_menu();
        else if (strcmp("echo index resultados_idx", input) == 0)
            imprimir_resultados_idx_menu();
        else if (strcmp("echo index treinador_possui_bolsomon_idx", input) == 0)
            imprimir_treinador_possui_bolsomon_idx_menu();


        /* Imprimir índices secundários - CORRIGIDO: Removido prefixo '\' */
        else if (strcmp("echo index preco_bolsomon_idx", input) == 0)
            imprimir_preco_bolsomon_idx_menu();
        else if (strcmp("echo index data_idx", input) == 0)
            imprimir_data_idx_menu();
        else if (strcmp("echo index treinador_bolsomons_secundario_idx", input) == 0)
            imprimir_treinador_bolsomons_secundario_idx_menu();
        else if (strcmp("echo index treinador_bolsomons_primario_idx", input) == 0)
            imprimir_treinador_bolsomons_primario_idx_menu();


        /* Liberar memória eventualmente alocada e encerrar programa - CORRIGIDO: Removido prefixo '\' */
        else if (strcmp("q", input) == 0)
        { liberar_memoria_menu(); return 0; }
        else if (sscanf(input, "SET SRAND %lu;", &seed) == 1)
        { prng_srand(seed); printf(SUCESSO); continue; }
        else if (sscanf(input, "SET TIME '%[^']';", datetime) == 1)
        { if (set_time(datetime)) printf(SUCESSO); else printf(ERRO_VALOR_INVALIDO); continue; }


        else
            printf(ERRO_OPCAO_INVALIDA);

        tick_time();
    }

    return 0;
}