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
#include <unistd.h>
#include <termios.h>
#include <math.h>

/**
 * Makro używane do usuwania wszystkich znaków z ekranu.
 */
#define clear_screen() printf("\e[1;1H\e[2J")

/**
 * Makro odpowiadające liczbie, która odpowiada znakowi
 * symbolizującemu kombinację klawiszy Ctrl + D.
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
#define LEFT_BRACKET 91

/**
 * Makro odpowiadające liczbie, która odpowiada
 * literze A.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_UP 65

/**
 * Makro odpowiadające liczbie, która odpowiada
 * literze B.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_DOWN 66

/**
 * Makro odpowiadające liczbie, która odpowiada
 * literze C.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_RIGHT 67

/**
 * Makro odpowiadające liczbie, która odpowiada
 * literze D.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_LEFT 68
struct game_information {
    uint32_t max_width;
    uint32_t max_height;
    uint32_t max_players;
    gamma_t *game;
    uint32_t curr_x;
    uint32_t curr_y;
    char* board;
};
typedef struct game_information game_information;

/** @brief Wypisuje aktualny stan planszy z zaznaczonym polem.
 * Wypisuje aktualny stan planszy na podstawie informacji
 * zawartych w strukturze @p game_info. Jeśli składowa
 * @p board tej struktury ma wartość NULL, wywołuje funkcję
 * @ref gamma_board.
 * @param game_info          – informacje na temat gry.
 */
static void print_board(game_information *game_info) {
    char *board = gamma_board(game);
    uint32_t line = max_height - y - 1;
    uint32_t column = x;
    uint32_t player_width = log10(max_players) + 1;
    uint32_t l = 0;

    for(uint32_t i = 0; i < max_height; i++) {
        for(uint32_t j = 0; j < max_width; j++) {
            if(i == line && j == column) {
                printf("\x1b[104m");
            }
            for(uint32_t k = 0; k < player_width; k++) {
                printf("%c", board[l]);
                l++;
            }
            printf("\x1b[0m");
            if(max_players > 9) {
                printf("%c", board[l]);
                l++;
            }
        }
        // Wypisywanie końca linii.
        printf("%c", board[l]);
        l++;
    }

    free(board);
}

static bool is_character_arrow_symbol(int ch) {
    return (ch == ARROW_UP || ch == ARROW_DOWN || ch == ARROW_LEFT || ch == ARROW_RIGHT);
}

static void move_cursor(int ch) {
    switch (ch) {
        case ARROW_UP:
            curr_y += 1;
            print_board(game_board, curr_x, curr_y);
            break;
        default:
            break;
    }
}

/** @brief Inicjalizuje strukturę z informacjami o grze.
 * @param[out] info             – struktura z informacjami,
 * @param[in] game              – struktura gry,
 * @param[in] command           – polecenie, którym utworzono grę.
 */
static void initalize_game_info(struct game_information *game_info,
                            gamma_t *game, command_t *command) {
    game_info->max_width = command->first_par;
    game_info->max_height = command->second_par;
    game_info->max_players = command->third_par;
    game_info->game = game;
    game_info->curr_x = 0;
    game_info->curr_y = 0;
    game_info->board = NULL;
}

// kod z https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
// Koniec kodu z neta

/** @brief Uruchamia tryb interaktywny gry gamma.
 * Uruchamia i przeprowadza grę w trybie interaktywnym.
 * @param[in,out] game          – struktura gry,
 * @param[in] command           – komenda użyta do stworzenia gry.
 */
void run_interactive_mode(gamma_t *game, command_t *command) {
    enableRawMode();
    struct game_information game_info;
    initalize_game_info(&game_info, game, command);
    clear_screen();
    print_board(&game_info);
    int ch;
    do {
        ch = getchar();
        if(ch == ESCAPE_SYMBOL) {
            int d = getchar();
            if(d == LEFT_BRACKET) {
                int f = getchar();
                if(is_character_arrow_symbol(f)) {
                    move_cursor(f);
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
    } while(ch != END_GAME_SYMBOL && ch != 'c');

    print_board(game, 1, 1);
}