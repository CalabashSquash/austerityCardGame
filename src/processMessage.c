#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "gameAndPlayerData.h"
#include "errorCheck.h"
#include "processMessage.h"

#define MAX_COMMAND_LENGTH 8 /*"purchase" is the longest message*/
#define MAX_LENGTH_OF_INT 10 

/**
 * General message processing function
 * Takes a string (message), figures out what type of message it is,
 * checks it, changes game state accordingly.
 */
char* process_message(char* inMessage, struct Game* game) {
    char* newMessage;// = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    //memset(newMessage, '\0', MAX_MESSAGE_LENGTH);
    //char* newMessage;
    if(!strcmp(inMessage, "wild\n")) {
        newMessage = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
        memset(newMessage, '\0', MAX_MESSAGE_LENGTH);
        game->players[game->currentPlayer]->tokens[4]++; /*5th slot in tokens
                                                           is the wild slot*/
        snprintf(newMessage, MAX_MESSAGE_LENGTH, "wild%c\n", 
                game->players[game->currentPlayer]->playerNum + 65);
        printf("Player %c took a wild\n", 
                game->players[game->currentPlayer]->playerNum + 65); 
        return newMessage; //todo broadcast change to players.
    } 
    //memset(newMessage, '\0', MAX_MESSAGE_LENGTH);
    newMessage = slice_command(inMessage, game);
    return newMessage;
    
}

/**
 * todo: Can change this to using strncmp like in sharedplayer.c
 * Checks what type of message and acts accordingly
 */
char* slice_command(char* inMessage, struct Game* game) {
    //char command;
    if (!strncmp("take", inMessage, LEN_TOOK_WILD - 1)) {
        return take_command(inMessage, game);
    }
    if (!strncmp("purchase", inMessage, LEN_PURCHASED - 1)) {
        return purchase_command(inMessage, game); 
    }
    //if (command == "purchase") {
    //    memset(command, '\0', MAX_MESSAGE_LENGTH);
    //    command = process_purchase(inMessage);
    //    return command;
    //}
    //memset(command, '\0', MAX_MESSAGE_LENGTH);
    if (game->hasntChanged) {
        game->invalidMessage++;
        game->hasntChanged = 0;
    } 
    return "!";/*If reached, message is invalid*/
    exit(0);
    //char wahu[] = {'h', 'h'};
    //return wahu;
}


/**
 * broadcasts a message
 * The string passed as a parameter must be newline terminated
 */
void broadcast(struct Game* game, char* message) {
    for (int i = 0; i < game->numPlayers; i++) {
        fprintf(game->players[i]->childToRead, "%s", message);
        fflush(game->players[i]->childToRead);
    }
}


/**
 * Draws new card if one is purchased
 */
void draw_new_card(struct Game* game) {
    if (game->cardCounter >= game->numOfCards) {
        return;
    }
    if (game->gameDeck[game->cardCounter]->cardNum == -1) {
        return;
    }
    struct Card* card = game->gameDeck[game->cardCounter];
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (game->currentEight[i]->cardNum == -1) {
            free(game->currentEight[i]);
            char message[MAX_MESSAGE_LENGTH];
            memset(message, '\0', MAX_MESSAGE_LENGTH);
            game->cardsFaceUp++;
            printf("New card = Bonus %c, worth %d, costs %d,%d,%d,%d\n",
                    card->colourDiscount, card->points, card->price[0],
                    card->price[1], card->price[2], card->price[3]);
            game->currentEight[i] = card;
            snprintf(message, MAX_MESSAGE_LENGTH, "newcard%c:%d,%d,%d,%d,%d\n",
                    card->colourDiscount, card->points, card->price[0],
                    card->price[1], card->price[2], card->price[3]);
            broadcast(game, message);
        }
    }
}


/**
 * Makes the purchase message to broadcast
 */
