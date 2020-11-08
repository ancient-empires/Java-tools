#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#include <math.h>

#define BYTE_CAP (size_t)pow(2, CHAR_BIT)

#define ERROR_RW -1
#define ERROR_ARGS 1

// special characters
#define VERT '|' // 0x7C
#define CR '\r' // 0x0D
#define LF '\n' // 0x0A
#define CARET '^' // 0x5E
#define DBQUOTE '"' // 0x22
#define BACKSLASH '\\' // 0x5C

// Represent an integer with four bytes
// Modify the four bytes in-place
void intToFourBytes(int i, unsigned char* c1, unsigned char* c2, unsigned char* c3, unsigned char* c4);

// Concatenate four bytes into an integer
int fourBytesToInt(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);

// Reverse the input string in-place
char* strrev(char* str);

#endif
