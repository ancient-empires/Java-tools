#ifndef UTILS_H
#define UTILS_H

#include <limits.h>

#define BYTE_CAP (((unsigned int)1) << CHAR_BIT)

#define ERROR_RW -1
#define ERROR_ARGS 1

// special characters
#define VERT '|' // 0x7C
#define CR '\r' // 0x0D
#define LF '\n' // 0x0A
#define CARET '^' // 0x5E
#define DBQUOTE '"' // 0x22
#define SLASH '/' // 0x2F
#define BACKSLASH '\\' // 0x5C

// Represent an integer with four bytes
// Modify the four bytes in-place
void unsignedIntToFourBytes(const unsigned int i, unsigned char* c1, unsigned char* c2, unsigned char* c3, unsigned char* c4);

// Concatenate four bytes into an integer
unsigned int fourBytesToUnsignedInt(const unsigned char c1, const unsigned char c2, const unsigned char c3, const unsigned char c4);

// Reverse the input string in-place
char* strrev(char* str);

// Replace one character with another character
// Modify the input string in-place
char* strrep(char* str, const char src, const char dest);

#endif