char* purchase_command(char* inMessage, struct Game* game) {
    char** sections = split_string(inMessage, ':', ',', game);
    check_purchase(inMessage, game, sections);
    game->players[game->currentPlayer]->points += 
            game->currentEight[inMessage[8] - '0']->points;
    printf("Player %c purchased %c using %s,%s,%s,%s,%s\n",
            game->currentPlayer + 65, inMessage[8], sections[1], sections[2],
            sections[3], sections[4], sections[5]);
    char* message = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    memset(message, '\0', MAX_MESSAGE_LENGTH);
//todo drawing cards if invalid Message
    char playerNum[3];
    sprintf(playerNum, "%c", game->currentPlayer + 65);
    strcpy(message, "purchased");
    strcat(message, playerNum);
    strcat(message, ":");
    message[strlen(message)] = inMessage[8]; /*Card number*/
    strcat(message, ":");             /*All the tokens used in purchase*/
    strcat(message, sections[1]);
    strcat(message, ",");
    strcat(message, sections[2]);
    strcat(message, ",");
    strcat(message, sections[3]);
    strcat(message, ",");
    strcat(message, sections[4]);
    strcat(message, ",");
    strcat(message, sections[5]);
    message[strlen(message)] = '\n';
    remove_and_update(game, (inMessage[8] - '0'));
    //game->cardsFaceUp--;
    if (game->invalidMessage) {
        return "!";
    }
    free_sections(sections);
    return message;
}

/**
 * Frees the array of char arrays referred to as sections
 */
void free_sections(char** sections) {
    for (int i = 0; i < MAX_SECTIONS - 1; i++) {
        free(sections[i]);
    }
    free(sections);
}


/**
 * Removes a card from the current Eight on table, and updates the current
 * eight with  the next card in pile.
 */
void remove_and_update(struct Game* game, int index) {
    for (int i = index + 1; index < CARDS_AT_A_TIME; index++) {
	game->currentEight[i - 1] = game->currentEight[i];
    }
    game->cardCounter++;
    if (game->gameDeck[game->cardCounter - 1]->cardNum == -1) {
        //game->cardCounter--;
    }
    game->cardsFaceUp--;
    game->currentEight[CARDS_AT_A_TIME - 1] = 
	    malloc(sizeof(struct Card*));
    game->currentEight[CARDS_AT_A_TIME - 1]->cardNum = -1;

}


/**
 * Checks validity of purchase message
 */
void check_purchase(char* inMessage, struct Game* game, char** sections) {
    int numSections = 6;
    /*We can skip the first 8 chars, since we know they are "purchase"*/
    if (!isdigit(inMessage[8]) || inMessage[9] != ':') {
        if (game->hasntChanged) {
            game->invalidMessage++;
            game->hasntChanged = 0;
        }
    }
    if (strlen(sections[0]) > 10) {
        if (game->hasntChanged) {
            game->invalidMessage++;
            game->hasntChanged = 0;
        }
    }
    for (int i = 1; i < numSections; i++) {
        if (sections[i][0] == '\0') { /*Checking for empty sections*/
            if (game->hasntChanged) {
                game->invalidMessage++;
                game->hasntChanged = 0;
            }
        }
        check_values_purchase(game, sections);
        for (int j = 0; j < strlen(sections[i]); j++) {
            if (!isdigit(sections[i][j])) {
                if (game->hasntChanged) {
                    game->invalidMessage++;
                    game->hasntChanged = 0;
                }
            }
        }
    }

}

/**
 * Checks to see if token values used in purchase are valid
 */
void check_values_purchase(struct Game* game, char** sections) {
    int numSections = 6;
    for (int i = 1; i < numSections; i++) {
        int val = atoi(sections[i]);
        if (val > game->players[game->currentPlayer]->tokens[i - 1] ||
                val < 0) {
            game->invalidMessage++;
            game->hasntChanged = 0;       
        }
    }
//todo check to see if card number is valid
}


/**
 * Splits a string by two delimiters given. Give same delimiter twice if you
 * only want to use one
 */
