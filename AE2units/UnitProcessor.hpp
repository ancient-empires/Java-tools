#ifndef UNITPROCESSOR_H
#define UNITPROCESSOR_H

#include <string>

class UnitProcessor {
public:
	class UnitInfo;
public:
	static void extract(const std::string& unitsBinFile, const std::string& extractDir);
	static void pack(const std::string& unitsBinFile, const std::string& packDir);
};

#endif
