#ifndef PACK_H
#define PACK_H

#include "pak_limits.h"

// Create the .pak archive, using file paths specified in the file list .log file.
// The .log file contains the paths to the resource files to be added to the .pak. These paths are relative to the CURRENT WORKING DIRECTORY.
void pack(const char* pakFile, const char* fileListLOG);

#endif
