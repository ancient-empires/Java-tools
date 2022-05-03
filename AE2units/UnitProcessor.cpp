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
    inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        inputStream.open(unitsBinFilePath, std::ios_base::binary);
    }
    catch (const std::ifstream::failure& error) {
        std::cerr << error.what() << "\n";
        std::cerr << "ERROR: Failed to open input file \"" << unitsBinFilePath << "\"\n";
        exit(ERROR_RW);
    }

    // initialize all output file streams
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
        outputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            outputStream.open(unitPath);
        }
        catch (const std::ifstream::failure& error) {
            std::cerr << "ERROR: Failed to open output file \""
                << unitPath << "\"\n";
            ++numOutputErrors;
        }
    }
    if (numOutputErrors > 0) {
        std::cout << "Failure: " << numOutputErrors << "\n";
        exit(ERROR_RW);
    }

    // process all unit data
    unsigned int i = 0;
    for (; i < NUM_UNITS; ++i) {
        auto& unit = units.at(i);
        auto& outputStream = outputStreams.at(i);

        try {
            // read unit data from the .bin file
            unit.read_bin(inputStream);
            // write to the output .unit file
            outputStream << unit << "\n";
            std::cout << "Successfully written to: \"" << unitFilePaths.at(i)
                << "\"\n";
        }
        catch (const std::ifstream::failure& error) {
            // Exception: bad data (including unexpectedly reaching the end)
            std::cerr << "ERROR: Bad data encountered when reading file \"" << unitsBinFilePath << "\"\n";
            std::cerr << "Probably unexpectedly reaching the end of file?\n";
            std::cerr << "Currently processing: \"" << unitFilePaths.at(i) << "\"\n";
            break;
        }
    }
    std::cout << "Success: " << i << "\n";
    std::cout << "Failure: " << (NUM_UNITS - i) << "\n";
}

void UnitProcessor::pack(const std::string& unitsBinFile, const std::string& packDir) {
    const std::vector<std::string>& UNIT_NAMES = UnitInfo::UNIT_NAMES;
    const size_t& NUM_UNITS = UnitInfo::NUM_UNITS;

    // initialize all units
    std::vector<UnitInfo> units(NUM_UNITS);

    // initialize all input file streams
    std::vector<std::string> unitFilePaths(NUM_UNITS);
    std::vector<std::ifstream> inputStreams(NUM_UNITS);

    unsigned int numInputErrors = 0;
    for (unsigned int i = 0; i < NUM_UNITS; ++i) {
        // initialize each input stream
        auto& unitPath = unitFilePaths.at(i);
        unitPath = packDir + "/" + UNIT_NAMES.at(i) + UnitInfo::UNIT_EXT;
        auto& inputStream = inputStreams.at(i);
        inputStream.exceptions(std::ifstream::badbit);
        // try to open each input file and initialize streams
        // report any errors
        try {
            inputStream.exceptions(inputStream.exceptions() | std::ifstream::failbit);
            inputStream.open(unitPath);
            inputStream.exceptions(inputStream.exceptions() & (~std::ifstream::failbit));
        }
        catch (const std::ifstream::failure& error) {
            ++numInputErrors;
            std::cerr << "ERROR: Failed to open input file \""
                << unitPath << "\"\n";
        }
    }
    if (numInputErrors > 0) {
        std::cout << "Failure: " << numInputErrors << "\n";
        exit(ERROR_RW);
    }

    // initialize output stream
    std::ofstream outputStream;
    outputStream.exceptions(std::ofstream::failbit);
    try {
        outputStream.open(unitsBinFile, std::ios_base::binary);
    }
    catch (const std::ofstream::failure& error) {
        std::cerr << "ERROR: Failed to open output file \""
            << unitsBinFile << "\"\n";
        exit(ERROR_RW);
    }

    // process all unit data
    unsigned int i = 0;
    try {
        for (; i < NUM_UNITS; ++i) {
            auto& unit = units.at(i);
            auto& inputStream = inputStreams.at(i);
            // read unit data from the .unit file
            inputStream >> unit;
            std::cout << "Successfully read from: \"" << unitFilePaths.at(i)
                << "\"\n";
            // write to the .bin file
            unit.write_bin(outputStream);
        }
        std::cout << "Successfully written to: \"" << unitsBinFile
            << "\"\n";
    }
    catch (const std::ifstream::failure& error) {
        std::cerr << error.what() << "\n";
        std::cerr << "ERROR: Bad data encountered when processing file \""
            << unitFilePaths.at(i) << "\"\n";
    }

    std::cout << "Success: " << i << "\n";
    std::cout << "Failure: " << (NUM_UNITS - i) << "\n";
}
