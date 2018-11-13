#include "playerDowhat.h"

/**
 * Print purchase message if there is a card to purchase.
 */
void print_purchase_message(struct Game* game, int card) {
    struct Player* player = game->players[game->currentPlayer];
    struct Card* toPrint = game->currentEight[card];
    int wildsUsed = 0;
    int tokensSpent[TOKENS];
    for (int i = TOKENS - 1; i >= 0; i--) {
        tokensSpent[i] = 0;
        int thisPrice = toPrint->price[i];
        if (thisPrice == 0) {
            continue;
        }
        if (thisPrice > (player->tokens[i] + player->discounts[i])) {
            wildsUsed += (thisPrice - (player->tokens[i] + 
                    player->discounts[i]));
            thisPrice -= (thisPrice - 
                    (player->tokens[i] + player->discounts[i]));
        }

        if (thisPrice <= (player->tokens[i] + player->discounts[i])) {
            if (thisPrice - player->discounts[i] <= 0) {
                tokensSpent[i] = 0;
                continue;
            }
            tokensSpent[i] = thisPrice - player->discounts[i];
        }
    }
    printf("purchase%d:%d,%d,%d,%d,%d\n", card, tokensSpent[0], 
            tokensSpent[1], tokensSpent[2], tokensSpent[3], wildsUsed);
    fflush(stdout);
}

/**
 * calculates sum of wilds required by other players
 */
int sum_of_wild_required(struct Player* player, struct Card* card, 
        int index) {
    int sum = 0;
    for (int i = index - 1; i >= 0; i--) {
        if (card->price[i] - player->tokens[i] < 0) {
            continue;
        }   
        sum += (card->price[i] - player->tokens[i]);
    }
    return sum;
}


/**
 * Clears elements of an array.
 */
void clear_array(int* toClear) {
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        toClear[i] = 0;
    }
}


/**
 * Returns the first element in an array that is set
 */
int find_set_index(int* toCalc) {
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (toCalc[i]) {
            return i;
        }
    }
    return -1;
}


/**
 * Count how many elements of the array are non-zero
 */
int count_max(int* maxPoints) {
    int count = 0;
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (maxPoints[i]) {
            count++;
        }
    }
    return count;
}
