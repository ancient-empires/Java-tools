#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "system.h"

#define LARGE_SPACE_SIZE 2048

// Show help if user enters invalid arguments
void help(void) {
	fprintf(stderr, "Please use the following syntax:\n");
	fprintf(stderr, "- extract: ./AE2pak.out filename.pak -e <directory/of/extracted/files>\n");
	fprintf(stderr, "- pack: ./AE2pak.out filename.pak -p filelist.txt\n\n");
}

// Get filename from path, and save it in-place.
// Path separator is '\' (Windows convention).
// We can expect that the returned string is shorter than or equal to the input string.
char* getFilename(char* path) {
	char* lastBackslash = strrchr(path, BACKSLASH);
	if (!lastBackslash) {
		return path;
	}

	char* filename = &lastBackslash[1];
	size_t filenameLen = strlen(filename);
	memmove(path, filename, filenameLen + 1); // include '\0' at the end

	return path;
}

char extract(char* fo2s, char* fn2s, unsigned long int filepos, unsigned long int filesize) {
	FILE *fo2, *fn2;
	unsigned int i;
	unsigned char o1;

	fo2 = fopen(fo2s, "rb");
	if (!fo2) {
		fprintf(stderr, "ERROR: Could not open \"%s\" for extraction!\n", fo2s);
		return 0;
	}
	rewind(fo2);
	fseek(fo2, filepos, 0);

	fn2 = fopen(fn2s, "wb");
	if (!fn2) {
		fprintf(stderr, "ERROR: Could not open \"%s\" for writing !\n", fn2s);
		fclose(fo2);
		return 0;
	}

	for (i = 0; i < filesize; ++i) {
		o1 = getc(fo2);
		if (feof(fo2)) {
			fprintf(stderr, "ERROR: EOF reached when extracting!\n Check your file !");
			fclose(fo2);
			fclose(fn2);
			return 0;
		}
		putc(o1, fn2);
	}

	fclose(fo2);
	fclose(fn2);
	return 1;
}


