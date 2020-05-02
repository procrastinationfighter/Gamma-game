/** @file
 * Interfejs trybu wsadowego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 4.05.2020
 */

#ifndef GAMMA_GAMMA_BATCH_MODE_H
#define GAMMA_GAMMA_BATCH_MODE_H

#include <stdint.h>
#include "gamma.h"

/** @brief Przeprowadza grę za pomocą trybu wsadowego.
 * Zczytuje z wejścia i wykonuje polecenia zgodne z zasadami trybu wsadowego.
 * @param[in,out] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void run_batch_mode(gamma_t *game_board, uint32_t *lines);

/** @brief Wypisuje informacje o błędzie.
 * Wypisuje informację o błędzie oraz numer linii, w którym wystąpił.
 * @param[in] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void print_error(uint32_t lines);

#endif //GAMMA_GAMMA_BATCH_MODE_H
