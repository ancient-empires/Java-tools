#include <iostream>

#include "endl.hpp"
#include "units.hpp"
#include "UnitProcessor.hpp"

extern "C" {
	#include "../utils/utils.h"
}

void help() {
	std::cerr << "Usage:" << endl;
	std::cerr << "- Extract mode: ./AE2units.out -e <path/to/units.bin> <path/to/.units/directory>" << endl;
}

int main(int argc, char* argv[]) {
	// check arguments
	if (argc < 4) {
		help();
		exit(ERROR_ARGS);
	}

	std::string unitsBinFile = argv[2];
	std::string extractDir = argv[3];
	UnitProcessor::extract(unitsBinFile, extractDir);
	return 0;
}
