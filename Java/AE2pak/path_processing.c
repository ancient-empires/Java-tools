#include <string.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "path_processing.h"

// Convert Windows to Unix path (modify in-place), and return pointer to the converted path.
char* Windows2UnixPath(char* path) {
	strrep(path, BACKSLASH, SLASH);
	return path;
}

// Convert Unix to Windows path (modify in-place), and return pointer to the converted path.
char* Unix2WindowsPath(char* path) {
	strrep(path, SLASH, BACKSLASH);
	return path;
}

// Get filename from path, and modify path in-place.
char* getFilename(char** pPath) {

	Windows2UnixPath(*pPath);

	char* lastSlash = strrchr(*pPath, SLASH);
	if (!lastSlash) {
		// if slash is not found, then just use the whole path as filename
		return *pPath;
	}

	// find start of filename (after the last '/')
	char* filenameStart = lastSlash + 1;

	// overwrite original path
	size_t filenameLen = strlen(filenameStart);
	memmove(*pPath, filenameStart, filenameLen);

	// reallocate path
	*pPath = realloc(*pPath, filenameLen * sizeof(char));
	return *pPath;
}
