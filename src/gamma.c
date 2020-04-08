/** @file
 * Implementacja klasy przechowującej stan gry gamma
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 9.04.2020
 */

#include "gamma.h"
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include "gamma_field.h"

#define DEFAULT_PLAYER_NUMBER 0
#define DEFAULT_PLAYER_IDENTIFIER '.'

/**
 * Struktura przechowująca dane o jednym graczu.
 */
struct player{
    uint32_t number_of_areas;
    uint32_t number_of_fields;
    uint32_t adjacent_fields;
    bool has_golden_move_available;
};
typedef struct player player_t;

/**
 * Struktura przechowująca stan gry.
 */
struct gamma {
    player_t *players;
    gamma_field **fields;
    bool **visited_fields_board;
    uint32_t board_width;
    uint32_t board_height;
    uint32_t max_areas;
    uint32_t players_count;
    uint64_t free_fields;
};

/** @brief Sprawdza, czy parametry funkcji @ref gamma_new są poprawne.
 * Sprawdza, czy parametry @p width, @p height, @p players są niezerowe.
 * @param[in] width         – szerokość planszy,
 * @param[in] height        – wysokość planszy,
 * @param[in] players       – ilość graczy.
 * @return Wartość @p true, jeśli wszystkie parametry są większe od zera,
 * lub false jeśli którykolwiek jest równy zero.
 */
bool are_gamma_new_parameters_valid(uint32_t width, uint32_t height,
                                    uint32_t players) {
    if(width == 0 || height == 0 || players == 0) {
        return false;
    }
    else {
        return true;
    }
}

/** @brief Zwalnia zaalokowaną pamięć w dwuwymiarowej tablicy gamma_field.
 * Zwalnia pamięć zaalokowaną na pierwsze @p columns_count wskaźników w tablicy
 * @p fields oraz zwalnia pamięć zaalokowaną na wskaźnik @p fields.
 * @param[in] fields         – dwuwymiarowa tablica zmiennych typu gamma_field
 * @param[in] columns_count  – liczba zaalokowanych wskaźników w tablicy fields.
 */
void free_board(gamma_field **fields, uint32_t columns_count){
    for(uint32_t i = 0; i < columns_count; i++) {
        free(fields[i]);
    }
    free(fields);
}

/** @brief Zwalnia zaalokowaną pamięć w dwuwymiarowej tablicy gamma_field.
 * Zwalnia pamięć zaalokowaną na pierwsze @p columns_count wskaźników w tablicy
 * @p map oraz zwalnia pamięć zaalokowaną na wskaźnik @p map.
 * @param[in] map            – tablica "odwiedzonych" przez algorytm BFS pól,
 * @param[in] columns_count  – liczba zaalokowanych wskaźników w tablicy map.
 */
void free_visited_map(bool **map, uint32_t columns_count) {
    for(uint32_t i = 0; i < columns_count; i++) {
        free(map[i]);
    }
    free(map);
}

/** @brief Alokuje pamięć potrzebną do przechowywania danych o polach.
 * Alokuje pamięć potrzebną do przechowywania danych o polach w składowej fields
 * zmiennej @p board i ustawia domyślne wartości każdego z pól. W przypadku
 * niepowodzenia, zwalnia dotychczas zaalokowaną w tej funkcji pamięć.
 * @param[in,out] board      – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] width          – szerokość planszy, liczba dodatnia,
 * @param[in] height         – wysokość planszy, liczba dodatnia.
 * @return Wartość @p true jeśli alokacja się powiodła i @p false w przeciwnym
 * wypadku.
 */
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

/** @brief Alokuje pamięć potrzebną do przechowywania danych o graczach.
 * Alokuje tablicę rozmiaru @p players w składowej players zmiennej @p board.
 * @param[in,out] board      – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] players        – liczba graczy, wartość dodatnia.
 * @return Wartość @p true jeśli alokacja się powiodła i @p false w przeciwnym
 * wypadku.
 */
