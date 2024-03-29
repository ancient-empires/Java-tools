#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../utils/utils.h"
#include "pak_limits.h"
#include "path_processing.h"
#include "file_processing.h"
#include "pack.h"

// Get the file paths in the file list .log file.
// In the file list .log file, each line contains a single path. Each relative path must be relative to the CURRENT WORKING DIRECTORY.
// This function allocates a new string to store the file path.
// The user must call free() to free the memory afterwards.
static char* getFilePath(FILE* fileListDesc) {
    // get file path
    char* filePath = NULL;
    size_t n = 0;
    getline(&filePath, &n, fileListDesc);
    if (filePath) {
        size_t filePathLen = strlen(filePath);
        if ((filePathLen >= 1) && (filePath[filePathLen - 1] == LF)) {
            // replace line ending from '\n' to '\0'
            filePath[filePathLen - 1] = '\0';
        }
    }
    return filePath;
}

// Check all the files that are listed in the file list .log file.
// Update total resource files, total errors, and total file info length.
// If failed to open the file list, or if one or more errors encountered when checking resource files, then abort the program with exit code ERROR_RW.
// Otherwise, return total number of files that are successfully checked.
static unsigned int checkAllFiles(const char* fileListLOG, unsigned int* pTotalResourceFiles, unsigned int* pTotalErrors, unsigned int* pTotalFileInfoLen) {

    printf("Checking all files...\n\n");

    *pTotalResourceFiles = 0;
    *pTotalErrors = 0;
    *pTotalFileInfoLen = FILE_DATA_START_POS_BYTES + TOTAL_NUM_FILES_BYTES;

    unsigned long totalResourceFileSize = 0;

    // open the .log file
    FILE* fileListDesc = fopen(fileListLOG, "r");
    if (!fileListDesc) {
        fprintf(stderr, "Invalid file list: \"%s\"\n", fileListLOG);
        exit(ERROR_RW);
    }

    const char errorHeader[] = "ERROR: Found errors when checking these files:";

    // get all lines, each line containing a single file path
    while (!feof(fileListDesc)) {
        // get file path
        char* filePath = getFilePath(fileListDesc);
        if (feof(fileListDesc)) {
            fclose(fileListDesc);
            free(filePath);
            break;
        }

        // check file size
        long fileSize = getFileSize(filePath);
        if (fileSize < 0) {
            // invalid file
            ++(*pTotalErrors);
            if (*pTotalErrors == 1) {
                fprintf(stderr, "%s\n", errorHeader);
            }
            fprintf(stderr, "[%u] Invalid file path: \"%s\"\n", *pTotalErrors - 1, filePath);
        }
        else if (fileSize > FILE_SIZE_MAX) {
            // file size is too large
            ++(*pTotalErrors);
            if (*pTotalErrors == 1) {
                fprintf(stderr, "%s\n", errorHeader);
            }
            fprintf(stderr, "[%u] File size is too large: \"%s\" (%ld bytes)\n", *pTotalErrors - 1, filePath, fileSize);
        }
        else {
            // file is valid, and file size is valid

            // save file info
            fileinfo_t fileInfo = saveFileInfo(filePath, fileSize);

            if (fileInfo.filenameLen > FILENAME_LEN_MAX) {
                // check if filename is too long
                ++(*pTotalErrors);
                if (*pTotalErrors == 1) {
                    fprintf(stderr, "%s\n", errorHeader);
                }
                fprintf(stderr, "[%u] Filename is too long: \"%s\" (file path: %s) (maximum allowed filename length: %u)\n", *pTotalErrors - 1, fileInfo.filename, filePath, FILENAME_LEN_MAX);
            }
            else if (fileInfo.filenameLen <= 0) {
                // check if filename is empty
                ++(*pTotalErrors);
                if (*pTotalErrors == 1) {
                    fprintf(stderr, "%s\n", errorHeader);
                }
                fprintf(stderr, "[%u] Filename is empty: \"%s\" (file path: %s)\n", *pTotalErrors - 1, fileInfo.filename, filePath);
            }
            else {
                ++(*pTotalResourceFiles);
                unsigned int fileInfoLen = getFileInfoLen(&fileInfo);
                *pTotalFileInfoLen += fileInfoLen;
                totalResourceFileSize += fileSize;
            }
        }

        free(filePath);
    }

    // abort the program if there are any errors
    if (*pTotalErrors > 0) {
        fprintf(stderr, "\nPlease check your file list (%s), and then try again.\n\n", fileListLOG);
        exit(ERROR_RW);
    }
    else if (*pTotalFileInfoLen >= FILE_DATA_START_POS_MAX) {
        fprintf(stderr, "ERROR: File info is too long (%u bytes)\n", *pTotalFileInfoLen);
        fprintf(stderr, "Maximum allowed length of file info: %u bytes.\n", FILE_DATA_START_POS_MAX - 1);
        fprintf(stderr, "Please reduce the length of filenames, and then try again.\n\n");
        exit(ERROR_RW);
    }
    else if (*pTotalResourceFiles > TOTAL_NUM_FILES_MAX) {
        fprintf(stderr, "ERROR: Too many resource files to pack (%u files)\n", *pTotalResourceFiles);
        fprintf(stderr, "Maximum allowed number of resource files: %u.\n\n", TOTAL_NUM_FILES_MAX);
        exit(ERROR_RW);
    }
    else if (totalResourceFileSize > FILE_DATA_START_OFFSET_MAX) {
        fprintf(stderr, "ERROR: Total size of resource files is too large (%lu bytes)\n", totalResourceFileSize);
        fprintf(stderr, "Maximum allowed total size of resource files: %u bytes.\n", FILE_DATA_START_OFFSET_MAX);
        fprintf(stderr, "Please try again with less files to pack.\n\n");
        exit(ERROR_RW);
    }
    else if (*pTotalResourceFiles <= 0) {
        fprintf(stderr, "ERROR: No files to pack. Please check your file list .log file (%s).\n", fileListLOG);
        exit(ERROR_RW);
    }
    else {
        printf("Successfully checked %u files.\n", *pTotalResourceFiles);
        printf("Total errors: %u\n", *pTotalErrors);
        printf("Total file info length: %u\n", *pTotalFileInfoLen);
        return *pTotalResourceFiles;
    }
}

