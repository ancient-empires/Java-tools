#ifndef PATH_PROCESSING_H
#define PATH_PROCESSING_H

#include <string.h>

typedef struct {
	char* dir;
	char* filename;
} filepath_t;

// convert Windows to Unix path in-place
char* Windows2UnixPath(char* path);

// convert Windows to Unix path in-place
char* Unix2WindowsPath(char* path);

// Split path into directory and filename.
// Automaically convert to Unix path (separator is '/') in the struct, but not in the original path.
// After conversion, the directory will not contain the trailing "/" character.
filepath_t splitPath(char* path);

// Free the filepath struct created by splitPath.
void freeFilepathStruct(filepath_t* filepath);

#endif
