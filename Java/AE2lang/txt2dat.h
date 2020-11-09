#ifndef TXT2DAT_H
#define TXT2DAT_H

// Convert all strings from TXT to DAT format
// srcFileDesc: the .txt file descriptor (read from)
// destFileDesc: the .dat file descriptor (write to)
// return: the total number of strings converted
unsigned int str2dat(FILE* srcFileDesc, FILE* destFileDesc, unsigned int* stringsCount);

// Convert .txt back to .dat
void txt2dat(char* srcFilename, char* destFilename);

#endif
