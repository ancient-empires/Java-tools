#include <iostream>

#include "endl.hpp"
#include "UnitInfo.hpp"

extern "C" {
	#include "../utils/utils.h"
}

/* Print the unit data to a .unit file.
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
std::ostream& operator<<(std::ostream& outputStream, const UnitInfo& unitInfo) {
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

// write unit data to a .bin file.
std::ofstream& UnitInfo::write_bin(std::ofstream& outputStream) const {

	unsigned char c1, c2, c3, c4;

	// section 1: basic info
	outputStream.put(static_cast<char>(this->moveRange));
	outputStream.put(static_cast<char>(this->minAttack));
	outputStream.put(static_cast<char>(this->maxAttack));
	outputStream.put(static_cast<char>(this->defense));
	outputStream.put(static_cast<char>(this->maxAttackRange));
	outputStream.put(static_cast<char>(this->minAttackRange));
	uInt32ToFourBytes(static_cast<unsigned short>(this->price), &c1, &c2, &c3, &c4);
	outputStream.put(c3);
	outputStream.put(c4);

	// section 2: fight animation
	outputStream.put(static_cast<char>(this->charPos.size()));
	for (const auto& charPos: this->charPos) {
		outputStream.put(static_cast<char>(charPos.first));
		outputStream.put(static_cast<char>(charPos.second));
	}

	// section 3: unit properties
	outputStream.put(static_cast<char>(this->properties.size()));
	for (const auto& property: this->properties) {
		outputStream.put(property);
	}

	return outputStream;
}
