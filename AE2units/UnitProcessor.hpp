#ifndef UNITPROCESSOR_HPP
#define UNITPROCESSOR_HPP

#include <string>

class UnitProcessor {
public:
    static void extract(const std::string& unitsBinFile, const std::string& extractDir);
    static void pack(const std::string& unitsBinFile, const std::string& packDir);
};

#endif
