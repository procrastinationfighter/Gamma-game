#include "gamma_field.h"


gamma_field* find_root_field(gamma_field *field, gamma_field **board) {
    gamma_field *root = field;
    while(!is_field_root(root)) {
        root = &board[root->parent_x][root->parent_y];
    }

    /* Compressing path from current field to its root. */
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

bool is_field_root(gamma_field *f) {
    return (f->this_x == f->parent_x && f->this_y == f->parent_y);
}