/** @file
 * Implementacja trybu interaktywnego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 6.05.2020
 */

#include "gamma_interactive_mode.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "raw_mode.h"

/**
 * Makro używane do usuwania wszystkich znaków z ekranu
 * bez usuwania wcześniejszych wiadomości.
 */
#define clear_screen_without_deleting() printf("\033[2J\033[1;1H")

/**
 * Makro używane do usuwania wszystkich znaków z ekranu.
 */
#define clear_screen() printf("\033[1;1H\033[0J")

/**
 * Makro używane do usuwania wszystkich znaków z danej linii.
 */
#define clear_line() printf("\033[2K\033[1G")

/**
 * Makro używane do schowania kursora na ekranie.
 */
#define m_hide_cursor() printf("\033[?25l")

/**
 * Makro używane do odkrycie kursora na ekranie.
 */
#define m_show_cursor() printf("\033[?25h")

/**
 * Makro używane do ustawienia tła kolejnych napisów na niebieskie.
 */
#define set_background_color_blue() printf("\x1b[104m")

/**
 * Makro używanie do zresetowania koloru tła kolejnych napisów.
 */
#define reset_background_color() printf("\x1b[0m")

/**
 * Makro odpowiadające liczbie, która odpowiada znakowi
 * symbolizującemu zakończenie gry.
 */
#define END_GAME_SYMBOL 4

/**
 * Makro odpowiadające liczbie, która odpowiada znakowi
 * symbolizującemu wciśnięcie klawisza ESC.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ESCAPE_SYMBOL 27

/**
 * Makro odpowiadające liczbie, która odpowiada znakowi
 * symbolizującemu otwarcie nawiasu kwadratowego.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define LEFT_BRACKET '['

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze A.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_UP 'A'

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze B.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_DOWN 'B'

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze C.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_RIGHT 'C'

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze D.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_LEFT 'D'

/**
 * Makro odpowiadające liczbie, która
 * odpowiada znakowi symbolizującemu
 * wykonanie zwykłego ruchu.
 */
#define MAKE_MOVE ' '

/**
 * Makro odpowiadające liczbie, która
 * odpowiada znakowi symbolizującemu
 * wykonanie złotego ruchu.
 */
#define MAKE_GOLDEN_MOVE 'g'

/**
 * Makro odpowiadające liczbie, która
 * odpowiada znakowi symbolizującemu
 * pominięcie tury.
 */
#define SKIP_TURN 'c'


/**
 * Struktura przechowująca informacje o grze.
 */
typedef struct {
    uint32_t max_width;     ///< Maksymalna szerokość planszy.
    uint32_t max_height;    ///< Maksymalna wysokość planszy.
    uint32_t max_players;   ///< Ilość graczy.
    gamma_t *game;          ///< Wskaźnik na strukturę gry.
    uint32_t curr_x;        ///< Obecna kolumna, w której znajduje się kursor.
    uint32_t curr_y;        ///< Obecny wiersz, w którym znajduje się kursor.
    char* board;            ///< Wskaźnik na obecny wypis stanu planszy.
} game_information;

/**
 * Wskaźnik na obecny wypis stanu planszy.
 * Zasięg pliku, aby w razie błędu móc
 * zwolnić tę pamięć za pomocą atexit.
 */
static char* board_pointer = NULL;

/** @brief Zwalnia pamięć zaalokowaną na wypis stanu planszy.
 */
static void free_board_pointer() {
    if(board_pointer != NULL) {
        free(board_pointer);
    }
}

/** @brief Przywraca widoczność kursora.
 */
static inline void show_cursor() {
    m_show_cursor();
}

/** @brief Usuwa widoczność kursora.
 */
static inline void hide_cursor() {
    m_hide_cursor();
    atexit(show_cursor);
}

/** @brief Wypisuje wiadomość zachęcającą gracza do ruchu.
 * @param[in] game_info         - informacje o grze,
 * @param[in] curr_player       - indeks gracza.
 */
static void print_player_message(game_information *game_info, uint32_t curr_player) {
    if(curr_player > 0) {
        printf("TURN OF PLAYER %d, PLEASE MAKE A MOVE \n", curr_player);
        printf("FREE FIELDS: %lu, GOLDEN MOVE ",
                gamma_free_fields(game_info->game, curr_player));
        if(!gamma_golden_possible(game_info->game, curr_player)) {
            printf("NOT ");
        }
        printf("POSSIBLE\n");
        printf("PLAYER OWNED AREAS: %u/%u\n",
                gamma_player_owned_areas(game_info->game, curr_player),
                gamma_max_areas(game_info->game));
    }
}

