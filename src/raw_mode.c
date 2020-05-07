/** @file
 * Implementacja modułu obsługującego tryb raw w terminalu.
 *
 * Źródło: https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 */
#include "raw_mode.h"
struct termios orig_termios;

/** @brief Wyłącza tryb raw. */
static void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/** @brief Włącza tryb raw. */
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}