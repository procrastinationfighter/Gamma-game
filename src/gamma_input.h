/** @file
 * Interfejs modułu obsługującego wejście gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 3.05.2020
 */
#ifndef GAMMA_GAMMA_INPUT_H
#define GAMMA_GAMMA_INPUT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Symbol informujący o tym, że dana linia jest komentarzem.
 */
#define COMMENT_SIGN '#'

/**
 * Liczba symbolizująca domyślną wartość w przypadku, gdy parametr nie istnieje.
 */
#define BLANK_PARAMETER_NUMBER -42

struct command {
    char type;
    long first_par;
    long second_par;
    long third_par;
    long fourth_par;
};
typedef struct command command_t;


/** @brief Wypisuje informacje o błędzie.
 * Wypisuje informację o błędzie oraz numer linii, w którym wystąpił.
 * @param[in] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void print_error(uint32_t lines);

/** @brief Sprawdza, czy linię wejścia należy pominąć.
 * Sprawdza, czy linia nie składa się z komentarza
 * lub pojedyczego znaku końca linii.
 * @param command           – polecenie.
 * @return Wartość @p true jeśli znak komendy jest
 * znakiem komentarza lub znakiem końca linii.
 */
static bool should_line_be_skipped(command_t *command);

/** @brief Czyta linię z wejścia i przetwarza ją na komendę.
 * Czyta daną linię z wejścia i zapisuje
 * odczytane dane na zmiennej @p command.
 * @param[out] command  – komenda.
 * @return Wartość @p true jeśli się powiodło
 * lub wartość @p false jeśli zaszedł błąd.
 */
bool read_command(command_t *command);

#endif //GAMMA_GAMMA_INPUT_H
