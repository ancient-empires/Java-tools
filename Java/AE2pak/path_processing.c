#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "../utils/utils.h"
#include "path_processing.h"

// convert Windows to Unix path in-placefree(*pPath)
char* Windows2UnixPath(char* path) {
	strrep(path, BACKSLASH, SLASH);
	return path;
}

// convert Unix to Windows path in-place
char* Unix2WindowsPath(char* path) {
	strrep(path, SLASH, BACKSLASH);
	return path;
}

// Split path into directory and filename.
// Automaically convert to Unix path (separator is '/') in the struct, but not in the original path.
// After conversion, the directory will not contain the trailing "/" character.
filepath_t splitPath(char* path) {

	filepath_t filepath;

	// copy path string, and convert to Unix path
	size_t pathLen = strlen(path);
	char* buffer = calloc(pathLen + 1, sizeof(char));
	strcpy(buffer, path);
	Windows2UnixPath(buffer);

	// Find last occurence of '/'.
	// This is the separation of directory and filename.
	char* lastSlash = strrchr(buffer, SLASH);
	if (!lastSlash) {
		// If '/' does not exist, then directory is '.'
		// save directory name
		char dir[] = ".";
		filepath.dir = calloc(strlen(dir) + 1, sizeof(char));
		strcpy(filepath.dir, dir);

		// save filename
		char* filename = buffer;
		filepath.filename = calloc(pathLen + 1, sizeof(char));
		strcpy(filepath.filename, filename);
	}
	else {
		// Save directory name before '/'
		ssize_t dirLen = lastSlash - buffer;
		if (dirLen < 1) {
			// Edge case: directory is just "/"
			// save directory name`
			char dir[] = "/";
			filepath.dir = calloc(strlen(dir) + 1, sizeof(char));
			strcpy(filepath.dir, dir);

			// save filename
			char* filename = &buffer[1];
			filepath.filename = calloc(pathLen, sizeof(char));
			strcpy(filepath.filename, filename);
		}
		else {
			// save directory name
			*lastSlash = '\0';
			char* dir = buffer;
			size_t dirLen = strlen(dir);
			// trace back to remove redundant slashes
			ssize_t i = dirLen - 1;
			for (; i > 0; --i) {
				if (dir[i] == SLASH) {
					dir[i] = '\0';
				}
				else {
					break;
				}
			}
			dirLen = i + 1;
			filepath.dir = calloc(dirLen + 1, sizeof(char));
			strcpy(filepath.dir, dir);

			// save filename
			char* filename = &lastSlash[1];
			size_t filenameLen = strlen(filename);
			filepath.filename = calloc(filenameLen + 1, sizeof(char));
			strcpy(filepath.filename, filename);
		}
	}

	free(buffer);
	return filepath;
}

// Free the filepath struct created by splitPath.
void freeFilepathStruct(filepath_t* pFilepath) {
	free(pFilepath->dir);
	pFilepath->dir = NULL;
	free(pFilepath->filename);
	pFilepath->filename = NULL;
}
