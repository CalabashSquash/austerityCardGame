#ifndef SHARED_PLAYER_H
#define SHARED_PLAYER_H

#include <string.h>
#include <ctype.h>

#include "gameAndPlayerData.h"

void play_game(struct Game* game);

void setup_game(char** argv, struct Game* game);

struct Player* setup_player(struct Game* game);

int find_index(struct Game* game);


char** split_string(char* toSplit, char delimiter, char delimiterTwo, 
        struct Game* game);


void tokens(struct Game* game, char* input);

void remove_card(struct Game* game, int index);

void read_message(struct Game* game, struct Message* message);

void took_wild(struct Game* game, char* input);

void print_game_state(struct Game* game);

void end_of_game(struct Game* game);

void took_tokens(struct Game* game, char* input);

void purchased(struct Game* game, char* input);

void new_card(struct Game* game, struct Message* message, char* input);

#endif
