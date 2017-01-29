/*
SnakesAndLadders - ClientApp.h:
- the header file for ClientApp.c which contains declerations and structures of ClientApp
- this file contains the definitions, used structs and the decleration which used to call the main
function in SnakesAndLadders.exe
*/

#ifndef ClientApp_H
#define ClientApp_H

/* Constants: */
#define MAX_COMMAND_SIZE 120
#define MAX_USER_NAME 30
#define PLAYERS_CMD_SIZE 8
#define PLAY_CMD_SIZE 5
#define MAX_MESSAGE_SIZE 80
#define NUMBER_OF_THREADS 2
#define INVALID_CMD 999

#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )

/* Libraries: */
#include <Windows.h>
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'

/* Struct Declarations: */
enum inputCommands { playersCmd, messageCmd, broadcastCmd, playCmd};

/* Function Declarations: */
int MainClient(int argc, char *argv[]);

#endif#pragma once
#pragma once#pragma once
