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

// Get filename from path.
// Return the pointer to character at which the file path starts.
// Return NULL if filePath is NULL.
char* getFilename(char* filePath) {

    if (!filePath) {
        return NULL;
    }

    // convert to Unix path (modify filePath in-place)
    Windows2UnixPath(filePath);

    char* lastSlash = strrchr(filePath, SLASH);
    if (!lastSlash) {
        // if slash is not found, then just use the whole path as filename
        return filePath;
    }
    else {
        // get the start of filename (after the last '/')
        char* filenameStart = lastSlash + 1;
        return filenameStart;
    }
}
