#include <limits.h>

#include "utils.h"

// Represent an integer with four bytes
// Modify the four bytes in-place
void intToFourBytes(int i, unsigned char* c1, unsigned char* c2, unsigned char* c3, unsigned char* c4) {
	unsigned int j = *(unsigned int*)&i;
	*c4 = j & 0xFF;
	*c3 = (j >>= CHAR_BIT) & 0xFF;
	*c2 = (j >>= CHAR_BIT) & 0xFF;
	*c1 = (j >>= CHAR_BIT) & 0xFF;
}

// Concatenate four bytes into an integer
int fourBytesToInt(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4) {
	int i = c1;
	i = (i << CHAR_BIT) + c2;
	i = (i << CHAR_BIT) + c3;
	i = (i << CHAR_BIT) + c4;
	return i;
}

// Reverse the input string in-place
char* strrev(char* str) {
	size_t len = strlen(str);
	char* buffer = (char*)calloc(sizeof(char), len);
	if (!buffer) {
		printf("ERROR: Failed to allocate memory to reverse string \"%s\".\nExiting.\n", str);
		exit(ERROR_RW);
	}
	strncpy(buffer, str, len);
	for (int i = 0; i < len; ++i) {
		buffer[i] = str[len-1-i];
	}
	strncpy(str, buffer, len);
	free(buffer);
	return str;
}