bool allocate_players(gamma_t *board, uint32_t players) {
    board->players = malloc(players * sizeof(player_t));
    if(board->players == NULL) {
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

/** @brief Alokuje pamięć potrzebną do przechowywania mapy odwiedzonych pól.
 * Alokuje dwuwymiarową tablicę w składowej map zmiennej @p board. Tablica
 * ma wymiary @p board_width i @p board_height będące składowymi zmiennej @p board.
 * @param[in,out] board      – wskaźnik na strukturę przechowującą dane gry,.
 * @return Wartość @p true jeśli alokacja się powiodła i @p false w przeciwnym
 * wypadku.
 */
bool allocate_visited_map(gamma_t *board) {
    board->visited_fields_board = malloc(board->board_width * sizeof(bool*));
    if(board->visited_fields_board == NULL) {
        return false;
    }

    for(uint32_t i = 0; i < board->board_width; i++) {
        board->visited_fields_board[i] = malloc(board->board_height * sizeof(gamma_field));
        if(board->visited_fields_board[i] == NULL) {
            free_visited_map(board->visited_fields_board, i);
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

/** @brief Ustawia wszystkie wartości mapy odwiedzonych pól na @p false.
 * Ustawia wszystkie pola dwuwymiarowej tablicy @p map, będącej składową
 * zmiennej @p board na @p false.
 * @param[in,out] board      – wskaźnik na strukturę przechowującą dane gry,
 */
void reset_visited_map(gamma_t *board) {
    for(uint32_t i = 0; i < board->board_width; i++) {
        for(uint32_t j = 0; j < board->board_height; j++) {
            board->visited_fields_board[i][j] = false;
        }
    }
}

/** @brief Alokuje pamięć potrzebną do przechowywania tablic w zmiennej @p board.
 * Alokuje pamięć do składowych @p players, @p fields i @p map zmiennej @p board.
 * W przypadku niepowodzenia, zwalnia całą zaalokowaną w zmiennej @p board pamięć
 * i ustawia wartość @p errno na ENOMEM.
 * @param[in,out] board      – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] players        – liczba graczy, wartość dodatnia.
 * @return Wartość @p true jeśli alokacja się powiodła i @p false w przeciwnym
 * wypadku.
 */
gamma_t* allocate_board_arrays(gamma_t* board, uint32_t width,
                               uint32_t height, uint32_t players) {
    bool was_creating_successful = allocate_board(board, width, height);
    if(!was_creating_successful) {
        free(board);
        errno = ENOMEM;
        return NULL;
    }

    was_creating_successful = allocate_visited_map(board);
    if(!was_creating_successful) {
        free_board(board->fields, width);
        free(board);
        errno = ENOMEM;
        return NULL;
    }

    was_creating_successful = allocate_players(board, players);
    if(!was_creating_successful) {
        free_board(board->fields, width);
        free_visited_map(board->visited_fields_board, width);
        free(board);
        errno = ENOMEM;
        return NULL;
    }

    return board;
}

/** @brief Tworzy strukturę przechowującą stan gry.
 * Alokuje pamięć na nową strukturę przechowującą stan gry.
 * Inicjuje tę strukturę tak, aby reprezentowała początkowy stan gry.
 * @param[in] width   – szerokość planszy, liczba dodatnia,
 * @param[in] height  – wysokość planszy, liczba dodatnia,
 * @param[in] players – liczba graczy, liczba dodatnia,
 * @param[in] areas   – maksymalna liczba obszarów,
 *                      jakie może zająć jeden gracz.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 * zaalokować pamięci lub któryś z parametrów jest niepoprawny.
 */
gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if(!are_gamma_new_parameters_valid(width, height, players)) {
        return NULL;
    }

    gamma_t *new_board = malloc(sizeof(gamma_t));
    if(new_board == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    new_board->board_width = width;
    new_board->board_height = height;
    new_board->free_fields = ((uint64_t) width) * ((uint64_t) height);
    new_board->players_count = players;
    new_board->max_areas = areas;

    new_board = allocate_board_arrays(new_board, width, height, players);

    return new_board;
}

/** @brief Usuwa strukturę przechowującą stan gry.
* Usuwa z pamięci strukturę wskazywaną przez @p g.
* Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
* @param[in] g       – wskaźnik na usuwaną strukturę.
*/
void gamma_delete(gamma_t *g) {
    if(g != NULL) {
        free_board(g->fields, g->board_width);
        free_visited_map(g->visited_fields_board, g->board_width);
        free(g->players);
        free(g);
    }
}

/** @brief Sprawdza, czy parametr @p player jest poprawny.
 * Sprawdza, czy parametr @p player jest różny od zera
 * i niewiększy od wartości @p players_count ze zmiennej @p g.
 * @param[in] g      – wskaźnik na strukturę przechowującą dane gry.
 * @param[in] player – indeks gracza.
 * @return Wartość @p true, jeśli parametr znajduje się w podanym zakresie
 * lub @p false w przeciwnym wypadku.
 */
bool is_player_parameter_valid(gamma_t *g, uint32_t player) {
    if(player == 0 || player > g->players_count) {
        return false;
    }
    else {
        return true;
    }
}

/** @brief Sprawdza, czy parametry funkcji @ref gamma_move są prawidłowe.
 * Sprawdza, czy parametry @p player, @p x i @p y funkcji @ref gamma_move
 * mieszczą się w zakresach wyznaczanych przez składowe zmiennej @p board,
 * odpowiednio @p players_count, @p board_width i @p board_height.
 * @param[in] g      – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] player – indeks gracza, liczba dodatnia
 *                     i niewiększa od składowej @p players_count,
 * @param[in] x      – numer kolumny, mniejszy od składowej @p board_width,
 * @param[in] y      – numer wiersza, mniejszy od składowej @p board_height
 * @return Wartość @p true, jeśli parametry znajduje się we właściwych zakresacj
 * lub @p false w przeciwnym wypadku.
 */
bool are_gamma_move_parameters_valid(gamma_t *g, uint32_t player,
                                     uint32_t x, uint32_t y) {
    if(x < g->board_width && y < g->board_height && is_player_parameter_valid(g, player)) {
        return true;
    }
    else {
        return false;
    }
}

/** @brief Sprawdza, czy dany gracz jest w posiadaniu pól sąsiednich do (@p x, @p y).
 * Sprawdza, czy którekolwiek z pól o współrzędnych
 * (@p x + 1, @p y), (@p x - 1, @p y), (@p x, @p y + 1), (@p x, @p y - 1)
 * ma wartość @p owner_index równą @p player.
 * Jeśli powyższe pola nie istnieją, nie zostają sprawdzane.
 * @param[in] board  – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] player – indeks gracza, liczba dodatnia
 *                     i niewiększa od składowej @p players
 *                     ze zmiennej @p board
 * @param[in] x      – numer kolumny, mniejszy od składowej
 *                     @p board_width ze zmiennej @p board,
 * @param[in] y      – numer wiersza, mniejszy od składowej
 *                     @p board_height ze zmiennej @p board.
 * @return Wartość @p true jeśli którekolwiek z wymienionych pól
 *         należy do danego gracza lub wartość @p false w przeciwnym
 *         przypadku.
 */
bool does_player_own_adjacent_fields(gamma_t *board, uint32_t player,
                                     uint32_t x, uint32_t y) {
    //Checking whether player owns any fields adjacent to the one he wants to capture.
    if(x + 1 < board->board_width && board->fields[x+1][y].owner_index == player) {
        return true;
    }
    if(x > 0 && board->fields[x-1][y].owner_index == player) {
        return true;
    }
    if(y + 1 < board->board_height && board->fields[x][y+1].owner_index == player) {
        return true;
    }
    if(y > 0 && board->fields[x][y-1].owner_index == player) {
        return true;
    }

    return false;
}
/** @brief Wylicza, ile nowych pól sąsiaduje z graczem po wykonaniu ruchu.
 * Sprawdza, pól sąsiadujących z polem o współrzędnych (@p x, @p y)
 * nie sąsiaduje z innymi polami należącymi do danego gracza.
 * Przekazuje ilość tych pól.
 * @param[in] board  – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] player – indeks gracza, liczba dodatnia
 *                     i niewiększa od składowej @p players
 *                     ze zmiennej @p board
 * @param[in] x      – numer kolumny, mniejszy od składowej
 *                     @p board_width ze zmiennej @p board,
 * @param[in] y      – numer wiersza, mniejszy od składowej
 *                     @p board_height ze zmiennej @p board.
 * @return Liczba w zakresie od 0 do 4 włącznie, oznaczająca
 *         ilość nowych pól sąsiednich do wszystkich pól gracza
 *         @p player.
 */
uint32_t how_many_adjacent_fields_added(gamma_t *board, uint32_t player,
                                        uint32_t x, uint32_t y) {
    uint32_t new_fields_count = 0;

    if(x + 1 < board->board_width &&
       board->fields[x+1][y].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x + 1, y)) {
        new_fields_count++;
    }

    if(x > 0 &&
       board->fields[x-1][y].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x - 1, y)) {
        new_fields_count++;
    }

    if(y + 1 < board->board_height &&
       board->fields[x][y+1].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x, y + 1)) {
        new_fields_count++;
    }

    if(y > 0 &&
       board->fields[x][y-1].owner_index == DEFAULT_PLAYER_NUMBER &&
       !does_player_own_adjacent_fields(board, player, x, y - 1)) {
        new_fields_count++;
    }

    return new_fields_count;
}

/** @brief Łączy pole i sąsiadujące, należące do tego samego gracza pola.
 * Łączy singleton pola o współrzędnych (@p x, @p y) ze zbiorami, do których
 * należą pola (@p x + 1, @p y), (@p x - 1, @p y), (@p x, @p y + 1), (@p x, @p y - 1),
 * pod warunkiem że te pola należą do gracza @p player.
 * @param[in, out] board  – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] player      – indeks gracza, liczba dodatnia
 *                          i niewiększa od składowej @p players
 *                          ze zmiennej @p board
 * @param[in] x           – numer kolumny, mniejszy od składowej
 *                          @p board_width ze zmiennej @p board,
 * @param[in] y           – numer wiersza, mniejszy od składowej
 *                          @p board_height ze zmiennej @p board.
 * @return Liczba w zakresie od 0 do 4 włącznie, oznaczająca liczbę różnych
 * zbiorów, które zostały połączone.
 */
__uint32_t add_and_unite_field(gamma_t *g, uint32_t player,
                         uint32_t x, uint32_t y) {
    uint32_t united_sets = 0;
    if(x + 1 < g->board_width && g->fields[x+1][y].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x+1][y], g->fields)) {
            united_sets++;
        }
    }

    if(x > 0 && g->fields[x-1][y].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x-1][y], g->fields)) {
            united_sets++;
        }
    }

    if(y + 1 < g->board_height && g->fields[x][y+1].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x][y+1], g->fields)) {
            united_sets++;
        }
    }

    if(y > 0 && g->fields[x][y-1].owner_index == player) {
        if(unite_fields(&g->fields[x][y], &g->fields[x][y-1], g->fields)) {
            united_sets++;
        }
    }

    return united_sets;
}

