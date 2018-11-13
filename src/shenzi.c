#include "shenzi.h"

/*
 * Calls error_checker with "shenzi" as the player type.
 * I did it this way so you have the modularity of being able to have 
 * the same function called from sharedPlayer but with different player types
 */
void error_checker_start(int argc, char** argv, struct Game* game) {
    error_checker(argc, argv, "shenzi", game);
}


/*
 * Handles the overall move decision-making for a shenzi player
 */
void do_what(struct Game* game) {
    int cardsPurchasable[CARDS_AT_A_TIME];
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        cardsPurchasable[i] = 0;
    }
    int isPurchasable;
    int purchasableCount = 0;
    int wildsUsed = 0;
    struct Player* thisPlayer = game->players[game->currentPlayer];
    /*Find all purchasable cards*/
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        isPurchasable = 1;
        wildsUsed = 0;
        if (game->currentEight[i]->cardNum == -1) {
            break;
        }
        for (int j = 0; j < TOKENS; j++) {
            /*This checks first if it is purchasable with normal tokens*/
            /*If not, it will check if card is purchasable with wild tokens*/
            /*Accounting for how many wilds have been used in purchasing*/
            /*This card earlier in loop*/
            if (game->currentEight[i]->price[j] > (thisPlayer->tokens[j] +
                    thisPlayer->discounts[j])) {
                if (game->currentEight[i]->price[j] <= 
                        (thisPlayer->tokens[j] + thisPlayer->discounts[j] 
                        + (thisPlayer->tokens[TOKENS] - wildsUsed))) {
                    wildsUsed += game->currentEight[i]->price[j] -
                            (thisPlayer->tokens[j] + thisPlayer->discounts[j]);
                } else {
                    isPurchasable = 0;
                }
            }
        }
        if (isPurchasable) {
            cardsPurchasable[i] = 1;
            purchasableCount++;
        }
    }
    if (!purchasableCount) {
        take_tokens(game);
        return;
    }
    int* bestCards = find_max(game, cardsPurchasable);
    int priorityCard = prioritise_max(game, bestCards);
    print_purchase_message(game, priorityCard);
}


/**
 * Sends message of which tokens to take, if possible
 */
void take_tokens(struct Game* game) {
    int tokens[TOKENS];
    int tokensAvailable = 0;
    for (int i = 0; i < TOKENS; i++) {
        tokens[i] = 0;
        if (game->nonWildTokens[i] && tokensAvailable < 3) {
            tokens[i] = 1;
            tokensAvailable += 1;
        }
    }
    //printf("Tokens available: %d\n", tokensAvailable);
    if (tokensAvailable < 3) {
        printf("wild\n");
        fflush(stdout);
        return;
    }
    printf("take%d,%d,%d,%d\n", 
            tokens[0], tokens[1], tokens[2], tokens[3]);
    fflush(stdout);
}    


/*
 * Given a list of cards that have the max amount of points, prioritise
 * which one to buy based off algorithm in spec
 */
int prioritise_max(struct Game* game, int* bestCards) {
    int bestCount = 0;
    int* leastTokens = malloc(sizeof(int) * CARDS_AT_A_TIME);
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (bestCards[i] != 0) {
            bestCount++;
        }
        leastTokens[i] = 0;
    }
    if (bestCount == 1) {
        for (int i = 0; i < CARDS_AT_A_TIME; i++) {
            if (bestCards[i]) {
                return i;
            }
        }
    }
    bestCount = 0;
    int smallestTokens = INT_MAX;
    int tokenCount;
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        tokenCount = 0;
        if (game->currentEight[i]->cardNum == -1) {
            continue;
        }
        if (bestCards[i]) {
            for (int j = 0; j < TOKENS; j++) {
                if (game->currentEight[i]->price[j] == 0) {
                    continue;
                }
                tokenCount += (game->currentEight[i]->price[j] -
                        game->players[game->currentPlayer]->discounts[j]);
            }
            if (tokenCount < smallestTokens) {
                bestCount = 1;
                clear_array(leastTokens);
                smallestTokens = tokenCount;
                leastTokens[i] = 1;
            } else if (tokenCount == smallestTokens) {
                bestCount++;
                leastTokens[i] = 1;
            }
        }
    }
    if (bestCount == 1) {
        int best = find_set_index(leastTokens);
        return best;
    }
    int mostRecent = find_most_recent(leastTokens);
    return mostRecent;
}


/**
 * returns the last elemenrt in the array that is set to 1.
 * In this case, that is also the most recent card added to deck.
 */
int find_most_recent(int* leastTokens) {
    for (int i = CARDS_AT_A_TIME - 1; i >= 0; i--) {
        
        if (leastTokens[i]) {
            return i;
        }
    }
    return -1;
}
 
         
/**
 * Finds card with max points out of all purchasable cards
 */
int* find_max(struct Game* game, int* cardsPurchasable) {
    int* bestCards = malloc(sizeof(int) * CARDS_AT_A_TIME);
    int max = 0;

    clear_array(bestCards);    

    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (game->currentEight[i]->cardNum == -1) {
            continue;
        }
        if (cardsPurchasable[i] && game->currentEight[i]->points > max) {
            clear_array(bestCards);
            bestCards[i] = 1;
            max = game->currentEight[i]->points;
        }
        if (cardsPurchasable[i] && game->currentEight[i]->points == max) {
            bestCards[i] = 1;
        }
    }
    return bestCards;
}

