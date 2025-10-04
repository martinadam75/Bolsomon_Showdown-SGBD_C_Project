#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#define TAM_DATE 9
#define TAM_DATETIME 13


/*
Preenche uma string str com o caractere pad para completar o tamanho size.

@param str Ponteiro para a string a ser manipulada.
@param pad Caractere utilizado para fazer o preenchimento à direita.
@param size Tamanho desejado para a string.
*/
char* strpadright(char* str, char pad, unsigned size);

/*
Converte uma string str para letras maiúsculas.

@param str Ponteiro para a string a ser manipulada.
*/
char* strupr(char* str);

/*
Converte uma string str para letras minúsculas.

@param str Ponteiro para a string a ser manipulada.
*/
char* strlower(char* str);


void prng_srand(uint64_t value);
uint64_t prng_rand();
void new_uuid(char buffer[37]);
struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf);
time_t mktime(struct tm *tmbuf);
bool set_time(char *date);
void tick_time();
void current_date(char buffer[TAM_DATE]);
void current_datetime(char buffer[TAM_DATETIME]);
void clear_input(char *str);

#endif // UTILS_H
