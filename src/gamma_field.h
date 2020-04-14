/** @file
 * Interfejs klasy przechowującej stan pola w grze gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 9.04.2020
 */

#ifndef GAMMA_GAMMA_FIELD_H
#define GAMMA_GAMMA_FIELD_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Struktura przechowująca dane o jednym polu.
 */
typedef struct field{
    uint32_t owner_index; ///< Indeks gracza, którego pionek stoi na polu.
    uint32_t this_x;      ///< Numer kolumny, w której znajduje się pole.
    uint32_t this_y;      ///< Numer wiersza, w którym znajduje się pole.
    uint32_t parent_x;    ///< Numer kolumny, w której znajduje się rodzic pola.
    uint32_t parent_y;    ///< Numer wiersza, w którym znajduje się rodzic pola.
    uint32_t rank;        ///< Ranga pola w ramach algorytmu find and union.
} gamma_field;

/**
 * Struktura reprezentująca kolejkę obiektów typu gamma_field.
 */
typedef struct fieldqueue field_queue;

/** @brief Inicjalizuje kolejkę.
 * Ustawia pola kolejki @p na NULL.
 * @param[out] q     – wskaźnik na wskaźnik na strukturę kolejki.
 */
void field_queue_init(field_queue **q);

/** @brief Dodaje pole do kolejki.
 * Tworzy element kolejki zawierający pole, na które wskazuje @p element
 * oraz dodaje go do kolejki.
 * @param[in,out] q         – wskaźnik na strukturę kolejki,
 * @param[in] element       – wskaźnik na pole.
 */
void field_queue_push(field_queue *q, gamma_field *element);

/** @brief Wyjmuje pierwszy element z kolejki.
 * Usuwa z kolejki pierwszy element i ustanawia kolejny element jako pierwszy.
 * @param[in,out] q     – wskaźnik na strukturę kolejki.
 * @return Wskaźnik na pole będące pierwszym elementem w kolejce,
 * lub @p NULL jeśli kolejka jest pusta.
 */
gamma_field* field_queue_pop(field_queue *q);

/** @brief Sprawdza, czy kolejka jest pusta.
 * Sprawdza, czy kolejka nie zawiera żadnych elementów poprzez
 * sprawdzenie, czy wskaźnik na pierwszy element nie jest równy @p NULL.
 * @param[in] q     – wskaźnik na strukturę kolejki.
 * @return Wartość @p true, jeśli w kolejce nie ma elementów
 * lub wartość @p false jeśli kolejka zawiera elementy.
 */
bool field_queue_is_empty(field_queue *q);

/** @brief Usuwa kolejkę.
 * Usuwa wszystkie elementy w kolejce i zwalnia pamięć zaalokowaną na kolejkę.
 * @param[in,out] q    – wskaźnik na strukturę kolejki.
 */
void field_queue_clear(field_queue **q);

/** @brief Znajduje korzeń danego pola.
 * W ramach algorytmu find and union, znajduje korzeń pola, na które wskazuje
 * @p field oraz kompresuje jego ścieżkę do pola poprzez ustanowienie
 * rodzica tego pola na korzeń.
 * @param[in] field     – wskaźnik na pole,
 * @param[in] board     – tablica pól.
 * @return Wskaźnik na korzeń pola @p field.
 */
gamma_field* find_root_field(gamma_field *field, gamma_field **board);

/** @brief Łączy zbiory, do których należą pola.
 * Łączy zbiory, do których należą pola wskazywane przez @p f1 i @p f2,
 * poprzez podpięcie jednego z korzeni do drugiego.
 * @param[in,out] f1        – pierwsze pole,
 * @param[in,out] f2        – drugie pole,
 * @param[in] board         – tablica pól.
 * @return Wartość @p true, jeśli pola znajdowały się w różnych zbiorach
 * lub wartość @p false, jeśli znajdowały się w tym samym zbiorze.
 */
bool unite_fields(gamma_field *f1, gamma_field *f2, gamma_field **board);

/** @brief Sprawdza, czy pole jest korzeniem.
 * Sprawdza, czy współrzędne tego pola są równe współrzędnym jego rodzica.
 * @param[in] f     – wskaźnik na pole.
 * @return Wartość @p true, jeśli pole jest korzeniem
 * lub wartość @p false w przeciwnym wypadku.
 */
bool is_field_root(gamma_field *f);

#endif //GAMMA_GAMMA_FIELD_H
