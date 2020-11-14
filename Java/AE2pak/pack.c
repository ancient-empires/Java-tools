#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "pack.h"

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
	FILE *fn;
	unsigned long i, j, k;
	unsigned char c1, c2, c3, c4;
	char sdata2[LARGE_SPACE_SIZE][LARGE_SPACE_SIZE];
	uint16_t resourceFileSizes[LARGE_SPACE_SIZE];

	printf("Packing...\n");

	uint16_t totalFiles = 0;
	uint32_t fileDataPos = 0;
	unsigned int totalErrors = 0;

	// Check file list (.log file)
	printf("\nChecking file list...\n");
	FILE* fileListDesc = fopen(fileListLOG, "r");
	if (!fileListDesc) {
		fprintf(stderr, "\nERROR: Failed to open file list \"%s\".\n\n", fileListLOG);
		exit(ERROR_RW);
	}

	// read each line containing a file path
	rewind(fileListDesc);
	while(!feof(fileListDesc)) {
		// get each line containing a filename
		char filename[LARGE_SPACE_SIZE];
		filename[0] = '\0';
		fgets(filename, LARGE_SPACE_SIZE, fileListDesc);
		size_t filenameLen = strlen(filename);
		if ((filenameLen > 0) && ((filename[filenameLen-1] == CR) || (filename[filenameLen-1] == LF))) {
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
				break;
			}
		}
	}
	fclose(fileListDesc);

	// check errors
	if (totalErrors > 0) {
		fprintf(stderr, "ERROR: Could not found %d files. Fix the problem before retrying.\n", totalErrors);
		exit(ERROR_RW);
	}
	else if (totalFiles == 0) {
		fprintf(stderr, "ERROR: Nothing to pack. Check your files!\n");
		exit(ERROR_RW);
	}
	else if (totalFiles > LARGE_SPACE_SIZE) {
		fprintf(stderr, "ERROR: Sorry, this crappy exe cannot pack more than %d files!\n", LARGE_SPACE_SIZE);
		exit(ERROR_RW);
	}

	// create the .pak file
	fn = fopen(pakFile, "wb");
	if (!fn) {
		fprintf(stderr, "ERROR: Could not open .pak file \"%s\" for writing!\n", pakFile);
		fclose(fn);
		exit(ERROR_RW);
	}

	// reserve first 2 bytes to write the starting point of file data
	fseek(fn, DATA_START_BYTES, SEEK_SET);

	// write number of total files (2 bytes)
	uInt32ToFourBytes(totalFiles, &c1, &c2, &c3, &c4);
	if (c1 || c2) {
		fprintf(stderr, "ERROR: The .pak file format can only contain a maximum of %d files.\n", UINT16_MAX);
		fclose(fn);
		exit(ERROR_RW);
	}
	fputc(c3, fn);
	fputc(c4, fn);

	// write file info for each resource file
	for (i = 0; i < totalFiles; ++i) {
		// get filename
		char filename[LARGE_SPACE_SIZE];
		strcpy(filename, sdata2[i]);
		_getFilename(filename);

		// write filename length (2 byptes)
		size_t filenameLen = strlen(filename);
		uInt32ToFourBytes(filenameLen, &c1, &c2, &c3, &c4);
		if (c1 || c2) {
			fprintf(stderr, "ERROR: Filename \"%s\" is too long (more than %d characters). Check your resouce file directory and try again.\n",  filename, UINT16_MAX);
			exit(ERROR_RW);
		}
		fputc(c3, fn);
		fputc(c4, fn);

		// write filename
		fputs(filename, fn);

		// write starting offset of file data, relative to the start of data for all files (4 bytes)
		uInt32ToFourBytes(fileDataPos, &c1, &c2, &c3, &c4);
		fputc(c1, fn);
		fputc(c2, fn);
		fputc(c3, fn);
		fputc(c4, fn);

		// write file size (2 bytes)
		uint16_t fileSize = resourceFileSizes[i];
		uInt32ToFourBytes(fileSize, &c1, &c2, &c3, &c4);
		fputc(c3, fn);
		fputc(c4, fn);
		fileDataPos += fileSize;
	}

	// Writing the header of the end position of the header
	fseek(fn, 0, SEEK_END);
	fileDataPos = ftell(fn);
	k = fileDataPos;

	c3 = (k/256);
	j = (c3*256);

	k = (k-j);
	c4 = k;

	rewind(fn);
	fputc(c3, fn);
	fputc(c4, fn);

	// pack files
	rewind(fn);
	fseek (fn, fileDataPos, 0);

	for (i=0;i<totalFiles;i++) {
		FILE* fo = fopen(sdata2[i], "rb");
		if (!fo) {
			fprintf(stderr, "ERROR: Could not open file \"%s\" for reading\n", sdata2[i]);
			fclose(fn);
			exit(1);
		}
		j=0;
		while(!feof(fo)) {
			c1=getc(fo);
			if (!feof(fo)) fputc(c1, fn);
			++j;
		}
		if (j - 1 != resourceFileSizes[i]) {
			fprintf(stderr, "ERROR: Could not match size of file! j = %ld\n", j);
		}
		fclose(fo);
	}
	fclose(fn);


	printf("\n Uh yeah, its done! %d errors for %d (announced)\n", totalErrors, totalFiles);
}
