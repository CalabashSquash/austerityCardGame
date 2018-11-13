#include "ed.h"
#include "gameAndPlayerData.h"
#include "checkMessagePlayer.h"
#include "playerDowhat.h"

/**
 * A tutor said on piazza that we had to hard code the player types
 */
void error_checker_start(int argc, char** argv, struct Game* game) {
    error_checker(argc, argv, "ed", game);
}

/**
 * Decision making for ed
 */
void do_what(struct Game* game) {
    int* maxPoints = malloc(sizeof(int) * CARDS_AT_A_TIME);
    int max = 0;
    clear_array(maxPoints);
    /* Find cards worth the max points */
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (game->currentEight[i]->cardNum == -1) {
            continue;
        }
        if (is_purchasable(game, i)) {
            if (game->currentEight[i]->points > max) {
                max = game->currentEight[i]->points;
                clear_array(maxPoints);
                maxPoints[i] = 1;
            } else if (game->currentEight[i]->points == max) {
                maxPoints[i] = 1;
            }
        } else {
        }
    }
    if (!count_max(maxPoints)) {
        fflush(stderr);
        choose_tokens_no_card(game);
        return;
    } 
    prioritise_max_ed(game, maxPoints);
    return;
}


/**
 * Given an array of card indices of cards that have max points
 * purchasable by other players, prioritise which card
 */
void prioritise_max_ed(struct Game* game, int* maxPoints) {
    if (count_max(maxPoints) == 1) {
        int wildsUsed = 0;
        int cardIndex = find_set_index(maxPoints);
        //printf("cardIndex: %d\n", cardIndex);
        if (can_player_purchase(game, game->currentPlayer, 
                game->currentEight[cardIndex], &wildsUsed)) {
            print_purchase_message(game, cardIndex);
            return;
        } else {
            choose_tokens_card(game, cardIndex);
            return;
        }
    }
    prioritise_latter(game, maxPoints);
} 


/**
 * Prioritises cards purchasable by players after current one
 */
void prioritise_latter(struct Game* game, int* maxPoints) {
    int closestPlayer = MAX_NUM_OF_PLAYERS + game->currentPlayer; //arbitrary
    int* soonestAfter = malloc(sizeof(int) * CARDS_AT_A_TIME);
    clear_array(soonestAfter);
    int wilds = 0;
    for (int j = 0; j < CARDS_AT_A_TIME; j++) {
        if (maxPoints[j]) {
            for (int i = game->currentPlayer + 1; i < game->numPlayers; i++) {
                if (can_player_purchase(game, i, game->currentEight[j], 
                        &wilds)) {
                    if (i < closestPlayer) {
                        fprintf(stderr, "j1: %d\n", j);
                        clear_array(soonestAfter);
                        closestPlayer = i;
                        soonestAfter[j] = 1;
                    } else if (i == closestPlayer) {
                        fprintf(stderr, "j2: %d\n", j);
                        soonestAfter[j] = 1;
                    }
                }
            }
        }
    }
    if (count_max(soonestAfter) == 1) {
        int cardIndex = find_set_index(soonestAfter);
        wilds = 0;
        if (can_player_purchase(game, game->currentPlayer, 
                game->currentEight[cardIndex], &wilds)) {
            print_purchase_message(game, cardIndex);
            return;
        } else {
            choose_tokens_card(game, cardIndex);
            return;
        }
    } else if (count_max(soonestAfter) == 0) {
        prioritise_before(game, soonestAfter, closestPlayer, maxPoints);
        return;
    }
    choose_oldest(game, soonestAfter);
}


/**
 * If no cards can be purchased by a player after the players later in the 
 * round, find cards purchasable by players in front of, as specified in spec.
 */
void prioritise_before(struct Game* game, int* soonestAfter, 
        int closestPlayer, int* maxPoints) {
    int wilds = 0;
    for (int j = 0; j < CARDS_AT_A_TIME; j++) {
        if (maxPoints[j]) {
            for (int i = 0; i < game->currentPlayer; i++) {
                if (can_player_purchase(game, i, game->currentEight[j],
                        &wilds)) {
                    if (i < closestPlayer) {
                        clear_array(soonestAfter);
                        closestPlayer = i;
                        soonestAfter[j] = 1;
                    } else if (i == closestPlayer) {
                        soonestAfter[j] = 1;
                    }
                }
            }
        }
    }
    if (count_max(soonestAfter) == 1) {
        wilds = 0;
        int cardIndex = find_set_index(soonestAfter);
        if (can_player_purchase(game, game->currentPlayer,
                game->currentEight[cardIndex], &wilds)) {   
            print_purchase_message(game, cardIndex);
            return;
        }
        choose_tokens_card(game, cardIndex);
        return;
    }

}

/**
 * Out of all set elements in array, chooses the one that appears last
 */
void choose_oldest(struct Game* game, int* soonestAfter) {
    if (!count_max(soonestAfter)) {
        choose_tokens_no_card(game);
        return;
    }
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (soonestAfter[i]) {
            int wilds = 0;
            if (can_player_purchase(game, game->currentPlayer,
                    game->currentEight[i], &wilds)) {
                print_purchase_message(game, i);
                return;
            }
        }
    }
}

