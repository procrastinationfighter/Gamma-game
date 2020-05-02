/** @file
 * Implementacja klasy reprezentującej ciąg znaków.
 * Bazuje na klasie String użytej w małym zdaniu zaliczeniowym.
 * @author Adam Boguszewski <ab417730.students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2.05.2020
 */

#include <errno.h>
#include "String.h"


/** @brief Inicjalizuje wartości w łańcuchu.
 * Ustawia wartości w łańcuchu na domyślne i alokuje tablicę znaków.
 * @param[out] str   – łańcuch mający zostać zainicjalizowany.
 */
void initialize_string(String *str) {
    str->string = malloc(MULTIPLIER * sizeof(char));
    if (str->string == NULL) {
        errno = ENOMEM;
        exit(EXIT_FAILURE);
    }

    str->length = 0;
    str->size = MULTIPLIER;
}

/** @brief Deinicjalizuje łańcuch.
 * Zwalnia pamięć zaalokowaną na ciąg znaków.
 * @param[out] str       – deinicjalizowany łańcuch.
 */
void deinitialize_string(String *str) {
    free(str->string);
}

/** @brief Alokuje więcej pamięci na łańcuch znaków.
 * Alokuje @p MULTIPLIER więcej pamięci na dany łańcuch.
 * @param[in, out] str       – realokowany łańcuch.
 */
static void increase_string_size(String *str) {
    str->size = MULTIPLIER * str->size;
    str->string = realloc(str->string, (str->size * sizeof(char)));
    if (str->string == NULL) {
        errno = ENOMEM;
        exit(EXIT_FAILURE);
    }
}

/** @brief Dodaje nowy znak na końcu łańcucha.
 * Dodaje nowy znak na końcu łańcucha. Jeśli do tej pory
 * użyto całą zaalokowaną pamięć, pamięć zostaje zrealokowana.
 * @param[in,out] str       – łańcuch znaków,
 * @param[in] ch            – dodawany znak.
 */
void push_back_in_string(String *str, char ch) {
    if (str->size == str->length) {
        increase_string_size(str);
    }
    str->string[str->length] = ch;
    (str->length)++;
}


/** @brief Sprawdza, czy łańcuch jest pusty.
 * Sprawdza, czy dany łańcuch składa się jedynie ze znaku '/0'.
 * @param[in] str           – sprawdzany łańcuch.
 * @return Wartość @p true, jeśli jedynym znakiem w łańcuchu jest znak '\0',
 * lub wartość @p false w przeciwnym wypadku.
 */
bool is_string_empty(String *str) {
    return (strcmp(str->string, "\0") == 0);
}