/** @brief Sprawdza, czy dane pole należy do innego gracza.
 * Sprawdza, czy wartość @p owner_index w zmiennej @p field
 * jest różna od domyślnej i od wartości @p player.
 * @param[in] field      – wskaźnik na strukturę przechowującą dane pola,
 * @param[in] player     – indeks gracza, liczba dodatnia
 *                         i niewiększa od składowej @p players
 *                         ze zmiennej @p board.
 * @return Wartość @p true jeśli pole należy do innego gracza
 *         lub wartość @p false w przeciwnym wypadku.
 */
bool does_field_belong_to_other_player(gamma_field *field, uint32_t player) {
    return (field->owner_index != DEFAULT_PLAYER_NUMBER &&
            field->owner_index != player);
}

/** @brief Sprawdza, czy dany gracz został już uwzględniony przy aktualizacji.
 * Sprawdza, czy indeks @p player znajduje się w tablicy @p players o wielkości
 * @p players_count.
 * @param[in] player        – indeks gracza, liczba dodatnia
 *                            i niewiększa od składowej @p players
 *                            ze zmiennej @p board.
 * @param[in] players       – tablica przechowująca indeksy graczy, którzy
 *                            zostali już uwzględnieni.
 * @param[in] players_count – ilość indeksów w tablicy @p players.
 * @return Wartość @p true jeśli gracz już został uzwględniony lub
 *         wartość @p false jeśli nie został uwzględniony.
 */
