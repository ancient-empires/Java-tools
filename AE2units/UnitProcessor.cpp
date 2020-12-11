#include <vector>

#include "units.hpp"
#include "UnitProcessor.hpp"

typedef std::pair<char, char> charpos;

class UnitProcessor::UnitInfo {
public:
	char moveRange = 0;

	char minAttack = 0;
	char maxAttack = 0;

	char defense = 0;

	unsigned char minAttackRange = 0;
	unsigned char maxAttackRange = 0;

	short price = 0;

	std::vector<charpos> charPos;

	std::vector<char> properties;
};

void UnitProcessor::extract(const std::string& extractDir) {
	std::vector<UnitProcessor::UnitInfo> units(numUnits);
}
