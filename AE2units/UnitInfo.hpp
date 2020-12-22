#ifndef UNITINFO_HPP
#define UNITINFO_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <memory>

namespace UnitKey {
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
private:
	// using PIMPL
	class Impl;
	std::unique_ptr<Impl> impl;

public:
	UnitInfo();
	~UnitInfo();

	// read unit data from a .bin file
	std::ifstream& read_bin(std::ifstream& inputStream);

	// print the data to a .unit file
	friend std::ostream& operator<<(std::ostream& outputStream, const UnitInfo& unitInfo);

	// read unit data from a .unit file
	friend std::istream& operator>>(std::istream& inputStream, UnitInfo& unitInfo);

	// write unit data to a .bin file
	std::ofstream& write_bin(std::ofstream& outputStream) const;
};

#endif
