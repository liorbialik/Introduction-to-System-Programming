// SnakesAndLadders - main.c
// This is the main function of the program.
// This file runs function that initializes the game board.
// Also executes relevant mode of application: Server / Client.

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'

#include <stdio.h>
#include <conio.h>
#include "ServerApp.h"
#include "ClientApp.h"
#define SERVER '1'
#define CLIENT '2'

//#include "ServerApp.h"
//#include "ClientApp.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int main(int argc, char *argv[]) {

	char *ApplicationMode = NULL, mode[1];
	int c = 0;
	ApplicationMode = argv[1];

	printf("please enter running mode: 1 for server, 2 for client\n");
	scanf("%c", mode);
	printf("%c\n", mode);
	if (mode == SERVER) {
		// Run Server application
		return MainServer(argc, argv);
		return 0;
	}

	if (mode == CLIENT) {
		// Run Client application
		return MainClient(argc, argv);
	}

	else {
		// Mode application in command line is incorrect
		printf("Application mode in command line is incorrect\nValid modes are server / client only\n");
		return 1;
	}


}