// Get the information of all resource files.
// NOTE: This function does NOT check errors.
// Hence, it shall only be called after calling checkAllFiles().
// This function allocates dynamic memory to store all the files.
// The user must call free() to free the memory afterwards.
static fileinfo_t* readAllResourceFilesInfo(const char* fileListLOG, const unsigned int totalResourceFiles) {
    printf("\nReading information of all resource files...\n");

    fileinfo_t* allResourceFilesInfo = calloc(totalResourceFiles, sizeof(fileinfo_t));

    FILE* fileListDesc = fopen(fileListLOG, "r");
    for (unsigned int i = 0; i < totalResourceFiles; ++i) {
        char* filePath = getFilePath(fileListDesc);
        long fileSize = getFileSize(filePath);
        allResourceFilesInfo[i] = saveFileInfo(filePath, fileSize);
    }
    fclose(fileListDesc);

    printf("\nSuccessfully gathered information for all %u resource files.\n", totalResourceFiles);

    return allResourceFilesInfo;
}

// Free the dynamic array of all resource files info, which is previously created by the readAllResourceFilesInfo() function.
static void freeAllResourceFilesInfo(fileinfo_t* allResourceFilesInfo, const unsigned int totalResourceFiles) {
    for (unsigned int i = 0; i < totalResourceFiles; ++i) {
        free(allResourceFilesInfo[i].filePath);
    }
    free(allResourceFilesInfo);
}

