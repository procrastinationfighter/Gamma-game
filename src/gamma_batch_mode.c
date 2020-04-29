/** @file
 * Implementacja trybu wsadowego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 1.05.2020
 */

#include "gamma_batch_mode.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

/** @brief Wypisuje informacje o błędzie.
 * Wypisuje informację o błędzie oraz numer linii, w którym wystąpił.
 * @param[in] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void printError(uint32_t lines) {
    fprintf(stderr, "ERROR %u", lines);
}

/** @brief Czyta następny wyraz z wejścia.
 * Zczytuje kolejny wyraz w danej linii.
 * @return Spójny łańcuch znaków bez znaków białych w przypadku powodzenia
 * lub @p NULL jeśli zaszedł jakiś błąd.
 */
char *readNextWord() {
    //todo
}

/** @brief Sprawdza, czy podany wyraz symbolizuje poprawne polecenie.
 * Sprawdza, czy podany wyraz ma długość 1 oraz czy symbolizuje jedno z poleceń
 * (dane przez makra z prefixem CALL).
 * @param commandType       – łańcuch znaków z poleceniem.
 * @return Wartość @p true jeśli wyraz symbolizuje poprawne polecenie
 * lub @p false w przeciwnym wypadku.
 */
bool isValidCommand(char* commandType) {
    if(strlen(commandType) != 1) {
        return false;
    }
    else {
        char ch = commandType[0];
        return (ch == CALL_GAMMA_BOARD || ch == CALL_GAMMA_BUSY_FIELDS
                || ch == CALL_GAMMA_FREE_FIELDS || ch == CALL_GAMMA_GOLDEN_MOVE
                || ch == CALL_GAMMA_GOLDEN_POSSIBLE || ch == CALL_GAMMA_MOVE);
    }
}

/** @brief Czyta i wykonuje polecenie ze standardowego wejścia.
 * Zczytuje z wejścia jedną linię. Jeśli polecenie jest poprawne,
 * wykonuje je. W przeciwnym wypadku wypisuje informację o błędzie.
 * @param[in, out] lines     – liczba linii wejścia do tej pory.
 */
void readAndExecuteCommand(uint32_t *lines) {
    (*lines)++;
    char *commandType = readNextWord();
    if(isValidCommand(commandType)) {
        readCommandParameters(commandType);
    }
    else {
        printError(*lines);
    }
}

/** @brief Przeprowadza grę za pomocą trybu wsadowego.
 * Zczytuje z wejścia i wykonuje polecenia zgodne z zasadami trybu wsadowego.
 * @param[in, out] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void run_batch_mode(uint32_t *lines) {
    while(!feof(stdin)) {
        readAndExecuteCommand(lines);
    }
}