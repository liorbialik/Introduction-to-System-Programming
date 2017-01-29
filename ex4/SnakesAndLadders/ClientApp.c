// SnakesAndLadders - ClientApp.c
// This is the main thread of clientApp.
// The main rule of ClientApp is to play the game, and sending different types of messages to the server & other players.
// There can be up to 4 simultaneously available clients on the Network.
// The file also updates its own game board according to dice results of each drop.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include "ClientApp.h"

bool isCommandValid(char *command);
bool isMessageContainsValidChars(char *string);


HANDLE EngineDoneWithUserMessageSemaphore, IncomingMessageFromServerSemaphore, UserEnteredTextSemaphore;

int MainClient(int argc, char *argv[]) {

	char *userName = NULL, *logFileName = NULL, *pieceChar = NULL, userCommand[MAX_COMMAND_SIZE];
	int serverPort;
	HANDLE threadHandles[NUMBER_OF_THREADS] = { 0 }; /* An array of thread handles */

	// Verify that the number of command line argument is correct
	if (argc != 5) {
		printf("Number of Command line Arguments isn't compatible,  error %ul\n", GetLastError());
		exit(1);
	}

	logFileName = argv[2];
	serverPort = atoi(argv[3]);
	userName = argv[4];

	fgets(userCommand, MAX_COMMAND_SIZE, stdin);
	isCommandValid(userCommand);


	// 1. connect to server:
	//		- success: print to log file: "Connected to server on port <serverPort>" and continue run
	//		- failure: print to log file: "Failed connecting to server on port <server port>" and exit
	// 2. send server the userName
	// 3. check for return msg:
	//		- success: print to log file: "“<userName> your game piece is <pieceChar>”" and continue run
	//		- failure: print to log file: "Connection to server refused. Exiting." and exit
	// 4. get users command -> parse it -> validate command -> execut
	// 5. wait for "Your turn to play" message -> PlayersTurnEvent to signaled

	return 0;
}

bool isCommandValid(char *command){
	char *commandName = NULL, *userMessage = NULL, *userToMessage = NULL;
	char delimiter[] = " ";
	int i = 0, inputLength = 0, commandTypeIndex = INVALID_CMD;
	char *possibleCommandsArray[] = { "players\n" , "message" , "broadcast" , "play\n" };

	printf("Validating input command\n");

	inputLength = (int)strlen(command);
	if (inputLength > MAX_COMMAND_SIZE)
		return false;

	commandName = strtok(command, delimiter);

	for (i = 0; i < 4; i++) {
		if (STRINGS_ARE_EQUAL(commandName, possibleCommandsArray[i])) {
			commandTypeIndex = i;
			break;
		}
	}

	switch (commandTypeIndex)
	{
		case playersCmd:
			if (inputLength > PLAYERS_CMD_SIZE) // check if no additional input was written after the command
				return false;
			break;

		case messageCmd:
			userToMessage = strtok(NULL, delimiter);
			if (strlen(userToMessage) > MAX_USER_NAME)
				return false;
			userMessage = strtok(NULL, delimiter);
			if (strlen(userMessage) > MAX_MESSAGE_SIZE || !isMessageContainsValidChars(userMessage))
				return false;

			break;
		
		case broadcastCmd:
			userMessage = strtok(NULL, delimiter);
			if (strlen(userMessage) > MAX_MESSAGE_SIZE || !isMessageContainsValidChars(userMessage))
				return false;
			break;
		
		case playCmd:
			if (inputLength > PLAY_CMD_SIZE) // check if no additional input was written after the command
				return false; 
			break;

		case INVALID_CMD:
			return false;
	}
	return true;
}

bool isMessageContainsValidChars(char *string) {
	int i;

	for (i = 0; i < strlen(string); i++) {
		if ((string[i] >= 'a' && string[i] <= 'z') ||
			(string[i] >= 'A' && string[i] <= 'Z') ||
			(string[i] == ' ') ||
			(string[i] == '.') ||
			(string[i] == ',')) {
			continue;
		}

		if (string[i] == '\n')
			break;

		else {
			return false;
		}
	}

	return true;
}