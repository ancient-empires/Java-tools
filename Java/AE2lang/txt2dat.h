#ifndef TXT2DAT_H
#define TXT2DAT_H

#include "../utils/utils.h"

#define IGNORED_LINE_START CARET

// Convert .txt back to .dat
void txt2dat(char* srcFilename, char* destFilename);

#endif