// Copy data from all the resource files to the .pak file.
// NOTE: This function does NOT check errors.
// Hence, it shall only be called after calling checkAllFiles().
static void copyData(FILE* pakFileDesc, const fileinfo_t* allResourceFilesInfo, const unsigned int totalResourceFiles, const unsigned int fileDataStartPos) {
    // set start position for writing
    fseek(pakFileDesc, fileDataStartPos, SEEK_SET);

    // read and copy all resource files
    for (unsigned int i = 0; i < totalResourceFiles; ++i) {
        // open resource file
        const char* resourceFile = allResourceFilesInfo[i].filePath;
        FILE* resourceFileDesc = fopen(resourceFile, "r");

        // copy data
        const long fileSize = getFileSize(resourceFile);
        for (long j = 0; j < fileSize; ++j) {
            char c = fgetc(resourceFileDesc);
            fputc(c, pakFileDesc);
        }

        // close resource file
        fclose(resourceFileDesc);
    }
}

// Create the .pak archive, using file paths specified in the file list .log file.
// The .log file contains the paths to the resource files to be added to the .pak. These paths are relative to the CURRENT WORKING DIRECTORY.
void pack(const char* pakFile, const char* fileListLOG) {
    unsigned int totalResourceFiles = 0;
    unsigned int totalErrors = 0;
    unsigned int totalFileInfoLen = 0;

    printf("Packing...\n\n");

    // Check all resource files present in the file list .log file
    // Update the number of total resource files, total errors, and total file info length.
    checkAllFiles(fileListLOG, &totalResourceFiles, &totalErrors, &totalFileInfoLen);

    // read the information of all resource files
    fileinfo_t* allResourceFilesInfo = readAllResourceFilesInfo(fileListLOG, totalResourceFiles);

    // open the .pak file for writing
    FILE* pakFileDesc = fopen(pakFile, "wb");
    if (!pakFileDesc) {
        fprintf(stderr, "ERROR: Unable to write to .pak file: \"%s\".\n", pakFile);
        exit(ERROR_RW);
    }

    unsigned char c1, c2, c3, c4;

    printf("\nWriting info section...\n");

    // write the file data start position (first 2 bytes)
    const unsigned int fileDataStartPos = totalFileInfoLen;
    uInt32ToFourBytes(fileDataStartPos, &c1, &c2, &c3, &c4);
    fputc(c3, pakFileDesc);
    fputc(c4, pakFileDesc);

    // write the total number of resource files (next 2 bytes)
    uInt32ToFourBytes(totalResourceFiles, &c1, &c2, &c3, &c4);
    fputc(c3, pakFileDesc);
    fputc(c4, pakFileDesc);

    // write the header info part of all resource files
    unsigned int fileDataStartOffset = 0;
    for (unsigned int i = 0; i < totalResourceFiles; ++i) {
        // set the file data start offset of each resource file
        fileinfo_t* pFileInfo = &allResourceFilesInfo[i];
        setFileDataStartOffset(pFileInfo, fileDataStartOffset);
        fileDataStartOffset += pFileInfo->fileSize;

        // write the file header info into the .pak file
        unsigned int fileInfoLen = 0;
        char* fileInfoStr = getFileInfoStr(pFileInfo, &fileInfoLen);
        fwrite(fileInfoStr, sizeof(char), fileInfoLen, pakFileDesc);
        free(fileInfoStr);
    }

    printf("\nWriting info section completed.\n");

    // copy bytes from each resource file to the .pak file
    printf("\nCopying data from %u resource files into \"%s\"...\n", totalResourceFiles, pakFile);
    copyData(pakFileDesc, allResourceFilesInfo, totalResourceFiles, fileDataStartPos);
    printf("\nSuccessfully added %u resource files into \"%s\".\n", totalResourceFiles, pakFile);

    // clean up
    freeAllResourceFilesInfo(allResourceFilesInfo, totalResourceFiles);
    fclose(pakFileDesc);

    // finish
    printf("\nUh yeah, its done!\n");
    printf("Total files packed: %u\n", totalResourceFiles);
    printf("Total errors: %u\n\n", totalErrors);
}
