/** @file
 * Główny plik z funkcją main, zawierający implementację wyboru trybu gry.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 30.04.2020
 */

#include "gamma.h"
#include "gamma_batch_mode.h"
#include "gamma_interactive_mode.h"

/**
 * Symbol informujący o tym, że powinien zostać uruchomiony tryb wsadowy.
 */
#define BATCH_MODE_SYMBOL 'B'

/**
 * Symbol informujący o tym, że powinien zostać uruchomiony tryb interaktywny.
 */
#define INTERACTIVE_MODE_SYMBOL 'I'