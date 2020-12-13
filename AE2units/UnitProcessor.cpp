#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>

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

		unsigned char c1, c2;

		try {
			// section 1: basic information
			unit.moveRange = inputStream.get();
			unit.minAttack = inputStream.get();
			unit.maxAttack = inputStream.get();
			unit.defense = inputStream.get();
			unit.maxAttackRange = inputStream.get();
			unit.minAttackRange = inputStream.get();
			c1 = inputStream.get();
			c2 = inputStream.get();
			unit.price = static_cast<short>(fourBytesToUInt32(0, 0, c1, c2));

			// section 2: fight animation
			unsigned int numChars = inputStream.get();
			unit.charPos.resize(numChars);
			for (auto& charPos: unit.charPos) {
				charPos.first = inputStream.get();
				charPos.second = inputStream.get();
			}

			// section 3: unit properties
			unsigned int numProperties = inputStream.get();
			unit.properties.clear();
			for (unsigned int j = 0; j < numProperties; ++j) {
				unsigned short property = inputStream.get();
				unit.properties.emplace(property);
			}

			// write out to output file
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
			unit.properties.clear();

			auto& inputStream = inputStreams.at(i);
			while (!inputStream.eof()) {
				// get line and key
				std::string line, key;
				std::getline(inputStream, line);
				std::istringstream lineStream(line);
				lineStream >> key;

				// section 1: basic information
				if (key == Key::moveRange) {
					lineStream >> unit.moveRange;
				}
				else if (key == Key::attack) {
					lineStream >> unit.minAttack;
					lineStream >> unit.maxAttack;
				}
				else if (key == Key::defense) {
					lineStream >> unit.defense;
				}
				else if (key == Key::attackRange) {
					lineStream >> unit.maxAttackRange;
					lineStream >> unit.minAttackRange;
				}
				else if (key == Key::price) {
					lineStream >> unit.price;
				}

				// section 2: fight animation
				if (key == Key::charCount) {
					unsigned int numChars = 0;
					lineStream >> numChars;
					unit.charPos.resize(numChars);

					// process each CharPos line
					unsigned int j = 0;
					for (; j < numChars; ++j) {
						std::string line;
						do {
							std::getline(inputStream, line);
						} while (line.empty() && !inputStream.eof());

						std::istringstream lineStream(line);
						lineStream >> key;
						if (key == Key::charPos) {
							auto& charPos = unit.charPos.at(j);
							short n;
							lineStream >> n >> charPos.first >> charPos.second;
						}
						else {
							throw std::ifstream::failure("ERROR: Bad data encountered when processing " + Key::charPos);
						}
					}
				}

				// section 3: unit properties
				if (key == Key::hasProperty) {
					unsigned short property;
					lineStream >> property;
					unit.properties.emplace(property);
				}
			}

			unit.write_bin(outputStream);
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
