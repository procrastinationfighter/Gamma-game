/** @file
 * Interfejs modułu obsługującego tryb raw w terminalu.
 *
 * @source https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 */
#ifndef RAW_MODE
#define RAW_MODE

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

/** @brief Włącza tryb raw. */
void enableRawMode();

#endif