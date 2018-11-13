#ifndef BANZAI_H
#define BANZAI_H

#include <stdio.h>
#include <stdlib.h>

#include "gameAndPlayerData.h"
#include "playerErrorCheck.h"
#include "checkMessagePlayer.h"


/**
 * A tutor said on piazza that we had to hard code the player types for this
 */
void error_checker_start(int argc, char** argv, struct Game* game);

/**
 * Decision-making for banzai player
 */
void do_what(struct Game* game);


/**
 * Counts the tokens owned by player
 */
int count_tokens(struct Game* game);


/**
 * Takes tokens in order specified in spec
 */
void take_tokens_banzai(struct Game* game);


/**
 * Finds most expensive card(s)
 */
void find_most_expensive(struct Game* game, int* cardsPurchasable);


/**
 * Prioritises cards based on the ones that use the most wild tokens
 */
void most_wild_tokens(struct Game* game, int* mostExpensive);


/**
 * Finds and purchases the oldest card
 */
void find_oldest(struct Game* game, int* mostWildCards);


/**
 * Finds out if the player can purchase a card
 */
int can_purchase(struct Game* game, int index, int* wildsUsed);

/**
 * Counts price of a card
 */
int count_price(struct Card* card, struct Game* game);

#endif //BANZAI_H
