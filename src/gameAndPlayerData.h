#ifndef GAME_AND_PLAYER_DATA_H 
#define GAME_AND_PLAYER_DATA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define TOKENS 4
#define DIFFERENT_DISCOUNTS_PLUS_WILD 5
#define MAX_MESSAGE_LENGTH 68
#define MAX_LENGTH_OF_SECTION 10 /*length of int on unix system*/
#define MAX_SECTIONS 7 /*The total number of "sections" a message can have*/
#define CARDS_AT_A_TIME 8
#define LEN_EOG 3
#define LEN_TOOK_WILD 4
#define LEN_TOKENS_DOWHAT 6
#define LEN_NEWCARD 7
#define LEN_PURCHASED 9
#define MAX_NUM_OF_PLAYERS 26

enum MessageType {
    EOG = 0,
    DO_WHAT = 1,
    TOKENS_MESSAGE = 2,
    NEW_CARD = 3,
    PURCHASE = 4,
    TOOK = 5,
    WILD = 6,
};

enum CardColour {
    PURPLE = 0,
    BLUE = 1,
    YELLOW = 2,
    RED = 3,
};

struct Game {
    int numPlayers;
    struct Card** gameDeck; //normal array
    int numOfCards;
    struct Card** currentEight;
    int cardsFaceUp;
    int cardCounter;
    struct Player** players;
    int currentPlayer;
    int nonWildTokens[TOKENS];
    int pointsRequired;
    int* in;
    int* out;
    bool hasWinner;
    int firstIteration;
    int invalidMessage;
    int hasntChanged;
};

struct TokenPile {
    int purple;
    int blue;
    int yellow;
    int red;
};

struct Deck {
    struct Card* cards; //pointer to head of linked list
};

struct Card {
    int cardNum;
    char colourDiscount;   
    int* price;  //order: P,B,Y,R.
    int points;
    //struct Card* nextCard;
};

struct Player {
    pid_t id;
    char playerLetter; //i.e. Player A, player B etc.
    int playerNum;
    char* type;
    int points;
    int discounts[TOKENS]; //order: P,B,Y,R.
    int tokens[DIFFERENT_DISCOUNTS_PLUS_WILD]; //order: P,B,Y,R,W.
    FILE* childToWrite;
    FILE* childToRead;
};
    
struct Message {
    enum MessageType messageType;
    int tokens[TOKENS];
    int cardPurchased;
    int player;
    char cardColour; /*discount*/
    int pointsGained;
};

#endif //GAME_AND_PLAYER_DATA_H
