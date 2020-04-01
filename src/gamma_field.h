#ifndef GAMMA_GAMMA_FIELD_H
#define GAMMA_GAMMA_FIELD_H

#include <stdint.h>

struct field{
    uint32_t owner_index;
    uint32_t this_x;
    uint32_t this_y;
    uint32_t parent_x;
    uint32_t parent_y;
    uint32_t rank;
};
typedef struct field gamma_field;

gamma_field* find_root_field(gamma_field *field, gamma_field **board);

void unite_fields(gamma_field *f1, gamma_field *f2, gamma_field **board);

#endif //GAMMA_GAMMA_FIELD_H
