# Austerity Card Game
A multi-process command line card game. Computer Processes verse eachother by communicating with the hub process.

### Skills Tested
- C Programming
- Inter-Process-Communication & multi-processing
- Advanced FILE I/O
- Shell scripting
### Getting Started
##### Prerequisites
To run this program on a unix system, you must have gcc (or some other valid compiler). If you are using a compiler other than gcc, you will need to set up the compilation process accordingly.
##### Installing/Compiling
The Makefile in /src outlines which files should be compiled then linked to other object files. Running the "make" command should be valid for most unix systems, provided gcc is installed. This project was made on Ubuntu and compiles with the provided makefile on that.
##### Outline of File Structure
![Image of file structure](https://i.imgur.com/IKX048a.png)
### Game Walkthrough/Instructions
Austerity is a game in which computer "algorithms" verse eachother  Therefore, you, as a human, cannot interact with gameplay.
##### The hub process (austerity) runs as follows:
1. Begin reading the deck file.
    Each card in the deck file has the following features:
        - Cost of each colour token to purchase (number of each colour is always a non-negative integer)
        - Number of points the card is worth (always a non-negative integer)
        - Colour, used for discounts on all future card purchases
2. After processing the deck file, set up other details about the game, including:
    - Amount of tokens of each colour in their piles
    - The amount of points required to win the game
3. Once the game's starting information has been processed, begin excecuting the child processes (i.e. the players) based on the argument variables given.
4. broadcast the game information to all the players.
5. Manage the game, requesting one players turn, checking and processing it, broadcasting the move, then prompting the next player for their turn.
6. Austerity repeats step 5 until a game is over:
    - A game ends when there are either no cards left or a player has reached the point threshold.
##### The child processes (shenzai, banzai & ed) run as follows:
1. Once the austerity process executes a given player, wait for a message from austerity.
2. The first messages received should be game information.
3. The following messages will be either a "dowhat" message, or a message detailing the details of another player's move.
4. a) In the case of a "dowhat" message, make a decision based off its algorithm **(described below)**
4. b) Otherwise, use austerity's message to update the game state, which is used to determine the future decisions of the player
5. Repeat steps 3-4 until an "eog" (end of game) message is received.
**Usage: austerity tokens points deck player player [player ...]**
### Authors
Benjamin Patch (myself)








