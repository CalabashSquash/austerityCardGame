#include <stdio.h>
#include <stdlib.h>

#include "gameAndPlayerData.h"

/**
 * Prints purchase message if there is a card to purchase
 */
void print_purchase_message(struct Game* game, int card);

/**
 * Clears elements of an array
 */
void clear_array(int* toclear);

/**
 * Returns the first element in an array that is set
 */
int find_set_index(int* toCalc);

/**
 * Count how many elements of an array are non-zero
 */
int count_max(int* maxPoints);


/**
 * Finds sum of wild tokens required by other tokens
 */
int sum_of_wild_required(struct Player* player, struct Card* card, int index);
