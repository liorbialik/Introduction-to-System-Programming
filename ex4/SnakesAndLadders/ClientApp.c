// SnakesAndLadders - ClientApp.c
// This is the main thread of clientApp.
// The main rule of ClientApp is to play the game, and sending different types of messages to the server & other players.
// There can be up to 4 simultaneously available clients on the Network.
// The file also updates its own game board according to dice results of each drop.

#include <stdio.h>
#include <stdlib.h>

int MainClient(char *argv[]) {

	char *userName = NULL, *logFileName = NULL, *pieceChar = NULL;
	int serverPort;

	logFileName = argv[2];
	serverPort = atoi(argv[3]);
	userName = argv[4];

	// 1. connect to server:
	//		- success: print to log file: "Connected to server on port <serverPort>" and continue run
	//		- failure: print to log file: "Failed connecting to server on port <server port>" and exit
	// 2. send server the userName
	// 3. check for return msg:
	//		- success: print to log file: "“<userName> your game piece is <pieceChar>”" and continue run
	//		- failure: print to log file: "Connection to server refused. Exiting." and exit

	return 0;
}