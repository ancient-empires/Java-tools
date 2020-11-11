#include <stdio.h>

#include "path_processing.h"

int main(void) {
	char test[] = "usr/";
	filepath_t t = splitPath(test);
	printf("%s %s\n", t.dir, t.filename);
	freeFilepathStruct(&t);

	return 0;
}
