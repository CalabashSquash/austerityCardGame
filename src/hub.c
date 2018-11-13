#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#include "gameAndPlayerData.h"
#include "hub.h"
#include "errorCheck.h"
#include "processMessage.h"

#define DECK_FILE_ERROR 3
#define MIN_DECK_LINE_SIZE 11
#define INPUT_NO 0
#define OUTPUT_NO 1

/*Global for error handling*/
struct Game* game;// = malloc(sizeof(struct Game));
pid_t pids[26];

//todo cards at a time can be less than 8.
int main(int argc, char** argv) {
    game = malloc(sizeof(struct Game));
    set_up_signal();
    error_checker(argc, argv, ""); 
    //struct Game* game = malloc(sizeof(struct Game));
    int* numberCards;
    int temp = 0;
    numberCards = &temp;
    game->gameDeck = read_deck(argv[3], numberCards);
    game->numOfCards = *numberCards;
    game_variables(game, argv, argc - 4);
    create_game(argc - 4, game, argv + 4);
    char* tokensMessage = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    memset(tokensMessage, '\0', MAX_MESSAGE_LENGTH);
    strcpy(tokensMessage, "tokens");
    strcat(tokensMessage, argv[1]);
    strcat(tokensMessage, "\n");
    message_all_players(game, tokensMessage);
    free(tokensMessage);
    play_game(game);
    for (int i = 0; i < game->numPlayers; i++) {
        kill(game->players[i]->id, SIGKILL);
    }
    exit_game(game);


    return 0;
}

/**
 * Sets up signal handlers
 */
void set_up_signal() {
    struct sigaction action;
    action.sa_handler = signal_handler;
    struct sigaction signalInt;
    signalInt.sa_handler = signal_handler;
    struct sigaction sigChild;
    sigChild.sa_handler = signal_handler;
    sigaction(SIGPIPE, &action, 0);
    sigaction(SIGINT, &signalInt, 0);
    sigaction(SIGCHLD, &sigChild, 0);
}


/**
 * Main signal handler
 */
void signal_handler(int sigNum) {
    /*For the life of me I couldn't figure out how to do any of this*/
    if (sigNum == SIGPIPE) {
        int* pids = malloc(sizeof(int) * game->numPlayers);
        if (game->firstIteration) {
            //todo kill child processes
            for (int i = 0; i < game->numPlayers; i++) {
                kill(game->players[i]->id, SIGKILL);
            }
            process_error(START_PLAYER_FAIL, "");
        }
        for (int i = 0; i < game->numPlayers; i++) {
            int status;
            if ((pids[i] = 
                    waitpid(game->players[i]->id, &status, WNOHANG))) {
            }
        }
    } else if (sigNum == SIGINT) {
        if (game->firstIteration) {
            process_error(SIGINT_RECEIVED, "");
        }
    }
    if (sigNum == SIGCHLD) {
        for (int i = 0; i < game->numPlayers; i++) {
            kill(game->players[i]->id, SIGKILL);
        }
        return;
    }
                
                
}   


/**
 * Main game-playing function. Loops over all players and asks for their move,
 * processes their move, repeat.
 */
void play_game(struct Game* game) {
    bool hasWinner = false;
    draw_cards(game);
    char* inMessage = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    char* outMessage;// = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    while (!hasWinner) {
        game->currentPlayer = -1;
        for (int i = 0; i < game->numPlayers; i++) {
            game->currentPlayer++;
            if (game->currentPlayer == game->numPlayers) {
                game->currentPlayer = 0;
            }
            memset(inMessage, '\0', MAX_MESSAGE_LENGTH);
            memset(outMessage, '\0', MAX_MESSAGE_LENGTH);
            fprintf(game->players[i]->childToRead, "dowhat\n");
            fflush(game->players[i]->childToRead);
            game->hasntChanged = 1;
            fgets(inMessage, MAX_MESSAGE_LENGTH, 
                    game->players[i]->childToWrite);
            outMessage = process_message(inMessage, game);
            if (!game->cardsFaceUp) {
                message_all_players(game, "eog\n");
                hasWinner = true;
                shut_down(game);
            }
            game->invalidMessage = 0;
            message_all_players(game, outMessage);
            if (!strncmp("purchased", outMessage, LEN_PURCHASED)) {
                draw_new_card(game);
                fflush(stdout);
            }
            if (game->invalidMessage == 1) {
                i--;
                game->currentPlayer--;
                continue;
            } else if (game->invalidMessage == 2) {
                exit_game(game);
                process_error(PLAYER_PROTOCOL, "");
            }
            free(outMessage);
        }
        if (check_winner(game)) {
            message_all_players(game, "eog\n");
            hasWinner = true;
            shut_down(game);
        }
        game->firstIteration = 0;
    }
    free(inMessage);
}

