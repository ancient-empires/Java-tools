#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "pak_limits.h"
#include "path_processing.h"
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

// Save file info, and store it in a struct.
static fileinfo_t saveFileInfo(char* filePath, uint16_t fileSize) {
	fileinfo_t fileInfo;

	fileInfo.filePath = filePath;
	fileInfo.filename = getFilename(filePath);
	fileInfo.filenameLen = strlen(fileInfo.filename);
	fileInfo.fileDataStartOffset = 0;
	fileInfo.fileSize = fileSize;

	return fileInfo;
}

/* Get the length to store the information for each resource file in the .pak file.
	Each resource file is represented as follows at the beginning section of the file:
		1. filename length
		2. filename
		3. file data start offset
		4. file size
*/
static unsigned int getFileInfoLen(const fileinfo_t* pFileInfo) {
	unsigned int totalLen = FILENAME_LEN_BYTES;
	totalLen += strlen(pFileInfo->filename);
	totalLen += FILE_DATA_START_OFFSET_BYTES;
	totalLen += FILE_SIZE_BYTES;
	return totalLen;
}

// Check all the files that are listed in the file list .log file
// If failed to open the file list, or if one or more errors encountered when checking resource files, then abort the program with exit code ERROR_RW.
// Otherwise, return total number of files that are successfully checked.
static unsigned int checkAllFiles(const char* fileListLOG, unsigned int* pTotalFiles, unsigned int *pTotalErrors, unsigned int *pTotalFileInfoLen) {
	*pTotalFiles = 0;
	*pTotalErrors = 0;

	*pTotalFileInfoLen = FILE_DATA_START_POS_BYTES + TOTAL_NUM_FILES_BYTES;

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
		if (feof(fileListDesc)) {
			fclose(fileListDesc);
			free(filePath);
			break;
		}
		size_t filePathLen = strlen(filePath);
		if (filePathLen >= 1) {
			// replace line ending from '\n' to '\0'
			filePath[filePathLen - 1] = '\0';
		}

		// check file size
		const char errorHeader[] = "ERROR: Found errors when checking these files:";
		long fileSize = getFileSize(filePath);
		if (fileSize < 0) {
			// invalid file
			++(*pTotalErrors);
			if (*pTotalErrors == 1) {
				fprintf(stderr, "%s\n", errorHeader);
			}
			fprintf(stderr, "[%u] Invalid file path: \"%s\"\n", *pTotalErrors - 1, filePath);
		}
		else if (fileSize > FILE_SIZE_MAX) {
			// file size is too large
			++(*pTotalErrors);
			if (*pTotalErrors == 1) {
				fprintf(stderr, "%s\n", errorHeader);
			}
			fprintf(stderr, "[%u] File size is too large: \"%s\" (%ld bytes)\n", *pTotalErrors - 1, filePath, fileSize);
		}
		else {
			// file is valid, and file size is valid
			++(*pTotalFiles);
			fileinfo_t fileInfo = saveFileInfo(filePath, fileSize);
			unsigned int fileInfoLen = getFileInfoLen(&fileInfo);
			*pTotalFileInfoLen += fileInfoLen;
		}

		free(filePath);
	}

	// abort the program if there are any errors
	if (*pTotalErrors > 0) {
		fprintf(stderr, "\nPlease check your file list (%s), and then try again.\n\n", fileListLOG);
		exit(ERROR_RW);
	}
	else if (*pTotalFileInfoLen >= FILE_DATA_START_POS_MAX) {
		fprintf(stderr, "ERROR: File info is too long (%u bytes).\n", *pTotalFileInfoLen);
		fprintf(stderr, "The maximum length of file info is %u bytes.\n", FILE_DATA_START_POS_MAX - 1);
		fprintf(stderr, "Please reduce the length of filenames, and then try again.\n\n");
		exit(ERROR_RW);
	}
	else if (*pTotalFiles > TOTAL_NUM_FILES_MAX) {
		fprintf(stderr, "ERROR: Too many resource files to pack.\n");
		fprintf(stderr, "You have %u resource files.\n", *pTotalFiles);
		fprintf(stderr, "The .pak archive can only hold a maximum of %u resource files.\n\n", TOTAL_NUM_FILES_MAX);
		exit(ERROR_RW);
	}
	else {
		printf("Successfully checked %u files.\n", *pTotalFiles);
		printf("Total errors: %u\n", *pTotalErrors);
		printf("Total file info length: %u\n\n", *pTotalFileInfoLen);
		return *pTotalFiles;
	}
}

// Create the .pak archive, using files specified in the file list .log file.
void pack(const char* pakFile, const char* fileListLOG) {
	unsigned int totalFiles = 0;
	unsigned int totalErrors = 0;
	unsigned int totalFileInfoLen = 0;

	printf("Packing...\n\n");

	// check all files present in the file list .log file.
	checkAllFiles(fileListLOG, &totalFiles, &totalErrors, &totalFileInfoLen);

	// process all files
	fileinfo_t* allFilesInfo = calloc(totalFiles, sizeof(fileinfo_t));

	free(allFilesInfo);

	printf("\nUh yeah, its done! %u errors for %u announced files.\n", totalErrors, totalFiles);
}
