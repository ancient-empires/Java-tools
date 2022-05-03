#include <iostream>
#include <fstream>
#include <vector>

#include "UnitProcessor.hpp"
#include "UnitInfo.hpp"

extern "C" {
#include "../utils/utils.h"
}

void UnitProcessor::extract(const std::string& unitsBinFilePath, const std::string& extractDir) {
    const std::vector<std::string>& UNIT_NAMES = UnitInfo::UNIT_NAMES;
    const size_t& NUM_UNITS = UnitInfo::NUM_UNITS;

    // vector to store the information of all units
    std::vector<UnitInfo> units(NUM_UNITS);

    // initialize input file stream
    std::ifstream inputStream;
    inputStream.exceptions(std::ifstream::badbit);
    try {
        inputStream.open(unitsBinFilePath, std::ios_base::binary);
    }
    catch (const std::ifstream::failure& error) {
        std::cerr << error.what() << std::endl;
        std::cerr << "ERROR: Failed to open input file \"" << unitsBinFilePath << "\"" << std::endl;
        exit(ERROR_RW);
    }

    // initialize an output stream for each unit, output to a .unit file
    // check if any stream failed to open
    std::vector<std::string> unitFilePaths(NUM_UNITS);
    std::vector<std::ofstream> outputStreams(NUM_UNITS);
    unsigned int numOutputErrors = 0;
    for (unsigned int i = 0; i < NUM_UNITS; ++i) {
        // get unit path
        auto& unitPath = unitFilePaths.at(i);
        unitPath = extractDir + "/" + UNIT_NAMES.at(i) + UnitInfo::UNIT_EXT;
        // setup output stream, check if any errors exist
        auto& outputStream = outputStreams.at(i);
        outputStream.exceptions(std::ifstream::badbit);
        try {
            outputStream.open(unitPath);
        }
        catch (const std::ifstream::failure& error) {
            std::cerr << "ERROR: Failed to open output file \""
                << unitPath << "\"" << std::endl;
            ++numOutputErrors;
        }
    }
    if (numOutputErrors > 0) {
        std::cout << "Failure: " << numOutputErrors << std::endl;
        exit(ERROR_RW);
    }

    // process all unit data, output to .unit files
    unsigned int i = 0;
    for (; i < NUM_UNITS; ++i) {
        auto& unit = units.at(i);
        auto& outputStream = outputStreams.at(i);

        try {
            // read unit data from the .bin file
            unit.read_bin(inputStream);
            // write to the output .unit file
            outputStream << unit << "\n";
            std::cout << "Successfully written to: \"" << unitFilePaths.at(i) << "\"" << std::endl;
        }
        catch (const std::ifstream::failure& error) {
            // Exception: bad data (including unexpectedly reaching the end)
            std::cerr << "ERROR: Bad data encountered when reading file \""
                << unitsBinFilePath << "\"," << std::endl;
            std::cerr << "Probably unexpectedly reaching the end of file?" << std::endl;
            std::cerr << "Currently extracting to: \""
                << unitFilePaths.at(i) << "\"" << std::endl;
            break;
        }
    }
    std::cout << "Success: " << i << std::endl;
    std::cout << "Failure: " << (NUM_UNITS - i) << std::endl;
}

void UnitProcessor::pack(const std::string& unitsBinFile, const std::string& packDir) {
    const std::vector<std::string>& UNIT_NAMES = UnitInfo::UNIT_NAMES;
    const size_t& NUM_UNITS = UnitInfo::NUM_UNITS;

    // initialize all units
    std::vector<UnitInfo> units(NUM_UNITS);

    // for each .unit file, initialize an input stream from it
    std::vector<std::string> unitFilePaths(NUM_UNITS);
    std::vector<std::ifstream> inputStreams(NUM_UNITS);
    unsigned int numInputErrors = 0;
    for (unsigned int i = 0; i < NUM_UNITS; ++i) {
        // get unit path
        auto& unitPath = unitFilePaths.at(i);
        unitPath = packDir + "/" + UNIT_NAMES.at(i) + UnitInfo::UNIT_EXT;
        // initialize input stream
        auto& inputStream = inputStreams.at(i);
        inputStream.exceptions(std::ifstream::badbit);
        // try to open each input file and initialize streams
        // report any errors
        try {
            inputStream.open(unitPath);
        }
        catch (const std::ifstream::failure& error) {
            ++numInputErrors;
            std::cerr << "ERROR: Failed to open input file \""
                << unitPath << "\"" << std::endl;
        }
    }
    if (numInputErrors > 0) {
        std::cout << "Failure: " << numInputErrors << std::endl;
        exit(ERROR_RW);
    }

    // initialize output stream
    std::ofstream outputStream;
    outputStream.exceptions(std::ofstream::badbit);
    try {
        outputStream.open(unitsBinFile, std::ios_base::binary);
    }
    catch (const std::ofstream::failure& error) {
        std::cerr << "ERROR: Failed to open output file \""
            << unitsBinFile << "\"" << std::endl;
        exit(ERROR_RW);
    }

    // process all unit data, output to the .bin file
    unsigned int i = 0;
    try {
        for (; i < NUM_UNITS; ++i) {
            auto& unit = units.at(i);
            auto& inputStream = inputStreams.at(i);
            // read unit data from the .unit file
            inputStream >> unit;
            std::cout << "Successfully read from: \"" <<
                unitFilePaths.at(i) << "\"" << std::endl;
            // write to the .bin file
            unit.write_bin(outputStream);
        }
        std::cout << "Successfully written to: \"" << unitsBinFile
            << "\"" << std::endl;
    }
    catch (const std::ifstream::failure& error) {
        std::cerr << error.what() << std::endl;
        std::cerr << "ERROR: Bad data encountered when processing file \""
            << unitFilePaths.at(i) << "\"" << std::endl;
    }

    std::cout << "Success: " << i << std::endl;
    std::cout << "Failure: " << (NUM_UNITS - i) << std::endl;
}
