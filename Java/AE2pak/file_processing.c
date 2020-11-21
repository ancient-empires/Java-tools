#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// Get the length to store the header information for each resource file in the .pak file. (excluding the '\0' at the end, since '\0' is not written to the .pak file)
unsigned int getFileInfoLen(const fileinfo_t* pFileInfo) {
	unsigned int totalLen = FILENAME_LEN_BYTES;
	totalLen += strlen(pFileInfo->filename);
	totalLen += FILE_DATA_START_OFFSET_BYTES;
	totalLen += FILE_SIZE_BYTES;
	return totalLen;
}

// Generate the header string for the file info struct.
// Update pFileInfoLen with the length to store the file info (excluding '\0' at the end).
// If pFileInfoLen is NULL, then the function will do nothing on the pointer.
// This will allocate a new string to store the struct.
// The user should call free() afterwards.
char* getFileInfoStr(const fileinfo_t* pFileInfo, unsigned int* pFileInfoLen) {
	// get length of file information (excluding the '\0')
	unsigned int fileInfoLen = getFileInfoLen(pFileInfo);
	if (pFileInfoLen) {
		*pFileInfoLen = fileInfoLen;
	}

	// allocate file info string
	char* fileInfoStr = calloc(fileInfoLen + 1, sizeof(char));
	if (!fileInfoStr) {
		fprintf(stderr, "ERROR: failed to allocate file info string for file \"%s\".\n", pFileInfo->filePath);
		free(fileInfoStr);
		exit(ERROR_RW);
	}

	char* pos = fileInfoStr;

	unsigned char c1, c2, c3, c4;

	// save filename length
	unsigned int filenameLen = pFileInfo->filenameLen;
	uInt32ToFourBytes(filenameLen, &c1, &c2, &c3, &c4);
	char filenameLenStr[FILENAME_LEN_BYTES] = {c3, c4};
	memcpy(pos, filenameLenStr, FILENAME_LEN_BYTES);
	pos += FILENAME_LEN_BYTES;

	// save filename
	const char* filename = pFileInfo->filename;
	memcpy(pos, filename, filenameLen);
	pos += filenameLen;

	// save file data start offset
	unsigned int fileDataStartOffset = pFileInfo->fileDataStartOffset;
	uInt32ToFourBytes(fileDataStartOffset, &c1, &c2, &c3, &c4);
	char fileDataStartOffsetStr[FILE_DATA_START_OFFSET_BYTES] = {c1, c2, c3, c4};
	memcpy(pos, fileDataStartOffsetStr, FILE_DATA_START_OFFSET_BYTES);
	pos += FILE_DATA_START_OFFSET_BYTES;

	// save file size
	unsigned int fileSize = pFileInfo->fileSize;
	uInt32ToFourBytes(fileSize, &c1, &c2, &c3, &c4);
	char fileSizeStr[FILE_SIZE_BYTES] = {c3, c4};
	memcpy(pos, fileSizeStr, FILE_SIZE_BYTES);
	pos += FILE_SIZE_BYTES;

	return fileInfoStr;
}