/** @brief Wypisuje aktualny stan planszy z zaznaczonym polem.
 * Wypisuje aktualny stan planszy na podstawie informacji
 * zawartych w strukturze @p game_info. Jeśli składowa
 * @p board tej struktury ma wartość NULL, wywołuje funkcję
 * @ref gamma_board.
 * @param[in] game_info          – informacje na temat gry,
 * @param[in] curr_player        - numer gracza, który ma teraz ruch.
 */
static void print_board(game_information *game_info, uint32_t curr_player) {
    clear_screen();
    if(game_info->board == NULL) {
        game_info->board = gamma_board(game_info->game);
        board_pointer = game_info->board;
    }
    uint32_t line = game_info->max_height - game_info->curr_y - 1;
    uint32_t column = game_info->curr_x;
    uint32_t player_width = log10(game_info->max_players) + 1;
    uint32_t l = 0;

    for(uint32_t i = 0; i < game_info->max_height; i++) {
        for(uint32_t j = 0; j < game_info->max_width; j++) {
            if(i == line && j == column && curr_player != 0) {
                set_background_color_blue();
            }
            for(uint32_t k = 0; k < player_width; k++) {
                printf("%c", game_info->board[l]);
                l++;
            }
            reset_background_color();
            if(game_info->max_players > 9) {
                printf("%c", game_info->board[l]);
                l++;
            }
        }
        // Wypisywanie końca linii.
        printf("%c", game_info->board[l]);
        l++;
    }
    print_player_message(game_info, curr_player);
}

/** @brief Sprawdza, czy znak odpowiada wciśnięciu strzałki.
 * Sprawdza, czy podana liczba symbolizuje znak
 * odpowiadający jednej z liter występujących
 * w łańcuchach oznaczających wciśnięcie strzałki
 * @param[in] ch            – liczba, w zakresie typu char.
 * @return Wartość @p true jeśli liczba odpowiada
 * takiemu znakowi lub @p false w przeciwnym wypadku.
 */
static inline bool is_character_arrow_symbol(int ch) {
    return (ch == ARROW_UP || ch == ARROW_DOWN ||
            ch == ARROW_LEFT || ch == ARROW_RIGHT);
}

/** @brief Przemieszcza kursor na planszy.
 * Przemieszcza kursor gracza na planszy
 * na podstawie wciśniętego klawisza.
 * @param[in] ch                – znak oznaczający kierunek,
 * @param[in,out] game_info     – informacje o grze,
 * @param[in] curr_player       - indeks obecnego gracza.
 */
static void move_cursor(int ch, game_information *game_info, uint32_t curr_player) {
    switch (ch) {
        case ARROW_UP:
            if(game_info->curr_y < game_info->max_height - 1) {
                game_info->curr_y += 1;
            }
            break;
        case ARROW_DOWN:
            if(game_info->curr_y > 0) {
                game_info->curr_y -= 1;
            }
            break;
        case ARROW_RIGHT:
            if(game_info->curr_x < game_info->max_width - 1) {
                game_info->curr_x += 1;
            }
            break;
        case ARROW_LEFT:
            if(game_info->curr_x > 0) {
                game_info->curr_x -= 1;
            }
            break;
        default:
            break;
    }
    print_board(game_info, curr_player);
}

/** @brief Czyta z wejścia wciśnięcie strzałki na klawiaturze.
 * Zakłada, że poprzednim wczytanym znakiem był ESC.
 * Zczytuje dwa kolejne znaki z wejścia. Jeśli pierwszy
 * z nich jest otwarciem nawiasu kwadratowego,
 * a drugi jedną z liter A, B, C, D to właśnie
 * wczytano wciśnięcie strzałki i zostaje
 * wykonany ruch kursora.
 * @param[in,out] game_info     - informacje o grze,
 * @param[in] curr_player       - indeks obecnego gracza.
 */
static void read_arrow_key(game_information *game_info, uint32_t curr_player) {
    int d = getchar();
    if(d == LEFT_BRACKET) {
        int f = getchar();
        if(is_character_arrow_symbol(f)) {
            move_cursor(f, game_info, curr_player);
        }
        else {
            ungetc(f, stdin);
            ungetc(d, stdin);
        }
    }
    else {
        ungetc(d, stdin);
    }
}

/** @brief Informuje gracza o błędnym ruchu.
 */
static inline void print_move_error() {
    clear_line();
    printf("MOVE NOT POSSIBLE. TRY AGAIN.");
}

/** @brief Wykonuje ruch danego gracza.
 * Wykonuje ruch w imieniu gracza o indeksie @p curr_player.
 * @param[in,out] game_info     - informacje o grze,
 * @param[in] curr_player       - indeks gracza.
 * @return Wartość @p true jeśli ruch był prawidłowy
 * lub wartość @p false w przeciwnym wypadku.
 */
