#include <string>

#ifndef UNITPROCESSOR_H
#define UNITPROCESSOR_H

class UnitProcessor {
private:
	class UnitInfo;
public:
	static void extract(const std::string& unitsBinFile, const std::string& extractDir);
};

#endif