bool was_player_adjacent_already_updated(uint32_t player, const uint32_t *players,
                                         uint32_t players_count) {
    for(uint32_t i = 0; i < players_count; i++) {
        if(players[i] == player) {
            return true;
        }
    }
    return false;
}

/** @brief Po ruchu, aktualizuje liczbę pól sąsiadnich innych graczy.
 * Sprawdza, czy pola sąsiednie do pola o współrzędnych (@p x, @p y)
 * należą do innego gracza, niż tego o indeksie @p player. Jeśli tak,
 * aktualizuje dla każdego danego gracza całkowitą ilość pól sąsiadujących
 * z jego polami.
 * @param[in, out] board      – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] player              – indeks gracza, liczba dodatnia
 *                              i niewiększa od składowej @p players
 *                              ze zmiennej @p board,
 * @param[in] x                   – numer kolumny, mniejszy od składowej
 *                               @p board_width ze zmiennej @p board,
 * @param[in] y                   – numer wiersza, mniejszy od składowej
 *                              @p board_height ze zmiennej @p board.
 */
void update_other_players_adjacent_fields_after_move(gamma_t *g, uint32_t player,
                                                     uint32_t x, uint32_t y) {
    uint32_t players_checked[3];
    uint32_t players_count = 0;
    if(x + 1 < g->board_width &&
       does_field_belong_to_other_player(&g->fields[x + 1][y], player)) {
        (g->players[g->fields[x + 1][y].owner_index - 1].adjacent_fields)--;
        players_checked[players_count] = g->fields[x + 1][y].owner_index;
        players_count++;
    }

    if(x > 0 &&
       does_field_belong_to_other_player(&g->fields[x - 1][y], player) &&
       !was_player_adjacent_already_updated(g->fields[x - 1][y].owner_index,
                                            players_checked, players_count)) {
        (g->players[g->fields[x - 1][y].owner_index - 1].adjacent_fields)--;
        players_checked[players_count] = g->fields[x - 1][y].owner_index;
        players_count++;
    }

    if(y + 1 < g->board_height &&
       does_field_belong_to_other_player(&g->fields[x][y + 1], player)&&
       !was_player_adjacent_already_updated(g->fields[x][y + 1].owner_index,
                                            players_checked, players_count)) {
        (g->players[g->fields[x][y + 1].owner_index - 1].adjacent_fields)--;
        players_checked[players_count] = g->fields[x][y + 1].owner_index;
        players_count++;
    }

    if(y > 0 &&
       does_field_belong_to_other_player(&g->fields[x][y - 1], player)&&
       !was_player_adjacent_already_updated(g->fields[x][y - 1].owner_index,
                                            players_checked, players_count)) {
        (g->players[g->fields[x][y - 1].owner_index - 1].adjacent_fields)--;
    }
}
/** @brief Wykonuje ruch.
 * Ustawia pionek gracza @p player na polu (@p x, @p y).
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return Wartość @p true, jeśli ruch został wykonany, a @p false,
 * gdy ruch jest nielegalny lub któryś z parametrów jest niepoprawny.
 */
