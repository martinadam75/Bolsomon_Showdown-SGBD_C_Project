
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char *strpadright(char *str, char pad, unsigned size){
        for (unsigned i = strlen(str); i < size; ++i)
                str[i] = pad;
        str[size] = '\0';
        return str;
}

char *strupr(char *str){
        for (char *p = str; *p; ++p)
                *p = toupper(*p);
        return str;
}

char *strlower(char *str){
        for (char *p = str; *p; ++p)
                *p = tolower(*p);
        return str;
}

/* Implementação das funções geração determinística de números pseudo-aleatórios e das funções auxilares, NÃO EDITAR NADA */

uint64_t prng_seed;

/* Funções de manipulação de data */
time_t epoch;

#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)

#define TIME_MAX                2147483647L

long _dstbias = 0;                  // Offset for Daylight Saving Time
long _timezone = 0;                 // Difference in seconds between GMT and local time

const int _ytab[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};


/* Funções de geração determinística de números pseudo-aleatórios */
void prng_srand(uint64_t value) {
	prng_seed = value;
}

uint64_t prng_rand() {
	// https://en.wikipedia.org/wiki/Xorshift#xorshift*
	uint64_t x = prng_seed; // O estado deve ser iniciado com um valor diferente de 0
	x ^= x >> 12;			// a
	x ^= x << 25;			// b
	x ^= x >> 27;			// c
	prng_seed = x;
	return x * UINT64_C(0x2545F4914F6CDD1D);
}

/**
Gera um <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">UUID Version-4 Variant-1</a>
(<i>string</i> aleatória) de 36 caracteres utilizando o gerador de números pseudo-aleatórios
<a href="https://en.wikipedia.org/wiki/Xorshift#xorshift*">xorshift*</a>. O UUID é
escrito na <i>string</i> fornecida como parâmetro.

Exemplo de uso:
<code>
char chave_aleatoria[37];
new_uuid(chave_aleatoria);
printf("chave aleatória: %s&#92;n", chave_aleatoria);
</code>
 *
@param buffer String de tamanho 37 no qual será escrito
o UUID. É terminado pelo caractere <code>\0</code>.
 */
void new_uuid(char buffer[37]) {
	uint64_t r1 = prng_rand();
	uint64_t r2 = prng_rand();

	sprintf(buffer, "%08x-%04x-%04lx-%04lx-%012lx", (uint32_t)(r1 >> 32), (uint16_t)(r1 >> 16), 0x4000 | (r1 & 0x0fff), 0x8000 | (r2 & 0x3fff), r2 >> 16);
}

/* Funções de manipulação de data */
struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) {
	// based on http://www.jbox.dk/sanos/source/lib/time.c.html
	time_t time = *timer;
	unsigned long dayclock, dayno;
	int year = EPOCH_YR;

	dayclock = (unsigned long)time % SECS_DAY;
	dayno = (unsigned long)time / SECS_DAY;

	tmbuf->tm_sec = dayclock % 60;
	tmbuf->tm_min = (dayclock % 3600) / 60;
	tmbuf->tm_hour = dayclock / 3600;
	tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
	while (dayno >= (unsigned long) YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	tmbuf->tm_year = year - YEAR0;
	tmbuf->tm_yday = dayno;
	tmbuf->tm_mon = 0;
	while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
		dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
		tmbuf->tm_mon++;
	}
	tmbuf->tm_mday = dayno + 1;
	tmbuf->tm_isdst = 0;
	return tmbuf;
}