/**
 * Finds winners and prints message at end of game
 */
void find_winners(struct Game* game) {
    int lenOfWinners = 10; /* Length of "Winner(s) "*/
    int highestScore = 0;
    int best[game->numPlayers];
    char* winners = malloc(sizeof(char) * (game->numPlayers + lenOfWinners));
    memset(winners, '\0', game->numPlayers + lenOfWinners);
    strcpy(winners, "Winner(s) ");
    printf("Winner(s) ");
    for (int i = 0; i < game->numPlayers; i++) {
        best[i] = 0;
        if (game->players[i]->points > highestScore) {
            for (int i = 0; i < game->numPlayers; i++) {
                best[i] = 0;
            }
            highestScore = game->players[i]->points;
            best[i] = 1;
        } else if (game->players[i]->points == highestScore) {
            best[i] = 1;
        }
    }
    int first = 1;
    for (int i = 0; i < game->numPlayers; i++) {
        if (best[i]) {
            if (!first) {
                printf(",");
            }
            printf("%c", i + 65);
            first = 0;
        }

    }
    free(winners);
    printf("\n");
}


void shut_down(struct Game* game) {
    find_winners(game);
    time_t startT, endT;
    time(&startT);
    while(1) {
        time(&endT);
        if (difftime(endT, startT) > 2) {
            break;
        }
    }
    int status;
    for (int i = 0; i < game->numPlayers; i++) {
        if (!waitpid(game->players[i]->id, &status, WNOHANG)) {
            kill(game->players[i]->id, SIGKILL);
            printf("Player %d shutdown after receiving signal %d\n", i, 
                    SIGKILL);
        } else if (WEXITSTATUS(status)) {
            printf("Player %d ended with status %d\n", i, WEXITSTATUS(status));
        }
    }
}


/**
 * Returns non-zero if there is a winner
 */
int check_winner(struct Game* game) {
    for (int i = 0; i < game->numPlayers; i++) {
        if (game->players[i]->points >= game->pointsRequired) {
            return 1;
        }
    }
    return 0;
}

/**
 * Draws eight cards for start of game
 */
void draw_cards(struct Game* game) {
    for (int i = 0; i < CARDS_AT_A_TIME; i++) {
        if (game->gameDeck[game->cardCounter]->cardNum == -1) {
            break;
        }
        game->cardsFaceUp++;
	game->currentEight[i] = game->gameDeck[game->cardCounter];
	game->cardCounter++;
	char message[MAX_MESSAGE_LENGTH];
	memset(message, '\0', MAX_MESSAGE_LENGTH);
        printf("New card = Bonus %c, worth %d, costs %d,%d,%d,%d\n",
                game->currentEight[i]->colourDiscount,
                game->currentEight[i]->points,
                game->currentEight[i]->price[0],
                game->currentEight[i]->price[1],
                game->currentEight[i]->price[2],
                game->currentEight[i]->price[3]);

	snprintf(message, MAX_MESSAGE_LENGTH, "newcard%c:%d,%d,%d,%d,%d\n", 
		game->currentEight[i]->colourDiscount, 
		game->currentEight[i]->points,
		game->currentEight[i]->price[0],
		game->currentEight[i]->price[1],
		game->currentEight[i]->price[2],
		game->currentEight[i]->price[3]);
	message_all_players(game, message); 
    }
    game->cardCounter--;
}

/**
 * Frees all of the memory allocated to the hub.
 */
void exit_game(struct Game* game) {

    free(game->in);
    free(game->out);
    for (int i = 0; i < game->numPlayers; i++) {
	fclose(game->players[i]->childToWrite);
	//fclose(game->players[i]->childToRead); This causes game to segfault
	//printf("Hii\n");
	free(game->players[i]);
    }
    free(game->players);
    for (int i = 0; i < game->numOfCards; i++) {
/*
	if(i < CARDS_AT_A_TIME) {
	    free(game->currentEight[i]->price);
	    free(game->currentEight[i]);
	}
*/
	free(game->gameDeck[i]->price);
	free(game->gameDeck[i]);
    }
    free(game->currentEight);
    free(game->gameDeck); 

    free(game);
    //free(game->players);
}    
     
/*
 * Sets up variables in the player struct
 */
struct Player* setup_player(char* player, int playerNum, struct Game* game) {
    struct Player* newPlayer = malloc(sizeof(struct Player));
    newPlayer->playerLetter = 'A' + playerNum;
    newPlayer->playerNum = playerNum;
    newPlayer->type = player;
    newPlayer->points = 0;

