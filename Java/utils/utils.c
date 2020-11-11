#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "utils.h"

// Represent an integer with four bytes
// Modify the four bytes in-place
void unsignedIntToFourBytes(unsigned int i, unsigned char* c1, unsigned char* c2, unsigned char* c3, unsigned char* c4) {
	*c4 = i & 0xFF;
	*c3 = (i >>= CHAR_BIT) & 0xFF;
	*c2 = (i >>= CHAR_BIT) & 0xFF;
	*c1 = (i >>= CHAR_BIT) & 0xFF;
}

// Concatenate four bytes into an integer
unsigned int fourBytesToUnsignedInt(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4) {
	unsigned int i = c1;
	i = (i << CHAR_BIT) + c2;
	i = (i << CHAR_BIT) + c3;
	i = (i << CHAR_BIT) + c4;
	return i;
}

// Reverse the input string in-place
char* strrev(char* str) {
	size_t len = strlen(str);
	char* buffer = calloc(sizeof(char), len);
	if (!buffer) {
		fprintf(stderr, "ERROR: Failed to allocate memory to reverse string \"%s\".\nExiting.\n", str);
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

// Replace one character with another character
// Modify the input string in-place
char* strrep(char* str, char src, char dest) {
	size_t len = strlen(str);
	for (size_t i = 0; i < len; ++i) {
		if (str[i] == src) {
			str[i] = dest;
		}
	}
	return str;
}
