#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#include "errorCheck.h"

/**
 * Checks for errors in user input that are not checked elswhere
 * throughout the program. (i.e. read_deck checks characters as it goes).
 */
void error_checker(int argc, char** args, char* playerType) {
    check_arg_count(argc, playerType);
    check_arguments(args, playerType);
}

/**
 * Input: Error number
 * Output: Correct statement to stderr.
 * Exits with appropriate error number.
 */
void process_error(int errorNumber, char* playerType) {
    switch(errorNumber) {
	case GAME_OVER:
	    exit(GAME_OVER);
	case WRONG_ARG_COUNT:
	    fprintf(stderr, "Usage: austerity tokens points deck player player"
		    " [player ...]\n");
	    exit(WRONG_ARG_COUNT); 
	case INVALID_ARGUMENT:
	    fprintf(stderr, "Bad argument\n");
	    exit(INVALID_ARGUMENT);
	case NO_ACCESS_DECK_FILE:
	    fprintf(stderr, "Cannot access deck file\n");
	    exit(NO_ACCESS_DECK_FILE);
	case INVALID_DECK_FILE:
	    fprintf(stderr, "Invalid deck file contents\n");
	    exit(INVALID_DECK_FILE);
	case START_PLAYER_FAIL:
	    fprintf(stderr, "Bad start\n");
	    exit(START_PLAYER_FAIL);
	case PLAYER_DISCONNECT:
	    fprintf(stderr, "Client disconnected\n");
	    exit(PLAYER_DISCONNECT);
	case PLAYER_PROTOCOL:
	    fprintf(stderr, "Protocol error by client\n");
	    exit(PLAYER_PROTOCOL);
	case SIGINT_RECEIVED:
	    fprintf(stderr, "SIGINT caught\n");
	    exit(SIGINT_RECEIVED);
    }
}

/**
 * Checks if argument count is valid
 */	
void check_arg_count(int argc, char* playerType) {
    if (argc < 6 || argc > 30) {
	process_error(WRONG_ARG_COUNT, playerType);
    }
}

/**
 * Checks arguments are valid
 */
void check_arguments(char** args, char* playerType) {
    for(int i = 0; i < strlen(args[1]); i++) {
	if(args[1][i] < '0' || args[1][i] > '9') {
	    process_error(INVALID_ARGUMENT, playerType);
	}
    }
    for(int i = 0; i < strlen(args[2]); i++) {
	if(args[2][i] < '0' || args[2][i] > '9') {
	    process_error(INVALID_ARGUMENT, playerType);
	}
    }
    if (atoi(args[1]) < 0 || atoi(args[2]) < 0) {
	process_error(INVALID_ARGUMENT, playerType);
    }
}
