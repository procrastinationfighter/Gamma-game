/** @file
 * Implementacja trybu wsadowego gry gamma.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 4.05.2020
 */

#include "gamma_batch_mode.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <values.h>
#include "String.h"

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_move.
 */
#define CALL_GAMMA_MOVE 'm'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_golden_move.
 */
#define CALL_GAMMA_GOLDEN_MOVE 'g'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_busy_fields.
 */
#define CALL_GAMMA_BUSY_FIELDS 'b'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_free_fields.
 */
#define CALL_GAMMA_FREE_FIELDS 'f'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_golden_possible.
 */
#define CALL_GAMMA_GOLDEN_POSSIBLE 'q'

/**
 * Symbol informujący o tym, że powinna zostać wywołana funkcja @ref gamma_board.
 */
#define CALL_GAMMA_BOARD 'p'

/**
 * Symbol informujący o tym, że dana linia jest komentarzem.
 */
#define COMMENT_SIGN '#'

/**
 * Liczba symbolizująca domyślną wartość w przypadku, gdy parametr nie istnieje.
 */
#define BLANK_PARAMETER_NUMBER -42

/**
 * Enumerator symbolizujący możliwe wyniki wczytywania.
 */
enum read_result_type {
    SKIP_LINE,
    ERROR,
    CORRECT
};
typedef enum read_result_type Result;

/** @brief Wypisuje informacje o błędzie.
 * Wypisuje informację o błędzie oraz numer linii, w którym wystąpił.
 * @param[in] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void print_error(uint32_t lines) {
    fprintf(stderr, "ERROR %u", lines);
}

/** @brief Pomija wszystkie znaki na wejściu do końca linii.
 * Pomija wszystkie znaki, aż nie napotka na wejściu
 * znaku @p '\n' lub @p EOF.
 */
static void skip_input_line() {
    int ch;
    do {
        ch = getchar();
    } while(ch != '\n' && ch != EOF);
}

/** @brief Pomija następne białe znaki.
 * Pomija wszystkie białe znaki na wejściu, poza znakiem końca linii @p '\n.
 */
static void skip_spaces() {
    int ch;
    do {
        ch = getchar();
    } while(ch != '\n' && isspace(ch));
    if(ch == '\n') {
        ungetc(ch, stdin);
    }
}

/** @brief Czyta następny parametr z wejścia.
 * Zczytuje kolejny parametr w danej linii. Poprawny parametr jest ciągiem cyfr.
 * @return Wskaźnik na spójny łańcuch cyfr w przypadku powodzenia
 * lub @p NULL jeśli zaszedł jakiś błąd.
 */
static String *read_next_parameter() {
    String *word = malloc(sizeof(String));
    if(word == NULL) {
        errno = ENOMEM;
        exit(EXIT_FAILURE);
    }
    initialize_string(word);

    skip_spaces();
    int ch = getchar();
    if(ch == '0') {
        // Parametr z zerami wiodącymi uznajemy za niepoprawny.
        // Zmieniamy ch, aby na pewno nie wskazywał na cyfrę ani znak biały.
        ch = 'a';
    }
    while (isdigit(ch)) {
        push_back_in_string(word, (char) ch);
        ch = getchar();
    }
    push_back_in_string(word, '\0');

    if(!isspace(ch)) {
        // Zakończono wczytywanie z innego powodu niż znak biały na wejściu,
        // więc parametr jest niepoprawny
        deinitialize_string(word);
        free(word);
        return NULL;
    }
    else if(ch == '\n') {
        // Zwracamy na wejście znak końca linii, aby inne funkcje działy prawidłowo.
        ungetc(ch, stdin);
    }

    return word;
}

/** @brief Sprawdza, czy podany wyraz symbolizuje poprawne polecenie.
 * Sprawdza, czy podany wyraz ma długość 1 oraz czy symbolizuje jedno z poleceń
 * (dane przez makra z prefixem CALL).
 * @param commandType       – łańcuch znaków z poleceniem.
 * @return Wartość @p true jeśli wyraz symbolizuje poprawne polecenie
 * lub @p false w przeciwnym wypadku.
 */
