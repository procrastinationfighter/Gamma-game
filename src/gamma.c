#include "gamma.h"

#define DEFAULT_PLAYER_NUMBER -1

bool are_gamma_new_parameters_valid(uint32_t width, uint32_t height, uint32_t players) {
    if(width == 0 || height == 0 || players == 0) {
        return false;
    }
    else {
        return true;
    }
}

void free_board(gamma_field **fields, uint32_t columns_count){
    for(uint32_t i = 0; i < columns_count; i++) {
        free(fields[i]);
    }
    free(fields);
}

bool allocate_board(gamma_t *board, uint32_t width, uint32_t height) {
    board->fields = malloc(width * sizeof(gamma_field*));
    if(board->fields == NULL) {
        return false;
    }

    for(uint32_t i = 0; i < width; i++) {
        board->fields[i] = malloc(height * sizeof(gamma_field));
        if(board->fields[i] == NULL) {
            free_board(board->fields, i);
            return false;
        }
        else {
            for(uint32_t j = 0; j < height; j++) {
                board->fields[i][j].owner_index = DEFAULT_PLAYER_NUMBER;
                board->fields[i][j].parentX = i;
                board->fields[i][j].parentY = j;
            }
        }
    }
    return true;
}

bool allocate_players(gamma_t *board, uint32_t players) {
    board->players = malloc(players * sizeof(player));
    if(board->players == NULL) {
        free_board(board->fields, board->board_width);
        free(board);
        return false;
    }
    else {
        for(uint32_t i = 0; i < players; i++) {
            board->players[i].number_of_areas = 0;
            board->players[i].has_golden_move_available = true;
        }
    }
    return true;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    bool was_creating_successful = are_gamma_new_parameters_valid(width, height, players);
    if(!was_creating_successful) {
        return NULL;
    }

    gamma_t *new_board = malloc(sizeof(gamma_t));
    if(new_board == NULL) {
        return NULL;
    }

    new_board->board_width = width;
    new_board->board_height = height;
    new_board->busy_fields = 0;
    new_board->max_areas = areas;

    was_creating_successful = allocate_board(new_board, width, height);
    if(!was_creating_successful) {
        free(new_board);
        return NULL;
    }

    was_creating_successful = allocate_players(new_board, players);
    if(!was_creating_successful) {
        free_board(new_board->fields, width);
        free(new_board);
        return NULL;
    }

    return new_board;
}

void gamma_delete(gamma_t *g) {
    if(g != NULL) {
        free_board(g->fields, g->board_width);
        free(g);
    }
}

/*
 //TODO dodać przypadek gdy braknie pamięci w trakcie dodawania
bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {

}


bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {

}


uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {

}


uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {

}


bool gamma_golden_possible(gamma_t *g, uint32_t player) {

}


char* gamma_board(gamma_t *g) {

}
 */