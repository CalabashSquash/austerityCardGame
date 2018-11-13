#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv) {
    printf("wahuuu\n");
    fflush(stdout);
    char* input;// = malloc(sizeof(char) * 70);;
    usleep(500);
    scanf("%s", input);
    usleep(500);
    printf("input from within player: %s", input);
    fflush(stdout); 
    return 0;
}
