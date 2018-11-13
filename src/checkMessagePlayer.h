#ifndef CHECK_MESSAGE_PLAYER_H
#define CHECK_MESSAGE_PLAYER_H

#include <stdio.h>
#include <ctype.h>


/**
 * Frees variables if about to exit
 */
void free_variables(struct Game* game, char** sections);

/**
 * Checks a newcard message's validity
 */
void check_new_card(char** sections, struct Game* game);

/**
 * Checks a took message's validity
 */
void check_took_tokens(char** sections, struct Game* game);

/**
 * Checks a purchased message's validity
 */
void check_purchased(char** sections, struct Game* game);

/**
 * Checks an array of char arrays to make sure everything is a digit
 */
void check_digits(char** sections, int sectionCount, int startingSection, 
        struct Game* game);

/**
 * Checks an array of char arrays to make sure none are empty
 */
void check_empty(char** sections, int sectionCount, struct Game* game);

#endif //CHECK_MESSAGE_PLAYER_H
