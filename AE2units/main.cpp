#include <iostream>

#include "endl.hpp"
#include "units.hpp"
#include "UnitProcessor.hpp"

extern "C" {
	#include "../utils/utils.h"
}

void help() {
	std::cerr << "Usage:" << endl;
	std::cerr << "- Extract mode: ./AE2units.out -e <path/to/units.bin> <extract/directory>" << endl;
	std::cerr << "- Pack mode:    ./AE2units.out -p <path/to/units.bin> <pack/directory>" << endl << endl;

	std::cerr << "The directory of " << UNIT_EXT
		<< " files must contain exactly these "
		<< NUM_UNITS << " files for packing:" << endl;
	for (const auto& unitName: UNIT_NAMES) {
		std::cerr << "- " << unitName << UNIT_EXT << endl;
	}
}

int main(int argc, char* argv[]) {
	// check arguments
	if (argc < 4) {
		help();
		exit(ERROR_ARGS);
	}

	std::string mode = argv[1];
	std::string unitsBinFile = argv[2];
	std::string dir = argv[3];
	if (mode == "-e") {
		UnitProcessor::extract(unitsBinFile, dir);
	}
	else if (mode == "-p") {
		UnitProcessor::pack(unitsBinFile, dir);
	}

	return 0;
}