int main(int argc, char *argv[]) {
	FILE *fo, *fo3, *fn, *fl;
	unsigned long int i, j, k, fileDataStartPos, totalFiles=0, totalextracted=0, filepos=0, filesize, totalerrors=0;
	unsigned char o1, o2, o3, o4;
	char sdata[LARGE_SPACE_SIZE], sdata3[LARGE_SPACE_SIZE];
	char sdata2[LARGE_SPACE_SIZE][LARGE_SPACE_SIZE];
	unsigned int sdata2s[LARGE_SPACE_SIZE];


	// Program title
	printf("\n=== Ancient Empires II packer / unpacker v0.11b ===\n\n");

	if (argc < 2) {
		help();
		exit(ERROR_ARGS);
	}

	if (!strcmp(argv[2], "-e")) {
		goto extract;
		exit(0);
	}

	if (!strcmp(argv[2], "-p")) {
		goto pack;
		exit(0);
	}

	help();

extract:
	printf("Extracting...\n");
	mkdir(argv[3], MKDIR_DEFAULT_MODE);
	fo = fopen(argv[1], "rb");
	if (!fo) {
		fprintf(stderr, "ERROR: File \"%s\" not found.\n", argv[1]);
		fclose(fo);
		exit(1);
	}
	rewind(fo);
	o1=getc(fo);
	o2=getc(fo);
	fileDataStartPos = o1 * BYTE_CAP + o2;
	printf("File data starts at byte: %ld\n", fileDataStartPos);
	o1=getc(fo);
	o2=getc(fo);
	totalFiles = o1 * BYTE_CAP + o2;
	printf("Total Files announced: %ld\n", totalFiles);

	if (feof(fo)) {
		fclose(fo);
		fprintf(stderr, "ERROR: Unexpected end of file:\n\"%s\"\n", argv[1]);
		exit(1);
	}

	if (argv[3]) {
		strcpy(sdata3, argv[3]);
		k = strlen(sdata3);
		if (sdata3[k-1] == DBQUOTE) {
			sdata3[k-1] = 0;
		}
		if (sdata3[k-1] == BACKSLASH) {
			sdata3[k-1] = 0;
		}
		strcat(sdata3, "/_filelist.txt");
		strcpy(sdata, sdata3);
	}
	else {
		strcpy(sdata, "_filelist.txt");
	}

	printf("Filelist is: \"%s\"\n", sdata);

	fl = fopen(sdata, "wb");

	if (!fl) {
		fprintf(stderr, "ERROR: Logging file \"%s\" cannot be created for writing.\n", sdata);
		fclose(fo);
		fclose(fl);
		exit(1);
	}

	for (i = 0; i < totalFiles; ++i) {
		// get filename length
		o1 = getc(fo);
		o2 = getc(fo);
		k = o1 * BYTE_CAP + o2;
		// get filename
		for (j = 0; j < k; ++j) {
			sdata[j]=getc(fo);
		}
		sdata[j]=0;
		Windows2UnixPath(sdata);

		o1=getc(fo);
		o2=getc(fo);
		o3=getc(fo);
		o4=getc(fo);
		filepos = ((o1*16777216)+(o2*65536)+(o3*256)+o4+fileDataStartPos);
		o1=getc(fo);
		o2=getc(fo);
		filesize = ((o1*BYTE_CAP)+o2);
		if (argv[3]) {
			strcpy(sdata3, argv[3]);
			k = strlen(sdata3);
			if (sdata3[k-1] == DBQUOTE) {
				sdata3[k-1]=0;
			}
			if (sdata3[k-1] == BACKSLASH) {
				sdata3[k-1]=0;
			}
			strcat(sdata3, "/");
			strcat(sdata3, sdata);
			strcpy(sdata, sdata3);
		}
		fprintf(fl, "%s", sdata);
		putc(LF, fl);

		if (!extract(argv[1], sdata, filepos, filesize)) {
			totalerrors++;
		}
		totalextracted++;
	}
	fclose(fl);
	fclose(fo);


	printf("\n Uh yeah, its done! %ld errors for %ld/%ld (extracted/announced)\n", totalerrors, totalextracted, totalFiles);
	exit(0);

pack:
	if (!argv[3]) {
		help();
		exit(ERROR_ARGS);
	}

	printf("Packing...\n");
	fo = fopen(argv[3], "r");
	if (!fo) {
		fprintf(stderr, "ERROR: File %s not found.\n", argv[3]);
		help();
		}
	rewind(fo);
	printf("Checking filelist...\n");
	while(!feof(fo)) {
		sdata[0]=0;
		fgets(sdata, LARGE_SPACE_SIZE, fo);
		k = strlen(sdata);
		if ((k>0) && ((sdata[k-1]==0x0A) || (sdata[k-1]==0x0D))) {
			sdata[k-1]=0;
		}
		if ((strcmp(sdata2[totalFiles], sdata)) && (k>1)) {
			strcpy(sdata2[totalFiles], sdata);
			fo3 = fopen(sdata2[totalFiles], "rb");
			if (!fo3) {
				fprintf(stderr, "ERROR: Could not find \"%s\"\n", sdata2[totalFiles]);
				totalerrors++;
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
	fclose(fo);

	if (totalerrors > 0) {
		printf("Sorry, could not found %ld files, fix the problem before retrying.\n", totalerrors);
		exit(0);
	}
	else if (totalFiles == 0) {
		printf("Nothing to pack. Check your files!\n");
		exit(1);
	}
	else if (totalFiles > LARGE_SPACE_SIZE) {
		printf("Sorry, this crappy exe cannot pack more than %d files!\n", LARGE_SPACE_SIZE);
		exit(1);
	}

	fn = fopen(argv[1], "wb");
	if (!fn) {
		fprintf(stderr, "ERROR: Could not open file \"%s\" for writing\n", argv[1]);
		exit(1);
	}


	rewind(fn);
	//r�serv�
	putc(0xFF, fn);
	putc(0xFF, fn);

	o3 = totalFiles / BYTE_CAP;
	o4 = totalFiles % BYTE_CAP;
	putc(o3, fn);
	putc(o4, fn);


	for (i = 0; i < totalFiles; ++i) {
		strcpy(sdata, sdata2[i]);
		getFilename(sdata);
		k = strlen(sdata);
		o3 = k / 256;
		j = o3 * 256;
		k = k-j;
		o4 = k;

		putc(o3, fn);
		putc(o4, fn);

		fputs(sdata, fn);


		// position relative
		k = filepos;
		o1 = k / 16777216;
		j = o1 * 16777216;

		k = k - j;
		o2 = k / 65536;
		j = o2 * 65536;

		k = k - j;
		o3 = k / 256;
		j = o3 * 256;

		k = k-j;
		o4 = k;
		putc(o1, fn);
		putc(o2, fn);
		putc(o3, fn);
		putc(o4, fn);


		// file size
		k = sdata2s[i];
		filepos = (filepos+k);

		o3 = (k/256);
		j = (o3*256);

		k = (k-j);
		o4 = k;

		putc(o3, fn);
		putc(o4, fn);

	}

	// Writing the header of the end position of the header
	fseek(fn, 0, SEEK_END);
	filepos = ftell(fn);
	k = filepos;

	o3 = (k/256);
	j = (o3*256);

	k = (k-j);
	o4 = k;

	rewind(fn);
	putc(o3, fn);
	putc(o4, fn);

	// pack files
	rewind(fn);
	fseek (fn, filepos, 0);

	for (i=0;i<totalFiles;i++) {
		fo = fopen(sdata2[i], "rb");
		if (!fo) {
			fprintf(stderr, "ERROR: Could not open file \"%s\" for reading\n", sdata2[i]);
			fclose(fn);
			exit(1);
		}
		j=0;
		while(!feof(fo)) {
			o1=getc(fo);
			if (!feof(fo)) putc(o1, fn);
			j++;
		}
		if (j - 1 != sdata2s[i]) {
			fprintf(stderr, "ERROR: Could not match size of file! j = %ld\n", j);
		}
		fclose(fo);
	}
	fclose(fn);


	printf("\n Uh yeah, its done! %ld errors for %ld (announced)\n", totalerrors, totalFiles);
	exit(0);
}
