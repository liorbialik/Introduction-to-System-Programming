

#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <Strsafe.h>
#include <tchar.h>

char *getFileName(char *path);
char *getFileExtention(char *filename);
char *getOutputFileName(char *fileName, char *outputFileNameEnding);
BOOL GetFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString);

int main(int argc, char *argv[]) {
	char *filePath = NULL, *outputLogFilePath = NULL, *outputLogFileName = NULL, *fileName = NULL, *fileExtention = NULL,
		*outputFileNameEnding = { "_log.txt" }, firstFiveCharsFromFile[6];
	FILE *inputFile = NULL;
	HANDLE fileHandler;
	DWORD fileSize = 0; 
	LPTSTR *creationTimeString = NULL, *lastAccessedTimeString = NULL;
	FILETIME fileCreationTime, fileLastModifiedTime;
	TCHAR fileCreationTimeString[21], fileLastModifiedTimeString[21];

	filePath = argv[1]; 
	outputLogFilePath = argv[2];
	fileName = getFileName(filePath);
	fileExtention = getFileExtention(fileName);
	outputLogFileName = getOutputFileName(fileName, outputFileNameEnding);

	inputFile = fopen(fileName, "r");
	if ( inputFile == NULL ) {
		printf("Could not open file, error %ul\n", GetLastError());
		exit( 1 );
	}


	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	if ( fileHandler == INVALID_HANDLE_VALUE ) {
		printf("Could not open file, error %ul\n", GetLastError());
		exit( 1 );
	}

	// getting file's size:
	fileSize = GetFileSize( fileHandler, NULL );

	printf("%s size is %d bytes\n", fileName, fileSize); // REMOVE - just for testing
	
	//getting file's creating time and last accessed time
	GetFileTime( fileHandler, &fileCreationTime, NULL, &fileLastModifiedTime );
	
	if( GetFileTimeString(fileCreationTime, fileCreationTimeString) );
		_tprintf(TEXT("file creation time is: %s\n"), fileCreationTimeString); // REMOVE - just for testing

	if( GetFileTimeString(fileLastModifiedTime, fileLastModifiedTimeString) );
		_tprintf(TEXT("file last accessed time is: %s\n"), fileLastModifiedTimeString); // REMOVE - just for testing

	// getting first five chars from file
	fgets(firstFiveCharsFromFile, 5, inputFile);
	printf("firstFiveCharsFromFile: %s\n", firstFiveCharsFromFile);

	getchar();
	free( outputLogFileName );
	return 0;
}




BOOL GetFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString){
	/* 
	inspired by https://msdn.microsoft.com/en-us/library/windows/desktop/ms724926(v=vs.85).aspx 
	*/

	DWORD timeInStringFormatSize = 21;
	SYSTEMTIME utcTime, localTime;
	DWORD timeToStringFormatResult;

	// Convert the last-write time to local time.
	FileTimeToSystemTime( &fileCreationTime, &utcTime );
	SystemTimeToTzSpecificLocalTime( NULL, &utcTime, &localTime );

	// Build a string showing the date and time.
	timeToStringFormatResult = StringCchPrintf( bufferForString, timeInStringFormatSize,
		TEXT("%02d/%02d/%04d  %02d:%02d:%02d"),
		localTime.wMonth, localTime.wDay, localTime.wYear,
		localTime.wHour, localTime.wMinute, localTime.wSecond );

	if (timeToStringFormatResult == S_OK )
		return TRUE;
	else return FALSE;
}


char *getFileName(char *path){
	/*
	inspired by http://stackoverflow.com/questions/5901624/extract-file-name-from-full-path-in-c-using-msvs2005
	*/

	char *filename = strrchr( path, '\\' );
	if ( filename == NULL )
		filename = path;
	else
		filename++;
	return filename;
}


char *getOutputFileName(char *fileName, char *outputFileNameEnding) {
	/*
	inspired by our program from EX1
	*/
	
	size_t outputFileNameLength = 0;
	char *outputFileName = NULL;

	outputFileNameLength = strlen(fileName) + strlen(outputFileNameEnding) - 4 + 1; // '-4' deducts the '.txt' ending since it appearse twice. '+1' accounts for the '\0' ending
	outputFileName = malloc(sizeof(char) * outputFileNameLength); // the '4' stands for the shared ".txt" 
	if (outputFileName == NULL) {
		printf("outputFileName allocation faild.");
		exit(1);
	}
	strcpy(outputFileName, fileName);
	outputFileName[strlen(outputFileName) - 4] = '\0'; // mark the end of the precious string before the '.txt' ending
	strcat(outputFileName, outputFileNameEnding); // add the correct ending
	return outputFileName;
}


char *getFileExtention(char *filename) {
	/*
	inspired by http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
	*/
	
	Sleep(10);
	char *dotLocationInFileName = strrchr(filename, '.');
	if (!dotLocationInFileName || dotLocationInFileName == filename)
		return "No";
	return dotLocationInFileName + 1;
}



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
