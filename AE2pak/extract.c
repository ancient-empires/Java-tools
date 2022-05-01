#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../utils/utils.h"
#include "extract.h"
#include "path_processing.h"

// Extract single file (internal use only)
// Return true if successfully extracted; otherwise return false.
static bool extractFile(const char* pakFile, const char* resourceFile, uint32_t fileDataPos, uint16_t fileSize) {

    // open .pak file for reading
    FILE* pakFileDesc = fopen(pakFile, "rb");
    if (!pakFileDesc) {
        fprintf(stderr, "ERROR: Could not open .pak file \"%s\" for extraction!\n\n", pakFile);
        return false;
    }
    fseek(pakFileDesc, fileDataPos, SEEK_SET);

    // open target file for writing
    FILE* resourceFileDesc = fopen(resourceFile, "wb");
    if (!resourceFileDesc) {
        fprintf(stderr, "ERROR: Could not open resource file \"%s\" for writing!\n\n", resourceFile);
        fclose(pakFileDesc);
        return false;
    }

    // copy bytes from .pak to target file
    for (size_t i = 0; i < fileSize; ++i) {
        unsigned char ch = fgetc(pakFileDesc);
        if (feof(pakFileDesc)) {
            fprintf(stderr, "ERROR: EOF reached in .pak file \"%s\" when extracting! Please check your file!\n\n", pakFile);
            fclose(pakFileDesc);
            fclose(resourceFileDesc);
            return false;
        }
        fputc(ch, resourceFileDesc);
    }

    fclose(pakFileDesc);
    fclose(resourceFileDesc);
    return true;
}

// Extract .pak file to extract directory.
// The extract directory must exist beforehand, or the program will NOT work.
// Write the .log file containing the list of all extracted files as specified in the argument.
void extract(const char* pakFile, const char* extractDir, const char* fileListLOG) {

    printf("Extracting...\n");

    // open .pak file for reading
    FILE* pakFileDesc = fopen(pakFile, "rb");
    if (!pakFileDesc) {
        fprintf(stderr, "ERROR: .pak file \"%s\" not found!\n\n", pakFile);
        exit(ERROR_RW);
    }

    // open the file list .log file for writing
    FILE* fileListDesc = fopen(fileListLOG, "wb");
    if (!fileListDesc) {
        fclose(pakFileDesc);
        fprintf(stderr, "ERROR: file list \"%s\" cannot be created for writing!\n\n", fileListLOG);
        exit(ERROR_RW);
    }
    printf("\nStoring file list in log file: \"%s\"\n\n", fileListLOG);

    unsigned char c1, c2, c3, c4;

    // get starting position of file data (first 2 bytes)
    rewind(pakFileDesc);
    c1 = fgetc(pakFileDesc);
    c2 = fgetc(pakFileDesc);
    uint16_t fileDataStartPos = fourBytesToUInt32(0, 0, c1, c2);
    printf("File data start at byte: %u\n", fileDataStartPos);

    // get number of total files (next 2 bytes)
    c1 = fgetc(pakFileDesc);
    c2 = fgetc(pakFileDesc);
    uint16_t totalFiles = fourBytesToUInt32(0, 0, c1, c2);
    printf("Total Files announced: %u\n", totalFiles);

    // check unexpected ending of .pak file
    if (feof(pakFileDesc)) {
        fclose(pakFileDesc);
        fprintf(stderr, "ERROR: Unexpected end of .pak file:\n\"%s\"\n\n", pakFile);
        fclose(pakFileDesc);
        fclose(fileListDesc);
        exit(ERROR_RW);
    }

    // process all files
    size_t totalExtracted = 0, totalErrors = 0;
    for (size_t i = 0; i < totalFiles; ++i) {
        // get filename length (2 bytes)
        c1 = fgetc(pakFileDesc);
        c2 = fgetc(pakFileDesc);
        size_t filenameLen = fourBytesToUInt32(0, 0, c1, c2);

        // get filename
        char* filename = calloc(filenameLen + 1, sizeof(char));
        for (size_t j = 0; j < filenameLen; ++j) {
            filename[j] = fgetc(pakFileDesc);
        }

        // get file data position (4 bytes)
        c1 = fgetc(pakFileDesc);
        c2 = fgetc(pakFileDesc);
        c3 = fgetc(pakFileDesc);
        c4 = fgetc(pakFileDesc);
        uint32_t fileDataPos = fourBytesToUInt32(c1, c2, c3, c4) + fileDataStartPos;

        // get file size (2 bytes)
        c1 = fgetc(pakFileDesc);
        c2 = fgetc(pakFileDesc);
        uint16_t fileSize = fourBytesToUInt32(0, 0, c1, c2);

        // get the path of the extracted file, relative to current working directory
        // write to file list (.log file)
        size_t extractDirLen = strlen(extractDir);
        char* extractedFilePath = calloc(extractDirLen + 1 /* '/' character */ + filenameLen + 1, sizeof(char));
        strcpy(extractedFilePath, extractDir);
        Windows2UnixPath(extractedFilePath);
        if (extractedFilePath[extractDirLen - 1] == DBQUOTE) {
            extractedFilePath[extractDirLen - 1] = '\0';
        }
        else if (extractedFilePath[extractDirLen - 1] == SLASH) {
            extractedFilePath[extractDirLen - 1] = '\0';
        }
        strcat(extractedFilePath, SLASH_STR);
        strcat(extractedFilePath, filename);

        // extract file
        if (extractFile(pakFile, extractedFilePath, fileDataPos, fileSize)) {
            // write to file list if extraction is successful
            fprintf(fileListDesc, "%s\n", extractedFilePath);
            ++totalExtracted;
        }
        else {
            // failed
            ++totalErrors;
        }

        free(filename);
        free(extractedFilePath);
    }

    // finish
    fclose(pakFileDesc);
    fclose(fileListDesc);

    printf("\nUh yeah, it's done!\n");
    printf("Extracted to: \"%s\"\n", extractDir);
    printf("Written file list .log to: \"%s\"\n", fileListLOG);
    printf("Total files: %u\n", totalFiles);
    printf("Total extracted: %lu\n", totalExtracted);
    printf("Total errors: %lu\n\n", totalErrors);
}
