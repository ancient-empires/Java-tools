#ifndef UTILS_H
#define UTILS_H

#define BYTE_CAP (unsigned int)pow(2, CHAR_BIT)

#define ERROR_RW -1
#define ERROR_ARGS 1

#define VERT '|' // 0x7C
#define CR '\r' // 0x0D
#define LF '\n' // 0x0A
#define CARET '^' // 0x5E

// Represent an integer with four bytes
// Modify the four bytes in-place
void intToFourBytes(int i, unsigned char* c1, unsigned char* c2, unsigned char* c3, unsigned char* c4);

// Concatenate four bytes into an integer
int fourBytesToInt(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);

#endif
