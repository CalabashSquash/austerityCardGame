#ifndef HUB_ERROR_CHECK_H
#define HUB_ERROR_CHECK_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum HubExitCode {
    GAME_OVER = 0,
    WRONG_ARG_COUNT = 1,
    INVALID_ARGUMENT = 2,
    NO_ACCESS_DECK_FILE = 3,
    INVALID_DECK_FILE = 4,
    START_PLAYER_FAIL = 5,
    PLAYER_DISCONNECT = 6,
    PLAYER_PROTOCOL = 7,
    SIGINT_RECEIVED = 10,
} HubExitCode;

typedef enum PlayerExitCode {
    WRONG_PLAYER_COUNT = 2,
    WRONG_PLAYER_ID = 3,
    PIPE_CLOSED = 6,
} PlayerExitCode;

void error_checker(int argc, char** args, char* charType);
void process_error(int errorNumber, char* charType);
void check_arg_count(int argc, char* charType);
void check_arguments(char** args, char* charType);

#endif