static bool make_move(game_information *game_info, uint32_t curr_player) {
    bool was_move_successful = gamma_move(game_info->game, curr_player,
                                game_info->curr_x, game_info->curr_y);
    if(was_move_successful) {
        free(game_info->board);
        game_info->board = NULL;
        board_pointer = NULL;
    }
    else {
        print_move_error();
    }

    return was_move_successful;
}

/** @brief Wykonuje złoty ruch danego gracza.
 * Wykonuje złoty ruch w imieniu gracza o indeksie @p curr_player.
 * @param[in,out] game_info     - informacje o grze,
 * @param[in] curr_player       - indeks gracza.
 * @return Wartość @p true jeśli ruch był prawidłowy
 * lub wartość @p false w przeciwnym wypadku.
 */
static bool make_golden_move(game_information *game_info, uint32_t curr_player) {
    bool was_move_successful = gamma_golden_move(game_info->game, curr_player,
                                        game_info->curr_x, game_info->curr_y);
    if(was_move_successful) {
        free(game_info->board);
        game_info->board = NULL;
        board_pointer = NULL;
    }
    else {
        print_move_error();
    }

    return was_move_successful;
}

/** @brief Pozwala graczowi wykonać turę.
 * Wczytuje z wejścia znaki do momentu aż gracz
 * wykona poprawny ruch, zrezygnuje z ruchu
 * lub zostanie wciśnięty klawisz kończący grę.
 * @param[in,out] game_info        - informacje o grze,
 * @param[in] curr_player          - indeks obecnego gracza.
 * @return Wartość @p true jeśli gra powinna toczyć się dalej
 * lub @p false jeśli po tym ruchu gra powinna się skończyć.
 */
static bool play_turn(game_information *game_info, uint32_t curr_player) {
    int ch;
    bool finished = false;
    print_board(game_info, curr_player);
    do {
        ch = tolower(getchar());
        switch (ch) {
            case ESCAPE_SYMBOL:
                read_arrow_key(game_info, curr_player);
                break;
            case MAKE_MOVE:
                finished = make_move(game_info, curr_player);
                break;
            case MAKE_GOLDEN_MOVE:
                finished = make_golden_move(game_info, curr_player);
                break;
            case EOF:
                exit(EXIT_FAILURE);
            default:
                break;
        }
    } while(ch != END_GAME_SYMBOL && ch != SKIP_TURN && !finished);

    return (ch != END_GAME_SYMBOL);
}

/** @brief Wypisuje wynik gry.
 * Wypisuje ilość pól posiadanych przez każdego gracza.
 * @param[in] game_info             - informacje o grze.
 */
static void print_game_result(game_information *game_info) {
    print_board(game_info, 0);
    for(uint32_t i = 0; i < game_info->max_players; i++) {
        printf("PLAYER %u: %lu OWNED FIELDS\n",
                i + 1, gamma_busy_fields(game_info->game, i + 1));
    }
    printf("THANKS FOR PLAYING!\n");
}

/** @brief Przeprowadza grę w trybie interaktywnym.
 * Przeprowadza grę w trybie interaktywnym.
 * Gra kończy się, gdy żaden gracz nie ma już ruchu
 * lub wciśnięto kombinację klawiszy oznaczającą koniec.
 * Więcej informacji w moodlu.
 * @param[in,out] game_info          - informacje o grze.
 */
static void run_game(game_information *game_info) {
    bool game_finished = false;
    uint32_t players_skipped;
    while(!game_finished) {
        players_skipped = 0;
        for(uint32_t i = 0; i < game_info->max_players; i++) {
            if(gamma_golden_possible(game_info->game, i + 1) ||
                    gamma_free_fields(game_info->game, i + 1)) {
                game_finished = !play_turn(game_info, i + 1);
                if(game_finished) {
                    break;
                }
            }
            else {
                players_skipped++;
            }
        }

        if(players_skipped == game_info->max_players) {
            game_finished = true;
        }
    }

    print_game_result(game_info);
}

/** @brief Inicjalizuje strukturę z informacjami o grze.
 * @param[out] game_info        – struktura z informacjami,
 * @param[in] game              – struktura gry,
 * @param[in] command           – polecenie, którym utworzono grę.
 */
static void initialize_game(game_information *game_info,
                            gamma_t *game, command_t *command) {
    game_info->max_width = command->first_par;
    game_info->max_height = command->second_par;
    game_info->max_players = command->third_par;
    game_info->game = game;
    game_info->curr_x = 0;
    game_info->curr_y = 0;
    game_info->board = NULL;
    atexit(free_board_pointer);
    clear_screen_without_deleting();
}

void run_interactive_mode(gamma_t *game, command_t *command) {
    enableRawMode();
    hide_cursor();

    game_information game_info;
    initialize_game(&game_info, game, command);
    run_game(&game_info);
}