    for (int i = 0; i < 5; i++) {
	if (i < 4) {
	    newPlayer->discounts[i] = 0;
	}
	newPlayer->tokens[i] = 0;
    }
    return newPlayer;
}

/**
 * Creates a child process that execs the given player process.
 */
void create_child(struct Game* game, int playerNum, int* in, int* out) {
    pid_t pid;
    if ((pid = fork()) < 0) {
	//todo error for failed fork.
    } else if (pid == 0) {

	char* playerCount = malloc(sizeof(char) * 3);
	char* numOfPlayer = malloc(sizeof(char) * 3);
	memset(playerCount, '\0', 3);
	memset(numOfPlayer, '\0', 3);
	sprintf(playerCount, "%d", game->numPlayers);
	sprintf(numOfPlayer, "%d", playerNum);
	game->players[playerNum]->id = getpid();
        //if(playerNum) {
        freopen("/dev/null", "w", stderr);
        fclose(stderr);
        //}
	//child's input. Change input FD and close output on pipe.
	if (dup2(in[INPUT_NO], STDIN_FILENO) == -1 
		|| close(in[OUTPUT_NO]) == -1) {
	    exit(0);
	}
	//child's output. Change output FD and close input on pipe.
	if (dup2(out[OUTPUT_NO], STDOUT_FILENO) == -1 
		|| close(out[INPUT_NO]) == -1) {
	    exit(0);
	}
	if (close(in[INPUT_NO]) == -1 || close(out[OUTPUT_NO]) == -1) {
	    exit(0);
        }
        if(execl(game->players[playerNum]->type, 
                game->players[playerNum]->type,
		playerCount, numOfPlayer, (char*)NULL) == -1) {
            //exit(START_PLAYER_FAIL);
            //kill_and_end(game, START_PLAYER_FAIL);
            //kill(0, SIGKILL);
            exit(1); 
        }
    } else {
        game->players[playerNum]->id = pid;
        pids[playerNum] = pid;
    }
}

/**
 * Sets up variables in game struct
 */
void game_variables(struct Game* game, char** argv, int playerCount) {
    game->firstIteration = 1;
    game->numPlayers = playerCount;
    game->invalidMessage = 0;
    game->cardsFaceUp = 0;
    game->players = malloc(sizeof(game->players) * playerCount);
    game->in = malloc(sizeof(int) * (playerCount * 2));
    game->out = malloc(sizeof(int) * (playerCount * 2));
    game->cardCounter = 0;
    for (int i = 0; i < 4; i++) {
	game->nonWildTokens[i] = atoi(argv[1]);
    }
    game->currentEight = malloc(sizeof(struct Card*) * CARDS_AT_A_TIME);
    game->pointsRequired = atoi(argv[2]);
}
    
/*
 * Sets up players (aka children processes) in the game struct
 */
void create_game(int playerCount, struct Game* game, char** player) {
    for (int i = 0; i < playerCount; i++) {
	pipe(&(game->in[2 * i]));
	pipe(&(game->out[2 * i]));
    }
    for (int i = 0; i < playerCount; i++) {	
	struct Player* interPlayer;
	interPlayer = setup_player(player[i], i, game);
	game->players[i] = interPlayer;

        //int retVal = fcntl(game->clo[2 * i], 
        //        F_SETFL, fcntl(game->clo[2 * i], F_GETFL) | O_NONBLOCK);    


	create_child(game, i, &(game->in[2 * i]), &(game->out[2 * i]));
        close(game->out[2 * i + OUTPUT_NO]);
	close(game->in[2 * i + INPUT_NO]);
	game->players[i]->childToRead = 
		fdopen(game->in[2 * i + OUTPUT_NO], "w");

	game->players[i]->childToWrite = 
		fdopen(game->out[2 * i + INPUT_NO], "r");

    }
    //freopen("/dev/null", "w", stdout);
    //fclose(stdout);
}

/**
 * Messages all players in game
 * The string passed in must be newline terminated
 */
void message_all_players(struct Game* game, char* message) {
    for (int i = 0; i < game->numPlayers; i++) {
	fprintf(game->players[i]->childToRead, "%s", message);
        fflush(game->players[i]->childToRead);
    }
}

/*
 * Parses one line in the deck file
 */
