#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <set>

extern "C" {
#include "../utils/utils.h"
}

#include "UnitInfo.hpp"

class UnitInfo::Impl {
public:
    unsigned short moveRange = 0;

    short minAttack = 0;
    short maxAttack = 0;

    short defense = 0;

    unsigned short maxAttackRange = 0;
    unsigned short minAttackRange = 0;

    short price = 0;

    typedef std::pair<short, short> charpos_t;
    std::vector<charpos_t> charPos;

    std::set<unsigned short> properties;
};

namespace unit_keys {
    static const std::string MOVE_RANGE = "MoveRange";
    static const std::string ATTACK = "Attack";
    static const std::string DEFENSE = "Defence";
    static const std::string ATTACK_RANGE = "AttackRange";
    static const std::string PRICE = "Cost";
    static const std::string CHAR_COUNT = "CharCount";
    static const std::string CHAR_POS = "CharPos";
    static const std::string HAS_PROPERTY = "HasProperty";
};

UnitInfo::UnitInfo() : impl(std::make_unique<Impl>()) {
}

UnitInfo::~UnitInfo() {
}

// names of all units in AE2
// Unit names are taken from Ancient Empires 1, for compatibility purposes.
// These names are hard-coded in the original game. They shall NOT be changed, or the program will break.
const std::vector<std::string> UnitInfo::UNIT_NAMES({
        "soldier",
        "archer",
        "lizard",    // Elemental
        "wizard",    // Sorceress
        "wisp",
        "spider",    // Dire Wolf
        "golem",
        "catapult",
        "wyvern",    // Dragon
        "king",      // Galamar / Valadorn / Demon Lord / Saeth
        "skeleton",
        "crystall",  // new unit in Ancient Empires 2
    });

// get the number of all units in AE2
const size_t UnitInfo::NUM_UNITS = UnitInfo::UNIT_NAMES.size();

// unit file extension
const std::string UnitInfo::UNIT_EXT = ".unit";