static bool is_command_valid(char ch) {
    return (ch == CALL_GAMMA_BOARD || ch == CALL_GAMMA_BUSY_FIELDS
            || ch == CALL_GAMMA_FREE_FIELDS || ch == CALL_GAMMA_GOLDEN_MOVE
            || ch == CALL_GAMMA_GOLDEN_POSSIBLE || ch == CALL_GAMMA_MOVE);
}

/** @brief Sprawdza poprawność pierwszego wyrazu w linii.
 * Sprawdza, czy pierwszy wyraz w linii oznacza pustą linię,
 * komentarz lub prawidłowe polecenie.
 * @param str           – sprawdzany wyraz
 * @return @p SKIP_LINE jeśli wyraz jest znakiem końca linii, '#' lub EOF,
 *         @p CORRECT jeśli wyraz jest pojedynczym znakiem prawidłowego polecenia,
 *         @p ERROR w przeciwnym wypadku.
 */
static Result check_command_type(uint32_t command) {
    switch (command) {
        case '\n':
        case COMMENT_SIGN:
        case '\0':
            return SKIP_LINE;
        default:
            return (is_command_valid(command) ? CORRECT : ERROR);
        }
}

/** @brief Zamienia ciąg cyfr na liczbę.
 * Zamienia ciąg cyfr zapisany w parametrze @p parameter
 * na liczbę, jeśli jest to możliwe. Liczba nie powinna wykraczać
 * poza zasięg typu uint32_t.
 * @param parameter         – łańcuch znaków.
 * @return Liczba zakodowana wcześniej jako ciąg znaków
 * lub @p BLANK_PARAMETER_NUMBER, jeśli ciąg jest pusty.
 */
static long parse_number(String *parameter) {
    if(is_string_empty(parameter)) {
        return BLANK_PARAMETER_NUMBER;
    }
    else {
        char *ch;
        unsigned long result = strtoul(parameter->string, &ch, 10);
        if(result > LONG_MAX) {
            return UINT32_MAX + 1;
        }
        else {
            return (long) result;
        }
    }
}

/** @brief Czyta następny parametr i zamienia go na liczbę.
 * Zczytuje z wejścia kolejny parametr i jeśli jest poprawny, zamienia go na liczbę.
 * @param[out] number       – zczytany numer.
 * @return @p CORRECT jeśli wczytany numer był prawidłowy (w tym pusty)
 * lub @p ERROR w przeciwnym wypadku.
 */
static Result read_and_parse_parameter(long *number) {
    String *param = read_next_parameter();
    if(param == NULL) {
        return ERROR;
    }

    *number = parse_number(param);
    if(errno == ERANGE || *number > UINT32_MAX) {
        // Zbyt duży parametr.
        return ERROR;
    }

    deinitialize_string(param);
    free(param);
    return CORRECT;
}

static bool is_parameter_count_correct(uint32_t command, const long args[]) {
    int i = 0;
    while(args[i] != BLANK_PARAMETER_NUMBER && i < 3) {
        i++;
    }
    switch (command) {
        case CALL_GAMMA_BOARD:
            return i == 0;
        case CALL_GAMMA_GOLDEN_POSSIBLE:
        case CALL_GAMMA_FREE_FIELDS:
        case CALL_GAMMA_BUSY_FIELDS:
            return i == 1;
        case CALL_GAMMA_GOLDEN_MOVE:
        case CALL_GAMMA_MOVE:
            return i == 3;
        default:
            return false;
    }
}

/** @brief Wykonuje polecenie z podanymi parametrami.
 * Jeśli ilość parametrów jest poprawna,
 * wykonuje zadane w parametrze @p command polecenie.
 * @param game_board        – wskaźnik na strukturę gry,
 * @param command           – polecenie,
 * @param args              – parametry polecenia.
 * @return Wartość @p true jeśli ilość parametrów była prawidłowa
 * lub @p false w przeciwnym wypadku.
 */