bool gamma_move(gamma_t *g, uint32_t player,
                uint32_t x, uint32_t y) {
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

    (curr_player->number_of_fields)++;
    (curr_player->adjacent_fields) += how_many_adjacent_fields_added(g, player, x, y);
    (g->free_fields)--;
    if(is_creating_new_area) {
        curr_player->number_of_areas++;
    }
    else {
        uint32_t united_fields = add_and_unite_field(g, player, x, y);
        (curr_player->number_of_areas) -= united_fields;
        (curr_player->number_of_areas) += 1;
        (curr_player->adjacent_fields)--;
    }
    g->fields[x][y].owner_index = player;

    update_other_players_adjacent_fields_after_move(g, player, x, y);
    return true;
}

/** @brief Sprawdza, czy parametry funkcji @ref golden_move śa prawidłowe.
 * Sprawdza, czy parametry @p player, @p x i @p y mieszczą się w zakresach
 * ustalanych przez składowe zmiennej @p board, odpowiednio @p players_count,
 * @p board_width i @p board_height.
 * @param[in] g          – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] player     – indeks gracza, liczba dodatnia
 *                         i niewiększa od składowej @p players
 *                         ze zmiennej @p g,
 * @param[in] x          – numer kolumny, mniejszy od składowej
 *                         @p board_width ze zmiennej @p g,
 * @param[in] y          – numer wiersza, mniejszy od składowej
 *                         @p board_height ze zmiennej @p g.
 * @return
 */
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

