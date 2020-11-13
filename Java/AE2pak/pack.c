#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "pack.h"

#define LARGE_SPACE_SIZE 2048

static void getFilename(char* sdata) {
	int i, k;
	char sdata2[LARGE_SPACE_SIZE];
	strcpy(sdata2,sdata);
	strrev(sdata2);
	k = strlen(sdata2);
	for (i=0;i<k;i++){
		if (sdata2[i]=='/'){
			sdata2[i]=0;
			break;
		}
	}
	strrev(sdata2);
	strcpy(sdata,sdata2);
	return;
}

// Create the .pak archive, using files specified in the file list.
void pack(const char* pakFile, const char* fileListLOG) {
	FILE *fo3, *fn;
	unsigned long i, j, k, totalFiles=0, fileDataPos=0, totalErrors=0;
	unsigned char c1, c2, c3, c4;
	char sdata[LARGE_SPACE_SIZE];
	char sdata2[LARGE_SPACE_SIZE][LARGE_SPACE_SIZE];
	unsigned int sdata2s[LARGE_SPACE_SIZE];

	printf("Packing...\n");
	FILE* fileListDesc = fopen(fileListLOG, "r");
	if (!fileListDesc) {
		fprintf(stderr, "ERROR: Failed to open file list \"%s\".\n", fileListLOG);
		exit(ERROR_RW);
	}
	rewind(fileListDesc);
	printf("Checking file list...\n");
	while(!feof(fileListDesc)) {
		sdata[0]=0;
		fgets(sdata, LARGE_SPACE_SIZE, fileListDesc);
		k = strlen(sdata);
		if ((k > 0) && ((sdata[k-1] == CR) || (sdata[k-1] == LF))) {
			sdata[k-1]=0;
		}
		if ((strcmp(sdata2[totalFiles], sdata)) && (k>1)) {
			strcpy(sdata2[totalFiles], sdata);
			fo3 = fopen(sdata2[totalFiles], "rb");
			if (!fo3) {
				fprintf(stderr, "ERROR: Could not find \"%s\"\n", sdata2[totalFiles]);
				totalErrors++;
			}
			else {
				rewind(fo3);
				fseek (fo3, 0, SEEK_END);
				sdata2s[totalFiles] = ftell(fo3);
				fclose(fo3);
			}
			totalFiles++;
			if (totalFiles > LARGE_SPACE_SIZE) break;
		}
	}
	fclose(fileListDesc);

	if (totalErrors > 0) {
		printf("Sorry, could not found %ld files, fix the problem before retrying.\n", totalErrors);
		exit(ERROR_RW);
	}
	else if (totalFiles == 0) {
		printf("Nothing to pack. Check your files!\n");
		exit(ERROR_RW);
	}
	else if (totalFiles > LARGE_SPACE_SIZE) {
		printf("Sorry, this crappy exe cannot pack more than %d files!\n", LARGE_SPACE_SIZE);
		exit(ERROR_RW);
	}

	fn = fopen(pakFile, "wb");
	if (!fn) {
		fprintf(stderr, "ERROR: Could not open .pak file \"%s\" for writing!\n", pakFile);
		fclose(fn);
		exit(ERROR_RW);
	}


	rewind(fn);
	//r�serv�
	putc(0xFF, fn);
	putc(0xFF, fn);

	c3 = totalFiles / BYTE_CAP;
	c4 = totalFiles % BYTE_CAP;
	putc(c3, fn);
	putc(c4, fn);


	for (i = 0; i < totalFiles; ++i) {
		strcpy(sdata, sdata2[i]);
		getFilename(sdata);
		k = strlen(sdata);
		c3 = k / 256;
		j = c3 * 256;
		k = k-j;
		c4 = k;

		putc(c3, fn);
		putc(c4, fn);

		fputs(sdata, fn);


		// position relative
		k = fileDataPos;
		c1 = k / 16777216;
		j = c1 * 16777216;

		k = k - j;
		c2 = k / 65536;
		j = c2 * 65536;

		k = k - j;
		c3 = k / 256;
		j = c3 * 256;

		k = k-j;
		c4 = k;
		putc(c1, fn);
		putc(c2, fn);
		putc(c3, fn);
		putc(c4, fn);


		// file size
		k = sdata2s[i];
		fileDataPos = (fileDataPos+k);

		c3 = (k/256);
		j = (c3*256);

		k = (k-j);
		c4 = k;

		putc(c3, fn);
		putc(c4, fn);

	}

	// Writing the header of the end position of the header
	fseek(fn, 0, SEEK_END);
	fileDataPos = ftell(fn);
	k = fileDataPos;

	c3 = (k/256);
	j = (c3*256);

	k = (k-j);
	c4 = k;

	rewind(fn);
	putc(c3, fn);
	putc(c4, fn);

	// pack files
	rewind(fn);
	fseek (fn, fileDataPos, 0);

	for (i=0;i<totalFiles;i++) {
		FILE* fo = fopen(sdata2[i], "rb");
		if (!fo) {
			fprintf(stderr, "ERROR: Could not open file \"%s\" for reading\n", sdata2[i]);
			fclose(fn);
			exit(1);
		}
		j=0;
		while(!feof(fo)) {
			c1=getc(fo);
			if (!feof(fo)) putc(c1, fn);
			j++;
		}
		if (j - 1 != sdata2s[i]) {
			fprintf(stderr, "ERROR: Could not match size of file! j = %ld\n", j);
		}
		fclose(fo);
	}
	fclose(fn);


	printf("\n Uh yeah, its done! %ld errors for %ld (announced)\n", totalErrors, totalFiles);
}
