#ifndef TXT2DAT_H
#define TXT2DAT_H

#include "../utils/utils.h"
#include "field_sizes.h"

#define IGNORED_LINE_START CARET

// Convert .txt back to .dat
void txt2dat(const char* srcFilename, const char* destFilename);

#endif
