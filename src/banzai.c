#include "banzai.h"
#include "playerDowhat.h"


/**
 * A tutor said on piazza we had to hard code the player types
 */
void error_checker_start(int argc, char** argv, struct Game* game) {
    error_checker(argc, argv, "banzai", game);
}

/**
 * Decision-making for banzai player
 */
void do_what(struct Game* game) {
    int tokensOwned = count_tokens(game);
    int* cardsPurchasable = malloc(sizeof(int) * CARDS_AT_A_TIME);
    int wildsUsed = 0;
    if (tokensOwned < 3) {
        take_tokens_banzai(game);
        return;
    }
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        wildsUsed = 0;
        //fprintf(stderr, "i: %d game->currentEight[%d]->cardNum: %d\n", i, i, 
        //        game->currentEight[i]->cardNum);
        cardsPurchasable[i] = 0;
        if (game->currentEight[i]->cardNum == -1 ||
                !game->currentEight[i]->points) {
        //    fprintf(stderr, "I: %d @@@@@@@@@@@@\n", i);
            continue;
        }
        if (can_purchase(game, i, &wildsUsed)) {
       //     fprintf(stderr, "i: %d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", i);
            cardsPurchasable[i] = 1;
        }
    }
    if (count_max(cardsPurchasable) == 1) {
        int index = find_set_index(cardsPurchasable);
        print_purchase_message(game, index);
        free(cardsPurchasable);
        return;
    }
    if (!count_max(cardsPurchasable)) {
        printf("wild\n");
        fflush(stdout);
        free(cardsPurchasable);
        return;
    }
    find_most_expensive(game, cardsPurchasable);
            
}

/**
 * Prints the purchase message for banzai
 */
/*
void print_final_message(struct Game* game, int index) {
    struct Player* player = game->players[game->currentPlayer];
    struct Card* toPrint = game->currentEight[index];
    int wildsUsed = 0;
    int tokensSpent[TOKENS];
    int wildsLeft = player->tokens[TOKENS];
    for (int i = TOKENS - 1; i >= 0; i--) {
        tokensSpent[i] = 0;
        int thisPrice = toPrint->price[i];
        if (thisPrice == 0) {
            continue;
        }
        if (player->tokens[TOKENS] - wildsUsed) {
            if (wildsLeft <= thisPrice) {
                wildsUsed += wildsLeft - sum_of_wild_required(player,
                        toPrint, i);
                thisPrice -= (wildsLeft - sum_of_wild_required(player,
                        toPrint, i));
                wildsLeft = sum_of_wild_required(player, toPrint, i);
            } else if (wildsLeft > thisPrice) {
                wildsUsed += wildsLeft - sum_of_wild_required(player,
                        toPrint, i);
                thisPrice -= (wildsLeft - sum_of_wild_required(player,
                        toPrint, i));
                wildsLeft = sum_of_wild_required(player, toPrint, i);
            }
        }
        if (thisPrice <= (player->tokens[i] + player->discounts[i])) {
            tokensSpent[i] = thisPrice - player->discounts[i];
        }
    }
    printf("purchase%d:%d,%d,%d,%d,%d\n", index, tokensSpent[0], 
            tokensSpent[1], tokensSpent[2], tokensSpent[3], wildsUsed);
    fflush(stdout);

}
*/

/**
 * Finds most expensive card(s)
 */
void find_most_expensive(struct Game* game, int* cardsPurchasable) {
    int highestPrice = 0;
    int* mostExpensive = malloc(sizeof(int) * CARDS_AT_A_TIME);
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        mostExpensive[i] = 0;
        if (count_price(game->currentEight[i], game) > highestPrice) {
            highestPrice = count_price(game->currentEight[i], game);
            clear_array(mostExpensive);
            mostExpensive[i] = 1;
        }
        if (count_price(game->currentEight[i], game) == highestPrice) {
            mostExpensive[i] = 1;
        }
    }
    if (count_max(mostExpensive) == 1) {
        int index = find_set_index(cardsPurchasable);
        print_purchase_message(game, index);
        return;
    }
    most_wild_tokens(game, mostExpensive);
}

/**
 * Prioritises cards based on the ones that use the most wild tokens
 */
