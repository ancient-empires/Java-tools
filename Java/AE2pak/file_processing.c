#include <stdio.h>

#include "../utils/utils.h"

// get the size of the file at specified path
// in case of error, return ERROR_RW
long getFileSize(const char* filePath) {
	FILE* fileDesc = fopen(filePath, "rb");
	if (!fileDesc) {
		fclose(fileDesc);
		return ERROR_RW;
	}

	fseek(fileDesc, 0, SEEK_END);
	long fileSize = ftell(fileDesc);
	fclose(fileDesc);
	return fileSize;
}
