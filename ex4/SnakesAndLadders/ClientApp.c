// SnakesAndLadders - ClientApp.c
// This is the main thread of clientApp.
// The main rule of ClientApp is to play the game, and sending different types of messages to the server & other players.
// There can be up to 4 simultaneously available clients on the Network.
// The file also updates its own game board according to dice results of each drop.

#include <stdio.h>
#include <stdlib.h>

int MainClient(char *argv[]) {

	char *userName = NULL, *logFileName = NULL;
	int clientPort;

	logFileName = argv[2];
	clientPort = atoi(argv[3]);
	userName = argv[4];

	return 0;
}