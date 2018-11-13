#ifndef HUB_H
#define HUB_H

#include <fcntl.h>

#include "gameAndPlayerData.h"

#define PRICES_NO 4
#define TOKENS 4
#define DIFFERENT_DISCOUNT_PLUS_WILD 5


/**
 * Main game-playing function
 */
void play_game(struct Game* game);

/**
 * Draws first cards for game
 */
void draw_cards(struct Game* game);


/**
 * Returns non-zero if there is a winner
 */
int check_winner(struct Game* game);

/**
 * Shuts down game if end of game is reached
 */
void shut_down(struct Game* game);

/**
 * Parses one line in the deck file and assigns Card Struct variables the
 * correct values
 */
void parse_line(char* line, struct Card* current);

/**
 * Processes the deck file.
 * Returns given deck.
 */
struct Card** read_deck(char* deckFile, int* numberCards);


void signal_handler(int sigNum);

void set_up_signal();

/**
 * sets up the variables in game struct.
 */
void game_variables(struct Game* game, char** argv, int playerCount);

/**
 * Sets up the players in Game struct.
 */
void create_game(int playerCount, struct Game* game, char** players);

/**
 * Sets up players. A.k.a child processes.
 */
struct Player* setup_players(char* player, int count, struct Game* game);

void message_all_players(struct Game* game, char* message);

/**
 * Checks to see if char in deck file meets its requirements
 */
void check_char(int i, char toBeChecked);

/**
 * Kills child process and ends game
 */
void kill_and_end(struct Game* game, int errorType);

/**
 * Frees all memory and exits game
 */
void exit_game(struct Game* game);

#endif //HUB_H
