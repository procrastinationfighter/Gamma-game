/** @file
 * Implementacja trybu wsadowego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 4.05.2020
 */

#include "gamma_batch_mode.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gamma_input.h"

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_move.
 */
#define CALL_GAMMA_MOVE 'm'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_golden_move.
 */
#define CALL_GAMMA_GOLDEN_MOVE 'g'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_busy_fields.
 */
#define CALL_GAMMA_BUSY_FIELDS 'b'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_free_fields.
 */
#define CALL_GAMMA_FREE_FIELDS 'f'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_golden_possible.
 */
#define CALL_GAMMA_GOLDEN_POSSIBLE 'q'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_board.
 */
#define CALL_GAMMA_BOARD 'p'

/** @brief Sprawdza, czy podany wyraz symbolizuje poprawne polecenie.
 * Sprawdza, czy podany wyraz ma długość 1 oraz czy symbolizuje jedno z poleceń
 * (dane przez makra z prefixem CALL).
 * @param commandType       – łańcuch znaków z poleceniem.
 * @return Wartość @p true jeśli wyraz symbolizuje poprawne polecenie
 * lub @p false w przeciwnym wypadku.
 */
static inline bool is_command_valid(char ch) {
    return (ch == CALL_GAMMA_BOARD || ch == CALL_GAMMA_BUSY_FIELDS
            || ch == CALL_GAMMA_FREE_FIELDS || ch == CALL_GAMMA_GOLDEN_MOVE
            || ch == CALL_GAMMA_GOLDEN_POSSIBLE || ch == CALL_GAMMA_MOVE);
}

/** @brief Sprawdza, czy ilość parametrów jest poprawna.
 * Sprawdza, czy ilość parametrów jest zgodna
 * z tym, ile wymaga konkretna komenda.
 * @param command           – dane polecenia.
 * @return Wartość @p true jeśli ilośc parametrów
 * jest poprawna lub @p false w przeciwnym wypadku.
 */
static bool is_parameter_count_correct(command_t *command) {
    int i = 0;
    long args[4] = {command->first_par, command->second_par,
                    command->third_par, command->fourth_par};
    while(args[i] != BLANK_PARAMETER_NUMBER && i < 4) {
        i++;
    }
    switch (command->type) {
        case CALL_GAMMA_BOARD:
            return i == 0;
        case CALL_GAMMA_GOLDEN_POSSIBLE:
        case CALL_GAMMA_FREE_FIELDS:
        case CALL_GAMMA_BUSY_FIELDS:
            return i == 1;
        case CALL_GAMMA_GOLDEN_MOVE:
        case CALL_GAMMA_MOVE:
            return i == 3;
        default:
            return false;
    }
}

/** @brief Sprawdza, czy parametry są w odpowiednim zakresie.
 * Sprawdza, czy parametry spełniają jeden z warunków:
 * są w zakresie od 0 do UINT32_MAX
 * LUB są równe @p BLANK_PARAMETER_NUMBER.
 * @param command           – komenda.
 * @return Wartość @p true jeśli jeden z tych warunków
 * jest spełniony lub @p false w przeciwnym wypadku.
 */
static bool are_parameters_in_bound(command_t *command) {
    long params[4] = {command->first_par, command->second_par,
                      command->third_par, command->fourth_par};
    for(int i = 0; i < 4; i++) {
        long q = params[i];
        if(q != BLANK_PARAMETER_NUMBER && (q > UINT32_MAX || q < 0)) {
            return false;
        }
    }

    return true;
}

/** @brief Sprawdza poprawność parametrów polecenia.
 * Sprawdza, czy znak komendy jest poprawny
 * oraz czy ilość parametrów zgadza się z jego parametrami.
 * @param command           – komenda.
 * @return Wartość @p true jeśli znak oraz parametry są poprawne
 * lub @p false w przeciwnym wypadku.
 */
static inline bool are_parameters_correct(command_t *command) {
    return is_command_valid(command->type) &&
           is_parameter_count_correct(command) &&
           are_parameters_in_bound(command);
}

/** @brief Wykonuje polecenie z podanymi parametrami.
 * Jeśli ilość parametrów jest poprawna,
 * wykonuje zadane w parametrze @p command polecenie.
 * @param game_board        – wskaźnik na strukturę gry,
 * @param command           – polecenie.
 * @return Wartość @p true jeśli ilość parametrów była prawidłowa
 * lub @p false w przeciwnym wypadku.
 */
static bool execute_command(gamma_t *game_board, command_t *command) {
    bool params_ok = are_parameters_correct(command);
    if(params_ok) {
        char *board;
        switch (command->type) {
            case CALL_GAMMA_BOARD:
                board = gamma_board(game_board);
                printf("%s", board);
                free(board);
                break;
            case CALL_GAMMA_GOLDEN_POSSIBLE:
                printf("%i\n", gamma_golden_possible(game_board, command->first_par));
                break;
            case CALL_GAMMA_FREE_FIELDS:
                printf("%lu\n", gamma_free_fields(game_board, command->first_par));
                break;
            case CALL_GAMMA_BUSY_FIELDS:
                printf("%lu\n", gamma_busy_fields(game_board, command->first_par));
                break;
            case CALL_GAMMA_GOLDEN_MOVE:
                printf("%i\n", gamma_golden_move(game_board, command->first_par,
                                            command->second_par, command->third_par));
                break;
            case CALL_GAMMA_MOVE:
                printf("%i\n", gamma_move(game_board, command->first_par,
                                        command->second_par, command->third_par));
                break;
            default:
                break;
        }
    }

    return params_ok;
}

/** @brief Czyta i wykonuje polecenie ze standardowego wejścia.
 * Zczytuje z wejścia jedną linię. Jeśli polecenie jest poprawne,
 * wykonuje je. W przeciwnym wypadku wypisuje informację o błędzie.
 * @param[in,out] game_board    – struktura gry,
 * @param[in] command           – polecenie,
 * @param[in,out] lines         – liczba linii wejścia do tej pory.
 */
static bool read_and_execute_command(gamma_t *game_board, command_t  *command,
                                                            uint32_t *lines) {
    (*lines)++;
    if(read_command(command, lines)) {
        if(!execute_command(game_board, command)) {
            print_error(*lines);
        }
        return true;
    }
    else {
        return false;
    }
}

/** @brief Przeprowadza grę za pomocą trybu wsadowego.
 * Zczytuje z wejścia i wykonuje polecenia zgodne z zasadami trybu wsadowego.
 * @param[in,out] game_board  – struktura gry,
 * @param[in,out] lines       – liczba linii wejścia przed uruchomieniem trybu.
 */
void run_batch_mode(gamma_t *game_board, uint32_t *lines) {
    command_t curr_command;
    while(read_and_execute_command(game_board, &curr_command, lines)) {
        // Puste.
    }
}