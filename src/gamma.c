#include "gamma.h"

#define DEFAULT_PLAYER_NUMBER 0

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

void free_visited_map(bool **map, uint32_t columns_count) {
    for(uint32_t i = 0; i < columns_count; i++) {
        free(map[i]);
    }
    free(map);
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
                board->fields[i][j].parent_x = i;
                board->fields[i][j].parent_y = j;
                board->fields[i][j].this_x = i;
                board->fields[i][j].this_y = j;
                board->fields[i][j].rank = 0;
            }
        }
    }
    return true;
}

bool allocate_players(gamma_t *board, uint32_t players) {
    board->players = malloc(players * sizeof(player_t));
    if(board->players == NULL) {
        free_board(board->fields, board->board_width);
        free(board);
        return false;
    }
    else {
        for(uint32_t i = 0; i < players; i++) {
            board->players[i].number_of_areas = 0;
            board->players[i].number_of_fields = 0;
            board->players[i].adjacent_fields = 0;
            board->players[i].has_golden_move_available = true;
        }
    }
    return true;
}

bool allocate_visited_map(gamma_t *board) {
    board->visited_fields_board = malloc(board->board_width * sizeof(bool*));
    if(board->fields == NULL) {
        return false;
    }

    for(uint32_t i = 0; i < board->board_width; i++) {
        board->fields[i] = malloc(board->board_height * sizeof(gamma_field));
        if(board->fields[i] == NULL) {
            free_board(board->fields, i);
            return false;
        }
        else {
            for(uint32_t j = 0; j < board->board_height; j++) {
                board->visited_fields_board[i][j] = false;
            }
        }
    }
    return true;
}

gamma_t* allocate_gamma_board_members(gamma_t* board, uint32_t width,
                                      uint32_t height, uint32_t players) {
    bool was_creating_successful = allocate_board(board, width, height);
    if(!was_creating_successful) {
        free(board);
        return NULL;
    }

    was_creating_successful = allocate_visited_map(board);
    if(!was_creating_successful) {
        free_board(board->fields, width);
        free(board);
        return NULL;
    }

    was_creating_successful = allocate_players(board, players);
    if(!was_creating_successful) {
        free_board(board->fields, width);
        free_visited_map(board->visited_fields_board, width);
        free(board);
        return NULL;
    }

    return board;
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
    new_board->free_fields = ((uint64_t) width) * ((uint64_t) height);
    new_board->players_count = players;
    new_board->max_areas = areas;

    new_board = allocate_gamma_board_members(new_board, width, height, players);

    return new_board;
}

void gamma_delete(gamma_t *g) {
    if(g != NULL) {
        free_board(g->fields, g->board_width);
        free_visited_map(g->visited_fields_board, g->board_width);
        free(g->players);
        free(g);
    }
}

bool is_player_parameter_valid(gamma_t *g, uint32_t player) {
    if(player == 0 || player > g->players_count) {
        return false;
    }
    else {
        return true;
    }
}

bool are_gamma_move_parameters_valid(gamma_t *g, uint32_t player,
                                     uint32_t x, uint32_t y) {
    if(x < g->board_width && y < g->board_height && is_player_parameter_valid(g, player)) {
        return true;
    }
    else {
        return false;
    }
}

bool does_player_own_adjacent_fields(gamma_t *board, uint32_t player,
                                     uint32_t x, uint32_t y) {
    //Checking whether player owns any fields adjacent to the one he wants to capture.
    if(x + 1 != board->board_width && board->fields[x+1][y].owner_index == player) {
        return true;
    }
    if(x != 0 && board->fields[x-1][y].owner_index == player) {
        return true;
    }
    if(y + 1 != board->board_height && board->fields[x][y+1].owner_index == player) {
        return true;
    }
    if(y != 0 && board->fields[x][y-1].owner_index == player) {
        return true;
    }

    return false;
}

uint32_t how_many_adjacent_fields_added(gamma_t *board, uint32_t player,
                                        uint32_t x, uint32_t y) {
    uint32_t new_fields_count = 0;

    if(x + 1 != board->board_width &&
       board->fields[x+1][y].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x + 1, y)) {
        new_fields_count++;
    }

    if(x != 0 &&
       board->fields[x-1][y].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x - 1, y)) {
        new_fields_count++;
    }

    if(y + 1 != board->board_height &&
       board->fields[x][y+1].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x, y + 1)) {
        new_fields_count++;
    }

    if(y != 0 &&
       board->fields[x+1][y].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x, y - 1)) {
        new_fields_count++;
    }

    return new_fields_count;
}

//Returns number of different sets that were combined by unification
__uint32_t add_and_unite_field(gamma_t *g, uint32_t player,
                         uint32_t x, uint32_t y) {
    uint32_t united_sets = 0;
    if(x + 1 != g->board_width && g->fields[x+1][y].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x+1][y], g->fields)) {
            united_sets++;
        }
    }

    if(x != 0 && g->fields[x-1][y].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x-1][y], g->fields)) {
            united_sets++;
        }
    }

    if(y + 1 != g->board_height && g->fields[x][y+1].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x][y+1], g->fields)) {
            united_sets++;
        }
    }

    if(x != 0 && g->fields[x][y-1].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x][y-1], g->fields)) {
            united_sets++;
        }
    }

    return united_sets;
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(!are_gamma_move_parameters_valid(g, player, x, y) ||
       g->fields[x][y].owner_index != DEFAULT_PLAYER_NUMBER) {
        return false;
    }
    player_t *curr_player = &g->players[player - 1];

    bool is_creating_new_area = !does_player_own_adjacent_fields(g, player, x, y);
    if(curr_player->number_of_areas == g->max_areas &&
       is_creating_new_area) {
        return false;
    }

    if(is_creating_new_area) {
       curr_player->number_of_areas++;
       curr_player->number_of_fields++;
       curr_player->adjacent_fields += how_many_adjacent_fields_added(g, player, x, y);
       g->fields[x][y].owner_index = player;
       g->free_fields--;
    }
    else {
        curr_player->number_of_fields++;
        curr_player->adjacent_fields += how_many_adjacent_fields_added(g, player, x, y);
        g->fields[x][y].owner_index = player;
        g->free_fields--;
        uint32_t united_fields = add_and_unite_field(g, player, x, y);
        (curr_player->number_of_areas) -= united_fields + 1;
    }

    return true;
}


