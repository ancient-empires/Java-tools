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

// Check all the files that are listed in the file list .log file
// If failed to open the file list, or if one or more errors encountered when checking resource files, then abort the program with exit code ERROR_RW.
// Otherwise, return total number of files that are successfully checked.
static unsigned int checkAllFiles(const char* fileListLOG, unsigned int* pTotalResourceFiles, unsigned int *pTotalErrors, unsigned int *pTotalFileInfoLen) {

	printf("Checking all files...\n\n");

	*pTotalResourceFiles = 0;
	*pTotalErrors = 0;
	*pTotalFileInfoLen = FILE_DATA_START_POS_BYTES + TOTAL_NUM_FILES_BYTES;

	unsigned long totalResourceFileSize = 0;

	// open the .log file
	FILE* fileListDesc = fopen(fileListLOG, "r");
	if (!fileListLOG) {
		fprintf(stderr, "Invalid file list: \"%s\"\n", fileListLOG);
		fclose(fileListDesc);
		exit(ERROR_RW);
	}

	const char errorHeader[] = "ERROR: Found errors when checking these files:";

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
		if ((filePathLen >= 1) && (filePath[filePathLen - 1] == LF)) {
			// replace line ending from '\n' to '\0'
			filePath[filePathLen - 1] = '\0';
		}

		// check file size
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

			// save file info
			fileinfo_t fileInfo;
			unsigned long fileDataStartOffset = (totalResourceFileSize <= FILE_DATA_START_OFFSET_MAX) ? totalResourceFileSize : FILE_DATA_START_OFFSET_MAX;
			fileInfo = saveFileInfo(filePath, fileDataStartOffset, fileSize);

			++(*pTotalResourceFiles);
			unsigned int fileInfoLen = getFileInfoLen(&fileInfo);
			*pTotalFileInfoLen += fileInfoLen;
			totalResourceFileSize += fileSize;
		}

		free(filePath);
	}

	// abort the program if there are any errors
	if (*pTotalErrors > 0) {
		fprintf(stderr, "\nPlease check your file list (%s), and then try again.\n\n", fileListLOG);
		exit(ERROR_RW);
	}
	else if (*pTotalFileInfoLen >= FILE_DATA_START_POS_MAX) {
		fprintf(stderr, "ERROR: File info is too long (%u bytes)\n", *pTotalFileInfoLen);
		fprintf(stderr, "Maximum allowed length of file info: %u bytes.\n", FILE_DATA_START_POS_MAX - 1);
		fprintf(stderr, "Please reduce the length of filenames, and then try again.\n\n");
		exit(ERROR_RW);
	}
	else if (*pTotalResourceFiles > TOTAL_NUM_FILES_MAX) {
		fprintf(stderr, "ERROR: Too many resource files to pack (%u files)\n", *pTotalResourceFiles);
		fprintf(stderr, "Maximum allowed number of resource files: %u.\n\n", TOTAL_NUM_FILES_MAX);
		exit(ERROR_RW);
	}
	else if (totalResourceFileSize > FILE_DATA_START_OFFSET_MAX) {
		fprintf(stderr, "ERROR: Total size of resource files is too large (%lu bytes)\n", totalResourceFileSize);
		fprintf(stderr, "Maximum allowed total size of resource files: %u bytes.\n", FILE_DATA_START_OFFSET_MAX);
		fprintf(stderr, "Please try again with less files to pack.\n\n");
		exit(ERROR_RW);
	}
	else {
		printf("Successfully checked %u files.\n", *pTotalResourceFiles);
		printf("Total errors: %u\n", *pTotalErrors);
		printf("Total file info length: %u\n\n", *pTotalFileInfoLen);
		return *pTotalResourceFiles;
	}
}

// Create the .pak archive, using files specified in the file list .log file.
void pack(const char* pakFile, const char* fileListLOG) {
	unsigned int totalResourceFiles = 0;
	unsigned int totalErrors = 0;
	unsigned int totalFileInfoLen = 0;

	printf("Packing...\n\n");

	// check all files present in the file list .log file.
	checkAllFiles(fileListLOG, &totalResourceFiles, &totalErrors, &totalFileInfoLen);

	// process all files
	fileinfo_t* allResourceFilesInfo = calloc(totalResourceFiles, sizeof(fileinfo_t));

	free(allResourceFilesInfo);

	printf("\nUh yeah, its done! %u errors for %u announced files.\n\n", totalErrors, totalResourceFiles);
}
