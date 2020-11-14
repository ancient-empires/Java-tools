#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "pack.h"
#include "pak_limits.h"

#define LARGE_SPACE_SIZE 2048

static void _getFilename(char* path) {
	char buffer[LARGE_SPACE_SIZE];
	strcpy(buffer, path);
	strrev(buffer);
	size_t filenameLen = strlen(buffer);
	for (size_t i = 0; i < filenameLen; ++i){
		if (buffer[i] == SLASH){
			buffer[i] = '\0';
			break;
		}
	}
	strrev(buffer);
	strcpy(path, buffer);
	return;
}

// Create the .pak archive, using files specified in the file list.
void pack(const char* pakFile, const char* fileListLOG) {
	char sdata2[LARGE_SPACE_SIZE][LARGE_SPACE_SIZE];
	uint16_t resourceFileSizes[LARGE_SPACE_SIZE];

	printf("Packing...\n");

	unsigned char c1, c2, c3, c4;

	uint16_t totalFiles = 0;

	// Check file list (.log file)
	printf("\nChecking file list...\n");
	FILE* fileListDesc = fopen(fileListLOG, "r");
	if (!fileListDesc) {
		fprintf(stderr, "\nERROR: Failed to open file list \"%s\".\n\n", fileListLOG);
		exit(ERROR_RW);
	}

	// read each line containing a file path
	rewind(fileListDesc);
	unsigned int totalErrors = 0;
	while (!feof(fileListDesc)) {
		// get each line containing a filename
		char* filename = NULL;
		size_t n = 0;
		ssize_t filenameLen = getline(&filename, &n, fileListDesc);
		if ((filenameLen > 0) && (filename[filenameLen-1] == LF)) {
			filename[filenameLen-1] = '\0';
		}

		if ((strcmp(sdata2[totalFiles], filename)) && (filenameLen > 1)) {
			// open the resource file, and check the size
			strcpy(sdata2[totalFiles], filename);
			FILE* resourceFileDesc = fopen(sdata2[totalFiles], "rb");
			if (!resourceFileDesc) {
				fprintf(stderr, "ERROR: Could not find resouce file \"%s\"\n", sdata2[totalFiles]);
				fclose(resourceFileDesc);
				++totalErrors;
			}
			else {
				// get size of the resource file
				fseek(resourceFileDesc, 0, SEEK_END);
				resourceFileSizes[totalFiles] = ftell(resourceFileDesc);
				fclose(resourceFileDesc);
				++totalFiles;
			}
			if (totalFiles > LARGE_SPACE_SIZE) {
				free(filename);
				break;
			}
		}
		free(filename);
	}
	fclose(fileListDesc);

	// check errors
	if (totalErrors > 0) {
		fprintf(stderr, "ERROR: Cannot find %d files. Fix the problem before retrying.\n", totalErrors);
		exit(ERROR_RW);
	}
	else if (totalFiles == 0) {
		fprintf(stderr, "ERROR: Nothing to pack. Check your files!\n");
		exit(ERROR_RW);
	}
	else if (totalFiles > LARGE_SPACE_SIZE) {
		fprintf(stderr, "ERROR: Sorry, this crappy packer cannot pack more than %d files!\n", LARGE_SPACE_SIZE);
		exit(ERROR_RW);
	}

	// create the .pak file
	FILE* pakFileDesc = fopen(pakFile, "wb");
	if (!pakFileDesc) {
		fprintf(stderr, "ERROR: Cannot open .pak file \"%s\" for writing!\n", pakFile);
		fclose(pakFileDesc);
		exit(ERROR_RW);
	}

	// reserve first 2 bytes to write the starting point of file data
	fseek(pakFileDesc, FILE_DATA_START_POS_BYTES, SEEK_SET);

	// write number of total files (2 bytes)
	uInt32ToFourBytes(totalFiles, &c1, &c2, &c3, &c4);
	if ((totalFiles > TOTAL_NUM_FILES_MAX) || (c1 || c2)) {
		fprintf(stderr, "ERROR: You have a total of %d files. ", totalFiles);
		fprintf(stderr, "The .pak archive can only contain a maximum of %d files.\n", TOTAL_NUM_FILES_MAX);
		fclose(pakFileDesc);
		exit(ERROR_RW);
	}
	fputc(c3, pakFileDesc);
	fputc(c4, pakFileDesc);

	// write file info for each resource file
	// included information: filename length (2 bytes), filename, file data starting offset (4 bytes), and file size (2 bytes)
	uint32_t fileDataStartOffset = 0;
	for (unsigned int i = 0; i < totalFiles; ++i) {
		// get filename (omitting directory name)
		char filename[LARGE_SPACE_SIZE];
		strcpy(filename, sdata2[i]);
		_getFilename(filename);

		// write filename length (2 byptes)
		size_t filenameLen = strlen(filename);
		uInt32ToFourBytes(filenameLen, &c1, &c2, &c3, &c4);
		if (c1 || c2) {
			fprintf(stderr, "ERROR: Filename \"%s\" is too long (more than %d characters). Check your resouce file directory and try again.\n",  filename, FILENAME_LEN_MAX);
			fclose(pakFileDesc);
			exit(ERROR_RW);
		}
		fputc(c3, pakFileDesc);
		fputc(c4, pakFileDesc);

		// write filename
		fputs(filename, pakFileDesc);

		// write starting offset of file data, relative to the start of data for all files (4 bytes)
		// The first file has offset 0.
		// The second file has offset (0 + size of first file).
		// The third file has offset (0 + size of first file + size of second file); and so on.
		uInt32ToFourBytes(fileDataStartOffset, &c1, &c2, &c3, &c4);
		fputc(c1, pakFileDesc);
		fputc(c2, pakFileDesc);
		fputc(c3, pakFileDesc);
		fputc(c4, pakFileDesc);

		// write file size (2 bytes)
		uint16_t fileSize = resourceFileSizes[i];
		uInt32ToFourBytes(fileSize, &c1, &c2, &c3, &c4);
		fputc(c3, pakFileDesc);
		fputc(c4, pakFileDesc);
		fileDataStartOffset += fileSize;
	}

	// writing the starting position of the data of ALL files (2 bytes)
	fseek(pakFileDesc, 0, SEEK_END);
	long fileDataStartPos = ftell(pakFileDesc);
	uInt32ToFourBytes(fileDataStartPos, &c1, &c2, &c3, &c4);
	if (c1 || c2) {
		fprintf(stderr, "ERROR: Illegal file data start position: %ld. ", fileDataStartPos);
		fprintf(stderr, "(A valid file data starting position is not greater than %d)\n", FILE_DATA_START_POS_MAX);
	}
	rewind(pakFileDesc);
	fputc(c3, pakFileDesc);
	fputc(c4, pakFileDesc);

	// copy bytes from the resource files to the .pak file
	fseek(pakFileDesc, fileDataStartPos, SEEK_SET);
	totalErrors = 0;
	for (unsigned int i = 0 ; i < totalFiles; ++i) {
		// check resource file
		FILE* resourceFileDesc = fopen(sdata2[i], "rb");
		if (!resourceFileDesc) {
			fprintf(stderr, "ERROR: Could not open resource file \"%s\" for reading\n", sdata2[i]);
			fclose(pakFileDesc);
			fclose(resourceFileDesc);
			exit(ERROR_RW);
		}

		// copy bytes from resource file to .pak file
		unsigned int j = 0;
		while (!feof(resourceFileDesc)) {
			c1 = getc(resourceFileDesc);
			if (!feof(resourceFileDesc)) {
				fputc(c1, pakFileDesc);
				++j;
			}
		}
		if (j != resourceFileSizes[i]) {
			fprintf(stderr, "ERROR: Could not match size of file! (File size read: %d'; File size expected: %d)\n", j, resourceFileSizes[i]);
			++totalErrors;
		}

		fclose(resourceFileDesc);
	}
	fclose(pakFileDesc);


	printf("\nUh yeah, its done! %d errors for %d (announced)\n", totalErrors, totalFiles);
}
