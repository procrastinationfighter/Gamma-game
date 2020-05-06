/** @file
 * Interfejs trybu interaktywnego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 6.05.2020
 */
#ifndef GAMMA_GAMMA_INTERACTIVE_MODE_H
#define GAMMA_GAMMA_INTERACTIVE_MODE_H

#include "gamma.h"
#include "gamma_input.h"

/** @brief Uruchamia tryb interaktywny gry gamma.
 * Uruchamia i przeprowadza grę w trybie interaktywnym.
 * @param[in,out] game          – struktura gry,
 * @param[in] command           – komenda użyta do stworzenia gry.
 */
void run_interactive_mode(gamma_t *game, command_t *command);

#endif //GAMMA_GAMMA_INTERACTIVE_MODE_H
