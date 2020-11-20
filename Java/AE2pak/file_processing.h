#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include "../utils/utils.h"

// get the size of the file at specified path
// in case of error, return ERROR_RW
long getFileSize(const char* filePath);

#endif
