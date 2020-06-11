/** @file
 * Główny plik z funkcją main, zawierający implementację wyboru trybu gry.
 *
 * @author Adam Boguszewski <ab417730@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 5.04.2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "gamma.h"
#include "gamma_batch_mode.h"
#include "gamma_interactive_mode.h"
#include "gamma_input.h"

/**
 * Symbol informujący o tym, że powinien zostać uruchomiony tryb wsadowy.
 */
#define BATCH_MODE_SYMBOL 'B'

/**
 * Symbol informujący o tym, że powinien zostać uruchomiony tryb interaktywny.
 */
#define INTERACTIVE_MODE_SYMBOL 'I'

/**
 * Z góry ustalona minimalna szerokość terminala,
 * dla której komunikaty trybu interaktywnego będą wypisywane poprawnie.
 */
#define MINIMAL_WINDOW_WIDTH 50

/**
 * Z góry ustalona dodatkowa wysokość terminala
 * (nie licząc wysokości planszy), dla której
 * komunikaty trybu interaktywnego zmieszczą się na ekranie.
 */
#define MINIMAL_WINDOW_BONUS_HEIGHT 7
/**
 * Wskaźnik na strukturę gry. Zasięg pliku, aby można
 * było użyć funkcji atexit do jego zwolnienia.
 */
static gamma_t *game_pointer = NULL;

/** @brief Zwalnia pamięć odpowiadającą za grę.
 * Zwalnia strukturę gry poprzez wywołanie @ref gamma_delete.
 * Funkcja używana jedynie przez funkcję atexit.
 */
static void free_game_memory() {
    gamma_delete(game_pointer);
    game_pointer = NULL;
}

/** @brief Sprawdza, czy parametry są w zakresie typu uint32_t.
 * Sprawdza, czy parametry są w zakresie od 0 do UINT32_MAX.
 * Funkcja celowo uznaje parametry równe @p BLANK_PARAMETER_NUMBER
 * za niepoprawne, gdyż poprawna będzie linia z 4 parametrami.
 * @param[in] command           – komenda.
 * @return Wartość @p true jeśli warunek
 * jest spełniony lub @p false w przeciwnym wypadku.
 */
static bool are_parameters_in_bound(command_t *command) {
    long params[4] = {command->first_par, command->second_par,
                      command->third_par, command->fourth_par};
    for(int i = 0; i < 4; i++) {
        long q = params[i];
        if(q > UINT32_MAX || q < 0) {
            return false;
        }
    }

    return true;
}

/** @brief Sprawdza poprawność symbolu trybu.
 * Sprawdza, czy symbol oznaczający tryb jest poprawny,
 * czyli równy @p INTERACTIVE_MODE_SYMBOL
 * lub @p BATCH_MODE_SYMBOL.
 * @param[in] sign          – symbol.
 * @return Wartość @p true jeśli symbol jest poprawny
 * lub @p false w przeciwnym wypadku.
 */
static inline bool is_command_sign_correct(char sign) {
    return (sign == INTERACTIVE_MODE_SYMBOL || sign == BATCH_MODE_SYMBOL);
}

/** @brief Sprawdza poprawność polecenia utworzenia nowej gry.
 * Sprawdza, czy parametry mieszczą się w zakresie typu uint32_t,
 * czy znak polecenia jest poprawny.
 * Jeśli parametry lub znak jest niepoprawny, wypisuje
 * informację o błędzie.
 * @param[in] command           – polecenie,
 * @param[in] lines             – numer aktualnej linii.
 * @return Wartość @p true jeśli wczytane polecenie
 * jest poprawnym poleceniem uruchamiającym tryb
 * lub @p false w przeciwnym wypadku.
 */
static bool check_command_correctness(command_t *command, uint32_t lines) {
        if(are_parameters_in_bound(command) &&
           is_command_sign_correct(command->type)) {
            return true;
        }
        else {
            print_error(lines);
            return false;
        }
}

/** @brief Sprawdza, czy można grać w trybie interaktywnym.
 * Sprawdza, czy rozmiar terminala pozwala na komfortowe
 * rozegranie gry w trybie interaktywnym.
 * @param[in] width             – szerokość planszy,
 * @param[in] height            – wysokość planszy,
 * @param[in] players           – ilość graczy.
 * @return Wartość @p true, jeśli gra zmieści się w terminalu
 * lub @p false w przeciwnym wypadku.
 */
static bool is_terminal_size_ok(uint32_t width, uint32_t height, uint32_t players) {
    if(isatty(STDOUT_FILENO)) {
        uint64_t real_board_width = width;
        if (players >= 10) {
            real_board_width = width * (log10(players) + 2);
        }
        struct winsize win_info;
        int error_control = ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_info);
        if(error_control == -1) {
            exit(EXIT_FAILURE);
        }

        // ws_row jest typu unsigned short,
        // więc nie może być tak wielkie,
        // w takim wypadku zabezpieczamy się przed
        // przekroczeniem uint32_max przy dodawaniu
        if(height > UINT32_MAX - MINIMAL_WINDOW_BONUS_HEIGHT ||
           win_info.ws_col < MINIMAL_WINDOW_WIDTH) {
            return false;
        }
        else {
            return (win_info.ws_col > real_board_width &&
                    win_info.ws_row > height + MINIMAL_WINDOW_BONUS_HEIGHT);
        }
    }
    else {
        return true;
    }
}

/** @brief Uruchamia odpowiedni tryb.
 * W zależności od symbolu @p type w zmiennej
 * @p command, uruchamia odpowiedni tryb.
 * @param[in] command       – polecenie.
 * @param[in] lines         – wskaźnik na numer aktualnej linii wejścia.
 * @return Wartość @p true jeśli udało się stworzyć grę
 * lub @p false w przeciwnym wypadku.
 */
static bool run_mode(command_t *command, uint32_t *lines) {
    game_pointer = gamma_new(command->first_par, command->second_par,
                             command->third_par, command->fourth_par);
    if(game_pointer == NULL) {
        print_error(*lines);
        return false;
    }
    else if(command->type == BATCH_MODE_SYMBOL) {
        printf("OK %u\n", *lines);
        run_batch_mode(game_pointer, lines);
    }
    else {
        if(is_terminal_size_ok(command->first_par, command->second_par,
                               command->third_par)) {
            run_interactive_mode(game_pointer, command);
        }
        else {
            fprintf(stderr, "Terminal size too small. "
                            "Resize your terminal window and try again.\n");
            free_game_memory();
            return false;
        }
    }

    return true;
}

/** @brief Główna funkcja programu.
 * Sczytuje komendy do momentu uruchomienia któregoś z trybów gry.
 * @return 0, jeśli program zakończył się w poprawny sposób.
 * W innym wypadku następuje wyjście z programu w innym miejscu.
 */
int main() {
    atexit(free_game_memory);

    uint32_t lines = 0;
    bool right_command = false;
    command_t command;
    while(!feof(stdin) && !right_command) {
        lines++;
        if(read_command(&command, &lines)) {
            right_command = check_command_correctness(&command, lines);
        }
        else if(!feof(stdin)){
            print_error(lines);
        }

        if(right_command) {
            right_command = run_mode(&command, &lines);
        }
    }

    return 0;
}