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


void field_queue_init(field_queue **q) {
    *q = malloc(sizeof(field_queue));
    if(*q == NULL) {
        errno = ENOMEM;
        exit(EXIT_FAILURE);
    }

    (*q)->front = NULL;
    (*q)->end = NULL;
}

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

inline bool field_queue_is_empty(field_queue *q) {
    return q->front == NULL;
}

void field_queue_clear(field_queue **q) {
    if(*q != NULL) {
        while (!field_queue_is_empty(*q)) {
            field_queue_pop(*q);
        }
        free(*q);
        *q = NULL;
    }
}

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

inline bool is_field_root(gamma_field *f) {
    return (f->this_x == f->parent_x && f->this_y == f->parent_y);
}