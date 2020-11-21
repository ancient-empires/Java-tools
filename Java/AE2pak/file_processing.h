#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include "../utils/utils.h"

// This header file provides utilities to pack the resource files into the .pak file.
// Each resource file is represented as follows at the beginning section of the file:
// 1. filename length (2 bytes)
// 2. filename
// 3. file data start offset (4 bytes)
// 4. file size (2 bytes)

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
// The file data start offset and file size must be provided in advance.
fileinfo_t saveFileInfo(char* filePath, uint16_t fileSize);

// Save the offset value in the file info struct.
// Return the pointer to the file info struct.
fileinfo_t* setFileDataStartOffset(fileinfo_t* pFileInfo, uint32_t offset);

// Get the length to store the header information for each resource file in the .pak file.
unsigned int getFileInfoLen(const fileinfo_t* pFileInfo);

#endif
