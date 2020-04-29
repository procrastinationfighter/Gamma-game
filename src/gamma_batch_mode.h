/** @file
 * Interfejs trybu wsadowego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 1.05.2020
 */

#ifndef GAMMA_GAMMA_BATCH_MODE_H
#define GAMMA_GAMMA_BATCH_MODE_H

#include <stdint.h>

/** @brief Przeprowadza grę za pomocą trybu wsadowego.
 * Zczytuje z wejścia i wykonuje polecenia zgodne z zasadami trybu wsadowego.
 * @param[in,out] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void run_batch_mode(uint32_t *lines);

/** @brief Wypisuje informacje o błędzie.
 * Wypisuje informację o błędzie oraz numer linii, w którym wystąpił.
 * @param[in] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void printError(uint32_t lines);

#endif //GAMMA_GAMMA_BATCH_MODE_H