/** @brief Ustawia sąsiednie pola należące do gracza jako korzenie w ramach find and union.
 * Jeśli pola (@p x + 1, @p y), (@p x - 1, @p y), (@p x, @p y + 1), (@p x, @p y - 1)
 * należą do gracza @p player, zostają ustanowione jako korzenie w ramach algorytmu
 * find and union. Jest to przygotowanie do przejścia planszy za pomocą
 * algorytmu BFS.
 * @param[in, out] g     – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] player     – indeks gracza, liczba dodatnia
 *                         i niewiększa od składowej @p players
 *                         ze zmiennej @p g,
 * @param[in] x          – numer kolumny, mniejszy od składowej
 *                         @p board_width ze zmiennej @p g,
 * @param[in] y          – numer wiersza, mniejszy od składowej
 *                         @p board_height ze zmiennej @p g.
 */
void set_adjacent_fields_as_root(gamma_t *g, uint32_t player,
                                 uint32_t x, uint32_t y) {
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

/** @brief Sprawdza, czy dane pole powinno zostać odwiedzone podczas przechodzenia planszy.
 * Sprawdza, czy dane pole spełnia następujące warunki:
 * nie zostało jeszcze odwiedzone,
 * należy do danego gracza.
 * @param[in] field       – wskaźnik na strukturę przechowującą dane pola,
 * @param[in] player      – indeks gracza, liczba dodatnia,
 * @param[in] visited_map – dwuwymiarowa tablica przechowująca informacje
 *                          o tym, czy pola zostały już odwiedzone.
 * @return Wartosć @p true jeśli pole spełnia powyższe warunki
 *         lub wartość @p false, jeśli ich nie spełnia.
 */
bool should_field_be_visited(gamma_field *field, uint32_t player,
                             bool **visited_map) {
    return (field->owner_index == player && !visited_map[field->this_x][field->this_y]);
}

/** @brief Dodaje do kolejki sąsiednie pola w ramach algorytmu BFS.
 * Dodaje do kolejki @p queue pola o współrzędnych
 * (@p field->this_x + 1, @p field->this_y), (@p field->this_x - 1, @p field->this_y),
 * (@p field->this_x, @p field->this_y + 1), (@p field->this_x, @p field->this_y - 1),
 * jeśli nie zostały one odwiedzone i należą do gracza o indeksie @p field->owner_index.
 * @param queue[in, out] – wskaźnik na strukturę reprezentującą kolejkę
 *                         danych typu gamma_field,
 * @param g[in]          – wskaźnik na strukturę przechowującą dane gry,
 * @param field[in]      – wskaźnik na strukturę przechowującą dane pola,
 */
void add_adjacent_fields_to_queue(field_queue *queue, gamma_t *g,
                                  gamma_field *field) {
    uint32_t curr_x = field->this_x;
    uint32_t curr_y = field->this_y;

    if(curr_x + 1 != g->board_width &&
       should_field_be_visited(&g->fields[curr_x + 1][curr_y],
                               field->owner_index, g->visited_fields_board)) {
        field_queue_push(queue, &g->fields[curr_x + 1][curr_y]);
        g->visited_fields_board[curr_x + 1][curr_y] = true;
    }

    if(curr_x != 0 &&
       should_field_be_visited(&g->fields[curr_x - 1][curr_y],
                               field->owner_index, g->visited_fields_board)) {
        field_queue_push(queue, &g->fields[curr_x - 1][curr_y]);
        g->visited_fields_board[curr_x - 1][curr_y] = true;
    }

    if(curr_y + 1 != g->board_height &&
       should_field_be_visited(&g->fields[curr_x][curr_y + 1],
                               field->owner_index, g->visited_fields_board)) {
        field_queue_push(queue, &g->fields[curr_x][curr_y + 1]);
        g->visited_fields_board[curr_x][curr_y + 1] = true;
    }

    if(curr_y != 0 &&
       should_field_be_visited(&g->fields[curr_x][curr_y - 1],
                               field->owner_index, g->visited_fields_board)) {
        field_queue_push(queue, &g->fields[curr_x][curr_y - 1]);
        g->visited_fields_board[curr_x][curr_y - 1] = true;
    }
}

/** @brief Ustawia dane pole jako korzeń swojego zbioru w ramach find and union.
 * Korzystając z algorytmu BFS, ustawia współrzędne korzenia wszystkich elementów
 * zbioru na (@p x, @p y). Aktualizowany zbiór jest zbiorem, do którego należy
 * pole o współrzędnych (@p x, @p y).
 * @param[in, out] g     – wskaźnik na strukturę przechowującą dane gry,
 * @param[in] x          – numer kolumny, mniejszy od składowej
 *                         @p board_width ze zmiennej @p g,
 * @param[in] y          – numer wiersza, mniejszy od składowej
 *                         @p board_height ze zmiennej @p g.
 */
void set_field_as_set_root(gamma_t *g, uint32_t x, uint32_t y) {
    field_queue *queue;
    field_queue_init(&queue);
    field_queue_push(queue, &g->fields[x][y]);
    g->visited_fields_board[x][y] = true;

    while(!field_queue_is_empty(queue)) {
        gamma_field *curr_field = field_queue_pop(queue);
        // Setting new root
        curr_field->parent_x = x;
        curr_field->parent_y = y;

        add_adjacent_fields_to_queue(queue, g, curr_field);
    }
    reset_visited_map(g);
    field_queue_clear(&queue);
}

//Uses BFS to check how many areas were left by removing field ownership.
//Returns number of these areas - 0 if field was alone, at least 1 otherwise.
int update_areas_after_removal(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    int areas_count = 0;
    if(x + 1 < g->board_width && g->fields[x + 1][y].owner_index == player) {
        set_field_as_set_root(g, x + 1, y);
        areas_count++;
    }

    if(x > 0 && g->fields[x - 1][y].owner_index == player) {
        if (is_field_root(&g->fields[x - 1][y])){
            set_field_as_set_root(g, x - 1, y);
            areas_count++;
        }
    }

    if(y + 1 < g->board_height && g->fields[x][y + 1].owner_index == player) {
        if (is_field_root(&g->fields[x][y + 1])){
            set_field_as_set_root(g, x, y + 1);
            areas_count++;
        }
    }

    if(y > 0 && g->fields[x][y - 1].owner_index == player) {
        if (is_field_root(&g->fields[x][y - 1])){
            set_field_as_set_root(g, x, y - 1);
            areas_count++;
        }
    }

    return areas_count;
}

void update_other_players_adjacent_fields_after_removing(gamma_t *g,
                            uint32_t player, uint32_t x, uint32_t y) {
    uint32_t players_checked[3];
    uint32_t players_count = 0;
    if(x + 1 != g->board_width &&
       does_field_belong_to_other_player(&g->fields[x + 1][y], player)) {
        (g->players[g->fields[x + 1][y].owner_index - 1].adjacent_fields)++;
        players_checked[players_count] = g->fields[x + 1][y].owner_index;
        players_count++;
    }

    if(x != 0 &&
       does_field_belong_to_other_player(&g->fields[x - 1][y], player) &&
       !was_player_adjacent_already_updated(g->fields[x - 1][y].owner_index,
                                            players_checked, players_count)) {
        (g->players[g->fields[x - 1][y].owner_index - 1].adjacent_fields)++;
        players_checked[players_count] = g->fields[x - 1][y].owner_index;
        players_count++;
    }

    if(y + 1 != g->board_height &&
       does_field_belong_to_other_player(&g->fields[x][y + 1], player) &&
       !was_player_adjacent_already_updated(g->fields[x][y + 1].owner_index,
                                            players_checked, players_count)) {
        (g->players[g->fields[x][y + 1].owner_index - 1].adjacent_fields)++;
        players_checked[players_count] = g->fields[x][y + 1].owner_index;
        players_count++;
    }

    if(y != 0 &&
       does_field_belong_to_other_player(&g->fields[x][y - 1], player) &&
       !was_player_adjacent_already_updated(g->fields[x][y - 1].owner_index,
                                              players_checked, players_count)) {
        (g->players[g->fields[x][y - 1].owner_index - 1].adjacent_fields)++;
    }
}

void update_player_adjacent_fields_after_removing(gamma_t *g, uint32_t player,
                                                  uint32_t x, uint32_t y) {
    uint32_t removed_fields = how_many_adjacent_fields_added(g, player, x , y);
    g->players[player - 1].adjacent_fields -= removed_fields;
}

//True if removal was legal(player would not be left with more areas than he should own),
// false otherwise. Assumes correct parameters.
bool remove_field_ownership(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    gamma_field *curr_field = &g->fields[x][y];
    curr_field->owner_index = DEFAULT_PLAYER_NUMBER;
    bool is_removal_legal = true;

    set_adjacent_fields_as_root(g, player, x, y);

    curr_field->parent_x = x;
    curr_field->parent_y = y;

    int area_count = update_areas_after_removal(g, player, x, y);
    if((g->players[player - 1].number_of_areas) + (area_count - 1) > g->max_areas) {
        is_removal_legal = false;
    }
    (g->players[player - 1].number_of_areas) += (area_count - 1);
    (g->players[player - 1].number_of_fields)--;
    (g->free_fields)++;
    update_player_adjacent_fields_after_removing(g, player, x, y);
    update_other_players_adjacent_fields_after_removing(g, player, x, y);
    if(area_count > 0) {
        //If field was not alone, it should be accounted as
        // an adjacent field that player can move onto.
        (g->players[player - 1].adjacent_fields)++;
    }

    return is_removal_legal;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    //Wrong parameters or moving player here would be illegal
    if(!are_golden_move_parameters_valid(g, player, x, y) ||
      (g->players[player - 1].number_of_areas == g->max_areas &&
             !does_player_own_adjacent_fields(g, player, x, y))) {
        return false;
    }

    uint32_t target_player = g->fields[x][y].owner_index;
    bool was_removing_successful = remove_field_ownership(g, target_player, x, y);
    if(was_removing_successful) {
        gamma_move(g, player, x, y);
        g->players[player - 1].has_golden_move_available = false;
        return true;
    }
    else { //Removing wasn't successful, so we have to recover state from before removing.
        gamma_move(g, target_player, x, y);
        return false;
    }
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if(g == NULL || !is_player_parameter_valid(g, player)) {
        return 0;
    }
    else {
        return g->players[player - 1].number_of_fields;
    }
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if(g == NULL || !is_player_parameter_valid(g, player)) {
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
    if(g == NULL || !is_player_parameter_valid(g, player) ||
       !g->players[player-1].has_golden_move_available) {
        return false;
    }

    for(uint32_t i = 0; i < g->players_count; i++) {
        if(i != player - 1 && g->players[i].number_of_fields != 0) {
            return true;
        }
    }

    return false;
}

char digit_to_char(uint32_t digit) {
    return (char)(digit + (uint32_t) '0');
}

void parse_multidigit_number(char* s, uint32_t number, uint64_t *length) {
    s[*length] = '[';
    (*length)++;
    uint32_t digits = (uint32_t) log10(number) + 1;
    uint32_t divider = pow(10, digits - 1);
    for(uint32_t i = 0; i < digits; i++) {
        s[*length] = digit_to_char(number / divider);
        number = number % divider;
        divider /= 10;
        (*length)++;
    }
    s[*length] = ']';
}

uint64_t how_many_characters_will_map_have(gamma_t *g) {
    //One for every field + one column of '\n'
    uint64_t size = ((uint64_t) g->board_height * (uint64_t)(g->board_width + 1));
    //Every player with number above 9 will use:
    // a pair of brackets and more than one digit
    uint32_t next_limiter = 99;
    uint32_t bonus_digits = 1;
    for(uint32_t i = 9; i < g->players_count; i++) {
        if(i == next_limiter) {
            next_limiter = next_limiter * 10 + 9;
            bonus_digits++;
        }
        size += g->players[i].number_of_fields * bonus_digits;
    }
    //One place for '\0'
    size++;

    return size;
}

char* gamma_board(gamma_t *g) {
    uint64_t array_size = how_many_characters_will_map_have(g);
    char* map_string = malloc(array_size * sizeof(char));
    uint64_t curr_index = 0;
    if(map_string == NULL) {
        return NULL;
    }

    for(uint32_t y = g->board_height; y > 0; y--) {
        for(uint32_t x = 0; x < g->board_width; x++) {
            uint32_t owner = g->fields[x][y - 1].owner_index;
            if(owner > 9) {
                parse_multidigit_number(map_string, owner, &curr_index);
            }
            else if (owner == DEFAULT_PLAYER_NUMBER) {
                map_string[curr_index] = DEFAULT_PLAYER_IDENTIFIER;
                curr_index++;
            }
            else {
                map_string[curr_index] = digit_to_char(owner);
                curr_index++;
            }
        }
        map_string[curr_index] = '\n';
        curr_index++;
    }
    map_string[curr_index] = '\0';

    return map_string;
}