static bool execute_command(gamma_t *game_board, uint32_t command,
                            long args[]) {
    bool are_parameters_correct = is_parameter_count_correct(command, args);
    if(are_parameters_correct) {
        char *board;
        switch (command) {
            case CALL_GAMMA_BOARD:
                board = gamma_board(game_board);
                printf("%s", board);
                free(board);
                break;
            case CALL_GAMMA_GOLDEN_POSSIBLE:
                printf("%i", gamma_golden_possible(game_board, args[0]));
                break;
            case CALL_GAMMA_FREE_FIELDS:
                printf("%lu", gamma_free_fields(game_board, args[0]));
                break;
            case CALL_GAMMA_BUSY_FIELDS:
                printf("%lu", gamma_busy_fields(game_board, args[0]));
                break;
            case CALL_GAMMA_GOLDEN_MOVE:
                printf("%i", gamma_golden_move(game_board, args[0],
                                                     args[1], args[2]));
                break;
            case CALL_GAMMA_MOVE:
                printf("%i", gamma_move(game_board, args[0],
                                              args[1], args[2]));
                break;
            default:
                break;
        }
    }

    return are_parameters_correct;
}

/** @brief Czyta z wejścia parametry komendy i wykonuje ją.
 * Zczytuje z wejścia parametry komendy i jeśli są prawidłowe to wykonuje ją.
 * Jeśli parametry są nieprawidłowe, wypisuje informację o błędzie.
 * @param game_board
 * @param command
 * @return @p CORRECT jeśli wykonano polecenie
 * lub @p ERROR jeśli parametry były niepoprawne.
 */
static Result read_parameters_and_execute_command(gamma_t *game_board,
                                                  uint32_t command) {
    long parameters[3];
    long temp;
    Result currResult = read_and_parse_parameter(&temp);

    if(currResult == CORRECT) {
        parameters[0] = temp;
        currResult = read_and_parse_parameter(&temp);

        if(currResult == CORRECT) {
            parameters[1] = temp;
            currResult = read_and_parse_parameter(&temp);

            if(currResult == CORRECT) {
                parameters[2] = temp;
                bool was_right = execute_command(game_board, command, parameters);
                if(!was_right) {
                    currResult = ERROR;
                }
            }
        }
    }

    return currResult;
}

/** @brief Czyta z wejścia znak odpowiadający za typ komendy.
 * Zczytuje z wejścia pierwszy znak i zapisuje go w parametrze @p command.
 * @param[out] command          – wskaźnik na znak odpowiadający za typ komendy.
 * @return @p ERROR jeśli pierwsze słowo nie jest
 * częścią komentarza ani pojedynczym wyrazem,
 * lub typ zwrócony przez funkcję
 * @ref check_command_type w przeciwnym wypadku.
 */
static Result read_command(uint32_t *command) {
    *command = getchar();
    Result proposition_of_result = check_command_type(*command);
    if(proposition_of_result == CORRECT) {
        // Jeśli pierwszy znak w linii odpowiada prawidłowemu poleceniu,
        // sprawdzamy, czy jest pojedynczym wyrazem.
        uint32_t temp = getchar();
        if(!isspace(temp)) {
            return ERROR;
        }
    }

    return proposition_of_result;
}

/** @brief Czyta i wykonuje polecenie ze standardowego wejścia.
 * Zczytuje z wejścia jedną linię. Jeśli polecenie jest poprawne,
 * wykonuje je. W przeciwnym wypadku wypisuje informację o błędzie.
 * @param[in, out] lines     – liczba linii wejścia do tej pory.
 */
static void read_and_execute_command(gamma_t *game_board, uint32_t *lines) {
    (*lines)++;
    uint32_t command;
    Result res = read_command(&command);
    if(res == CORRECT) {
        res = read_parameters_and_execute_command(game_board, command);
    }

    if (res == ERROR){
        skip_input_line();
        print_error(*lines);
    }
}

/** @brief Przeprowadza grę za pomocą trybu wsadowego.
 * Zczytuje z wejścia i wykonuje polecenia zgodne z zasadami trybu wsadowego.
 * @param[in, out] lines     – liczba linii wejścia przed uruchomieniem trybu.
 */
void run_batch_mode(gamma_t *game_board, uint32_t *lines) {
    while(!feof(stdin)) {
        read_and_execute_command(game_board, lines);
    }
}