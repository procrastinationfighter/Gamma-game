/** @file
 * Główny plik z funkcją main, zawierający implementację wyboru trybu gry.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 5.04.2020
 */

#include <stdlib.h>
#include <stdio.h>
#include "gamma.h"
#include "gamma_batch_mode.h"
#include "gamma_interactive_mode.h"
#include "gamma_input.h"

/**
 * Symbol informujący o tym, że powinien zostać uruchomiony tryb wsadowy.
 */
#define BATCH_MODE_SYMBOL 'B'

/**
 * Symbol informujący o tym, że powinien zostać uruchomiony tryb interaktywny.
 */
#define INTERACTIVE_MODE_SYMBOL 'I'

/**
 * Wskaźnik na strukturę gry. Zasięg pliku, aby można
 * było użyć funkcji atexit do jego zwolnienia.
 */
static gamma_t *game_pointer = NULL;

/** @brief Zwalnia pamięć odpowiadającą za grę.
 * Zwalnia strukturę gry poprzez wywołanie @ref gamma_delete.
 * Funkcja używana jedynie przez funkcję @ref atexit.
 */
static void free_game_memory() {
    gamma_delete(game_pointer);
}

/** @brief Sprawdza, czy parametry są w zakresie typu uint32_t.
 * Sprawdza, czy parametry są w zakresie od 0 do UINT32_MAX.
 * Funkcja celowo uznaje parametry równe @p BLANK_PARAMETER_NUMBER
 * za niepoprawne, gdyż poprawna będzie linia z 4 parametrami.
 * @param[in] command           – komenda.
 * @return Wartość @p true jeśli warunek
 * jest spełniony lub @p false w przeciwnym wypadku.
 */
static bool are_parameters_in_bound(command_t *command) {
    long params[4] = {command->first_par, command->second_par,
                      command->third_par, command->fourth_par};
    for(int i = 0; i < 4; i++) {
        long q = params[i];
        if(q > UINT32_MAX || q < 0) {
            return false;
        }
    }

    return true;
}

/** @brief Sprawdza poprawność symbolu trybu.
 * Sprawdza, czy symbol oznaczający tryb jest poprawny,
 * czyli równy @p INTERACTIVE_MODE_SYMBOL
 * lub @p BATCH_MODE_SYMBOL.
 * @param[in] sign          – symbol.
 * @return Wartość @p true jeśli symbol jest poprawny
 * lub @p false w przeciwnym wypadku.
 */
static bool is_command_sign_correct(char sign) {
    return (sign == INTERACTIVE_MODE_SYMBOL || sign == BATCH_MODE_SYMBOL);
}

/** @brief Sprawdza poprawność polecenia utworzenia nowej gry.
 * Sprawdza, czy parametry mieszczą się w zakresie typu uint32_t,
 * czy znak polecenia jest poprawny oraz czy linia nie powinna zostać
 * pominięta. Jeśli parametry lub znak jest niepoprawny, wypisuje
 * informację o błędzie.
 * @param[in] command           – polecenie,
 * @param[in] lines             – numer aktualnej linii.
 * @return Wartość @p true jeśli wczytane polecenie
 * jest poprawnym poleceniem uruchamiającym tryb
 * lub @p false w przeciwnym wypadku.
 */
static bool check_command_correctness(command_t *command, uint32_t lines) {
    if(should_line_be_skipped(command)) {
        return false;
    }
    else {
        if(are_parameters_in_bound(command) &&
           is_command_sign_correct(command->type)) {
            return true;
        }
        else {
            print_error(lines);
            return false;
        }
    }
}

/** @brief Uruchamia odpowiedni tryb.
 * W zależności od symbolu @p type w zmiennej
 * @p command, uruchamia odpowiedni tryb.
 * @param command       – polecenie.
 * @param lines         – wskaźnik na numer aktualnej linii wejścia.
 */
static void run_mode(command_t *command, uint32_t *lines) {
    game_pointer = gamma_new(command->first_par, command->second_par,
                             command->third_par, command->fourth_par);
    if(command->type == BATCH_MODE_SYMBOL) {
        printf("OK %iu\n", *lines);
        run_batch_mode(game_pointer, lines);
    }
    else {
        run_interactive_mode(game_pointer);
    }
}

int main() {
    atexit(free_game_memory);

    uint32_t lines = 0;
    bool right_command = false;
    command_t command;
    while(!feof(stdin) && !right_command) {
        lines++;
        if(read_command(&command)) {
            right_command = check_command_correctness(&command, lines);
        }
        else {
            print_error(lines);
        }
    }

    if(right_command) {
        run_mode(&command, &lines);
    }
    return 0;
}