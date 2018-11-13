CFLAGS = -Wall -pedantic -std=gnu99 -g
.PHONY: debug

.DEFAULT: all
all: hub.o hubErrorCheck.o playerDowhat.o processMessage.o shenzi.o banzai.o sharedPlayer.o ed.o checkMessagePlayer.o playerErrorCheck.o ed shenzi banzai austerity 

debug: CFLAGS += -g

clean:
	rm -f test

hub.o: hub.c
	gcc $(CFLAGS) -c hub.c -o hub.o

hubErrorCheck.o: hubErrorCheck.c
	gcc $(CFLAGS) -c hubErrorCheck.c -o hubErrorCheck.o

sharedPlayer.o: sharedPlayer.c
	gcc $(CFLAGS) -c sharedPlayer.c -o sharedPlayer.o

processMessage.o: processMessage.c
	gcc $(CFLAGS) -c processMessage.c -o processMessage.o

playerDowhat.o: playerDowhat.c
	gcc $(CFLAGS) -c playerDowhat.c -o playerDowhat.o

checkMessagePlayer.o: checkMessagePlayer.c
	gcc $(CFLAGS) -c checkMessagePlayer.c -o checkMessagePlayer.o

shenzi.o: shenzi.c
	gcc $(CFLAGS) -c shenzi.c -o shenzi.o

banzai.o: banzai.c
	gcc $(CFLAGS) -c banzai.c -o banzai.o

ed.o: ed.c
	gcc $(CFLAGS) -c ed.c -o ed.o

playerErrorCheck.o: playerErrorCheck.c
	gcc $(CFLAGS) -c playerErrorCheck.c -o playerErrorCheck.o

banzai: sharedPlayer.o banzai.o playerErrorCheck.o checkMessagePlayer.o playerDowhat.o
	gcc $(CFLAGS) -o banzai sharedPlayer.o banzai.o playerErrorCheck.o checkMessagePlayer.o playerDowhat.o

ed: sharedPlayer.o ed.o playerErrorCheck.o checkMessagePlayer.o playerDowhat.o
	gcc $(CFLAGS) -o ed sharedPlayer.o ed.o playerErrorCheck.o checkMessagePlayer.o playerDowhat.o

shenzi: sharedPlayer.o shenzi.o playerErrorCheck.o checkMessagePlayer.o playerDowhat.o
	gcc $(CFLAGS) -o shenzi sharedPlayer.o shenzi.o playerErrorCheck.o checkMessagePlayer.o playerDowhat.o

austerity: hubErrorCheck.o hub.o processMessage.o
	gcc $(CFLAGS) -o austerity hub.o hubErrorCheck.o processMessage.o