// read unit data from a .bin file
std::ifstream& UnitInfo::read_bin(std::ifstream& inputStream) {
    unsigned char c1, c2;

    // section 1: basic information
    impl->moveRange = inputStream.get();
    impl->minAttack = inputStream.get();
    impl->maxAttack = inputStream.get();
    impl->defense = inputStream.get();
    impl->maxAttackRange = inputStream.get();
    impl->minAttackRange = inputStream.get();
    c1 = inputStream.get();
    c2 = inputStream.get();
    impl->price = static_cast<short>(fourBytesToUInt32(0, 0, c1, c2));

    // section 2: fight animation
    unsigned int numChars = inputStream.get();
    impl->charPos.resize(numChars);
    for (auto& charPos : impl->charPos) {
        charPos.first = inputStream.get();
        charPos.second = inputStream.get();
    }

    // section 3: unit properties
    size_t numProperties = inputStream.get();
    impl->properties.clear();
    for (size_t i = 0; i < numProperties; ++i) {
        unsigned short property = inputStream.get();
        impl->properties.emplace(property);
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
std::ostream& operator<<(std::ostream& outputStream, const UnitInfo& unit) {
    auto& impl = unit.impl;

    // section 1: basic information
    outputStream << unit_keys::MOVE_RANGE << " " << impl->moveRange << "\n";
    outputStream << unit_keys::ATTACK << " "
        << impl->minAttack << " " << impl->maxAttack << "\n";
    outputStream << unit_keys::DEFENSE << " " << impl->defense << "\n";
    outputStream << unit_keys::ATTACK_RANGE << " "
        << impl->maxAttackRange << " "
        << impl->minAttackRange << "\n";
    outputStream << unit_keys::PRICE << " " << impl->price << "\n";

    // section 2: fight animation information
    size_t numChars = impl->charPos.size();
    outputStream << "\n" << unit_keys::CHAR_COUNT << " " << numChars << "\n";
    if (numChars > 0) {
        outputStream << "\n";
        for (size_t i = 0; i < numChars; ++i) {
            const auto& coord = impl->charPos.at(i);
            outputStream << unit_keys::CHAR_POS << " " << i << " "
                << coord.first << " " << coord.second << "\n";
        }
    }

    // section 3: unit properties
    if (!impl->properties.empty()) {
        outputStream << "\n";
        size_t i = 0, numProperties = impl->properties.size();
        for (const auto& property : impl->properties) {
            outputStream << unit_keys::HAS_PROPERTY << " " << property;
            if (i < numProperties - 1) {
                outputStream << "\n";
            }
            ++i;
        }
    }

    return outputStream;
}

// read unit data from a .unit file
// see the comments before operator<< overloading for the file structure
std::istream& operator>>(std::istream& inputStream, UnitInfo& unit) {

    auto& impl = unit.impl;

    impl->properties.clear();

    while (!inputStream.eof()) {
        // get line and key
        std::string line, key;
        std::getline(inputStream, line);
        std::istringstream lineStream(line);
        lineStream >> key;

        // section 1: basic information
        if (key == unit_keys::MOVE_RANGE) {
            lineStream >> impl->moveRange;
        }
        else if (key == unit_keys::ATTACK) {
            lineStream >> impl->minAttack;
            lineStream >> impl->maxAttack;
        }
        else if (key == unit_keys::DEFENSE) {
            lineStream >> impl->defense;
        }
        else if (key == unit_keys::ATTACK_RANGE) {
            lineStream >> impl->maxAttackRange;
            lineStream >> impl->minAttackRange;
        }
        else if (key == unit_keys::PRICE) {
            lineStream >> impl->price;
        }

        // section 2: fight animation
        if (key == unit_keys::CHAR_COUNT) {
            size_t numChars = 0;
            lineStream >> numChars;
            impl->charPos.resize(numChars);

            // process each CharPos line
            for (size_t i = 0; i < numChars; ++i) {
                std::string line;
                do {
                    std::getline(inputStream, line);
                } while (line.empty() && !inputStream.eof());

                std::istringstream lineStream(line);
                lineStream >> key;
                if (key == unit_keys::CHAR_POS) {
                    auto& charPos = impl->charPos.at(i);
                    short n;
                    lineStream >> n >> charPos.first >> charPos.second;
                }
                else {
                    throw std::ifstream::failure("ERROR: Bad data encountered when processing " + unit_keys::CHAR_POS);
                }
            }
        }

        // section 3: unit properties
        if (key == unit_keys::HAS_PROPERTY) {
            unsigned short property;
            lineStream >> property;
            impl->properties.emplace(property);
        }
    }

    return inputStream;
}

// write unit data to a .bin file
std::ofstream& UnitInfo::write_bin(std::ofstream& outputStream) const {

    unsigned char c1, c2, c3, c4;

    // section 1: basic info
    outputStream.put(static_cast<char>(impl->moveRange));
    outputStream.put(static_cast<char>(impl->minAttack));
    outputStream.put(static_cast<char>(impl->maxAttack));
    outputStream.put(static_cast<char>(impl->defense));
    outputStream.put(static_cast<char>(impl->maxAttackRange));
    outputStream.put(static_cast<char>(impl->minAttackRange));
    uInt32ToFourBytes(static_cast<unsigned short>(impl->price), &c1, &c2, &c3, &c4);
    outputStream.put(c3);
    outputStream.put(c4);

    // section 2: fight animation
    outputStream.put(static_cast<char>(impl->charPos.size()));
    for (const auto& charPos : impl->charPos) {
        outputStream.put(static_cast<char>(charPos.first));
        outputStream.put(static_cast<char>(charPos.second));
    }

    // section 3: unit properties
    outputStream.put(static_cast<char>(impl->properties.size()));
    for (const auto& property : impl->properties) {
        outputStream.put(property);
    }

    return outputStream;
}
