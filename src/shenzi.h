#ifndef SHENZI_H
#define SHENZI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "gameAndPlayerData.h"
#include "playerErrorCheck.h"
#include "playerDowhat.h"

void error_checker_start(int argc, char** args, struct Game* game);

void do_what(struct Game* game);

int prioritise_max(struct Game* game, int* bestCards);

int find_most_recent(int* leastTokens);

void take_tokens(struct Game* game);

int find_set_element(int* leastTokens);

void final_print(struct Game* game, int priorityCard);

int* find_max(struct Game* game, int* cardsPurchasable);

void clear_array(int* toClear);

int sum_of_wild_required(struct Player* player, struct Card* card, int index);
#endif
