#ifndef PROCESS_MOVE_H
#define PROCESS_MOVE_H

char* process_message(char* inMessage, struct Game* game);

char* slice_command(char* inMessage, struct Game* game);

char* take_command(char* inMessage, struct Game* game);

int* process_take(char* inMessage, struct Game* game, char** sections);

void free_sections(char** sections);


char** split_string(char* toSplit, char firstDelimiter, 
        char secondDelimiter, struct Game* game);

char* purchase_command(char* inMessage, struct Game* game);

void check_purchase(char* inMessage, struct Game* game, char** sections);

void check_values_purchase(struct Game* game, char** sections);


void draw_new_card(struct Game* game);


/**
 * Checks validity of take command
 */
void check_take(struct Game* game, char** sections);



void remove_and_update(struct Game* game, int index);
#endif //PROCESS_MOVE_H
