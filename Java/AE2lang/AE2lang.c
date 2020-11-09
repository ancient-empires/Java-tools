#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "txt2dat.h"

#define DAT ".dat"
#define DAT_LEN strlen(DAT)
#define TXT ".txt"
#define TXT_LEN strlen(TXT)

// Convert .dat to .txt
void dat2txt(char* srcFilename, char* destFilename) {
	printf("Converting .dat to .txt ...\n\n");

	// Check source file (.dat)
	FILE* srcFileDesc = fopen(srcFilename, "rb");
	if (!srcFileDesc) {
		printf("ERROR: could not open \"%s\" for reading !\n", srcFilename);
		exit(ERROR_RW);
	}

	// Check destination file (.txt)
	FILE* destFileDesc = fopen(destFilename, "w");
	if (!destFileDesc) {
		fclose(srcFileDesc);
		printf("ERROR: could not open \"%s\" for writing !\n", destFilename);
		exit(ERROR_RW);
	}

	unsigned char c1, c2, c3, c4;

	// Get number of total strings.
	// Number of total strings is specified in the first 4 bytes.
	c1 = fgetc(srcFileDesc);
	c2 = fgetc(srcFileDesc);
	c3 = fgetc(srcFileDesc);
	c4 = fgetc(srcFileDesc);
	unsigned int totalStrings = fourBytesToUnsignedInt(c1, c2, c3, c4);
	printf("Number of total strings: %d\n\n", totalStrings);
	if (totalStrings < 1) {
		// incorrect format
		fclose(srcFileDesc);
		printf("\nERROR: incorrect format:\ntotalStrings: %d (announced in first 4 bytes)\n\n", totalStrings);
		exit(ERROR_RW);
	}

	// Process all strings
	unsigned int strIdx = 0;
	for (; strIdx < totalStrings; ++strIdx) {
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
	unsigned int stringsCount = strIdx;

	// finish
	printf("Uh yeah, its done!\n");
	printf("Announced: %d; Extracted: %d\n\n", totalStrings, stringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
}

// Show help when user enters invalid arguments in command line
void help(void) {
	printf("Please use the following syntax:\n");
	printf("- dat2txt: ./AE2lang.out lang.dat lang.txt\n");
	printf("- txt2dat: ./AE2lang.out lang.txt lang.dat\n\n");
	printf("Note that the appropriate function is selected according to the files extensions (minuscule only).\n\n");
}

/* Usage:
	- dat2txt: ./AE2lang.out lang.dat lang.txt
	- txt2dat: ./AE2lang.out lang.txt lang.dat
*/
int main(int argc, char *argv[]) {

	// Program title
	printf("\n=== Ancient Empires II language file text converter v0.1b ===\n\n");

	// Check if the user has entered 3 parameters
	if (argc < 3) {
		help();
		return ERROR_ARGS;
	}

	// Check source and destination file names
	char* srcFilename = argv[1];
	unsigned int srcFilenameLen = strlen(srcFilename);
	char* destFilename = argv[2];
	unsigned int destFilenameLen = strlen(destFilename);
	if ((srcFilenameLen < DAT_LEN) || (destFilenameLen < TXT_LEN)) {
		help();
		return ERROR_ARGS;
	}

	// .dat to .txt conversion
	if (!strncmp(&srcFilename[srcFilenameLen-DAT_LEN], DAT, DAT_LEN)
		&& !strncmp(&destFilename[destFilenameLen-TXT_LEN], TXT, TXT_LEN)) {
		dat2txt(srcFilename, destFilename);
		return 0;
	}

	// .txt to .dat conversion
	else if (!strncmp(&srcFilename[srcFilenameLen-TXT_LEN], TXT, TXT_LEN)
		&& !strncmp(&destFilename[destFilenameLen-DAT_LEN], DAT, DAT_LEN)) {
		txt2dat(srcFilename, destFilename);
		return 0;
	}

	// If we got invalid arguments, then just show help, and return 0
	help();
	return ERROR_ARGS;
}