bool are_golden_move_parameters_valid(gamma_t *g, uint32_t player,
                                      uint32_t x, uint32_t y) {
    if(x >= g->board_width || y >= g->board_height) {
        return false;
    }

    uint32_t field_owner = g->fields[x][y].owner_index;
    return (gamma_golden_possible(g, player) &&
            field_owner != DEFAULT_PLAYER_NUMBER &&
            field_owner != player);
}

void set_adjacent_fields_as_root(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(x + 1 != g->board_width && g->fields[x + 1][y].owner_index == player) {
        g->fields[x + 1][y].parent_x = g->fields[x + 1][y].this_x;
        g->fields[x + 1][y].parent_y = g->fields[x + 1][y].this_y;
    }

    if(x  != 0 && g->fields[x - 1][y].owner_index == player) {
        g->fields[x - 1][y].parent_x = g->fields[x - 1][y].this_x;
        g->fields[x - 1][y].parent_y = g->fields[x - 1][y].this_y;
    }

    if(y + 1 != g->board_height && g->fields[x][y + 1].owner_index == player) {
        g->fields[x][y + 1].parent_x = g->fields[x][y + 1].this_x;
        g->fields[x][y + 1].parent_y = g->fields[x][y + 1].this_y;
    }

    if(y != 0 && g->fields[x][y - 1].owner_index == player) {
        g->fields[x][y - 1].parent_x = g->fields[x][y - 1].this_x;
        g->fields[x][y - 1].parent_y = g->fields[x][y - 1].this_y;
    }
}

//Uses BFS to check how many areas were made by removing field ownership.
//Returns number of these areas.
uint32_t update_areas_after_removal(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t areas_count = 0;
    if(x + 1 != g->board_width && g->fields[x + 1][y].owner_index == player) {
        BFS(g, player, x + 1, y);
        areas_count++;
    }

    if(x != 0 && g->fields[x - 1][y].owner_index == player) {
        if (is_field_root(&g->fields[x - 1][y])){
            BFS(g, player, x - 1, y);
            areas_count++;
        }
    }

    if(y + 1 != g->board_width && g->fields[x][y + 1].owner_index == player) {
        if (is_field_root(&g->fields[x][y + 1])){
            BFS(g, player, x, y + 1);
            areas_count++;
        }
    }

    if(y != 0 && g->fields[x][y - 1].owner_index == player) {
        if (is_field_root(&g->fields[x][y - 1])){
            BFS(g, player, x, y - 1);
            areas_count++;
        }
    }

    return areas_count;
}

//True if removal was legal, false otherwise. Assumes correct parameters.
bool remove_field_ownership(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    gamma_field *curr_field = &g->fields[x][y];
    curr_field->owner_index = DEFAULT_PLAYER_NUMBER;
    bool is_removal_legal = true;

    set_adjacent_fields_as_root(g, player, x, y);

    g->fields[x][y].parent_x = x;
    g->fields[x][y].parent_y = y;

    uint32_t area_count = update_areas_after_removal(g, player, x, y);
    if((g->players[player - 1].number_of_areas) + (area_count - 1) > g->max_areas) {
        is_removal_legal = false;
    }
    else {
        (g->players[player - 1].number_of_areas) += (area_count - 1);
        (g->players[player - 1].number_of_fields)--;
    }

    return is_removal_legal;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(!are_golden_move_parameters_valid(g, player, x, y)) {
        return false;
    }

    uint32_t target_player = g->fields[x][y].owner_index;
    bool was_removing_successful = remove_field_ownership(g, player, x, y);
    bool was_adding_successful = true;
    if(was_removing_successful) {
        was_adding_successful =  gamma_move(g, player, x, y);
        if(was_adding_successful) {
            return true;
        }
    }
    //Removing or adding wasn't successful
    if(!was_removing_successful) {
        gamma_move(g, target_player, x, y);
        return true;
    }
}


uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if(!is_player_parameter_valid(g, player)) {
        return 0;
    }
    else {
        return g->players[player - 1].number_of_fields;
    }
}


uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if(!is_player_parameter_valid(g, player)) {
        return 0;
    }
    else if(g->players[player - 1].number_of_areas < g->max_areas){
        return g->free_fields;
    }
    else {
        return g->players[player-1].adjacent_fields;
    }
}


bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if(!is_player_parameter_valid(g, player) || !g->players[player-1].has_golden_move_available) {
        return false;
    }

    for(uint32_t i = 0; i < g->players_count; i++) {
        if(i != player - 1 && g->players[i].number_of_fields != 0) {
            return true;
        }
    }

    return false;
}


//TODO - dokończyć
/*
char* gamma_board(gamma_t *g) {
    for(uint32_t i = 0; i < g->board_width; i++) {
        for(uint32_t j = 0; j < g->board_height; j++) {

        }
    }
}
*/