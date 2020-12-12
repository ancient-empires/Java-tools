#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>

#include "endl.hpp"
#include "units.hpp"
#include "UnitProcessor.hpp"

extern "C" {
	#include "../utils/utils.h"
}

namespace Key {
	static const std::string moveRange = "MoveRange";
	static const std::string attack = "Attack";
	static const std::string defense = "Defence";
	static const std::string attackRange = "AttackRange";
	static const std::string price = "Cost";
	static const std::string charCount = "CharCount";
	static const std::string charPos = "CharPos";
	static const std::string hasProperty = "HasProperty";
};

class UnitProcessor::UnitInfo {
public:
	unsigned short moveRange = 0;

	short minAttack = 0;
	short maxAttack = 0;

	short defense = 0;

	unsigned short maxAttackRange = 0;
	unsigned short minAttackRange = 0;

	short price = 0;

	typedef std::pair<short, short> charpos;
	std::vector<charpos> charPos;

	std::set<unsigned short> properties;

	/* Output the unit data to a .unit file.
		Sample format (archer.unit):
		==============================
		MoveRange 5
		Attack 50 55
		Defence 5
		AttackRange 2 1
		Cost 250

		CharCount 5

		CharPos 0 30 63
		CharPos 1 30 101
		CharPos 2 8 80
		CharPos 3 8 121
		CharPos 4 8 41

		HasProperty 6
		==============================
	*/
	friend std::ostream& operator<<(std::ostream& outputStream, const UnitProcessor::UnitInfo& unitInfo) {
		// section 1: basic information
		outputStream << Key::moveRange << " " << unitInfo.moveRange << endl;
		outputStream << Key::attack << " "
			<< unitInfo.minAttack << " " << unitInfo.maxAttack << endl;
		outputStream << Key::defense << " " << unitInfo.defense << endl;
		outputStream << Key::attackRange << " "
			<< unitInfo.maxAttackRange << " "
			<< unitInfo.minAttackRange << endl;
		outputStream << Key::price << " " << unitInfo.price << endl;

		// section 2: fight animation information
		unsigned int numChars = unitInfo.charPos.size();
		outputStream << endl;
		outputStream << Key::charCount << " " << numChars << endl;
		if (numChars > 0) {
			outputStream << endl;
			for (unsigned int i = 0; i < numChars; ++i) {
				const auto& coord = unitInfo.charPos.at(i);
				outputStream << Key::charPos << " " << i << " "
					<< coord.first << " " << coord.second << endl;
			}
		}

		// section 3: unit properties
		if (!unitInfo.properties.empty()) {
			outputStream << endl;
			unsigned int i = 0;
			for (const auto& property: unitInfo.properties) {
				outputStream << "HasProperty " << property;
				if (i < unitInfo.properties.size() - 1) {
					outputStream << endl;
				}
				++i;
			}
		}

		return outputStream;
	}
};

void UnitProcessor::extract(const std::string& unitsBinFile, const std::string& extractDir) {
	// vector to store the information of all units
	std::vector<UnitProcessor::UnitInfo> units(numUnits);

	// initialize input file stream
	std::ifstream inputStream;
	inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		inputStream.open(unitsBinFile);
	}
	catch (const std::ifstream::failure& error) {
		std::cerr << error.what() << endl;
		std::cerr << "ERROR: Failed to open input file \"" << unitsBinFile << "\"" << endl;
		exit(ERROR_RW);
	}

	// initialize all output file streams
	std::vector<std::string> unitFilePaths(numUnits);
	std::vector<std::ofstream> outputStreams(numUnits);
	for (unsigned int i = 0; i < numUnits; ++i) {
		auto& unitPath = unitFilePaths.at(i);
		unitPath = extractDir + "/" + unitNames.at(i) + unitExt;
		outputStreams.at(i).open(unitPath);
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
	std::vector<UnitProcessor::UnitInfo> units(numUnits);

	// initialize all input file streams
	std::vector<std::string> unitFilePaths(numUnits);
	std::vector<std::ifstream> inputStreams(numUnits);

	unsigned int numErrors = 0;
	for (unsigned int i = 0; i < numUnits; ++i) {
		auto& unitPath = unitFilePaths.at(i);
		unitPath = packDir + "/" + unitNames.at(i) + unitExt;
		auto& inputStream = inputStreams.at(i);
		inputStream.exceptions(std::ifstream::badbit);

		// try to open each input file and initialize streams
		// report any errors
		try {
			inputStream.open(unitPath);
			if (inputStream.fail()) {
				throw std::ios_base::failure("ERROR: Failed to open input file \"" + unitPath + "\"");
			}
		}
		catch (const std::ios_base::failure& error) {
			++numErrors;
			std::cerr << error.what() << endl;
		}
	}
	if (numErrors > 0) {
		exit(ERROR_RW);
	}

	// initialize output stream
	std::ofstream outputStream;
	outputStream.open(unitsBinFile);

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
						} while (line.empty());

						std::istringstream lineStream(line);
						lineStream >> key;
						if (key == Key::charPos) {
							short n;
							lineStream >> n;
							lineStream >> unit.charPos.at(j).first
								>> unit.charPos.at(j).second;
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

			std::cout << unit << std::endl;
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
