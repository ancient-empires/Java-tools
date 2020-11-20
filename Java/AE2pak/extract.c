#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../utils/utils.h"
#include "extract.h"
#include "path_processing.h"

// Extract single file (internal use only)
// Return true if successfully extracted; otherwise return false.
static bool extractFile(const char* pakFile, const char* targetFile, unsigned int fileDataPos, unsigned int fileSize) {

	// open .pak file for reading
	FILE* pakFileDesc = fopen(pakFile, "rb");
	if (!pakFileDesc) {
		fprintf(stderr, "ERROR: Could not open .pak file \"%s\" for extraction!\n", pakFile);
		return false;
	}
	fseek(pakFileDesc, fileDataPos, SEEK_SET);

	// open target file for writing
	FILE* targetFileDesc = fopen(targetFile, "wb");
	if (!targetFileDesc) {
		fprintf(stderr, "ERROR: Could not open resource file \"%s\" for writing!\n", targetFile);
		fclose(pakFileDesc);
		return false;
	}

	// copy bytes from .pak to target file
	for (unsigned int i = 0; i < fileSize; ++i) {
		unsigned char c = fgetc(pakFileDesc);
		if (feof(pakFileDesc)) {
			fprintf(stderr, "ERROR: EOF reached in .pak file \"%s\" when extracting! Please check your file!", pakFile);
			fclose(pakFileDesc);
			fclose(targetFileDesc);
			return false;
		}
		fputc(c, targetFileDesc);
	}

	fclose(pakFileDesc);
	fclose(targetFileDesc);
	return true;
}

// Extract .pak file to extract directory.
// The extract directory must exist beforehand, or the program will NOT work.
// Write the .log file containing the list of all extracted files in current working directory.
void extract(const char* pakFile, const char* extractDir) {

	printf("Extracting...\n");

	// open .pak file for reading
	FILE* pakFileDesc = fopen(pakFile, "rb");
	if (!pakFileDesc) {
		fprintf(stderr, "ERROR: .pak file \"%s\" not found.\n", pakFile);
		exit(ERROR_RW);
	}

	// open the file list .log file for writing
	// the .log file will be in current working directory
	char fileListLOG[] = FILE_LIST_LOG;
	FILE* fileListDesc = fopen(fileListLOG, "wb");
	if (!fileListDesc) {
		fclose(pakFileDesc);
		fprintf(stderr, "ERROR: file list \"%s\" cannot be created for writing.\n", fileListLOG);
		exit(ERROR_RW);
	}
	printf("\nStoring file list in log file: \"%s\"\n\n", fileListLOG);

	unsigned char c1, c2, c3, c4;

	// get starting position of file data (first 2 bytes)
	rewind(pakFileDesc);
	c1 = fgetc(pakFileDesc);
	c2 = fgetc(pakFileDesc);
	unsigned int fileDataStartPos = fourBytesToUInt32(0, 0, c1, c2);
	printf("File data start at byte: %u\n", fileDataStartPos);

	// get number of total files (next 2 bytes)
	c1 = fgetc(pakFileDesc);
	c2 = fgetc(pakFileDesc);
	unsigned int totalFiles = fourBytesToUInt32(0, 0, c1, c2);
	printf("Total Files announced: %u\n", totalFiles);

	// check unexpected ending of .pak file
	if (feof(pakFileDesc)) {
		fclose(pakFileDesc);
		fprintf(stderr, "ERROR: Unexpected end of .pak file:\n\"%s\"\n", pakFile);
		fclose(pakFileDesc);
		fclose(fileListDesc);
		exit(ERROR_RW);
	}

	// process all files

	unsigned int totalExtracted = 0, totalErrors = 0;

	for (unsigned int i = 0; i < totalFiles; ++i) {
		// get filename length (2 bytes)
		c1 = fgetc(pakFileDesc);
		c2 = fgetc(pakFileDesc);
		unsigned int filenameLen = fourBytesToUInt32(0, 0, c1, c2);

		// get filename
		unsigned int j = 0;
		char* filename = calloc(filenameLen + 1, sizeof(char));
		for (; j < filenameLen; ++j) {
			filename[j] = fgetc(pakFileDesc);
		}

		// get file data position (4 bytes)
		c1 = fgetc(pakFileDesc);
		c2 = fgetc(pakFileDesc);
		c3 = fgetc(pakFileDesc);
		c4 = fgetc(pakFileDesc);
		unsigned int fileDataPos = fourBytesToUInt32(c1, c2, c3, c4) + fileDataStartPos;

		// get file size (2 bytes)
		c1 = fgetc(pakFileDesc);
		c2 = fgetc(pakFileDesc);
		unsigned int fileSize = fourBytesToUInt32(0, 0, c1, c2);

		// write to file list (.log file)
		// the file list will be created in the current working directory
		// the extracted file paths are relative to the current working directory
		size_t extractDirLen = strlen(extractDir);
		char* extractedFilePath = calloc(extractDirLen + 1 /* '/' character */ + filenameLen + 1, sizeof(char));
		strcpy(extractedFilePath, extractDir);
		Windows2UnixPath(extractedFilePath);
		if (extractedFilePath[extractDirLen-1] == DBQUOTE) {
			extractedFilePath[extractDirLen-1] = '\0';
		}
		else if (extractedFilePath[extractDirLen-1] == SLASH) {
			extractedFilePath[extractDirLen-1] = '\0';
		}
		strcat(extractedFilePath, SLASH_STR);
		strcat(extractedFilePath, filename);

		// extract file
		if (extractFile(pakFile, extractedFilePath, fileDataPos, fileSize)) {
			// write to file list if extraction is successful
			fprintf(fileListDesc, "%s\n", extractedFilePath);
		}
		else {
			++totalErrors;
		}
		++totalExtracted;

		free(filename);
		free(extractedFilePath);
	}

	// finish
	fclose(pakFileDesc);
	fclose(fileListDesc);

	printf("\nUh yeah, it's done!\n");
	printf("Total files: %u\n", totalFiles);
	printf("Total extracted: %u\n", totalExtracted);
	printf("Total errors: %u\n\n", totalErrors);
}
