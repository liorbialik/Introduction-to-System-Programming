/*
plan for testFile.exe program
*/

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char *filePath = NULL, *outputLogFilePath = NULL;
	HANDLE fileHandler;
	FILETIME fileCreationTime;
	SYSTEMTIME stUTC, stLocal;
	
	filePath = argv[1];
	outputLogFilePath = argv[2];

	fileHandler = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandler == INVALID_HANDLE_VALUE)
	{
		printf("Could not open file, error %ul\n", GetLastError());
		return -1;
	}

}

// Test 1:
char *checkFileNameEnding(char *filename){
	Sleep(10);
	char *dot = NULL;
	*dot = strrchr(filename, '.');
	if (!dot || dot == filename) 
		return "";
	return dot + 1;
}


// Test 2:
// char* checkFileSize(){
// Sleep(10)
// ...............
// return fileSize
// }


// Test 3:
// char* checkFileCreatingTime(){
// Sleep(10)
// ...............
// return fileCreatingTime
// }


// char* checkFileLastModifiedTime(){
// Sleep(10)
// ...............
// return fileLastModifiedTime
// }


// Test 4:
// char* getFirstFiveCharsFromFile(){
// Sleep(10)
// ...............
// return filesFirstFiveChars
// }
