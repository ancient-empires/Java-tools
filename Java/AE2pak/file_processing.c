#include <stdio.h>
#include <string.h>

#include "../utils/utils.h"
#include "pak_limits.h"
#include "path_processing.h"
#include "file_processing.h"

// Get the size of the file at specified path.
// In case of error, return ERROR_RW.
// Otherwise, return the size of file in bytes.
long getFileSize(const char* filePath) {
	FILE* fileDesc = fopen(filePath, "rb");
	if (!fileDesc) {
		return ERROR_RW;
	}

	fseek(fileDesc, 0, SEEK_END);
	long fileSize = ftell(fileDesc);
	fclose(fileDesc);
	return fileSize;
}

// Save file info, and store it in a struct.
// The file data start offset and file size must be provided in advance.
fileinfo_t saveFileInfo(char* filePath, uint16_t fileSize) {
	fileinfo_t fileInfo;

	fileInfo.filePath = filePath;
	fileInfo.filename = getFilename(filePath);
	fileInfo.filenameLen = strlen(fileInfo.filename);
	fileInfo.fileDataStartOffset = 0;
	fileInfo.fileSize = fileSize;

	return fileInfo;
}

// Save the offset value in the file info struct.
// Return the pointer to the file info struct.
fileinfo_t* setFileDataStartOffset(fileinfo_t* pFileInfo, uint32_t offset) {
	pFileInfo->fileDataStartOffset = offset;
	return pFileInfo;
}

// Get the length to store the header information for each resource file in the .pak file.
unsigned int getFileInfoLen(const fileinfo_t* pFileInfo) {
	unsigned int totalLen = FILENAME_LEN_BYTES;
	totalLen += strlen(pFileInfo->filename);
	totalLen += FILE_DATA_START_OFFSET_BYTES;
	totalLen += FILE_SIZE_BYTES;
	return totalLen;
}
