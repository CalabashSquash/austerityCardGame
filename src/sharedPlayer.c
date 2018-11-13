#include <stdio.h>
#include <stdbool.h>

#include "sharedPlayer.h"
#include "playerErrorCheck.h"
#include "shenzi.h"
#include "checkMessagePlayer.h"
#include "ed.h"
#include "banzai.h"

#define PRINT_DEBUG 1
#define MAX_PLAYERS 26


int main(int argc, char** argv) {
    struct Game* game = malloc(sizeof(struct Game));
    error_checker_start(argc, argv, game);
    setup_game(argv, game);
    play_game(game);
}

/**
 * Main game playing function
 */
void play_game(struct Game* game) {
    struct Message* message = malloc(sizeof(struct Message));
    bool hasWinner = false;
    while (!hasWinner) {
        read_message(game, message);
    }
}

/**
 * Reads a message and deals with it appropriately
 */
void read_message(struct Game* game, struct Message* message) {
    char* input = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    memset(input, '\0', MAX_MESSAGE_LENGTH);
    fgets(input, MAX_MESSAGE_LENGTH, stdin);
    if (!strcmp("eog\n", input)) {
        end_of_game(game);
        free(input);
        return;
    } else if (!strncmp("wild", input, LEN_TOOK_WILD)) {
        took_wild(game, input);
        free(input);
    } else if (!strncmp("took", input, LEN_TOOK_WILD)) {
        took_tokens(game, input);
        free(input);
    } else if (!strncmp("tokens", input, LEN_TOKENS_DOWHAT)) {
        tokens(game, input);
        free(input);
    } else if (!strncmp("newcard", input, LEN_NEWCARD)) {
        new_card(game, message, input);
        free(input);
    } else if (!strncmp("purchased", input, LEN_PURCHASED)) {
        purchased(game, input);
        free(input);
    } else if (!strncmp("dowhat\n", input, LEN_TOKENS_DOWHAT)) {
        fprintf(stderr, "Received dowhat\n");
        fflush(stderr);
        do_what(game);
        free(input);
        return;
    } else {
        if (feof(stdin)) {
        }
        fprintf(stderr, "Communication Error\n");
        free(input);
        exit(6); /*Only reached if bad*/
    }
    print_game_state(game);
}

/**
 * Prints current game state
 */
void print_game_state(struct Game* game) {
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        struct Card* card = game->currentEight[i];
        if (card->cardNum == -1) {
            break;
        }
        fprintf(stderr, "Card %d:%c/%d/%d,%d,%d,%d\n", i, 
                card->colourDiscount,
                card->points, card->price[0], card->price[1], card->price[2],
                card->price[3]);
    }
    for (int i = 0; i < game->numPlayers; i++) {
        struct Player* player = game->players[i];
        fprintf(stderr, 
                "Player %c:%d:Discounts=%d,%d,%d,%d:Tokens=%d,%d,%d,%d"
                ",%d\n", player->playerNum + 65, player->points,
                player->discounts[0], player->discounts[1], 
                player->discounts[2], player->discounts[3],
                player->tokens[0], player->tokens[1], player->tokens[2],
                player->tokens[3], player->tokens[4]);
    }
}

/**
 * Splits a string based on two delimiters
 */
char** split_string(char* toSplit, char delimiter, char delimiterTwo, 
        struct Game* game) {
    char** sections = malloc(sizeof(char*) * MAX_SECTIONS);
    int sectionCounter = 0;
    int indexInSection = 0;
    for (int i = 0; i < MAX_SECTIONS; i++) {
        sections[i] = malloc(sizeof(char) * MAX_LENGTH_OF_SECTION);
        memset(sections[i], '\0', MAX_LENGTH_OF_SECTION);
    }
    for (int i = 0; i < strlen(toSplit) - 1; i++) {
        if (sectionCounter >= MAX_SECTIONS) {
            process_error(PIPE_CLOSED, "", game);
        }
        if (toSplit[i] == delimiter || toSplit[i] == delimiterTwo) {
            sectionCounter++;
            indexInSection = 0;
            continue;
        }
        sections[sectionCounter][indexInSection] = toSplit[i];
        indexInSection++;
    }
    return sections;
}       


/*
 * Parses purchase message and takes card out of pile, gives discounts to
 * player
 */
void purchased(struct Game* game, char* input) {
    char** sections = split_string(input, ',', ':', game);
    check_purchased(sections, game);
    struct Card* cardPurchased = game->currentEight[atoi(sections[1])];
    if (cardPurchased->cardNum == -1) {
        process_error(PIPE_CLOSED, "", game);
    }
    for (int i = 0; i < DIFFERENT_DISCOUNTS_PLUS_WILD; i++) {
        if (atoi(sections[i + 2]) == 0) {
            continue;
        }
        if (i < 4) {
            game->players[(sections[0][9]) - 65]->tokens[i] -= 
                    (atoi(sections[i + 2])); 
            game->nonWildTokens[i] += atoi(sections[i + 2]);
        } else {
            game->players[(sections[0][9]) - 65]->tokens[i] -= 
                    atoi(sections[i + 2]);
        }
    }
    game->players[(sections[0][9] - 65)]->points += 
            cardPurchased->points;
    if (cardPurchased->colourDiscount == 'P') {
        game->players[(sections[0][9]) - 65]->discounts[0] += 1;
    }
    if (cardPurchased->colourDiscount == 'B') {
        game->players[(sections[0][9]) - 65]->discounts[1] += 1;
    }
    if (cardPurchased->colourDiscount == 'Y') {
        game->players[(sections[0][9]) - 65]->discounts[2] += 1;
    }
    if (cardPurchased->colourDiscount == 'R') {
        game->players[(sections[0][9]) - 65]->discounts[3] += 1;
    }
    remove_card(game, atoi(sections[1]));
}

