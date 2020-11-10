#ifndef PATH_PROCESSING_H
#define PATH_PROCESSING_H

#include <string.h>

typedef struct {
	char* dir;
	char* filename;
} filepath_t;

// Normalize path (path to normalize must exist).
// Return pointer to the normalized path.
char* normalizePath(char* path);

// Convert Windows to Unix path, and return pointer to the converted path.
char* Windows2UnixPath(char* path);

// Convert Unix to Windows path, and return pointer to the converted path.
char* Unix2WindowsPath(char* path);

// Split path into directory and filename.
// Automaically convert to Unix path (separator is '/') in the struct, but not in the original path.
// After conversion, the directory will not contain the trailing "/" character.
filepath_t splitPath(char* path);

// Free the filepath struct created by splitPath.
void freeFilepathStruct(filepath_t* filepath);

#endif
