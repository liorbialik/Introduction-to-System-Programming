#include <stdio.h>


// enumerations for possible commands:
enum inputCommands { createAccountCmd, closeAccountCmd, printBalancesCmd, depositeCmd, withdrawalCmd };


// struct for a single account
typedef struct account {
	unsigned long int accountNumber;
	unsigned long long accountBalance;
	int accountBalanceSign;
	// another field for the account's mutex
	struct account *nextInList;
}account;


// struct that will be using the the runtimeLogFile
typedef struct logFile {
	FILE logFile;
	// another field for the logFile's mutex
}logFile;


// struct for all accounts
typedef struct allAccounts {
	account *accountList;
	unsigned long long totalNumberOfAccounts;
	logFile *runtimeLogFile;
};


int main(argc, argv) {

	// initialize a new allAccounts
	// open CommandFile 

	// read CommandFile line by line:
	// while (CommandFileLine != EOF){
	//     - parse CommandFileLine into a command
	//     - check what command is it (switch-case using the enums)
	//	   - execute the relevant command in a new thread:
	//     switch(command)
	//	      * createAccountCmd:
	//             # check that all other threads closed
	//             # execute createAccount()
	//             # check return value :
	//                    success => 
	//                    
	//	      * closeAccountCmd:
	//             # check that all other threads closed
	//             # execute closeAccount()
	//	      * printBalancesCmd:
	//             # check that all other threads closed
	//             # execute printBalances()
	//	      * depositeCmd:
	//             # execute deposite()
	//	      * withdrawalCmd:
	//             # execute withdrawal()
	// }

	// printBalanceReport(accountsList, runTimeLogFileName);
	// print into log file the final line
	// free all handlers and memory allocations

	return 0;
}