/**
 * Removes a card from current eight
 */
void remove_card(struct Game* game, int index) {
    free(game->currentEight[index]);
    for (int i = index; i < CARDS_AT_A_TIME - 1; i++) {
        game->currentEight[i] = game->currentEight[i + 1];
    }
    struct Card* newCard = malloc(sizeof(struct Card*));
    newCard->cardNum = -1;
    game->currentEight[CARDS_AT_A_TIME - 1] = newCard;
}

/**
 * Deals with a player taking tokens
 */
void took_tokens(struct Game* game, char* input) {
    char** sections = split_string(input, ',', ':', game);
    check_took_tokens(sections, game);//todo: fix this into loop
    game->players[input[4] - 65]->tokens[0] += atoi(sections[1]);
    game->players[input[4] - 65]->tokens[1] += atoi(sections[2]);
    game->players[input[4] - 65]->tokens[2] += atoi(sections[3]);
    game->players[input[4] - 65]->tokens[3] += atoi(sections[4]);
    game->nonWildTokens[0] -= atoi(sections[1]);
    game->nonWildTokens[1] -= atoi(sections[2]);
    game->nonWildTokens[2] -= atoi(sections[3]);
    game->nonWildTokens[3] -= atoi(sections[4]);
}

/**
 * For when token count is declared
 */
void tokens(struct Game* game, char* input) {
    char* tokenNum = malloc(sizeof(char) * MAX_LENGTH_OF_SECTION);
    memset(tokenNum, '\0', MAX_LENGTH_OF_SECTION);
    for (int i = 6; i < strlen(input) - 1; i++) {
        if (!isdigit(input[i])) {
            process_error(PIPE_CLOSED, "", game);
        }
        tokenNum[i - 6] = input[i];
    }
    if (strlen(input) < 8) {
        process_error(PIPE_CLOSED, "", game);
    }
    int tokens = atoi(tokenNum);
    for (int i = 0; i < TOKENS; i++) {
        game->nonWildTokens[i] = tokens;
    }
    
}

/**
 * Deals with a newcard message
 */
void new_card(struct Game* game, struct Message* message, char* input) {
    struct Card* newCard = malloc(sizeof(struct Card));
    message->messageType = NEW_CARD;
    char** sections = split_string(input, ':', ',', game);
    check_new_card(sections, game);
    newCard->colourDiscount = input[7];
    newCard->points = atoi(sections[1]);
    newCard->price = malloc(sizeof(int) * TOKENS);
    newCard->price[0] = atoi(sections[2]); 
    newCard->price[1] = atoi(sections[3]); 
    newCard->price[2] = atoi(sections[4]); 
    newCard->price[3] = atoi(sections[5]);    
    int index;
    if ((index = find_index(game)) != -1) {
        game->currentEight[index] = newCard;
        game->currentEight[index]->cardNum = index;
    }
}

/**
 * Finds the index to place a new card
 */
int find_index(struct Game* game) {
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (game->currentEight[i]->cardNum == -1) {
            return i;
        }
    }
    return -1;
}

/**
 * Deals with a player taking a wild
 */
void took_wild(struct Game* game, char* message) {
    if (strlen(message) > 6) {
        fprintf(stderr, "Communcation Error\n");
        exit(6);
    }
    if (message[4] < 65 || message[4] > game->numPlayers + 64) {
        fprintf(stderr, "Communcation Error\n");
        exit(6); /*Only reached if bad*/
    } else {
        game->players[message[4] - 65]->tokens[4]++;
    }
}

/**
 * Deals with end of game i.e. prints out winners
 */
void end_of_game(struct Game* game) {
    int playerFound = 0;
    int winner = 0;
    char* winners = malloc(sizeof(char) * (MAX_PLAYERS + 25)); /*25 because*/
    memset(winners, '\0', MAX_PLAYERS + 25);                   /*that is the*/
    for (int i = 0; i < game->numPlayers; i++) {             /*num of commas*/ 
        if (game->players[i]->points > winner) {
            memset(winners, '\0', MAX_PLAYERS + 25);
            winners[0] = i + 65;
            winner = game->players[i]->points;
            playerFound++;
        } else if (game->players[i]->points == winner) {
            if (!playerFound) {
                winners[0] = i + 65;
                playerFound++;
            } else {
                winners[playerFound] = ',';
                playerFound++;
                winners[playerFound] = i + 65;
                playerFound++;
            }
        }
    }
    fprintf(stderr, "Game over. Winners are %s\n", winners);
    exit(0);
}

/**
 * Sets up new game
 */
void setup_game(char** argv, struct Game* game) {
    game->players = malloc(sizeof(game->players) * atoi(argv[1]));
    game->currentPlayer = atoi(argv[2]); 
    game->hasWinner = false;
    game->numPlayers = atoi(argv[1]);
    game->currentEight = malloc(sizeof(struct Card*) * CARDS_AT_A_TIME);
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        struct Card* newCard = malloc(sizeof(struct Card));
        game->currentEight[i] = newCard;
        game->currentEight[i]->cardNum = -1;
    }
    for (int i = 0; i < game->numPlayers; i++) {
        struct Player* interPlayer;
        interPlayer = setup_player(game);
        game->players[i] = interPlayer;
        game->players[i]->playerNum = i;
    }
}

/**
 * Sets up a player's variables
 */
struct Player* setup_player(struct Game* game) {
    struct Player* newPlayer = malloc(sizeof(struct Player));
    newPlayer->points = 0;
    for (int j = 0; j < 5; j++) {
        newPlayer->tokens[j] = 0;
        if (j < 4) {
            newPlayer->discounts[j] = 0;
        }
    }
    return newPlayer;
}
