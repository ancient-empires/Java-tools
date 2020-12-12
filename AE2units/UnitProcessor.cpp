#include <fstream>
#include <vector>

#include "units.hpp"
#include "UnitProcessor.hpp"

extern "C" {
	#include "../utils/utils.h"
}

typedef std::pair<char, char> charpos;

class UnitProcessor::UnitInfo {
public:
	char moveRange = 0;

	char minAttack = 0;
	char maxAttack = 0;

	char defense = 0;

	unsigned char maxAttackRange = 0;
	unsigned char minAttackRange = 0;

	short price = 0;

	std::vector<charpos> charPos;

	std::vector<char> properties;
};

void UnitProcessor::extract(const std::string& unitsBinFile, const std::string& extractDir) {
	// vector to store the information of all units
	std::vector<UnitProcessor::UnitInfo> units(numUnits);

	// initialize input file stream
	std::ifstream inputStream;
	inputStream.open(unitsBinFile);

	// initialize all output file streams
	std::vector<std::ofstream> outputStreams(numUnits);
	for (unsigned int i = 0; i < numUnits; ++i) {
		std::string unitFilePath = extractDir + unitNames.at(i) + unitExt;
		outputStreams.at(i).open(unitFilePath);
	}
}
