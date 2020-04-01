#ifndef GAMMA_GAMMA_FIELD_H
#define GAMMA_GAMMA_FIELD_H

#include "player.h"

struct field{
    uint32_t owner_index;
    uint32_t parentX;
    uint32_t parentY;
};
typedef struct field gamma_field;

#endif //GAMMA_GAMMA_FIELD_H
