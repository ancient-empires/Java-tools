#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "dat2txt.h"
#include "txt2dat.h"

#define DAT ".dat"
#define DAT_LEN strlen(DAT)
#define TXT ".txt"
#define TXT_LEN strlen(TXT)

// Show help when user enters invalid arguments in command line
void help(void) {
    fprintf(stderr, "Please use the following syntax:\n");
    fprintf(stderr, "- dat2txt: ./AE2lang.out lang.dat lang.txt\n");
    fprintf(stderr, "- txt2dat: ./AE2lang.out lang.txt lang.dat\n\n");
    fprintf(stderr, "Note that the appropriate function is selected according to the files extensions (minuscule only).\n\n");
}

/* Usage:
    - dat2txt: ./AE2lang.out lang.dat lang.txt
    - txt2dat: ./AE2lang.out lang.txt lang.dat
*/
int main(int argc, char* argv[]) {

    // Program title
    printf("\n=== Ancient Empires II language file text converter v0.1b ===\n\n");

    // check if the user has entered 3 parameters
    if (argc < 3) {
        help();
        return ERROR_ARGS;
    }

    // check source and destination file names
    char* srcFilename = argv[1];
    size_t srcFilenameLen = strlen(srcFilename);
    char* destFilename = argv[2];
    size_t destFilenameLen = strlen(destFilename);
    if ((srcFilenameLen < DAT_LEN) || (destFilenameLen < TXT_LEN)) {
        help();
        return ERROR_ARGS;
    }

    // .dat to .txt conversion
    if (!strncmp(&srcFilename[srcFilenameLen - DAT_LEN], DAT, DAT_LEN)
        && !strncmp(&destFilename[destFilenameLen - TXT_LEN], TXT, TXT_LEN)) {
        dat2txt(srcFilename, destFilename);
        return 0;
    }
    // .txt to .dat conversion
    else if (!strncmp(&srcFilename[srcFilenameLen - TXT_LEN], TXT, TXT_LEN)
        && !strncmp(&destFilename[destFilenameLen - DAT_LEN], DAT, DAT_LEN)) {
        txt2dat(srcFilename, destFilename);
        return 0;
    }
    // invalid arguments
    else {
        help();
        return ERROR_ARGS;
    }
}
