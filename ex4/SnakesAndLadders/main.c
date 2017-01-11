// SnakesAndLadders - main.c
// This is the main function of the program.
// This file runs function that initializes the game board.
// Also executes relevant mode of application: Server / Client.

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#include <stdio.h>
#include <conio.h>

//#include "ServerApp.h"
//#include "ClientApp.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int main(int argc, char *argv[]) {

	char *ApplicationMode = NULL;

	ApplicationMode = argv[1];

	if (ApplicationMode == "server") {
		// Run Server application
		return MainServer(argc, argv);
	}

	else if (ApplicationMode == "client") {
		// Run Client application
		return MainClient(argc, argv);
	}

	else {
		// Mode application in command line is incorrect
		printf("Mode application in command line is incorrect\nValid modes are 'server' \ 'client' only");
		return 1;
	}


}
