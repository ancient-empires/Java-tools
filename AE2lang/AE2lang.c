#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LARGE_SPACE_SIZE 2048
#define BYTE_CAP 256

#define VERT 0x7C
#define CR 0x0D
#define LF 0x0A

#define DAT ".dat"
#define DAT_LEN strlen(DAT)
#define TXT ".txt"
#define TXT_LEN strlen(TXT)

// Show help when user enters invalid arguments in command line
void help(void) {
	printf(" byblo - 200x - http://go.to/byblo - byblo@hotmail.com\n");
	printf("\n please use the following syntax :\n");
	printf(" AE2lang lang.dat lang.txt (dat2txt)\n");
	printf("  or\n");
	printf(" AE2lang lang.txt lang.dat (txt2dat)\n\n");
	printf(" Note that the appropriate function is selected\n");
	printf("  looking at the files extensions (minuscule only)\n\n");
	exit(0);
}

// Convert DAT to TXT
void dat2txt(char* srcFilename, char* destFilename) {
	printf("\n Converting DAT to TXT...\n\n");

	// Check source file (.dat)
	FILE* srcFileDesc = fopen(srcFilename,"rb");
	if (!srcFileDesc) {
		printf(" Error, could not open %s for reading !\n", srcFilename);
		exit(0);
	}

	// Get number of total strings
	// number of total strings is specified in the first 4 bytes
	unsigned char o1, o2, o3, o4;
	o1 = getc(srcFileDesc);
	o2 = getc(srcFileDesc);
	o3 = getc(srcFileDesc);
	o4 = getc(srcFileDesc);
	int totalStrings = o1 * pow(BYTE_CAP, 3) + o2 * pow(BYTE_CAP, 2) + o3 * BYTE_CAP + o4;
	printf("Number of total strings: %d\n", totalStrings);
	if (totalStrings < 1) {
		// incorrect format
		fclose(srcFileDesc);
		printf("\nError, incorrect format ? : %d (totalStrings announced in 4 first bytes)\n\n", totalStrings);
		exit(0);
	}

	// Check destination file (.txt)
	FILE* destFileDesc = fopen(destFilename, "wb");
	if (!destFileDesc) {
		fclose(srcFileDesc);
		printf("Error, could not open %s for writing !\n", destFilename);
		exit(0);
	}

	// Inits
	int totalStringsCount = 0;
	char sdata[LARGE_SPACE_SIZE];

	// process all strings
	for (int strIdx = 0; strIdx < totalStrings; ++strIdx) {
		// first check string validity
		o3 = getc(srcFileDesc);
		o4 = getc(srcFileDesc);
		int textLen = o3 * BYTE_CAP + o4;
		if ((textLen < 1) && (textLen != 0)) {
			long int currentPos = ftell(srcFileDesc);
			printf("Error when getting length for text no. %d at offset : %ld\n", strIdx, currentPos);
			fclose(srcFileDesc);
			fclose(destFileDesc);
			exit(0);
		}

		// then process each character of the string
		int charIdx = 0;
		for (; charIdx < textLen; ++charIdx) {
			o1 = getc(srcFileDesc);
			// Use '|' character to designate '\n' in each text field
			if (o1 == LF) {
				o1 = VERT;
			}

			// check if reached the end of file unexpectedly
			if (feof(srcFileDesc) && ((textLen + 1) != totalStrings)) {
				long int currentPos = ftell(srcFileDesc);
				printf("Reached end of file unexpected when reading text no. %d at offset : %ld\n", strIdx, currentPos);
				fclose(srcFileDesc);
				fclose(destFileDesc);
				exit(0);
			}

			// put string in buffer
			sdata[charIdx] = o1;
		}
		sdata[charIdx] = '\0'; // NULL byte

		// put CRLF at the end of each line, for each string
		fputs(sdata, destFileDesc);
		putc(CR, destFileDesc);
		putc(LF, destFileDesc);
		totalStringsCount++;
	}

	// finish
	printf(" Uh yeah, its done! %d/%d (anounced/extracted)\n", totalStrings, totalStringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
	exit(0);
}

// Convert TXT back to DAT
void txt2dat(char* srcFilename, char* destFilename) {
	printf("\n Converting TXT to DAT...\n\n");

	// Check source file (.txt)
	FILE* srcFileDesc = fopen(srcFilename, "rb");
	if (!srcFileDesc) {
		printf("Error, could not open \"%s\" for reading !\n", srcFilename);
		exit(0);
	}

	// Check destination file (.dat)
	FILE* destFileDesc = fopen(destFilename, "wb");
	if (!destFileDesc) {
		printf("Error, could not open \"%s\" for writing !\n", destFilename);
		fclose(srcFileDesc);
		exit(0);
	}

	// Put placeholder characters (not valid in UTF-8) in the first 4 bytes
	// They will be later replaced by valid bytes specifying total number of strings
	for (int i = 0; i < 4; ++i) {
		putc(0xff, destFileDesc);
	}

	unsigned char o1, o2, o3, o4;

	// Detect and avoid EFBB BF bytes
	o1 = getc(srcFileDesc);
	o2 = getc(srcFileDesc);
	o3 = getc(srcFileDesc);
	if ((o1==0xEF) && (o2==0xBB) && (o3==0xBF)) {
		printf("Detected and avoided the header EFBB BF from the text file.\n");
	}
	else {
		rewind(srcFileDesc);
	}

	// inits
	char sdata[LARGE_SPACE_SIZE];
	sdata[0] = 0;
	int l = 0;
	int totalStringsCount = 0;
	unsigned char ignoreLine = 0;

	// read and process the TXT file
	while (!feof(srcFileDesc)) {
		unsigned char o1 = getc(srcFileDesc);
		if ((l == 0) && (o1 == 0x5E)) {
			ignoreLine = 1;
		}
		// restore the "|" in 0x0a
		if (o1 == VERT) {
			o1 = 0x0a;
		}
		if (o1 == CR) {
			o1 = getc(srcFileDesc);
			if (o1 == VERT) {
				o1 = 0x0a;
			}
			if (o1 == LF) {
				sdata[l] = 0;
				int k = strlen(sdata);
				unsigned char o3 = k / BYTE_CAP;
				int j = o3 * BYTE_CAP;
				k = k - j;
				unsigned char o4 = k;
				if (ignoreLine != 1) {
					putc(o3, destFileDesc);
					putc(o4, destFileDesc);
					fputs(sdata, destFileDesc);
					totalStringsCount++;
				}
				l = 0;
				ignoreLine = 0;
			}
			else {
				sdata[l] = o1;
				++l;
			}
		}
		else {
			sdata[l] = o1;
			++l;
		}
	}

	// now we need to write total strings information in the first 4 bytes

	// go back to beginning
	rewind(destFileDesc);

	// put number of total strings in the first 4 bytes

	// first 2 bytes
	putc(0x0,destFileDesc);
	putc(0x0,destFileDesc);

	// 3rd and 4th bytes
	o3 = totalStringsCount / BYTE_CAP;
	o4 = totalStringsCount % BYTE_CAP;
	putc(o3,destFileDesc);
	putc(o4,destFileDesc);

	// finish
	printf(" Uh yeah, its done! %d (strings count)\n",totalStringsCount);
	fclose(srcFileDesc);
	fclose(destFileDesc);
	exit(0);
}

void str2dat(void) {
	printf("\n Inserting string to dat... TODO\n\n");
	exit(0);
}

int main(int argc, char *argv[]) {
	char srcFilename[LARGE_SPACE_SIZE], destFilename[LARGE_SPACE_SIZE];

	// Program title
	printf("\n Ancient Empires II language file text converter v0.1b\n\n");

	// Check if the user has entered 3 parameters
	if (argc < 3) {
		help();
		exit(0);
	}

	// Check source and destination file names
	strcpy(srcFilename, argv[1]);
	int srcFilenameLen = strlen(srcFilename);
	strcpy(destFilename, argv[2]);
	int destFilenameLen = strlen(destFilename);
	if (srcFilenameLen < 1) {
		help();
	}
	if (destFilenameLen < 1) {
		help();
	}

	// DAT to TXT conversion
	if (!strncmp(&srcFilename[srcFilenameLen - DAT_LEN], DAT, DAT_LEN)
		&& !strncmp(&destFilename[destFilenameLen - TXT_LEN], TXT, TXT_LEN)) {
		dat2txt(srcFilename, destFilename);
		return 0;
	}

	// TXT to DAT conversion
	else if (!strncmp(&srcFilename[srcFilenameLen - TXT_LEN], TXT, TXT_LEN)
		&& !strncmp(&destFilename[destFilenameLen - DAT_LEN], DAT, DAT_LEN)) {
		txt2dat(srcFilename, destFilename);
		return 0;
	}

	// If we get invalid arguments
	// then just show help, and return 0
	help();
	return 0;
}
