#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

#ifndef UNITINFO_HPP
#define UNITINFO_HPP

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

class UnitInfo {
public:

	// unit attributes

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

	// print the data to a .unit file
	friend std::ostream& operator<<(std::ostream& outputStream, const UnitInfo& unitInfo);

	// write data to a .bin file
	std::ofstream& write_bin(std::ofstream& outputStream) const;
};

#endif