//todo: change ed functions so that the closest after you get preference.
/**
 * Finds out if a particular player can purchase a particular card
 */
int can_player_purchase(struct Game* game, int player, struct Card* card,
        int* wildsUsed) {
    int isPurchasable = 1;
    for (int j = 0; j < TOKENS; j++) {
        //printf("card->cardNum: %d\n", card->cardNum);
        //printf("yeehaw\n");
        if (card->price[j] > game->players[player]->tokens[j] + 
                game->players[player]->discounts[j]) {
            //isPurchasable = 1;
            if (game->players[player]->tokens[TOKENS] - *wildsUsed <= 0) {
                isPurchasable = 0;
                continue;
            }
            if (card->price[j] > 
                    (game->players[player]->tokens[j] + 
                    game->players[player]->discounts[j] + 
                    (game->players[player]->tokens[TOKENS] - *wildsUsed))) {
                isPurchasable = 0;
            } else {
                *wildsUsed += card->price[j] - 
                        game->players[player]->tokens[j];     
            }          
        }
    }
    return isPurchasable;
}

/**
 * Finds out if a card is purchasable by any other player
 */
int is_purchasable(struct Game* game, int index) {
    struct Card* currentCard = game->currentEight[index];
    int wildsUsed;
    int isPurchasable;
    for (int i = 0; i < game->numPlayers; i++) {
        if (i == game->currentPlayer) {
            continue;
        }
        wildsUsed = 0;
        isPurchasable = 1;
        isPurchasable = can_player_purchase(game, i, currentCard, &wildsUsed);

        if (isPurchasable) {
            return isPurchasable;
        }
    }
    return isPurchasable;
}

/**
 * Given a card that meets the requirements, choose the tokens based on
 * the prices of that card
 */
void choose_tokens_card(struct Game* game, int index) {
    int tokens[TOKENS];
    int tokensAvailable = 0;
    struct Card* card = game->currentEight[index];
    struct Player* player = game->players[game->currentPlayer];
    for (int i = 0; i < TOKENS; i++) {
        tokens[i] = 0;
        if (game->nonWildTokens[i]) {
            tokensAvailable++;
        }
    }
    if (tokensAvailable < 3) {
        fprintf(stderr, "ONE\n");
        fflush(stderr);
        printf("wild\n");
        fflush(stdout);
        return;
    }
    tokensAvailable = 0;
    if (game->nonWildTokens[2] && card->price[2] > player->tokens[2]) {
        tokensAvailable++;
        tokens[2] = 1;
    }
    if (game->nonWildTokens[3] && card->price[3] > player->tokens[3]) {
        tokensAvailable++;
        tokens[3] = 1;
    }
    if (game->nonWildTokens[1] && card->price[1] > player->tokens[1]) {
        tokensAvailable++;
        tokens[1] = 1;
    }
    if (game->nonWildTokens[0] && card->price[0] > player->tokens[0] &&
            tokensAvailable < 3) {
        tokensAvailable++;
        tokens[0] = 1;
    }
    int tokenOrder[] = {2, 3, 1, 0};
    for (int i = 0; i < TOKENS; i++) {
        if (!tokens[tokenOrder[i]] && game->nonWildTokens[tokenOrder[i]] &&
                tokensAvailable < 3) {
            tokensAvailable++;
            tokens[tokenOrder[i]] = 1;
        }
    }
    printf("take%d,%d,%d,%d\n", tokens[0], tokens[1], tokens[2], tokens[3]);
    fflush(stdout);
}


/**
 * Chooses which tokens to take if no apprioriate card could be found
 */
void choose_tokens_no_card(struct Game* game) {
    int tokens[TOKENS];
    int tokensAvailable = 0;
    for (int i = 0; i < TOKENS; i++) {
        tokens[i] = 0;
    }
    if (game->nonWildTokens[2]) {
        tokens[2] = 1;
        tokensAvailable += 1;
    }
    if (game->nonWildTokens[3]) {
        tokens[3] = 1;
        tokensAvailable += 1;
    }
    if (game->nonWildTokens[1]) {
        tokens[1] = 1;
        tokensAvailable += 1;
    }
    if (game->nonWildTokens[0] && tokensAvailable < 3) {
        tokens[0] = 1;
        tokensAvailable++;
    }
    fprintf(stderr, "%d %d %d %d\n", game->nonWildTokens[0],
            game->nonWildTokens[1],
            game->nonWildTokens[2], game->nonWildTokens[3]);
    fflush(stderr);
    if (tokensAvailable < 3) {
        fprintf(stderr, "TWO\n");
        fflush(stderr);
        printf("wild\n");
        fflush(stdout);
        return;
    }
    printf("take%d,%d,%d,%d\n",
            tokens[0], tokens[1], tokens[2], tokens[3]);
    fflush(stdout);
}


