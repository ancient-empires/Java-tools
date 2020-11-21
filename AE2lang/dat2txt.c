#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "dat2txt.h"

// Convert all strings from DAT to TXT format (internal use only)
// srcFileDesc: the .dat file descriptor (read from)
// destFileDesc: the .txt file descriptor (write to)
// return: the number of strings successfully extracted
static uint32_t dat2str(FILE* srcFileDesc, FILE* destFileDesc, uint32_t* stringsCount, uint32_t* totalStrings) {
	*stringsCount = 0;
	*totalStrings = 0;

	unsigned char c1, c2, c3, c4;

	// Get number of total strings.
	// Number of total strings is specified in the first 4 bytes.
	c1 = fgetc(srcFileDesc);
	c2 = fgetc(srcFileDesc);
	c3 = fgetc(srcFileDesc);
	c4 = fgetc(srcFileDesc);
	*totalStrings = fourBytesToUInt32(c1, c2, c3, c4);
	printf("Number of total strings: %u\n\n", *totalStrings);
	if (*totalStrings < 1) {
		// incorrect format
		fclose(srcFileDesc);
		fprintf(stderr, "ERROR: incorrect format\n");
		fprintf(stderr, "Number of total strings: %u (announced in first 4 bytes)\n\n", *totalStrings);
		exit(ERROR_RW);
	}

	// Process all strings
	uint32_t strIdx = 0;
	for (; strIdx < *totalStrings; ++strIdx) {
		// First check string validity.
		// For each text field in the .dat file, the first 2 bytes indicate string length in bytes.
		c3 = fgetc(srcFileDesc);
		c4 = fgetc(srcFileDesc);
		uint32_t textLen = fourBytesToUInt32(0, 0, c3, c4);

		// Then process each character of the string, one by one.
		char* buffer = calloc(textLen + 1, sizeof(char));
		uint32_t charIdx = 0;
		for (; charIdx < textLen; ++charIdx) {
			c1 = fgetc(srcFileDesc);

			// Check if reached the end of file unexpectedly.
			if (feof(srcFileDesc)) {
				long currentPos = ftell(srcFileDesc);
				fprintf(stderr, "ERROR: Reached end of file unexpectedly when reading text No. %u at offset %ld.\n", strIdx, currentPos);
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
void dat2txt(const char* srcFilename, const char* destFilename) {
	printf("Converting .dat to .txt ...\n\n");

	// check source file (.dat)
	FILE* srcFileDesc = fopen(srcFilename, "rb");
	if (!srcFileDesc) {
		fprintf(stderr, "ERROR: could not open \"%s\" for reading !\n", srcFilename);
		exit(ERROR_RW);
	}

	// check destination file (.txt)
	FILE* destFileDesc = fopen(destFilename, "w");
	if (!destFileDesc) {
		fclose(srcFileDesc);
		fprintf(stderr, "ERROR: could not open \"%s\" for writing !\n", destFilename);
		exit(ERROR_RW);
	}

	// process all strings in the .dat file, and write to the .txt file
	uint32_t stringsCount = 0, totalStrings = 0;
	dat2str(srcFileDesc, destFileDesc, &stringsCount, &totalStrings);

	// finish
	printf("Uh yeah, its done!\n");
	printf("Announced: %u\n", totalStrings);
	printf("Extracted: %u\n\n", stringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
}
