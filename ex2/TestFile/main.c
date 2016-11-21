

#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

char *getFileName(char *path);
char *getFileExtention(char *filename);
char *getOutputFileName(char *fileName, char *outputFileNameEnding);

int main(int argc, char *argv[]) {
	char *filePath = NULL, *outputLogFilePath = NULL, *outputLogFileName = NULL, *fileName = NULL, *fileExtention = NULL, *outputFileNameEnding = { "_log.txt" };
	FILE *inputFile = NULL;
	HANDLE fileHandler;
	DWORD fileSize = 0;
//	FILETIME fileCreationTime;
//	SYSTEMTIME stUTC, stLocal;
//	
	filePath = argv[1]; 
	outputLogFilePath = argv[2];
	fileName = getFileName(filePath);
	fileExtention = getFileExtention(fileName);
	outputLogFileName = getOutputFileName(fileName, outputFileNameEnding);

	inputFile = fopen(fileName, "r");
	if (inputFile == NULL) {
		printf("Input File openning failed!");
		exit(1);
	}

	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	fileSize = GetFileSize(fileHandler, NULL);

	printf("%s size is %d bytes and file type is %d\n", fileName, fileSize);

//	fileHandler = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (fileHandler == INVALID_HANDLE_VALUE)
//	{
//		printf("Could not open file, error %ul\n", GetLastError());
//		return -1;
//	}
	getchar();
	free(outputLogFileName);
	return 0;
}


char *getFileName(char *path)
{
	char *filename = strrchr(path, '\\');
	if (filename == NULL)
		filename = path;
	else
		filename++;
	return filename;
}


char *getOutputFileName(char *fileName, char *outputFileNameEnding) {
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

// Test 1:
char *getFileExtention(char *filename) {
	Sleep(10);
	char *dotLocationInFileName = strrchr(filename, '.');
	if (!dotLocationInFileName || dotLocationInFileName == filename)
		return "No";
	return dotLocationInFileName + 1;
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
