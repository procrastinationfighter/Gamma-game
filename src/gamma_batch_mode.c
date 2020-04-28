/** @file
 * Implementacja trybu wsadowego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 1.05.2020
 */

#include "gamma_batch_mode.h"

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