#include <iostream>
#include <fstream>
#include <vector>

#include "endl.hpp"
#include "units.hpp"
#include "UnitProcessor.hpp"
#include "UnitInfo.hpp"

extern "C" {
	#include "../utils/utils.h"
}

void UnitProcessor::extract(const std::string& unitsBinFile, const std::string& extractDir) {
	// vector to store the information of all units
	std::vector<UnitInfo> units(numUnits);

	// initialize input file stream
	std::ifstream inputStream;
	inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		inputStream.open(unitsBinFile, std::ios_base::binary);
	}
	catch (const std::ifstream::failure& error) {
		std::cerr << error.what() << endl;
		std::cerr << "ERROR: Failed to open input file \"" << unitsBinFile << "\"" << endl;
		exit(ERROR_RW);
	}

	// initialize all output file streams
	// check if any stream failed to open
	std::vector<std::string> unitFilePaths(numUnits);
	std::vector<std::ofstream> outputStreams(numUnits);
	unsigned int numOutputErrors = 0;
	for (unsigned int i = 0; i < numUnits; ++i) {
		// get unit path
		auto& unitPath = unitFilePaths.at(i);
		unitPath = extractDir + "/" + unitNames.at(i) + unitExt;
		// setup output stream, check if any errors exist
		auto& outputStream = outputStreams.at(i);
		outputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			outputStream.open(unitPath);
		}
		catch (const std::ifstream::failure& error) {
			std::cerr << "ERROR: Failed to open output file \""
				<< unitPath << "\"" << endl;
			++numOutputErrors;
		}
	}
	if (numOutputErrors > 0) {
		exit(ERROR_RW);
	}

	// process all unit data
	unsigned int i = 0;
	for (; i < numUnits; ++i) {
		auto& unit = units.at(i);
		auto& outputStream = outputStreams.at(i);

		try {
			// read unit data from the .bin file
			unit.read_bin(inputStream);
			// write to the output .unit file
			outputStream << unit << endl;
			std::cout << "Successfully written to: \"" << unitFilePaths.at(i)
				<< "\"" << endl;
		}
		catch (const std::ifstream::failure& error) {
			// Exception: bad data (including unexpectedly reaching the end)
			std::cerr << "ERROR: Bad data encountered when reading file \"" << unitsBinFile << "\"" << endl;
			std::cerr << "Probably unexpectedly reaching the end of file?" << endl;
			std::cerr << "Currently processing: \"" << unitFilePaths.at(i) << "\"" << endl;
			break;
		}
	}
	std::cout << "Success: " << i << endl;
	std::cout << "Failure: " << (numUnits - i) << endl;
}

void UnitProcessor::pack(const std::string& unitsBinFile, const std::string& packDir) {
	// initialize all units
	std::vector<UnitInfo> units(numUnits);

	// initialize all input file streams
	std::vector<std::string> unitFilePaths(numUnits);
	std::vector<std::ifstream> inputStreams(numUnits);

	unsigned int numInputErrors = 0;
	for (unsigned int i = 0; i < numUnits; ++i) {
		// initialize each input stream
		auto& unitPath = unitFilePaths.at(i);
		unitPath = packDir + "/" + unitNames.at(i) + unitExt;
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
				<< unitPath << "\"" << endl;
		}
	}
	if (numInputErrors > 0) {
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
			<< unitsBinFile << "\"" << endl;
		exit(ERROR_RW);
	}

	// process all unit data
	unsigned int i = 0;
	try {
		for (; i < numUnits; ++i) {
			auto& unit = units.at(i);
			auto& inputStream = inputStreams.at(i);
			// read unit data from the .unit file
			inputStream >> unit;
			// write to the .bin file
			unit.write_bin(outputStream);
			std::cout << "Successfully written to: \"" << unitFilePaths.at(i)
				<< "\"" << endl;
		}
	}
	catch (const std::ifstream::failure& error) {
		std::cerr << error.what() << endl;
		std::cerr << "ERROR: Bad data encouuntered when processing file \""
			<< unitFilePaths.at(i) << "\"" << std::endl;
	}

	std::cout << "Success: " << i << endl;
	std::cout << "Failure: " << (numUnits - i) << endl;
}
