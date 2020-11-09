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

// Convert DAT to TXT
void dat2txt(char* srcFilename, char* destFilename) {
	printf("Converting DAT to TXT...\n\n");

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
	c1 = getc(srcFileDesc);
	c2 = getc(srcFileDesc);
	c3 = getc(srcFileDesc);
	c4 = getc(srcFileDesc);
	unsigned int totalStrings = fourBytesToUnsignedInt(c1, c2, c3, c4);
	printf("Number of total strings: %d\n\n", totalStrings);
	if (totalStrings < 1) {
		// incorrect format
		fclose(srcFileDesc);
		printf("\nERROR: incorrect format: %d (totalStrings announced in 4 first bytes)\n\n", totalStrings);
		exit(ERROR_RW);
	}

	// Inits
	unsigned int stringsCount = 0;
	char buffer[LARGE_SPACE_SIZE];

	// Process all strings
	for (int strIdx = 0; strIdx < totalStrings; ++strIdx) {
		// First check string validity.
		// For each text field in the .dat file, the first two bytes indicate string length in bytes.
		c3 = getc(srcFileDesc);
		c4 = getc(srcFileDesc);
		unsigned int textLen = fourBytesToUnsignedInt(0, 0, c3, c4);

		// Then process each character of the string, one by one.
		int charIdx = 0;
		for (; charIdx < textLen; ++charIdx) {
			c1 = getc(srcFileDesc);
			// In the extracted txt file, use '|' character to designate '\n' in each text field in the UI.
			if (c1 == LF) {
				c1 = VERT;
			}

			// check if reached the end of file unexpectedly
			if (feof(srcFileDesc) && ((textLen + 1) != totalStrings)) {
				long int currentPos = ftell(srcFileDesc);
				printf("ERROR: Reached end of file unexpected when reading text no. %d at offset %ld\n", strIdx, currentPos);
				fclose(srcFileDesc);
				fclose(destFileDesc);
				exit(ERROR_RW);
			}

			// Put extracted string in buffer, in order to write into TXT.
			buffer[charIdx] = c1;
		}
		buffer[charIdx] = '\0';

		// Put LF at the end of each line, for each string.
		fputs(buffer, destFileDesc);
		putc(LF, destFileDesc);
		++stringsCount;
	}

	// finish
	printf("Uh yeah, its done!\n");
	printf("Announced: %d; Extracted: %d\n\n", totalStrings, stringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
}

void str2dat(void) {
	printf("Inserting strings to dat...\n\n");
}

// Convert TXT back to DAT
void txt2dat(char* srcFilename, char* destFilename) {
	printf("Converting TXT to DAT...\n\n");

	// Check source file (.txt)
	FILE* srcFileDesc = fopen(srcFilename, "r");
	if (!srcFileDesc) {
		printf("ERROR: could not open \"%s\" for reading !\n", srcFilename);
		exit(ERROR_RW);
	}

	// Check destination file (.dat)
	FILE* destFileDesc = fopen(destFilename, "wb");
	if (!destFileDesc) {
		printf("ERROR: could not open \"%s\" for writing !\n", destFilename);
		fclose(srcFileDesc);
		exit(ERROR_RW);
	}

	// Put placeholder characters (not valid in UTF-8) in the first 4 bytes
	// They will be later replaced by valid bytes specifying total number of strings
	for (int i = 0; i < 4; ++i) {
		putc(0xff, destFileDesc);
	}

	unsigned char c1, c2, c3, c4;

	// Detect and avoid EFBB BF bytes
	c1 = getc(srcFileDesc);
	c2 = getc(srcFileDesc);
	c3 = getc(srcFileDesc);
	if ((c1 == 0xEF) && (c2 == 0xBB) && (c3 == 0xBF)) {
		printf("Detected and avoided the header EFBB BF from the text file.\n");
	}
	else {
		rewind(srcFileDesc);
	}

	// Inits
	char buffer[LARGE_SPACE_SIZE];
	buffer[0] = 0;
	unsigned int bufferPos = 0;

	unsigned int stringsCount = 0;

	bool ignoreLine = false;

	str2dat();

	// Read and process the TXT file, character by character.
	while (!feof(srcFileDesc)) {
		c1 = getc(srcFileDesc);

		if ((bufferPos == 0) && (c1 == CARET)) {
			// ignore lines starting with ^ character
			ignoreLine = true;
		}

		if (c1 == LF) {
			// process line endings in TXT
			buffer[bufferPos] = 0;
			unsigned int bufferLen = bufferPos;
			unsignedIntToFourBytes(bufferLen, &c1, &c2, &c3, &c4);
			if ((c1 != 0) || (c2 != 0)) {
				printf("ERROR: buffer content \"%s\" is too long to fit\n", buffer);
			}
			if (!ignoreLine) {
				putc(c3, destFileDesc);
				putc(c4, destFileDesc);
				fputs(buffer, destFileDesc);
				++stringsCount;
			}
			// line processing finished
			bufferPos = 0;
			ignoreLine = false;
		}
		else {
			// process all characters other than line endings
			if (c1 == VERT) {
				// Convert '|' in TXT to '\n' in DAT
				c1 = LF;
			}
			buffer[bufferPos] = c1;
			++bufferPos;
		}
	}

	// now we need to write total strings information in the first 4 bytes

	// go back to beginning
	rewind(destFileDesc);

	// put number of total strings in the first 4 bytes
	unsignedIntToFourBytes(stringsCount, &c1, &c2, &c3, &c4);
	putc(c1, destFileDesc);
	putc(c2, destFileDesc);
	putc(c3, destFileDesc);
	putc(c4, destFileDesc);

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
	strcpy(srcFilename, argv[1]);
	unsigned int srcFilenameLen = strlen(srcFilename);
	strcpy(destFilename, argv[2]);
	unsigned int destFilenameLen = strlen(destFilename);
	if ((srcFilenameLen < 1) || (destFilenameLen < 1)) {
		help();
		return ERROR_ARGS;
	}

	// DAT to TXT conversion
	if (!strncmp(&srcFilename[srcFilenameLen-DAT_LEN], DAT, DAT_LEN)
		&& !strncmp(&destFilename[destFilenameLen-TXT_LEN], TXT, TXT_LEN)) {
		dat2txt(srcFilename, destFilename);
		return 0;
	}

	// TXT to DAT conversion
	else if (!strncmp(&srcFilename[srcFilenameLen-TXT_LEN], TXT, TXT_LEN)
		&& !strncmp(&destFilename[destFilenameLen-DAT_LEN], DAT, DAT_LEN)) {
		txt2dat(srcFilename, destFilename);
		return 0;
	}

	// If we got invalid arguments, then just show help, and return 0
	help();
	return ERROR_ARGS;
}
