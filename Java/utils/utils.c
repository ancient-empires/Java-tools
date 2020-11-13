#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "utils.h"

// Represent a 32-bit unsigned integer as four bytes, and modify the four bytes in-place.
void unsignedIntToFourBytes(const uint32_t i, unsigned char* c1, unsigned char* c2, unsigned char* c3, unsigned char* c4) {
	uint32_t j = i;
	*c4 = j & 0xFF;
	*c3 = (j >>= CHAR_BIT) & 0xFF;
	*c2 = (j >>= CHAR_BIT) & 0xFF;
	*c1 = (j >>= CHAR_BIT) & 0xFF;
}

// Concatenate four bytes into a 32-bit unsigned integer.
uint32_t fourBytesToUnsignedInt(const unsigned char c1, const unsigned char c2, const unsigned char c3, const unsigned char c4) {
	uint32_t i = c1;
	i = (i << CHAR_BIT) + c2;
	i = (i << CHAR_BIT) + c3;
	i = (i << CHAR_BIT) + c4;
	return i;
}

// Reverse the input string in-place.
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

// Replace one character with another character in the input string, and modify the input string in-place.
char* strrep(char* str, const char src, const char dest) {
	size_t len = strlen(str);
	for (size_t i = 0; i < len; ++i) {
		if (str[i] == src) {
			str[i] = dest;
		}
	}
	return str;
}
