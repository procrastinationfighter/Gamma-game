/** @file
 * Implementacja klasy przechowującej stan pola w grze gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 9.04.2020
 */

#include "gamma_field.h"
#include <errno.h>
#include <stdlib.h>

/**
 * Struktura przechowująca dane o elemencie kolejki elementów typu gamma_field.
 */
typedef struct queueelement{
    struct queueelement *next; ///< Następny element.
    gamma_field *field;        ///< Pole na planszy.
} queue_element;

/**
 * Struktura reprezentująca kolejkę obiektów typu gamma_field.
 */
typedef struct fieldqueue{
    queue_element *front;   ///< Wskaźnik na pierwszy element kolejki.
    queue_element *end;     ///< Wskaźnik na ostatni element kolejki.
} field_queue;

/** @brief Inicjalizuje kolejkę.
 * Ustawia pola kolejki @p na NULL.
 * @param[out] q     – wskaźnik na wskaźnik na strukturę kolejki.
 */
void field_queue_init(field_queue **q) {
    *q = malloc(sizeof(field_queue));
    if(*q == NULL) {
        errno = ENOMEM;
        exit(EXIT_FAILURE);
    }

    (*q)->front = NULL;
    (*q)->end = NULL;
}

/** @brief Dodaje pole do kolejki.
 * Tworzy element kolejki zawierający pole, na które wskazuje @p element
 * oraz dodaje go do kolejki.
 * @param[in,out] q    – wskaźnik na strukturę kolejki,
 * @param[in] element   – wskaźnik na pole.
 */
void field_queue_push(field_queue *q, gamma_field *element) {
    if(q != NULL) {
        queue_element *el = malloc(sizeof(queue_element));
        if(el == NULL) {
            field_queue_clear(&q);
            errno = ENOMEM;
            exit(EXIT_FAILURE);
        }

        el->field = element;
        el->next = NULL;
        if(q->front == NULL) {
            q->front = el;
        }
        else {
            (q->end)->next = el;
        }
        q->end = el;
    }
}

/** @brief Wyjmuje pierwszy element z kolejki.
 * Usuwa z kolejki pierwszy element i ustanawia kolejny element jako pierwszy.
 * @param[in,out] q     – wskaźnik na strukturę kolejki.
 * @return Wskaźnik na pole będące pierwszym elementem w kolejce,
 * lub @p NULL jeśli kolejka jest pusta.
 */
gamma_field* field_queue_pop(field_queue *q) {
    if(q != NULL) {
        gamma_field *el = (q->front)->field;
        queue_element *temp = q->front;
        (q->front) = temp->next;
        free(temp);
        if(q->front == NULL) {
            q->end = NULL;
        }
        return el;
    }
    else {
        return NULL;
    }
}

/** @brief Sprawdza, czy kolejka jest pusta.
 * Sprawdza, czy kolejka nie zawiera żadnych elementów poprzez
 * sprawdzenie, czy wskaźnik na pierwszy element nie jest równy @p NULL.
 * @param[in] q     – wskaźnik na strukturę kolejki.
 * @return Wartość @p true, jeśli w kolejce nie ma elementów
 * lub wartość @p false jeśli kolejka zawiera elementy.
 */
inline bool field_queue_is_empty(field_queue *q) {
    return q->front == NULL;
}

/** @brief Usuwa kolejkę.
 * Usuwa wszystkie elementy w kolejce i zwalnia pamięć zaalokowaną na kolejkę.
 * @param[in,out] q    – wskaźnik na strukturę kolejki.
 */
void field_queue_clear(field_queue **q) {
    if(*q != NULL) {
        while (!field_queue_is_empty(*q)) {
            field_queue_pop(*q);
        }
        free(*q);
        *q = NULL;
    }
}

/** @brief Znajduje korzeń danego pola.
 * W ramach algorytmu find and union, znajduje korzeń pola, na które wskazuje
 * @p field oraz kompresuje jego ścieżkę do pola poprzez ustanowienie
 * rodzica tego pola na korzeń.
 * @param[in] field     – wskaźnik na pole,
 * @param[in] board     – tablica pól.
 * @return Wskaźnik na korzeń pola @p field.
 */
gamma_field* find_root_field(gamma_field *field, gamma_field **board) {
    gamma_field *root = field;
    while(!is_field_root(root)) {
        root = &board[root->parent_x][root->parent_y];
    }

    while(!is_field_root(field)) {
        gamma_field *temp = &board[field->parent_x][field->parent_y];
        field->parent_x = root->parent_x;
        field->parent_y = root->parent_y;
        field = temp;
    }

    return root;
}

/** @brief Łączy zbiory, do których należą pola.
 * Łączy zbiory, do których należą pola wskazywane przez @p f1 i @p f2,
 * poprzez podpięcie jednego z korzeni do drugiego.
 * @param[in,out] f1        – pierwsze pole,
 * @param[in,out] f2        – drugie pole,
 * @param[in] board         – tablica pól.
 * @return Wartość @p true, jeśli pola znajdowały się w różnych zbiorach
 * lub wartość @p false, jeśli znajdowały się w tym samym zbiorze.
 */
bool unite_fields(gamma_field *f1, gamma_field *f2, gamma_field **board) {
    gamma_field *root1 = find_root_field(f1, board);
    gamma_field *root2 = find_root_field(f2, board);

    if(root1 == root2) {
        return false;
    }
    else if (root1->rank < root2->rank){
        gamma_field *temp = root1;
        root1 = root2;
        root2 = temp;
    }

    root2->parent_x = root1->this_x;
    root2->parent_y = root1->this_y;

    if(root1->rank == root2->rank) {
        (root1->rank)++;
    }
    return true;
}

/** @brief Sprawdza, czy pole jest korzeniem.
 * Sprawdza, czy współrzędne tego pola są równe współrzędnym jego rodzica.
 * @param[in] f     – wskaźnik na pole.
 * @return Wartość @p true, jeśli pole jest korzeniem
 * lub wartość @p false w przeciwnym wypadku.
 */
inline bool is_field_root(gamma_field *f) {
    return (f->this_x == f->parent_x && f->this_y == f->parent_y);
}