#include <iostream>

#include "endl.hpp"
#include "UnitInfo.hpp"

extern "C" {
	#include "../utils/utils.h"
}

// read unit data from a .bin file
std::ifstream& UnitInfo::read_bin(std::ifstream& inputStream) {
	unsigned char c1, c2;

	// section 1: basic information
	this->moveRange = inputStream.get();
	this->minAttack = inputStream.get();
	this->maxAttack = inputStream.get();
	this->defense = inputStream.get();
	this->maxAttackRange = inputStream.get();
	this->minAttackRange = inputStream.get();
	c1 = inputStream.get();
	c2 = inputStream.get();
	this->price = static_cast<short>(fourBytesToUInt32(0, 0, c1, c2));

	// section 2: fight animation
	unsigned int numChars = inputStream.get();
	this->charPos.resize(numChars);
	for (auto& charPos: this->charPos) {
		charPos.first = inputStream.get();
		charPos.second = inputStream.get();
	}

	// section 3: unit properties
	unsigned int numProperties = inputStream.get();
	this->properties.clear();
	for (unsigned int j = 0; j < numProperties; ++j) {
		unsigned short property = inputStream.get();
		this->properties.emplace(property);
	}

	return inputStream;
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
	outputStream << UnitKey::moveRange << " " << unitInfo.moveRange << endl;
	outputStream << UnitKey::attack << " "
		<< unitInfo.minAttack << " " << unitInfo.maxAttack << endl;
	outputStream << UnitKey::defense << " " << unitInfo.defense << endl;
	outputStream << UnitKey::attackRange << " "
		<< unitInfo.maxAttackRange << " "
		<< unitInfo.minAttackRange << endl;
	outputStream << UnitKey::price << " " << unitInfo.price << endl;

	// section 2: fight animation information
	unsigned int numChars = unitInfo.charPos.size();
	outputStream << endl;
	outputStream << UnitKey::charCount << " " << numChars << endl;
	if (numChars > 0) {
		outputStream << endl;
		for (unsigned int i = 0; i < numChars; ++i) {
			const auto& coord = unitInfo.charPos.at(i);
			outputStream << UnitKey::charPos << " " << i << " "
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

// write unit data to a .bin file
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
