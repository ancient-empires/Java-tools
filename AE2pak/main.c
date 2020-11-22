#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "extract.h"
#include "pack.h"

// Show help if user enters invalid arguments
void help(void) {
	fprintf(stderr, "Please use the following syntax:\n");
	fprintf(stderr, "- extract: ./AE2pak.out filename.pak -e <directory/of/extracted/files> <path/to/file/list.log>\n");
	fprintf(stderr, "- pack: ./AE2pak.out filename.pak -p  <path/to/file/list.log>\n\n");
}

/* Usage:
	- extract: ./AE2pak.out path/to/pak.pak -e path/to/extracted/files (path to extracted files must be created beforehand)
	- pack: ./AE2pak.out path/to/pak.pak -p list/of/extracted/files.log
*/
int main(int argc, char *argv[]) {

	// Program title
	printf("\n=== Ancient Empires II packer / unpacker v0.11b ===\n\n");

	// check number of arguments entered by the user
	if (argc < 4) {
		help();
		return ERROR_ARGS;
	}

	// extract mode
	if (strcmp(argv[2], "-e") == 0) {
		if (argc < 5) {
			help();
			return ERROR_ARGS;
		}
		extract(argv[1], argv[3], argv[4]);
		return 0;
	}
	// pack mode
	else if (strcmp(argv[2], "-p") == 0) {
		pack(argv[1], argv[3]);
		return 0;
	}
	// invalid arguments
	else {
		help();
		return ERROR_ARGS;
	}
}
