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
#include <malloc.h>

struct field{
    uint32_t owner_index;
    uint32_t this_x;
    uint32_t this_y;
    uint32_t parent_x;
    uint32_t parent_y;
    uint32_t rank;
};
typedef struct field gamma_field;

struct queueelement{
    struct queueelement *next;
    gamma_field *field;
};
typedef struct queueelement queue_element;

struct fieldqueue{
    queue_element *front;
    queue_element *end;
};
typedef struct fieldqueue field_queue;

void field_queue_init(field_queue **q);

void field_queue_push(field_queue *q, gamma_field *element);

gamma_field* field_queue_pop(field_queue *q);

bool field_queue_is_empty(field_queue *q);

void field_queue_clear(field_queue **q);

gamma_field* find_root_field(gamma_field *field, gamma_field **board);

//Returns true if fields were in different sets before unification.
bool unite_fields(gamma_field *f1, gamma_field *f2, gamma_field **board);

bool is_field_root(gamma_field *f);

#endif //GAMMA_GAMMA_FIELD_H
