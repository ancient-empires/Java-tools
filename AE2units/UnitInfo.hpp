/*
  Header file to read and write properties of all units.
  In the original game the unit information is hard-coded.
  Hence it is impossible to add or remove units from the game.
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

    typedef std::vector<std::string> units_vector;

    // get the names of all units
    static const units_vector& unitNames();

    // get number of all units
    static const size_t numUnits();

    static const std::string UNIT_EXT;

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
