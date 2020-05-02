/** @file
 * Interfejs klasy reprezentującej ciąg znaków.
 * Bazuje na klasie String użytej w małym zdaniu zaliczeniowym.
 * @author Adam Boguszewski <ab417730.students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2.05.2020
 */

#ifndef GAMMA_STRING_H
#define GAMMA_STRING_H
#define MULTIPLIER 2

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * Struktura przechowująca łańcuch znaków.
 */
struct string {
    char *string;       ///< Tablica znaków przechowująca łańcuch.
    size_t size;        ///< Ilość aktualnie zaalokowanych komórek.
    size_t length;      ///< Ilość aktualnie zapisanych komórek.
};
typedef struct string String;

/** @brief Inicjalizuje wartości w łańcuchu.
 * Ustawia
 * @param[out] str   – łańcuch mający zostać zainicjalizowany.
 */
void initialize_string(String *str);

/** @brief Deinicjalizuje łańcuch.
 * Zwalnia pamięć zaalokowaną na ciąg znaków.
 * @param[out] str       – deinicjalizowany łańcuch.
 */
void deinitialize_string(String *str);

/** @brief Dodaje nowy znak na końcu łańcucha.
 * Dodaje nowy znam na końcu łańcucha. Jeśli do tej pory
 * użyto całą zaalokowaną pamięć, pamięć zostaje zrealokowana.
 * @param[in,out] str       – łańcuch znaków,
 * @param[in] ch            – dodawany znak.
 */
void push_back_in_string(String *str, char ch);

/** @brief Sprawdza, czy łańcuch jest pusty.
 * Sprawdza, czy dany łańcuch składa się jedynie ze znaku '/0'.
 * @param[in] str           – sprawdzany łańcuch.
 * @return Wartość @p true, jeśli jedynym znakiem w łańcuchu jest znak '\0',
 * lub wartość @p false w przeciwnym wypadku.
 */
bool is_string_empty(String *str);


#endif //GAMMA_STRING_H