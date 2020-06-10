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

/**
 * Struktura przechowująca wczytaną komendę.
 */
typedef struct {
    char type;          ///< Pierwszy znak linii, symbolizuje typ polecenia.
    long first_par;     ///< Pierwszy parametr.
    long second_par;    ///< Drugi parametr.
    long third_par;     ///< Trzeci parametr.
    long fourth_par;    ///< Czwarty parametr.
} command_t;


/** @brief Wypisuje informacje o błędzie.
 * Wypisuje informację o błędzie oraz numer linii, w którym wystąpił.
 * @param[in] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void print_error(uint32_t lines);

/** @brief Czyta linię z wejścia i przetwarza ją na komendę.
 * Czyta daną linię z wejścia i zapisuje
 * odczytane dane na zmiennej @p command.
 * @param[out] command      – komenda,
 * @param[out] lines        – wskaźnik na aktualną liczbę linii wejścia.
 * @return Wartość @p true jeśli się powiodło
 * lub wartość @p false jeśli zaszedł błąd.
 */
bool read_command(command_t *command, uint32_t *lines);

#endif //GAMMA_GAMMA_INPUT_H
