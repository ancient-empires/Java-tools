#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include "../utils/utils.h"

typedef struct fileinfo {
	uint16_t filenameLen;
	char* filePath;
	char* filename;
	uint32_t fileDataStartOffset;
	uint16_t fileSize;
} fileinfo_t;

// Get the size of the file at specified path.
// In case of error, return ERROR_RW.
// Otherwise, return the size of file in bytes.
long getFileSize(const char* filePath);

// Save file info, and store it in a struct.
// The file size must be provided in advance.
fileinfo_t saveFileInfo(char* filePath, uint16_t fileSize);

// Get the length to store the information for each resource file in the .pak file.
// Each resource file is represented as follows at the beginning section of the file:
// 1. filename length (2 bytes)
// 2. filename
// 3. file data start offset (4 bytes)
// 4. file size (2 bytes)
unsigned int getFileInfoLen(const fileinfo_t* pFileInfo);

#endif