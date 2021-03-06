// SnakesAndLadders - ServerApp.c
// This is the main thread of ServerApp.
// This file manages the game & chats.
// The main rule of ServerApp is to control all client applications that wants to connect the server in the network.
// It interconnects between clients by passing private and\or broadcast messages.

/* Constants */
#define NUM_OF_WORKER_THREADS 2
#define NUM_OF_CLIENTS 4
#define MAX_LOOPS 3
#define SEND_STR_SIZE 35

/* Libraries: */
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "ServerApp.h"
#include "SocketCommonUtils.h"
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#define _CRT_SECURE_NO_WARNINGS
//#include "SocketSendRecvTools.h"

/* Declerations */
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
static DWORD ServiceThread(SOCKET *t_socket);

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

	char *ServerPort = argv[3];
	int Ind;
	int Loop;
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;

	// Initialize Winsock.
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return 1;
	}

	/* The WinSock DLL is acceptable. Proceed. */

	// Create a socket for server  
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		goto server_cleanup_1;
	}

	// Bind the socket.
	/*
	For a server to accept client connections, it must be bound to a network address within the system.
	The following code demonstrates how to bind a socket that has already been created to an IP address
	and port.
	Client applications use the IP address and port to connect to the host network.
	The sockaddr structure holds information regarding the address family, IP address, and port number.
	sockaddr_in is a subset of sockaddr and is used for IP version 4 applications.
	*/
	// Create a sockaddr_in object and set its values.
	// Declare variables

	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			SERVER_ADDRESS_STR);
		goto server_cleanup_2;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = Address;
	service.sin_port = htons((u_short)ServerPort); //The htons function converts a u_short from host to TCP/IP network byte order 
										   //( which is big-endian ).
										   /*
										   The three lines following the declaration of sockaddr_in service are used to set up
										   the sockaddr structure:
										   AF_INET is the Internet address family.
										   "127.0.0.1" is the local IP address to which the socket will be bound.
										   ServerPort is a variable which determines according to argv[3]. it is the port number to which the socket will be bound.
										   */

										   // Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
										   // Check for general errors.
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Listen on the Socket.
	ListenRes = listen(MainSocket, NUM_OF_CLIENTS);
	if (ListenRes == SOCKET_ERROR)
	{
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;

	printf("Waiting for a client to connect...\n");

	for (Loop = 0; Loop < MAX_LOOPS; Loop++)
	{
		SOCKET AcceptSocket = accept(MainSocket, NULL, NULL);
		if (AcceptSocket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			goto server_cleanup_3;
		}

		printf("Client Connected.\n");

		Ind = FindFirstUnusedThreadSlot();

		if (Ind == NUM_OF_WORKER_THREADS) //no slot is available
		{
			printf("No slots available for client, dropping the connection.\n");
			closesocket(AcceptSocket); //Closing the socket, dropping the connection.
		}
		else
		{
			ThreadInputs[Ind] = AcceptSocket; // shallow copy: don't close 
											  // AcceptSocket, instead close 
											  // ThreadInputs[Ind] when the
											  // time comes.
			ThreadHandles[Ind] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ServiceThread,
				&(ThreadInputs[Ind]),
				0,
				NULL
			);
		}
	} 

server_cleanup_3:
	CleanupWorkerThreads();

server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());

	return 0;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static int FindFirstUnusedThreadSlot()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] == NULL)
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], 0);

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static void CleanupWorkerThreads()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] != NULL)
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], INFINITE);

			if (Res == WAIT_OBJECT_0)
			{
				closesocket(ThreadInputs[Ind]);
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
			else
			{
				printf("Waiting for thread failed. Ending program\n");
				return;
			}
		}
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Service thread is the thread that opens for each successful client connection and "talks" to the client.
static DWORD ServiceThread(SOCKET *t_socket)
{
	char SendStr[SEND_STR_SIZE];

	BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;

	strcpy(SendStr, "Welcome to this server!");

	SendRes = SendString(SendStr, *t_socket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(*t_socket);
		return 1;
	}

	while (!Done)
	{
		char *AcceptedStr = NULL;

		RecvRes = ReceiveString(&AcceptedStr, *t_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Service socket error while reading, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Connection closed while reading, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}
		else
		{
			printf("Got string : %s\n", AcceptedStr);
		}

		//After reading a single line, checking to see what to do with it
		//If got "hello" send back "what's up?"
		//If got "how are you?" send back "great"
		//If got "bye" send back "see ya!" and then end the thread
		//Otherwise, send "I don't understand"

		if (STRINGS_ARE_EQUAL(AcceptedStr, "hello"))
		{
			strcpy(SendStr, "what's up?");
		}
		else if (STRINGS_ARE_EQUAL(AcceptedStr, "how are you?"))
		{
			strcpy(SendStr, "great");
		}
		else if (STRINGS_ARE_EQUAL(AcceptedStr, "bye"))
		{
			strcpy(SendStr, "see ya!");
			Done = TRUE;
		}
		else
		{
			strcpy(SendStr, "I don't understand");
		}

		SendRes = SendString(SendStr, *t_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(*t_socket);
			return 1;
		}

		free(AcceptedStr);
	}

	printf("Conversation ended.\n");
	closesocket(*t_socket);
	return 0;
}