char** split_string(char* toSplit, char firstDelimiter, char secondDelimiter,
        struct Game* game) {
    int sectionCounter = 0;
    int indexInSection = 0;
    char** sections = malloc(sizeof(char*) * (MAX_SECTIONS - 1));
    for (int i = 0; i < MAX_SECTIONS - 1; i++) {
        sections[i] = malloc(sizeof(char) * MAX_LENGTH_OF_SECTION);
        memset(sections[i], '\0', MAX_LENGTH_OF_SECTION);
    }
    for (int i = 0; i < strlen(toSplit) - 1; i++) {
        if (sectionCounter >= MAX_SECTIONS - 1) {
            if (game->hasntChanged) {
                game->invalidMessage++;
                game->hasntChanged = 0;
            }
        }
        if (toSplit[i] == firstDelimiter || toSplit[i] == secondDelimiter) {
            sectionCounter++;
            indexInSection = 0;
            continue;
        }
        sections[sectionCounter][indexInSection] = toSplit[i];
        indexInSection++;
    }
    return sections;
}


/**
 * Makes the took message to broadcast
 */
char* take_command(char* inMessage, struct Game* game) {
    char* output = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    memset(output, '\0', MAX_MESSAGE_LENGTH);
    int* tokensTaken;
    if (strlen(inMessage) != 12) {
        printf("ONE\n");
        if (game->hasntChanged) {
            game->invalidMessage++;
            game->hasntChanged = 0;
        }
    }
    //split_string(inMessage);
    tokensTaken = process_take(inMessage, game, 
            split_string(inMessage, ',', ',', game));
    snprintf(output, MAX_MESSAGE_LENGTH, 
            "took%c:%d:%d:%d:%d\n", game->currentPlayer + 65,
            tokensTaken[0], tokensTaken[1], tokensTaken[2], tokensTaken[3]);
    if (game->invalidMessage) {
        return "!";
    }
    return output;
}


/**
 * Checks validity of a take command
 */
void check_take(struct Game* game, char** sections) {
    int count = 0;
    if (!isdigit(sections[0][4])) {
        printf("TWO\n");
        if (game->hasntChanged) {
            game->invalidMessage++;
            game->hasntChanged = 0;
        }
    }
    if (sections[0][4] - '0' != 1 && sections[0][4] - '0' != 0) {
        printf("THREE\n");
        if (game->hasntChanged) {
            game->invalidMessage++;
            game->hasntChanged = 0;
        }
    }
    count += sections[0][4] - '0';
    for (int i = 0; i < TOKENS; i++) {
        if (i > 0) {
            if (strlen(sections[i]) != 1) {
                printf("FOUR\n");
                if (game->hasntChanged) {
                    game->invalidMessage++;
                    game->hasntChanged = 0;//todo put allofthese into function
                }
            }
            if (sections[i][0] == '0') {
                continue;
            }
            if (sections[i][0] == '1') {
                count++;
                continue;
            }
            printf("FIVE\n");
            printf("sections[%d]: \"%s\"\n", i, sections[i]);
            if (game->hasntChanged) {
                game->invalidMessage++;
                game->hasntChanged = 0;
            }   
        }
    }
    if (count != 3) {
        printf("SIX\n");
        if (game->hasntChanged) {
            game->invalidMessage++;
            game->hasntChanged = 0;
        }
    }
}


/**
 * processes the "take" message from player
 */
int* process_take(char* inMessage, struct Game* game, char** sections) {
    //int charCounter = 4; /* Starts at index4 because the first4 are "take" */
    //int indexInSection = 0;
    int* tokensTaken = malloc(sizeof(int) * 4);
    check_take(game, sections);
    if (game->hasntChanged) {
        tokensTaken[0] = sections[0][4] - '0';
        game->nonWildTokens[0] -= sections[0][4] - '0';
        game->players[game->currentPlayer]->tokens[0] += sections[0][4] - '0';
        for (int i = 1; i < 4; i++) {
            tokensTaken[i] = atoi(sections[i]);
            game->nonWildTokens[i] -= atoi(sections[i]);
            game->players[game->currentPlayer]->tokens[i] += atoi(sections[i]);
        }
    }
    
    //todo deal with bad messages like so:
    if (!game->invalidMessage) {
        printf("Player %c drew %d,%d,%d,%d\n", game->currentPlayer + 65,
                tokensTaken[0], tokensTaken[1], tokensTaken[2], 
                tokensTaken[3]);
    }
    free_sections(sections);
    return tokensTaken;
}
        
