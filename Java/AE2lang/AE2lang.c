#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../utils/utils.h"

#define LARGE_SPACE_SIZE 2048

#define DAT ".dat"
#define DAT_LEN strlen(DAT)
#define TXT ".txt"
#define TXT_LEN strlen(TXT)

// Show help when user enters invalid arguments in command line
void help(void) {
	printf("Please use the following syntax:\n");
	printf("- dat2txt: ./AE2lang.out lang.dat lang.txt\n");
	printf("- txt2dat: ./AE2lang.out lang.txt lang.dat\n\n");
	printf("Note that the appropriate function is selected according to the files extensions (minuscule only).\n\n");
}

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
	char buffer[LARGE_SPACE_SIZE];
	unsigned int strIdx = 0;
	for (; strIdx < totalStrings; ++strIdx) {
		// First check string validity.
		// For each text field in the .dat file, the first two bytes indicate string length in bytes.
		c3 = fgetc(srcFileDesc);
		c4 = fgetc(srcFileDesc);
		unsigned int textLen = fourBytesToUnsignedInt(0, 0, c3, c4);

		// Then process each character of the string, one by one.
		unsigned int charIdx = 0;
		for (; charIdx < textLen; ++charIdx) {
			c1 = fgetc(srcFileDesc);

			// In the extracted .txt file, use '|' character to designate '\n' in each text field in the UI.
			if (c1 == LF) {
				c1 = VERT;
			}

			// check if reached the end of file unexpectedly
			if (feof(srcFileDesc) && ((textLen + 1) != totalStrings)) {
				long int currentPos = ftell(srcFileDesc);
				printf("ERROR: Reached end of file unexpectedly when reading text no. %d at offset %ld\n", strIdx, currentPos);
				fclose(srcFileDesc);
				fclose(destFileDesc);
				exit(ERROR_RW);
			}

			// Put extracted string in buffer, in order to write into the .txt file.
			buffer[charIdx] = c1;
		}
		buffer[charIdx] = '\0';

		// Put LF at the end of each line, for each string.
		fputs(buffer, destFileDesc);
		fputc(LF, destFileDesc);
	}
	unsigned int stringsCount = strIdx;

	// finish
	printf("Uh yeah, its done!\n");
	printf("Announced: %d; Extracted: %d\n\n", totalStrings, stringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
}

// Convert all strings from TXT to DAT format
// srcFileDesc: the .txt file descriptor (read from)
// destFileDesc: the .dat file descriptor (write to)
// return: the total number of strings converted
unsigned int str2dat(FILE* srcFileDesc, FILE* destFileDesc, unsigned int* stringsCount) {

	*stringsCount = 0;

	while (!feof(srcFileDesc)) {
		char* line = NULL;
		size_t n = 0;
		ssize_t lineLen = getline(&line, &n, srcFileDesc);

		// lineLen > 0 if getline gets a valid line
		// lineLen == -1 if getline fails
		if (lineLen > 0) {
			// ignore lines starting with '^'
			bool ignoreLine = line[0] == CARET;

			if (!ignoreLine) {
				// replace '\n' with '\0' for writing to .dat
				--lineLen;
				line[lineLen] = '\0';

				// check number of characters in the line
				unsigned char c1, c2, c3, c4;
				unsignedIntToFourBytes(lineLen, &c1, &c2, &c3, &c4);
				if (c1 || c2) {
					printf("ERROR: line content \"%s\" is too long to fit in\n", line);
					free(line);
					continue;
				}

				// write lines to .dat file
				for (size_t i = 0; i < lineLen; ++i) {
					// revert back the '|' character in .txt into '\n' in .dat
					if (line[i] == VERT) {
						line[i] = LF;
					}
				}
				fputc(c3, destFileDesc);
				fputc(c4, destFileDesc);
				fputs(line, destFileDesc);
				++(*stringsCount);
			}
		}
		free(line);
	}

	return *stringsCount;
}

// Convert .txt back to .dat
void txt2dat(char* srcFilename, char* destFilename) {
	printf("Converting .txt to .dat ...\n\n");

	// check source file (.txt)
	FILE* srcFileDesc = fopen(srcFilename, "r");
	if (!srcFileDesc) {
		printf("ERROR: could not open \"%s\" for reading !\n", srcFilename);
		exit(ERROR_RW);
	}

	// check destination file (.dat)
	FILE* destFileDesc = fopen(destFilename, "wb");
	if (!destFileDesc) {
		printf("ERROR: could not open \"%s\" for writing !\n", destFilename);
		fclose(srcFileDesc);
		exit(ERROR_RW);
	}

	// Start from the 4th byte (index starts from 0) of the output file.
	// The first 4 bytes are reserved for specifying the number of total strings, which will be written finally.
	fseek(destFileDesc, 4, SEEK_SET);

	unsigned char c1, c2, c3, c4;

	// detect and avoid EFBB BF bytes from the .txt file
	c1 = fgetc(srcFileDesc);
	c2 = fgetc(srcFileDesc);
	c3 = fgetc(srcFileDesc);
	if ((c1 == 0xEF) && (c2 == 0xBB) && (c3 == 0xBF)) {
		printf("Detected and avoided the header EFBB BF from the .txt file.\n");
	}
	else {
		rewind(srcFileDesc);
	}

	// process all strings in the .txt file
	unsigned int stringsCount = 0;
	str2dat(srcFileDesc, destFileDesc, &stringsCount);

	// Now we need to write total strings information in the first 4 bytes.

	// go back to beginning
	rewind(destFileDesc);

	// put number of total strings in the first 4 bytes
	unsignedIntToFourBytes(stringsCount, &c1, &c2, &c3, &c4);
	fputc(c1, destFileDesc);
	fputc(c2, destFileDesc);
	fputc(c3, destFileDesc);
	fputc(c4, destFileDesc);

	// finish
	printf("Uh yeah, it's done!\n");
	printf("Total strings: %d\n\n", stringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
}

int main(int argc, char *argv[]) {
	char srcFilename[LARGE_SPACE_SIZE], destFilename[LARGE_SPACE_SIZE];

	// Program title
	printf("\n=== Ancient Empires II language file text converter v0.1b ===\n\n");

	// Check if the user has entered 3 parameters
	if (argc < 3) {
		help();
		return ERROR_ARGS;
	}

	// Check source and destination file names
	strncpy(srcFilename, argv[1], LARGE_SPACE_SIZE - 1);
	unsigned int srcFilenameLen = strlen(srcFilename);
	strncpy(destFilename, argv[2], LARGE_SPACE_SIZE - 1);
	unsigned int destFilenameLen = strlen(destFilename);
	if ((srcFilenameLen < 1) || (destFilenameLen < 1)) {
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
