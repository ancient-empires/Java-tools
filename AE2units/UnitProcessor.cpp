#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "endl.hpp"
#include "units.hpp"
#include "UnitProcessor.hpp"

extern "C" {
	#include "../utils/utils.h"
}

static std::ostream& operator<<(std::ostream& outputStream, const int8_t& num) {
	outputStream << static_cast<int>(num);
	return outputStream;
}

static std::ostream& operator<<(std::ostream& outputStream, const uint8_t& num) {
	outputStream << static_cast<unsigned int>(num);
	return outputStream;
}

typedef std::pair<int8_t, int8_t> charpos;

class UnitProcessor::UnitInfo {
public:
	uint8_t moveRange = 0;

	int8_t minAttack = 0;
	int8_t maxAttack = 0;

	int8_t defense = 0;

	uint8_t maxAttackRange = 0;
	uint8_t minAttackRange = 0;

	int16_t price = 0;

	std::vector<charpos> charPos;

	std::vector<uint8_t> properties;

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
		outputStream << "MoveRange " << unitInfo.moveRange << endl;
		outputStream << "Attack "
			<< unitInfo.minAttack << " " << unitInfo.maxAttack << endl;
		outputStream << "Defence " << unitInfo.defense << endl;
		outputStream << "AttackRange "
			<< unitInfo.maxAttackRange << " "
			<< unitInfo.minAttackRange << endl;
		outputStream << "Cost " << unitInfo.price << endl;

		// section 2: fight animation information
		unsigned int numChars = unitInfo.charPos.size();
		outputStream << endl;
		outputStream << "CharCount " << numChars << endl;
		if (numChars > 0) {
			outputStream << endl;
			for (unsigned int i = 0; i < numChars; ++i) {
				const auto& coord = unitInfo.charPos.at(i);
				outputStream << "CharPos " << i << " "
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
	inputStream.open(unitsBinFile);
	inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);

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
			unit.price = static_cast<int16_t>(fourBytesToUInt32(0, 0, c1, c2));

			// section 2: fight animation
			unsigned int numChars = inputStream.get();
			unit.charPos.resize(numChars);
			for (auto& charPos: unit.charPos) {
				charPos.first = inputStream.get();
				charPos.second = inputStream.get();
			}

			// section 3: properties
			unsigned int numProperties = inputStream.get();
			unit.properties.resize(numProperties);
			for (auto& property: unit.properties) {
				property = inputStream.get();
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
