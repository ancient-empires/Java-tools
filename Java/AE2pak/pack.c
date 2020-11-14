#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "pack.h"

#define LARGE_SPACE_SIZE 2048

static void _getFilename(char* sdata) {
	char sdata2[LARGE_SPACE_SIZE];
	strcpy(sdata2, sdata);
	strrev(sdata2);
	size_t filenameLen = strlen(sdata2);
	for (size_t i = 0; i < filenameLen; ++i){
		if (sdata2[i] == SLASH){
			sdata2[i] = 0;
			break;
		}
	}
	strrev(sdata2);
	strcpy(sdata, sdata2);
	return;
}

// Create the .pak archive, using files specified in the file list.
void pack(const char* pakFile, const char* fileListLOG) {
	FILE *fn;
	unsigned long i, j, k, totalFiles=0, fileDataPos=0, totalErrors=0;
	unsigned char c1, c2, c3, c4;
	char sdata[LARGE_SPACE_SIZE];
	char sdata2[LARGE_SPACE_SIZE][LARGE_SPACE_SIZE];
	uint16_t resourceFileSizes[LARGE_SPACE_SIZE];

	printf("Packing...\n");

	// Check file list (.log file)

	// open file list
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
		sdata[0] = 0;
		fgets(sdata, LARGE_SPACE_SIZE, fileListDesc);
		size_t filenameLen = strlen(sdata);
		if ((filenameLen > 0) && ((sdata[filenameLen-1] == CR) || (sdata[filenameLen-1] == LF))) {
			sdata[filenameLen-1] = 0;
		}

		if ((strcmp(sdata2[totalFiles], sdata)) && (filenameLen > 1)) {
			// open the resource file, and check the size
			strcpy(sdata2[totalFiles], sdata);
			FILE* resourceFileDesc = fopen(sdata2[totalFiles], "rb");
			if (!resourceFileDesc) {
				fprintf(stderr, "ERROR: Could not find \"%s\"\n", sdata2[totalFiles]);
				fclose(resourceFileDesc);
				totalErrors++;
			}
			else {
				rewind(resourceFileDesc);
				fseek(resourceFileDesc, 0, SEEK_END);
				resourceFileSizes[totalFiles] = ftell(resourceFileDesc);
				fclose(resourceFileDesc);
			}
			totalFiles++;
			if (totalFiles > LARGE_SPACE_SIZE) {
				break;
			}
		}
	}
	fclose(fileListDesc);

	// check errors
	if (totalErrors > 0) {
		fprintf(stderr, "ERROR: Could not found %ld files. Fix the problem before retrying.\n", totalErrors);
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


	rewind(fn);
	putc(0xFF, fn);
	putc(0xFF, fn);

	c3 = totalFiles / BYTE_CAP;
	c4 = totalFiles % BYTE_CAP;
	putc(c3, fn);
	putc(c4, fn);


	for (i = 0; i < totalFiles; ++i) {
		strcpy(sdata, sdata2[i]);
		_getFilename(sdata);
		k = strlen(sdata);
		c3 = k / 256;
		j = c3 * 256;
		k = k-j;
		c4 = k;

		putc(c3, fn);
		putc(c4, fn);

		fputs(sdata, fn);


		// position relative
		k = fileDataPos;
		c1 = k / 16777216;
		j = c1 * 16777216;

		k = k - j;
		c2 = k / 65536;
		j = c2 * 65536;

		k = k - j;
		c3 = k / 256;
		j = c3 * 256;

		k = k-j;
		c4 = k;
		putc(c1, fn);
		putc(c2, fn);
		putc(c3, fn);
		putc(c4, fn);


		// file size
		k = resourceFileSizes[i];
		fileDataPos = (fileDataPos+k);

		c3 = (k/256);
		j = (c3*256);

		k = (k-j);
		c4 = k;

		putc(c3, fn);
		putc(c4, fn);

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
	putc(c3, fn);
	putc(c4, fn);

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
			if (!feof(fo)) putc(c1, fn);
			j++;
		}
		if (j - 1 != resourceFileSizes[i]) {
			fprintf(stderr, "ERROR: Could not match size of file! j = %ld\n", j);
		}
		fclose(fo);
	}
	fclose(fn);


	printf("\n Uh yeah, its done! %ld errors for %ld (announced)\n", totalErrors, totalFiles);
}
