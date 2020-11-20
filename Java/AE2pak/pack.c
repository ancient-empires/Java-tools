#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "pak_limits.h"
#include "file_processing.h"
#include "pack.h"

#define LARGE_SPACE_SIZE 2048

typedef struct fileinfo {
	uint16_t filenameLen;
	char* filePath;
	char* filename;
	uint32_t fileDataStartOffset;
	uint16_t fileSize;
} fileinfo_t;

/* Get the length to store the information for each resource file in the .pak file.
	Each resource file is represented as follows at the beginning section of the file:
		1. filename length
		2. filename
		3. file data start offset
		4. file size
*/
static unsigned int getFileInfoLength(const fileinfo_t* pFileInfo) {
	unsigned int totalLength = FILENAME_LEN_BYTES;
	totalLength += strlen(pFileInfo->filename);
	totalLength += FILE_DATA_START_OFFSET_BYTES;
	totalLength += FILE_SIZE_BYTES;
	return totalLength;
}

// static void _getFilename(char* path) {
// 	char buffer[LARGE_SPACE_SIZE];
// 	strcpy(buffer, path);
// 	strrev(buffer);
// 	size_t filenameLen = strlen(buffer);
// 	for (size_t i = 0; i < filenameLen; ++i){
// 		if (buffer[i] == SLASH){
// 			buffer[i] = '\0';
// 			break;
// 		}
// 	}
// 	strrev(buffer);
// 	strcpy(path, buffer);
// 	return;
// }

// Check all the files that are listed in the file list .log file
// If failed to open the file list, or if one or more errors encountered when checking resource files, then abort the program with exit code ERROR_RW.
// Otherwise, return total number of files that are successfully checked.
static unsigned int checkAllFiles(const char* fileListLOG, unsigned int* pTotalFiles, unsigned int *pTotalErrors) {
	*pTotalFiles = 0;
	*pTotalErrors = 0;

	// open the .log file
	FILE* fileListDesc = fopen(fileListLOG, "r");
	if (!fileListLOG) {
		fprintf(stderr, "Invalid file list: \"%s\"\n", fileListLOG);
		fclose(fileListDesc);
		exit(ERROR_RW);
	}

	// get all lines, each line containing a single file path
	while (!feof(fileListDesc)) {
		// get file path
		char* filePath = NULL;
		size_t n = 0;
		getline(&filePath, &n, fileListDesc);

		// check file size
		long fileSize = getFileSize(filePath);
		if (fileSize < 0) {
			++(*pTotalErrors);
			if (*pTotalErrors == 1) {
				fprintf(stderr, "Found errors when checking these files:\n");
			}
			fprintf(stderr, "[%d] %s\n", *pTotalErrors - 1, filePath);
		}
		else {
			++(*pTotalFiles);
		}

		free(filePath);
	}

	// if there are more than one error found, abort the execution
	if (*pTotalErrors > 0) {
		fprintf(stderr, "Please check your file list (%s), and then try again.\n", fileListLOG);
		exit(ERROR_RW);
	}
	else {
		printf("Successfully checked %d files.\n", *pTotalFiles);
		return *pTotalFiles;
	}
}

// Create the .pak archive, using files specified in the file list .log file.
void pack(const char* pakFile, const char* fileListLOG) {
	unsigned int totalFiles = 0;
	unsigned int totalErrors = 0;

	// check all files present in the file list .log file.
	checkAllFiles(fileListLOG, &totalFiles, &totalErrors);

	printf("\nUh yeah, its done! %d errors for %d (announced)\n", totalErrors, totalFiles);
}
