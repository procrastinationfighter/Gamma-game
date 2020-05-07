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
#define clear_screen() printf("\033[2J\033[1;1H")

/**
 * Makro używane do usuwania wszystkich znaków z danej linii.
 */
#define clear_line() printf("\033[2K\033[1G")

/**
 * Makro używane do schowania kursora w terminalu.
 */
#define hide_cursor() printf("\033[?25l")

/**
 * Makro używane do przywrócenia kursora w terminalu.
 */
#define show_cursor() printf("\033[?25h");

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
#define LEFT_BRACKET 91

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze A.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_UP 65

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze B.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_DOWN 66

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze C.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_RIGHT 67

/**
 * Makro odpowiadające liczbie, która
 * odpowiada literze D.
 * Jest fragmentem łańcucha symbolizującego
 * wciśnięcie klawisza strzałki.
 */
#define ARROW_LEFT 68

/**
 * Makro odpowiadające liczbie, która
 * odpowiada znakowi symbolizującemu
 * wykonanie zwykłego ruchu.
 */
#define MAKE_MOVE 32

/**
 * Makro odpowiadające liczbie, która
 * odpowiada znakowi symbolizującemu
 * wykonanie złotego ruchu.
 */
#define MAKE_GOLDEN_MOVE 103

/**
 * Makro odpowiadające liczbie, która
 * odpowiada znakowi symbolizującemu
 * pominięcie tury.
 */
#define SKIP_TURN 99


/**
 * Struktura przechowująca informacje o grze.
 */
struct game_information {
    uint32_t max_width;     ///< Maksymalna szerokość planszy.
    uint32_t max_height;    ///< Maksymalna wysokość planszy.
    uint32_t max_players;   ///< Ilość graczy.
    gamma_t *game;          ///< Wskaźnik na strukturę gry.
    uint32_t curr_x;        ///< Obecna kolumna, w której znajduje się kursor.
    uint32_t curr_y;        ///< Obecny wiersz, w którym znajduje się kursor.
    char* board;            ///< Wskaźnik na obecny wypis stanu planszy.
};
typedef struct game_information game_information;

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

/** @brief Wypisuje aktualny stan planszy z zaznaczonym polem.
 * Wypisuje aktualny stan planszy na podstawie informacji
 * zawartych w strukturze @p game_info. Jeśli składowa
 * @p board tej struktury ma wartość NULL, wywołuje funkcję
 * @ref gamma_board.
 * @param game_info          – informacje na temat gry.
 */
static void print_board(game_information *game_info) {
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
            if(i == line && j == column) {
                printf("\x1b[104m");
            }
            for(uint32_t k = 0; k < player_width; k++) {
                printf("%c", game_info->board[l]);
                l++;
            }
            printf("\x1b[0m");
            if(game_info->max_players > 9) {
                printf("%c", game_info->board[l]);
                l++;
            }
        }
        // Wypisywanie końca linii.
        printf("%c", game_info->board[l]);
        l++;
    }
}

/** @brief Sprawdza, czy znak odpowiada wciśnięciu strzałki.
 * Sprawdza, czy podana liczba symbolizuje znak
 * odpowiadający jednej z liter występujących
 * w łańcuchach oznaczających wciśnięcie strzałki
 * @param ch            – liczba, w zakresie typu char.
 * @return Wartość @p true jeśli liczba odpowiada
 * takiemu znakowi lub @p false w przeciwnym wypadku.
 */
static bool is_character_arrow_symbol(int ch) {
    return (ch == ARROW_UP || ch == ARROW_DOWN ||
            ch == ARROW_LEFT || ch == ARROW_RIGHT);
}

/** @brief Przemieszcza kursor na planszy.
 * Przemieszcza kursor gracza na planszy
 * na podstawie wciśniętego klawisza.
 * @param ch            – znak oznaczający kierunek,
 * @param game_info     – informacje o grze.
 */
static void move_cursor(int ch, game_information *game_info) {
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
    print_board(game_info);
}

/** @brief Czyta z wejścia wciśnięcie strzałki na klawiaturze.
 * Zakłada, że poprzednim wczytanym znakiem był ESC.
 * Zczytuje dwa kolejne znaki z wejścia. Jeśli pierwszy
 * z nich jest otwarciem nawiasu kwadratowego,
 * a drugi jedną z liter A, B, C, D to właśnie
 * wczytano wciśnięcie strzałki i zostaje
 * wykonany ruch kursora.
 * @param game_info         - informacje o grze.
 */
static void read_arrow_key(game_information *game_info) {
    int d = getchar();
    if(d == LEFT_BRACKET) {
        int f = getchar();
        if(is_character_arrow_symbol(f)) {
            move_cursor(f, game_info);
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
static void print_move_error() {
    clear_line();
    printf("MOVE NOT POSSIBLE. TRY AGAIN.");
}

/** @brief Wykonuje ruch danego gracza.
 * Wykonuje ruch w imieniu gracza o indeksie @p curr_player.
 * @param game_info         - informacje o grze,
 * @param curr_player       - indeks gracza.
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
 * @param game_info         - informacje o grze,
 * @param curr_player       - indeks gracza.
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
 * @param game_info            - informacje o grze,
 * @param curr_player          - indeks obecnego gracza.
 * @return
 */
static bool play_turn(game_information *game_info, uint32_t curr_player) {
    int ch;
    bool finished = false;
    do {
        ch = getchar();
        switch (ch) {
            case ESCAPE_SYMBOL:
                read_arrow_key(game_info);
                break;
            case MAKE_MOVE:
                finished = make_move(game_info, curr_player);
                break;
            case MAKE_GOLDEN_MOVE:
                finished = make_golden_move(game_info, curr_player);
                break;
            default:
                break;
        }
    } while(ch != END_GAME_SYMBOL && ch != SKIP_TURN && !finished);

    return (ch != END_GAME_SYMBOL);
}

static void run_game(game_information *game_info) {
    //todo
}

/** @brief Inicjalizuje strukturę z informacjami o grze.
 * @param[out] info             – struktura z informacjami,
 * @param[in] game              – struktura gry,
 * @param[in] command           – polecenie, którym utworzono grę.
 */
static void initialize_game(struct game_information *game_info,
                            gamma_t *game, command_t *command) {
    game_info->max_width = command->first_par;
    game_info->max_height = command->second_par;
    game_info->max_players = command->third_par;
    game_info->game = game;
    game_info->curr_x = 0;
    game_info->curr_y = 0;
    game_info->board = NULL;
    clear_screen();
    print_board(game_info);
    atexit(free_board_pointer);
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
    hide_cursor();

    struct game_information game_info;
    initialize_game(&game_info, game, command);
    run_game(&game_info);
    show_cursor()
}