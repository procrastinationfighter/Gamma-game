/** @file
 * Implementacja modułu obsługującego tryb raw w terminalu.
 *
 * Źródło: https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 */

#include "raw_mode.h"

/**
 * Zmienna przechowująca stan terminala sprzed włączenia trybu raw.
 */
struct termios orig_termios;

/** @brief Wyłącza tryb raw. */
static void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/** @brief Włącza tryb raw. */
void enableRawMode() {
    if(isatty(STDIN_FILENO)) {
        int error_control = tcgetattr(STDIN_FILENO, &orig_termios);
        if (error_control == -1) {
            exit(EXIT_FAILURE);
        }
        atexit(disableRawMode);

        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ECHO | ICANON);

        error_control = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        if (error_control == -1) {
            exit(EXIT_FAILURE);
        }
    }
}