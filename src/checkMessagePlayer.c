#include "playerErrorCheck.h"
#include "gameAndPlayerData.h"
#include "checkMessagePlayer.h"


void free_variables(struct Game* game, char** sections) {
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        free(game->currentEight[i]->price);
        free(game->currentEight[i]);
    }
    for (int i = 0; i < game->numPlayers; i++) {
        free(game->players[i]);
    }
    for (int i = 0; i < MAX_SECTIONS; i++) {
        free(sections[i]);
    }
    free(sections);
}

/**
 * Checks a "newcard" message
 */
void check_new_card(char** sections, struct Game* game) {
    int numOfSections = 6;
    check_empty(sections, numOfSections, game);
    /*Since by this stage, we already know sections[0] starts with*/
    /*"newcard", we can skip the first 7 chars*/
    if (strlen(sections[0]) > 8) {
        free_variables(game, sections);
        process_error(PIPE_CLOSED, "", game);
    }
    if (sections[0][7] != 'P' && sections[0][7] != 'B' && sections[0][7] != 'Y'
            && sections[0][7] != 'R') {
        free_variables(game, sections);
        process_error(PIPE_CLOSED, "", game);
    }
    check_digits(sections, numOfSections, 1, game);

}    

/**
 * Checks a "purchased" message
 */
void check_purchased(char** sections, struct Game* game) {
    int numOfSections = 7;
    check_empty(sections, numOfSections, game);
    /*Since by this stage, we already know sections[0] starts with*/
    /*"purchased", we can skip the first 9 chars*/
    if (strlen(sections[0]) > 11) {
        free_variables(game, sections);
        process_error(PIPE_CLOSED, "", game);
    }
    if (sections[0][9] < 65 || sections[0][9] > game->numPlayers + 64) {
        free_variables(game, sections);
        process_error(PIPE_CLOSED, "", game);
    }
    check_digits(sections, numOfSections, 1, game);
}

/**
 * Checks a "took" message
 */
void check_took_tokens(char** sections, struct Game* game) {
    int numOfSections = 5;
    check_empty(sections, numOfSections, game);
    /*Since by this stage, we already know sections[0] starts with*/
    /*"took", we can skip the first 4 chars*/
    if (sections[5][0] != '\0') { /*Too many parameters of message given*/
        free_variables(game, sections);
        process_error(PIPE_CLOSED, "", game);
    }
    if (strlen(sections[0]) > 6) {
        free_variables(game, sections);
        process_error(PIPE_CLOSED, "", game);
    }
    if (sections[0][4] < 65 || sections[0][4] > game->numPlayers + 64) {
        free_variables(game, sections);
        process_error(PIPE_CLOSED, "", game);
    }
    check_digits(sections, numOfSections, 1, game);
}

/**
 * Checks an array of strings to make sure it consists all of digits
 */
void check_digits(char** sections, int sectionCount, int startingSection,
        struct Game* game) {
    for (int j = startingSection; j < sectionCount; j++) {
        for (int i = 0; i < strlen(sections[j]); i++) {
            if (!isdigit(sections[j][i])) {
                free_variables(game, sections);
                process_error(PIPE_CLOSED, "", game);
            }
        }
    }
}

/**
 * Checks an array of strings to make sure they aren't empty
 */
void check_empty(char** sections, int sectionCount, struct Game* game) {
    for (int i = 0; i < sectionCount; i++) {
        if (sections[i][0] == '\0') {
            free_variables(game, sections);
            process_error(PIPE_CLOSED, "", game);
        }
    }
}
