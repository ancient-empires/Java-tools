/*
  Header file to read and write properties of all units.
  Note: In the original game the unit information is hard-coded. Hence it is impossible to add or remove units from the game.
*/

#ifndef UNITINFO_HPP
#define UNITINFO_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

class UnitInfo {
private:
    // using PIMPL
    class Impl;
    const std::unique_ptr<Impl> impl;

public:
    UnitInfo();
    ~UnitInfo();

    // names of all units in AE2
    // Unit names are taken from Ancient Empires 1, for compatibility purposes.
    // These names are hard-coded in the original game. They shall NOT be changed, or the program will break.
    static const std::vector<std::string> UNIT_NAMES;

    // number of all units
    static const size_t NUM_UNITS;

    // extension of unit files
    static const std::string UNIT_EXT;

    // read unit data from a .bin file
    std::ifstream& read_bin(std::ifstream& inputStream);

    // write unit data to a .bin file
    std::ofstream& write_bin(std::ofstream& outputStream) const;

    // read unit data from a .unit file
    friend std::istream& operator>>(std::istream& inputStream, UnitInfo& unitInfo);

    // write the data to a .unit file
    friend std::ostream& operator<<(std::ostream& outputStream, const UnitInfo& unitInfo);
};

#endif
