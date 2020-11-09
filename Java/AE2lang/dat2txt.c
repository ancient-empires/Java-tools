#include <stdio.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "dat2txt.h"

// Convert all strings from DAT to TXT format (internal use only)
// srcFileDesc: the .dat file descriptor (read from)
// destFileDesc: the .txt file descriptor (write to)
// return: the number of strings successfully extracted
static unsigned int dat2str(FILE* srcFileDesc, FILE* destFileDesc, unsigned int* stringsCount, unsigned int* totalStrings) {
	*stringsCount = 0;
	*totalStrings = 0;

	unsigned char c1, c2, c3, c4;

	// Get number of total strings.
	// Number of total strings is specified in the first 4 bytes.
	c1 = fgetc(srcFileDesc);
	c2 = fgetc(srcFileDesc);
	c3 = fgetc(srcFileDesc);
	c4 = fgetc(srcFileDesc);
	*totalStrings = fourBytesToUnsignedInt(c1, c2, c3, c4);
	printf("Number of total strings: %d\n\n", *totalStrings);
	if (*totalStrings < 1) {
		// incorrect format
		fclose(srcFileDesc);
		printf("\nERROR: incorrect format:\ntotalStrings: %d (announced in first 4 bytes)\n\n", *totalStrings);
		exit(ERROR_RW);
	}

	// Process all strings
	unsigned int strIdx = 0;
	for (; strIdx < *totalStrings; ++strIdx) {
		// First check string validity.
		// For each text field in the .dat file, the first two bytes indicate string length in bytes.
		c3 = fgetc(srcFileDesc);
		c4 = fgetc(srcFileDesc);
		unsigned int textLen = fourBytesToUnsignedInt(0, 0, c3, c4);

		// Then process each character of the string, one by one.
		char* buffer = (char*)calloc(textLen + 1, sizeof(char));
		unsigned int charIdx = 0;
		for (; charIdx < textLen; ++charIdx) {
			c1 = fgetc(srcFileDesc);

			// Check if reached the end of file unexpectedly.
			if (feof(srcFileDesc)) {
				long currentPos = ftell(srcFileDesc);
				printf("ERROR: Reached end of file unexpectedly when reading text No. %d at offset %ld.\n", strIdx, currentPos);
				free(buffer);
				fclose(srcFileDesc);
				fclose(destFileDesc);
				exit(ERROR_RW);
			}

			// Put extracted string in buffer, in order to write into the .txt file.
			// In the extracted .txt file, use '|' character to designate '\n' in each text field in the UI.
			if (c1 == LF) {
				c1 = VERT;
			}
			buffer[charIdx] = c1;
		}

		// Put LF at the end of each line, for each string.
		fputs(buffer, destFileDesc);
		fputc(LF, destFileDesc);
		free(buffer);
	}
	*stringsCount = strIdx;

	return *stringsCount;
}

// Convert .dat to .txt
void dat2txt(char* srcFilename, char* destFilename) {
	printf("Converting .dat to .txt ...\n\n");

	// check source file (.dat)
	FILE* srcFileDesc = fopen(srcFilename, "rb");
	if (!srcFileDesc) {
		printf("ERROR: could not open \"%s\" for reading !\n", srcFilename);
		exit(ERROR_RW);
	}

	// check destination file (.txt)
	FILE* destFileDesc = fopen(destFilename, "w");
	if (!destFileDesc) {
		fclose(srcFileDesc);
		printf("ERROR: could not open \"%s\" for writing !\n", destFilename);
		exit(ERROR_RW);
	}

	// process all strings in the .dat file, and write to the .txt file
	unsigned int stringsCount = 0, totalStrings = 0;
	dat2str(srcFileDesc, destFileDesc, &stringsCount, &totalStrings);

	// finish
	printf("Uh yeah, its done!\n");
	printf("Announced: %d; Extracted: %d\n\n", totalStrings, stringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
}
