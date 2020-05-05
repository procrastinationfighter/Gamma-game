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
#include <errno.h>
#include <assert.h>
#include <ctype.h>

/** @brief Wypisuje informacje o błędzie.
 * Wypisuje informację o błędzie oraz numer linii, w którym wystąpił.
 * @param[in] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void print_error(uint32_t lines) {
    fprintf(stderr, "ERROR %u\n", lines);
}

/** @brief Sprawdza, czy linię wejścia należy pominąć.
 * Sprawdza, czy linia nie składa się z komentarza
 * lub pojedyczego znaku końca linii.
 * @param line           – linia wejścia.
 * @return Wartość @p true jeśli znak komendy jest
 * znakiem komentarza lub znakiem końca linii.
 */
static bool should_line_be_skipped(char *line) {
    return(line != NULL && strlen(line) > 0 &&
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
/** @brief Sprawdza, czy wyraz jest poprawną liczbą.
 * Sprawdza czy wyraz @p p składa się z samych cyfr.
 * @param p         – łańcuch znaków
 * @return Wartość @p true jeśli wyraz składa się z samych
 * cyfr lub jest NULLem lub wartość @p false w przeciwnym wypadku.
 */
static bool is_string_valid_number(const char *p) {
    if(p == NULL) {
        return true;
    }
    else {
        int i = 0;
        while(p[i] != '\0') {
            if(!isdigit(p[i])) {
                return false;
            }
            i++;
        }
    }

    return true;
}

/** @brief Sprawdza poprawność pierwszego wyrazu w linii.
 * Sprawdza, czy pierwszy wyraz w linii na pewno jest
 * wyrazem jednoliterowym oraz czy istnieje.
 * @param word              – słowo.
 * @return Wartość @p true jeśli słowo spełnia warunki
 * lub @p false w przeciwnym wypadku.
 */
static bool is_first_word_correct(char *word) {
    return (word != NULL && strlen(word) == 1);
}

/** @brief Czyta jeden parametr za pomocą strtok.
 * Zczytuje za pomocą strtok następny wyraz
 * i przetwarza go na liczbę.
 * @param[out] param        – parametr,
 * @param[in]  delim        – ograniczniki dla strtok.
 * @return Wartość @p true jeśli poprawnie zamieniono
 * wyraz na liczbę lub @p false w przeciwnym razie.
 */
static bool read_parameter(long *param, char *delim) {
    char *p = strtok(NULL, delim);
    if(is_string_valid_number(p)) {
        *param = parse_number(p);
        return true;
    }
    else {
        return false;
    }
}

/** @Brief Zczytuje aktualną linię wejścia.
 * Zczytuje aktualną linię wejścia.
 * @return Aktualna linia wejścia
 * lub @p NULL jeśli wczytywanie się nie powiodło.
 */
static char * get_current_line() {
    char *line = NULL;
    size_t len = 0;
    int temp = getline(&line, &len, stdin);
    if(temp == -1) {
        free(line);
        return NULL;
    }
    else {
        return line;
    }
}

/** @brief Przetwarza wczytaną linię na parametry.
 * Zczytuje z wczytanej linii pojedynczy znak komendy
 * oraz maksymalnie 4 parametry liczbowe.
 * @param[out] command       – struktura komendy,
 * @param[in] line           – wczytana linia.
 * @return Wartość @p true jeśli parametry były poprawne
 * lub @p false w przeciwnym wypadku.
 */
static bool set_command(command_t *command, char *line) {
    char delim[] = " \n\t\v\f\r";
    char *com = strtok(line, delim);
    if(!is_first_word_correct(com)) {
        return false;
    }
    command->type = com[0];

    long *params[4] = {&command->first_par, &command->second_par,
                       &command->third_par, &command->fourth_par};
    int i = 0;
    bool writing_ok = true;
    while(i < 4 && writing_ok) {
        writing_ok = read_parameter(params[i], delim);
        i++;
    }

    return (writing_ok && strtok(NULL, delim) == NULL);
}

/** @brief Czyta linię z wejścia i przetwarza ją na komendę.
 * Czyta daną linię z wejścia i zapisuje
 * odczytane dane na zmiennej @p command.
 * @param[out] command      – komenda,
 * @param[out] lines        – wskaźnik na aktualną liczbę linii wejścia.
 * @return Wartość @p true jeśli się powiodło
 * lub wartość @p false jeśli zaszedł błąd
 * lub zaszedł koniec wejścia.
 */
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