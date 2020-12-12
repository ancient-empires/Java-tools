#include <iostream>

#include "endl.hpp"
#include "units.hpp"
#include "UnitProcessor.hpp"

extern "C" {
	#include "../utils/utils.h"
}

void help() {
	std::cerr << "Usage: ./AE2units.out <path/to/units.bin> <extract/directory>" << endl;
}

int main(int argc, char* argv[]) {
	// check arguments
	if (argc < 3) {
		help();
		exit(ERROR_ARGS);
	}

	std::string unitsBinFile = argv[1];
	std::string extractDir = argv[2];
	UnitProcessor::extract(unitsBinFile, extractDir);
	return 0;
}
