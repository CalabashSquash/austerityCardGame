#include <stdio.h> //todo: get rid of all these and put in header
#include "playerErrorCheck.h"

/**
 * Checks errors potential errors in players' input
 */
void error_checker(int argc, char** args, char* playerType, 
        struct Game* game) {
    check_arg_count(argc, playerType, game);
    check_arguments(args, playerType, game);
}

/**
 * takes an errorType and prints appropriate message to stderr before exiting.
 */
void process_error(int errorType, char* playerType, struct Game* game) {
    free(game);
    if (errorType == GAME_OVER) {
        exit(GAME_OVER);
    }
    if (errorType == WRONG_ARG_COUNT) {
        fprintf(stderr, "Usage: %s pcount myid\n", playerType);
        exit(WRONG_ARG_COUNT);
    }
    if (errorType == WRONG_PLAYER_COUNT) {
        fprintf(stderr, "Invalid player count\n");
        exit(WRONG_PLAYER_COUNT);
    }
    if (errorType == WRONG_PLAYER_ID) {
        fprintf(stderr, "Invalid player ID\n");
        exit(WRONG_PLAYER_ID);
    }
    if (errorType == PIPE_CLOSED) {
        fprintf(stderr, "Communication Error\n");
        exit(PIPE_CLOSED);
    } else {
        printf("No type found");
    }
}

/**
 * Checks argc of player invokation.
 */
void check_arg_count(int argc, char* playerType, struct Game* game) {
    if (argc != 3) {
        process_error(WRONG_ARG_COUNT, playerType, game);
    }
}

/**
 * Checks the arguments. Mostly just that they are (the right) digits
 */
void check_arguments(char** args, char* playerType, struct Game* game) {
    if (!atoi(args[1]) || atoi(args[1]) < 2 || atoi(args[1]) > 26) {
        process_error(WRONG_PLAYER_COUNT, playerType, game);
    }
    for (int i = 0; i < strlen(args[1]); i++) {
        if (!isdigit(args[1][i])) {
            process_error(WRONG_PLAYER_COUNT, playerType, game);
        }
    }
    if (atoi(args[2]) < 0 || 
            atoi(args[2]) >= atoi(args[1])) {
        process_error(WRONG_PLAYER_ID, playerType, game);
    }
    if (strcmp(args[2], "0") && !atoi(args[2])) {
        process_error(WRONG_PLAYER_ID, playerType, game);
    }
}
