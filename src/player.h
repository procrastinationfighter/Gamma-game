#ifndef GAMMA_PLAYER_H
#define GAMMA_PLAYER_H
#include <stdint.h>
#include <stdbool.h>

struct player{
    uint32_t number_of_areas;
    uint32_t number_of_fields;
    bool has_golden_move_available;
};
typedef struct player player;
#endif //GAMMA_PLAYER_H
