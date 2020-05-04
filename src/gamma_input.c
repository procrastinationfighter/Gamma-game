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
 * @param command           – polecenie.
 * @return Wartość @p true jeśli znak komendy jest
 * znakiem komentarza lub znakiem końca linii.
 */
static bool should_line_be_skipped(command_t *command) {
    return (command->type == COMMENT_SIGN || command->type == '\n');
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

/** @Brief Zczytuje aktualną linię wejścia.
 * Zczytuje aktualną linię wejścia.
 * W razie niepowodzenia dokonuje wyjścia.
 * @return Aktualna linia wejścia.
 */
static char * get_current_line() {
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    if(errno == ENOMEM) {
        exit(EXIT_FAILURE);
    }

    return line;
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
    *param = parse_number(p);
    // Jeśli funkcja strtoul zwróciła 0 to mógł zajść błąd.
    // Jeśli tak jest, a p nie jest równe "0" to przekaż false.
    return !(*param == 0 && (strcmp(p, "0") != 0));
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
    command->type = line[0];
    char *com = strtok(line, delim);

    if(com == NULL) {
        return (command->type == '\n' || command->type == EOF);
    }
    else if(strlen(com) != 1) {
        return false;
    }

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
 * @param[out] command  – komenda.
 * @return Wartość @p true jeśli się powiodło
 * lub wartość @p false jeśli zaszedł błąd.
 */
bool read_command(command_t *command) {
    char *line = get_current_line();
    if(feof(stdin)) {
        assert(line != NULL);
        return strlen(line) == 0;
    }

    bool correct_params = set_command(command, line);
    free(line);
    return correct_params;
}