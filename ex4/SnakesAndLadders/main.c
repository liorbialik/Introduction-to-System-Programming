// SnakesAndLadders - main.c
// This is the main function of the program.
// This file runs function that initializes the game board.
// Also executes relevant mode of application: Server / Client.

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "ServerApp.h"
#include "ClientApp.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int main(int argc, char *argv[]) {

	char *ApplicationMode = argv[1];

	if (ApplicationMode == "server") {
		// Run Server application
		printf("Running in server mode\n");
		return MainServer(argc, argv);
	}

	else if (ApplicationMode == "client") {
		// Run Client application
		printf("Running in client mode\n");
		return MainClient(argv);
	}

	else {
		// Mode application in command line is incorrect
		printf("Mode application in command line is incorrect\nValid modes are server / client only\n");
		return 1;
	}

}
