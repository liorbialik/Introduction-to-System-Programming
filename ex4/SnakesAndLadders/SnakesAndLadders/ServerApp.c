// SnakesAndLadders - ServerApp.c
// This is the main thread of ServerApp.
// This file manages the game & chats.
// The main rule of ServerApp is to control all client applications that wants to connect the server in the network.
// It interconnects between clients by passing private and\or broadcast messages.

/* Libraries: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <tchar.h>
#include <Strsafe.h>
#include <conio.h>
#include <process.h>
#include <sys/stat.h>
//#include ""
#include <sys/types.h>
#include <direct.h>

#pragma warning( disable: 4995 ) // disabling the deprecation warnings of 'strcpy' and 'sprintf'

// WorkFlow Server.c:
// 1. Initialize Winsock using WSAStartup() function.
// 2. Creating a SOCKET for listening to incoming connection requests, ie a server socket.
// 3. Define sockAddr sturcture and bind the SOCKET to server.
// 4. Listen for incoming connection requests on the created socket using listen() function.
//	  The number of Q of pending connections are 4.
// 5. In this level we should limit the waiting connection for 1 minute using a timer.
//	  If we didn't identify any accept of a client, ie there is zero players - The game will be closed.
// 6. In a loop, identify that there is a connection attemp.
//	  First, lock a mutex called BroadcastMutex to protect from recieving message while hanling the connection.
//	  Second, validate whether the new client can join the network and according to spec.
//    Third, if new connection succeeded, accept its SocketId, its username and insert the new socket into AcceptSocketArray.
//    Fourth, retrieve the new client its game piece. And retrieve all connected clients its symbol.
//    Sixth, free BroadcastMutex.

// 7. 

int MainServer(int argc, char *argv[]) {


	return 0;
}