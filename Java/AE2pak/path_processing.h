#ifndef PATH_PROCESSING_H
#define PATH_PROCESSING_H

#include <string.h>

// Convert Windows to Unix path (modify in-place), and return pointer to the converted path.
char* Windows2UnixPath(char* path);

// Convert Unix to Windows path (modify in-place), and return pointer to the converted path.
char* Unix2WindowsPath(char* path);

// Get filename from path.
// Return the pointer to character at which the file path starts.
// Return NULL if filePath is NULL.
char* getFilename(char* filePath);

#endif
