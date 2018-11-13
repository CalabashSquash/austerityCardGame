#ifndef ED_H
#define ED_H

#include <stdio.h>
#include <stdlib.h>
#include "gameAndPlayerData.h"
#include "playerErrorCheck.h"

void error_checker_start(int argc, char** argv, struct Game* game);

void do_what(struct Game* game);

void prioritise_max_ed(struct Game* game, int* maxPoints);


void prioritise_latter(struct Game* game, int* maxPoints);

void prioritise_before(struct Game* game, int* soonestAfter, 
        int closestPlayer, int* maxPoints);

void choose_oldest(struct Game* game, int* soonestAfter);

int can_player_purchase(struct Game* game, int player, struct Card* card,
        int* wildsUsed);

int is_purchasable(struct Game* game, int index);

void choose_tokens_no_card(struct Game* game);


void choose_tokens_card(struct Game* game, int cardIndex);



#endif
