#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "txt2dat.h"

// Convert all strings from TXT to DAT format (internal use only)
// srcFileDesc: the .txt file descriptor (read from)
// destFileDesc: the .dat file descriptor (write to)
// return: the total number of strings converted
static uint32_t str2dat(FILE* srcFileDesc, FILE* destFileDesc, uint32_t* pStringsCount) {
    unsigned char c1, c2, c3, c4;

    // detect and avoid "EF BB BF" bytes at the beginning of the .txt file
    c1 = fgetc(srcFileDesc);
    c2 = fgetc(srcFileDesc);
    c3 = fgetc(srcFileDesc);
    if ((c1 == 0xEF) && (c2 == 0xBB) && (c3 == 0xBF)) {
        printf("Detected and avoided the header EFBB BF from the .txt file.\n");
    }
    else {
        rewind(srcFileDesc);
    }

    // For the output .dat file, start from the 4th byte (index starts from 0).
    // The first 4 bytes are reserved for specifying the number of total strings, which will be written finally.
    fseek(destFileDesc, TOTAL_NUM_STRS_BYTES, SEEK_SET);

    *pStringsCount = 0;

    while (!feof(srcFileDesc)) {
        char* line = NULL;
        size_t n = 0;
        size_t lineLen = getline(&line, &n, srcFileDesc);

        // lineLen > 0 if getline gets a valid line
        // lineLen == -1 if getline fails
        if (lineLen > 0) {
            // ignore lines starting with '^'
            bool ignoreLine = line[0] == IGNORED_LINE_START;

            if (!ignoreLine) {
                // replace '\n' with '\0' for writing to .dat
                --lineLen;
                line[lineLen] = '\0';

                // check number of characters in the line
                uInt32ToFourBytes(lineLen, &c1, &c2, &c3, &c4);
                if (c1 || c2) {
                    fprintf(stderr, "ERROR: line content \"%s\" is too long to fit in\n", line);
                    free(line);
                    continue;
                }

                // write lines to .dat file
                for (size_t i = 0; i < lineLen; ++i) {
                    // revert back the '|' character in .txt into '\n' in .dat
                    if (line[i] == VERT) {
                        line[i] = LF;
                    }
                }
                // length of each text field takes 2 bytes
                fputc(c3, destFileDesc);
                fputc(c4, destFileDesc);
                // write back text field
                fputs(line, destFileDesc);

                ++(*pStringsCount);
            }
        }
        free(line);
    }

    // Now we need to write total strings information in the first 4 bytes.

    // go back to beginning
    rewind(destFileDesc);

    // put number of total strings in the first 4 bytes
    uInt32ToFourBytes(*pStringsCount, &c1, &c2, &c3, &c4);
    fputc(c1, destFileDesc);
    fputc(c2, destFileDesc);
    fputc(c3, destFileDesc);
    fputc(c4, destFileDesc);

    return *pStringsCount;
}

// Convert .txt back to .dat
void txt2dat(const char* srcFilename, const char* destFilename) {
    printf("Converting .txt to .dat ...\n\n");

    // check source file (.txt)
    FILE* srcFileDesc = fopen(srcFilename, "r");
    if (!srcFileDesc) {
        fprintf(stderr, "ERROR: could not open \"%s\" for reading!\n", srcFilename);
        exit(ERROR_RW);
    }

    // check destination file (.dat)
    FILE* destFileDesc = fopen(destFilename, "wb");
    if (!destFileDesc) {
        fprintf(stderr, "ERROR: could not open \"%s\" for writing!\n", destFilename);
        fclose(srcFileDesc);
        exit(ERROR_RW);
    }

    // process all strings in the .txt file, and write to the .dat file
    uint32_t stringsCount = 0;
    str2dat(srcFileDesc, destFileDesc, &stringsCount);

    // finish
    printf("Uh yeah, it's done!\n");
    printf("Total strings: %u\n\n", stringsCount);
    fclose(srcFileDesc);
    fclose(destFileDesc);
}