time_t mktime(struct tm *tmbuf) {
	// based on http://www.jbox.dk/sanos/source/lib/time.c.html
	long day, year;
	int tm_year;
	int yday, month;
	/*unsigned*/ long seconds;
	int overflow;
	long dst;

	tmbuf->tm_min += tmbuf->tm_sec / 60;
	tmbuf->tm_sec %= 60;
	if (tmbuf->tm_sec < 0) {
		tmbuf->tm_sec += 60;
		tmbuf->tm_min--;
	}
	tmbuf->tm_hour += tmbuf->tm_min / 60;
	tmbuf->tm_min = tmbuf->tm_min % 60;
	if (tmbuf->tm_min < 0) {
		tmbuf->tm_min += 60;
		tmbuf->tm_hour--;
	}
	day = tmbuf->tm_hour / 24;
	tmbuf->tm_hour= tmbuf->tm_hour % 24;
	if (tmbuf->tm_hour < 0) {
		tmbuf->tm_hour += 24;
		day--;
	}
	tmbuf->tm_year += tmbuf->tm_mon / 12;
	tmbuf->tm_mon %= 12;
	if (tmbuf->tm_mon < 0) {
		tmbuf->tm_mon += 12;
		tmbuf->tm_year--;
	}
	day += (tmbuf->tm_mday - 1);
	while (day < 0) {
		if(--tmbuf->tm_mon < 0) {
			tmbuf->tm_year--;
			tmbuf->tm_mon = 11;
		}
		day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
	}
	while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) {
		day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
		if (++(tmbuf->tm_mon) == 12) {
			tmbuf->tm_mon = 0;
			tmbuf->tm_year++;
		}
	}
	tmbuf->tm_mday = day + 1;
	year = EPOCH_YR;
	if (tmbuf->tm_year < year - YEAR0)
		return (time_t)-1;
	seconds = 0;
	day = 0; // Means days since day 0 now
	overflow = 0;

	// Assume that when day becomes negative, there will certainly
	// be overflow on seconds.
	// The check for overflow needs not to be done for leapyears
	// divisible by 400.
	// The code only works when year (1970) is not a leapyear.
	tm_year = tmbuf->tm_year + YEAR0;

	if (TIME_MAX / 365 < tm_year - year)
		overflow++;
	day = (tm_year - year) * 365;
	if (TIME_MAX - day < (tm_year - year) / 4 + 1)
		overflow++;
	day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
	day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

	yday = month = 0;
	while (month < tmbuf->tm_mon) {
		yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}
	yday += (tmbuf->tm_mday - 1);
	if (day + yday < 0)
		overflow++;
	day += yday;

	tmbuf->tm_yday = yday;
	tmbuf->tm_wday = (day + 4) % 7; // Day 0 was thursday (4)

	seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

	if ((TIME_MAX - seconds) / SECS_DAY < day)
		overflow++;
	seconds += day * SECS_DAY;

	// Now adjust according to timezone and daylight saving time
	if (((_timezone > 0) && (TIME_MAX - _timezone < seconds)) ||
		((_timezone < 0) && (seconds < -_timezone))) {
		overflow++;
		}
		seconds += _timezone;

	if (tmbuf->tm_isdst) {
		dst = _dstbias;
	} else {
		dst = 0;
	}

	if (dst > seconds){
		overflow++; // dst is always non-negative
		seconds -= dst;
	}

	if (overflow)
		return (time_t)-1;

	if ((time_t)seconds != seconds)
		return (time_t)-1;
	return (time_t)seconds;
}

bool set_time(char *date) {
	// http://www.cplusplus.com/reference/ctime/mktime/
	struct tm tm_;

	if (strlen(date) == TAM_DATETIME-1 && sscanf(date, "%4d%2d%2d%2d%2d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday, &tm_.tm_hour, &tm_.tm_min) == 5) {
		tm_.tm_year -= 1900;
		tm_.tm_mon -= 1;
		tm_.tm_sec = 0;
		tm_.tm_isdst = -1;
		epoch = mktime(&tm_);
		return true;
	}
	return false;
}

void tick_time() {
	epoch += prng_rand() % 864000; // 10 dias
}

/**
Escreve a <i>data</i> atual no formato <code>AAAAMMDD</code> em uma <i>string</i>
fornecida como parâmetro.

Exemplo de uso:
<code>
char timestamp[TAM_DATE];
current_date(timestamp);
printf("data atual: %s&#92;n", timestamp);
</code>
 *
@param buffer String de tamanho <code>TAM_DATE</code> no qual será escrita
a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_date(char buffer[TAM_DATE]) {
	// http://www.cplusplus.com/reference/ctime/strftime/
	// http://www.cplusplus.com/reference/ctime/gmtime/
	// AAAA MM DD
	// %Y   %m %d
	struct tm tm_;
	if (gmtime_r(&epoch, &tm_) != NULL)
		strftime(buffer, TAM_DATE, "%Y%m%d", &tm_);
}

/**
Escreve a <i>data</i> e a <i>hora</i> atual no formato <code>AAAAMMDDHHMM</code> em uma <i>string</i>
fornecida como parâmetro.

Exemplo de uso:
<code>
char timestamp[TAM_DATETIME];
current_datetime(timestamp);
printf("data e hora atual: %s&#92;n", timestamp);
</code>
 *
@param buffer String de tamanho <code>TAM_DATETIME</code> no qual será escrita
a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_datetime(char buffer[TAM_DATETIME]) {
	// http://www.cplusplus.com/reference/ctime/strftime/
	// http://www.cplusplus.com/reference/ctime/gmtime/
	// AAAA MM DD HH MM
	// %Y   %m %d %H %M
	struct tm tm_;
	if (gmtime_r(&epoch, &tm_) != NULL)
		strftime(buffer, TAM_DATETIME, "%Y%m%d%H%M", &tm_);
}

/* Remove comentários (--) e caracteres whitespace do começo e fim de uma string */
void clear_input(char *str) {
	char *ptr = str;
	int len = 0;

	for (; ptr[len]; ++len) {
		if (strncmp(&ptr[len], "--", 2) == 0) {
			ptr[len] = '\0';
			break;
		}
	}

	while(len-1 > 0 && isspace(ptr[len-1]))
		ptr[--len] = '\0';

	while(*ptr && isspace(*ptr))
		++ptr, --len;

	memmove(str, ptr, len + 1);
}

