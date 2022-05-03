#include <iostream>

#include "UnitInfo.hpp"
#include "UnitProcessor.hpp"

extern "C" {
#include "../utils/utils.h"
}

static void help() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "- Extract mode: ./AE2units.out -e <path/to/units.bin> <extract/directory>" << std::endl;
    std::cerr << "- Pack mode:    ./AE2units.out -p <path/to/units.bin> <pack/directory>" << std::endl << std::endl;

    std::cerr << "The directory of " << UnitInfo::UNIT_EXT
        << " files must contain exactly these "
        << UnitInfo::NUM_UNITS << " files for packing:" << std::endl;
    size_t i = 0;
    for (const auto& unitName : UnitInfo::UNIT_NAMES) {
        std::cerr << i << ". " << unitName << UnitInfo::UNIT_EXT << std::endl;
        ++i;
    }
}

int main(int argc, char* argv[]) {
    // check arguments
    if (argc < 4) {
        help();
        return ERROR_ARGS;
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
    else {
        help();
        return ERROR_ARGS;
    }

    return 0;
}
