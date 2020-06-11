/** @file
 * Implementacja modułu obsługującego wejście gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 3.05.2020
 */
#define _GNU_SOURCE

#include "gamma_input.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

inline void print_error(uint32_t lines) {
    fprintf(stderr, "ERROR %u\n", lines);
}

/** @brief Sprawdza, czy linię wejścia należy pominąć.
 * Sprawdza, czy linia nie składa się z komentarza
 * lub pojedyczego znaku końca linii.
 * @param line           – linia wejścia.
 * @return Wartość @p true jeśli znak komendy jest
 * znakiem komentarza lub znakiem końca linii.
 */
static inline bool should_line_be_skipped(const char *line) {
    return(line != NULL && line[0] != '\0' &&
          (line[0] == COMMENT_SIGN || line[0] == '\n'));
}

/** @brief Zamienia ciąg cyfr na liczbę.
 * Zamienia ciąg cyfr zapisany w parametrze @p parameter
 * na liczbę, jeśli jest to możliwe. Liczba nie powinna wykraczać
 * poza zasięg typu uint32_t.
 * @param[in] parameter         – łańcuch znaków.
 * @return Liczba zakodowana wcześniej jako ciąg znaków
 * lub @p BLANK_PARAMETER_NUMBER, jeśli ciąg jest pusty.
 */
static long parse_number(char *parameter) {
    if(parameter == NULL) {
        return BLANK_PARAMETER_NUMBER;
    }
    else {
        char *ch;
        unsigned long result = strtoul(parameter, &ch, 10);
        if(result > LONG_MAX) {
            return UINT32_MAX + 1;
        }
        else {
            return (long) result;
        }
    }
}

/** @brief Sprawdza, czy linia składa się tylko z prawidłowych znaków.
 * Sprawdza czy pierwszy znak linii jest osobnym słowem
 * oraz czy pozostałe znaki są znakami białymi lub cyframi.
 * @param line                  – sprawdzana linia.
 * @return Wartość @p true jeśli znaki w linii spełniają powyższe warunki
 * lub @p false w przeciwnym wypadku.
 */
static bool is_line_correct(const char *line) {
    if(line == NULL) {
        return false;
    }
    else {
        int i = 0;
        while (line[i] != '\n') {
            if (i == 0) {
                char next = line[1];
                if (isspace(line[0]) || (!isspace(next) && next != '\0')) {
                    return false;
                }
            }
            else {
                char curr = line[i];
                if (!isspace(curr) && !isdigit(curr)) {
                    return false;
                }
            }
            i++;
        }
        return true;
    }
}

/** @brief Czyta jeden parametr za pomocą strtok.
 * Sczytuje za pomocą strtok następny wyraz
 * i przetwarza go na liczbę.
 * Funkcja zakłada, że następny wyraz to same liczby,
 * w momencie wywołania funkcji powinno to już być sprawdzone.
 * @param[in]  delim        – ograniczniki dla strtok.
 * @return Wartość @p true jeśli poprawnie zamieniono
 * wyraz na liczbę lub @p false w przeciwnym razie.
 */
static long read_parameter(char *delim) {
    char *p = strtok(NULL, delim);
    return parse_number(p);
}

/** @brief Sczytuje aktualną linię wejścia.
 * Sczytuje aktualną linię wejścia.
 * @return Aktualna linia wejścia
 * lub @p NULL jeśli wczytywanie się nie powiodło.
 */
static char * get_current_line() {
    char *line = NULL;
    size_t len = 0;
    ssize_t temp = getline(&line, &len, stdin);
    if(temp == -1) {
        free(line);
        return NULL;
    }
    else {
        return line;
    }
}

/** @brief Przetwarza wczytaną linię na parametry.
 * Sczytuje z wczytanej linii pojedynczy znak komendy
 * oraz maksymalnie 4 parametry liczbowe.
 * @param[out] command       – struktura komendy,
 * @param[in] line           – wczytana linia.
 * @return Wartość @p true jeśli parametry były poprawne
 * lub @p false w przeciwnym wypadku.
 */
static bool set_command(command_t *command, char *line) {
    char first = line[0];
    if(!is_line_correct(line)) {
        return false;
    }
    char delim[] = " \n\t\v\f\r";
    char *com = strtok(line, delim);
    if(com[0] != first) {
        return false;
    }

    command->type = com[0];

    long *params[4] = {&command->first_par, &command->second_par,
                       &command->third_par, &command->fourth_par};
    for(int i = 0; i < 4; i++) {
        *params[i] = read_parameter(delim);
    }

    return (strtok(NULL, delim) == NULL);
}

bool read_command(command_t *command, uint32_t *lines) {
    bool is_reading_command_finished = false;
    char *line = NULL;
    while(!is_reading_command_finished) {
        line = get_current_line();
        if(line == NULL) {
            is_reading_command_finished = true;
        }
        else if(should_line_be_skipped(line)) {
            (*lines)++;
        }
        else {
            if(set_command(command, line)) {
                is_reading_command_finished = true;
            }
            else {
                print_error(*lines);
                (*lines)++;
            }
        }
        if(line != NULL) {
            free(line);
        }
    }

    return line != NULL;
}