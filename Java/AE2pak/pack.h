#ifndef PACK_H
#define PACK_H

#include "pak_limits.h"

// Create the .pak archive, using file paths specified in the file list .log file.
void pack(const char* pakFile, const char* fileListLOG);

#endif