void most_wild_tokens(struct Game* game, int* mostExpensive) {
    int* mostWildCards = malloc(sizeof(int) * CARDS_AT_A_TIME);
    int mostWilds = 0;
    int wildsUsed;
    struct Player* thisPlayer = game->players[game->currentPlayer];
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        wildsUsed = 0;
        mostWildCards[i] = 0;
        if (!mostExpensive[i]) {
            continue;
        }
        for (int j = TOKENS - 1; i >= 0; i--) {
            int thisPrice = game->currentEight[i]->price[j];
            if (thisPrice == 0) {
                continue;
            }
            if (thisPlayer->tokens[TOKENS] - wildsUsed) {
                if (thisPlayer->tokens[TOKENS] - wildsUsed <= thisPrice) {  
                    thisPrice -= thisPlayer->tokens[TOKENS] - wildsUsed;
                    wildsUsed = thisPlayer->tokens[TOKENS];
                } else if (thisPlayer->tokens[TOKENS] - 
                        wildsUsed > thisPrice) {
                    wildsUsed += thisPrice;
                    thisPrice = 0;
                }
            }
        }
        if (wildsUsed > mostWilds) {
            clear_array(mostWildCards);
            mostWildCards[i] = 1;
            mostWilds = wildsUsed;
        } else if (wildsUsed == mostWilds) {
            mostWildCards[i] = 1;
        }
    }
    if (count_max(mostWildCards) == 1) {
        int index = find_set_index(mostWildCards);
        print_purchase_message(game, index);
        return;
    }
    find_oldest(game, mostWildCards);
}
        

/**
 * Finds and purchases the oldest card
 */
void find_oldest(struct Game* game, int* mostWildCards) {
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (mostWildCards[i]) {
            print_purchase_message(game, i);
            return;
        }
    }
}

/**
 * Finds out if the player can purchase a card
 */
int can_purchase(struct Game* game, int index, int* wildsUsed) {
    int isPurchasable = 1;
    struct Card* card = game->currentEight[index];
    struct Player* player = game->players[game->currentPlayer];
    
    for (int j = 0; j < TOKENS; j++) {
        if (card->price[j] > player->tokens[j] + player->discounts[j]) {
            if (card->price[j] <= (player->tokens[j] + player->discounts[j]
                    + (player->tokens[TOKENS] - *wildsUsed))) {
                *wildsUsed += card->price[j] - (player->tokens[j] + 
                        player->discounts[j]);
            } else {
                isPurchasable = 0;
            }
        }
    }
    return isPurchasable;

/*
    for (int j = 0; j < TOKENS; j++) {
        if (card->price[j] > player->tokens[j] + 
                player->discounts[j]) {
            if (player->tokens[TOKENS] - *wildsUsed <= 0) {
                isPurchasable = 0;
                continue;
            }
            if (card->price[j] > (player->tokens[j] + player->discounts[j] +
                    (player->tokens[TOKENS] - *wildsUsed))) {
                isPurchasable = 0;
            } else {
                *wildsUsed += card->price[j] - player->tokens[j];
            }
        }
    }
    return isPurchasable;*/
}

/**
 * Counts the total price of a card
 */
int count_price(struct Card* card, struct Game* game) {
    int price;
    for (int i = 0; i < TOKENS; i++) {
        price += (card->price[i] - 
                game->players[game->currentPlayer]->discounts[i]);
    }
    return price;
}


/**
 * Counts the tokens owned by a player
 */
int count_tokens(struct Game* game) {
    int tokenCount = 0;
    for (int i = 0; i < TOKENS + 1; i++) {
        if (game->players[game->currentPlayer]->tokens[i]) {
            tokenCount += game->players[game->currentPlayer]->tokens[i];
        }
    }
    return tokenCount;
}

/**
 * Takes appropriate tokens for banzai player
 */
void take_tokens_banzai(struct Game* game) {
    int tokensAvailable = 0;
    int tokens[TOKENS];
    int tokenOrder[] = {2, 1, 0, 3};
    for (int i = 0; i < TOKENS; i++) {
        tokens[i] = 0;
        if (game->nonWildTokens[tokenOrder[i]] && tokensAvailable < 3) {
            tokens[i] = 1;
            tokensAvailable++;
        }
    }
    if (tokensAvailable < 3) {
        fprintf(stderr, "TWO\n");
        printf("wild\n");
        fflush(stdout);
        return;
    }
    printf("take%d,%d,%d,%d\n", tokens[0], tokens[1], tokens[2], tokens[3]);
    fflush(stdout);
    
}
