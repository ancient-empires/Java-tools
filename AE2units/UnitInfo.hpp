#ifndef UNITINFO_HPP
#define UNITINFO_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

class UnitInfo {
private:
    // using PIMPL
    class Impl;
    const std::unique_ptr<Impl> impl;

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
