#include <iostream>

#include "UnitInfo.hpp"
#include "UnitProcessor.hpp"

extern "C" {
#include "../utils/utils.h"
}

void help() {
  std::cerr << "Usage:\n";
  std::cerr << "- Extract mode: ./AE2units.out -e <path/to/units.bin> <extract/directory>\n";
  std::cerr << "- Pack mode:    ./AE2units.out -p <path/to/units.bin> <pack/directory>\n\n";

  std::cerr << "The directory of " << UnitInfo::UNIT_EXT
    << " files must contain exactly these "
    << UnitInfo::numUnits() << " files for packing:\n";
  for (const auto& unitName : UnitInfo::unitNames()) {
    std::cerr << "- " << unitName << UnitInfo::UNIT_EXT << "\n";
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