void parse_line(char* line, struct Card* current) {
    int columnInNew = 0;
    int charCount = 0;
    int yowza;
    int lengthOfLine = strlen(line);
    current->price = malloc(sizeof(int) * PRICES_NO);
    for (int i = 0; i < 6; i++) {
	char* section = malloc(sizeof(char) * charCount + 1);
	memset(section, '\0', charCount + 1);
	while (1) {
	    if (columnInNew == lengthOfLine) {
		current->price[3] = atoi(section);
		free(section);
		break;
	    }

	    if (line[columnInNew] == ':' || line[columnInNew] == ',') {
                if (i == 0) {
		    current->colourDiscount = section[0];
                }
                if (i == 1) {
		    yowza = atoi(section);
		    current->points = yowza;
                }
                if (i > 1 && i < 6) {
                    current->price[i - 2] = atoi(section);
                }
		free(section);
		charCount = 0;
		columnInNew++;
		break;
	    }
	    char* tempCopy = malloc(sizeof(char) * (charCount + 2));
	    memset(tempCopy, '\0', charCount + 2);
	    strcpy(tempCopy, section);
	    free(section);
	    section = malloc(sizeof(char) * (charCount + 2));
	    memset(section, '\0', charCount + 2);
	    strcpy(section, tempCopy);
	    free(tempCopy);
	    check_char(i, line[columnInNew]);
	    section[charCount] = line[columnInNew];
	    charCount++;
	    columnInNew++;
	}
    }
}

/**
 * Checks a char in deck file, based on what index it is in the line.
 */
void check_char(int i, char toBeChecked) {
    if (i == 0) {
        if (toBeChecked != 'P' && toBeChecked != 'B' && toBeChecked != 'Y'
	        && toBeChecked != 'R') {
	    process_error(INVALID_DECK_FILE, "");
	}
    } else if (toBeChecked < '0' || toBeChecked > '9') {
    	process_error(INVALID_DECK_FILE, "");
    } 

}


/**
 *
 */
void done_reading(char* currentLine, FILE* deckFile, int* numberCards,
        int cardCount, struct Card** cards) {
    free(currentLine);
    fclose(deckFile);
    *numberCards = cardCount;
    cards = realloc(cards, sizeof(struct Card*) * cardCount + 2);
    cards[cardCount] = malloc(sizeof(struct Card));
    cards[cardCount]->cardNum = -1;
}

/*
 * Reads the deck file
 * Creates linked list that links each card together, with the first card
 * as the card in Deck struct.
 * Sorts data into relevant variables within each card.
 * Took way to long to get right.
 */
struct Card** read_deck(char* fileName, int* numberCards) {
    struct Card** cards = malloc(sizeof(struct Card*));
    int c;
    int charCount = 0;
    int cardCount = 0;
    char* currentLine = malloc(sizeof(char) * (MIN_DECK_LINE_SIZE + 1));
    memset(currentLine, '\0', MIN_DECK_LINE_SIZE + 1);
    FILE* deckFile = fopen(fileName, "r");
    if (deckFile == NULL) {
	process_error(NO_ACCESS_DECK_FILE, "");
    }
    while(1) {
    	c = fgetc(deckFile);
	if (feof(deckFile)) {
            if (cardCount == 0) {
                process_error(INVALID_DECK_FILE, "");
            }
            done_reading(currentLine, deckFile, numberCards, cardCount,
                    cards);
	    return cards;
	}
	cards = realloc(cards, sizeof(struct Card*) * (cardCount + 2));
	cards[cardCount] = malloc(sizeof(struct Card));
	cards[cardCount]->cardNum = cardCount;
	if (c == '\n') {
	    currentLine[charCount] = '\0';
	    parse_line(currentLine, cards[cardCount]);
	    charCount = 0;
	    cardCount++;
	    free(currentLine);
	    currentLine = malloc(sizeof(char) * MIN_DECK_LINE_SIZE + 1);
	    memset(currentLine, '\0', MIN_DECK_LINE_SIZE + 1);
	    continue;
	}
	if (charCount > MIN_DECK_LINE_SIZE) {
            char* tempCopy = malloc(sizeof(char) * (charCount + 1));
            memset(tempCopy, '\0', charCount + 1);
            strncpy(tempCopy, currentLine, sizeof(char) * charCount);
            free(currentLine);
            currentLine = malloc(sizeof(char) * (charCount + 1));
            memset(currentLine, '\0', charCount + 1);
            tempCopy = realloc(tempCopy, sizeof(char) * (charCount + 1));
            strcpy(currentLine, tempCopy);
            free(tempCopy);
	}
	currentLine[charCount++] = c;  
    }
}   

/**
 * Attempt to kill failed processes
 */
void kill_and_end(struct Game* game, int errorType) {
    for (int i = 0; i < game->numPlayers - 1; i++) {
        kill(game->players[i]->id, SIGKILL);
    }
    kill(getppid(), SIGKILL);
    process_error(errorType, "");
}
