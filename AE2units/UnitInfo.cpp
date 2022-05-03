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
    uint8_t moveRange = 0;

    int8_t minAttack = 0;
    int8_t maxAttack = 0;

    int8_t defense = 0;

    uint8_t maxAttackRange = 0;
    uint8_t minAttackRange = 0;

    int16_t price = 0;

    typedef std::pair<uint8_t, uint8_t> charpos_t;
    std::vector<charpos_t> charPos;

    std::set<uint8_t> properties;
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
    impl->price = static_cast<int16_t>(fourBytesToUInt32(0, 0, c1, c2));

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
        uint8_t property = inputStream.get();
        impl->properties.emplace(property);
    }

    return inputStream;
}

// write unit data to a .bin file
std::ofstream& UnitInfo::write_bin(std::ofstream& outputStream) const {

    unsigned char c1, c2, c3, c4;

    // section 1: basic info
    outputStream.put(impl->moveRange);
    outputStream.put(impl->minAttack);
    outputStream.put(impl->maxAttack);
    outputStream.put(impl->defense);
    outputStream.put(impl->maxAttackRange);
    outputStream.put(impl->minAttackRange);
    uInt32ToFourBytes(impl->price, &c1, &c2, &c3, &c4);
    outputStream.put(c3);
    outputStream.put(c4);

    // section 2: fight animation
    outputStream.put(static_cast<uint8_t>(impl->charPos.size()));
    for (const auto& charPos : impl->charPos) {
        outputStream.put(charPos.first);
        outputStream.put(charPos.second);
    }

    // section 3: unit properties
    outputStream.put(static_cast<uint8_t>(impl->properties.size()));
    for (const auto& property : impl->properties) {
        outputStream.put(property);
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
            lineStream >> reinterpret_cast<uint16_t&>(impl->moveRange);
        }
        else if (key == unit_keys::ATTACK) {
            lineStream
                >> reinterpret_cast<int16_t&>(impl->minAttack)
                >> reinterpret_cast<int16_t&>(impl->maxAttack);
        }
        else if (key == unit_keys::DEFENSE) {
            lineStream >> reinterpret_cast<int16_t&>(impl->defense);
        }
        else if (key == unit_keys::ATTACK_RANGE) {
            lineStream
                >> reinterpret_cast<uint16_t&>(impl->maxAttackRange)
                >> reinterpret_cast<uint16_t&>(impl->minAttackRange);
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
                    uint16_t j = 0;
                    lineStream >> j
                        >> reinterpret_cast<uint16_t&>(charPos.first)
                        >> reinterpret_cast<uint16_t&>(charPos.second);
                }
                else {
                    throw std::ifstream::failure("ERROR: Bad data encountered when processing " + unit_keys::CHAR_POS);
                }
            }
        }

        // section 3: unit properties
        if (key == unit_keys::HAS_PROPERTY) {
            uint8_t property;
            lineStream >> reinterpret_cast<uint16_t&>(property);
            impl->properties.emplace(property);
        }
    }

    return inputStream;
}

/* Write the unit data to a .unit file.
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
    const auto& impl = unit.impl;

    // section 1: basic information
    outputStream << unit_keys::MOVE_RANGE << " " << static_cast<uint16_t>(impl->moveRange) << "\n";
    outputStream << unit_keys::ATTACK << " "
        << static_cast<int16_t>(impl->minAttack) << " "
        << static_cast<int16_t>(impl->maxAttack) << "\n";
    outputStream << unit_keys::DEFENSE << " " << static_cast<int16_t>(impl->defense) << "\n";
    outputStream << unit_keys::ATTACK_RANGE << " "
        << static_cast<uint16_t>(impl->maxAttackRange) << " "
        << static_cast<uint16_t>(impl->minAttackRange) << "\n";
    outputStream << unit_keys::PRICE << " " << impl->price << "\n";

    // section 2: fight animation information
    const size_t numChars = impl->charPos.size();
    outputStream << "\n" << unit_keys::CHAR_COUNT << " " << numChars;
    if (numChars > 0) {
        outputStream << "\n\n";
        for (size_t i = 0; i < numChars; ++i) {
            const auto& coord = impl->charPos.at(i);
            outputStream << unit_keys::CHAR_POS << " " << i << " "
                << static_cast<uint16_t>(coord.first) << " "
                << static_cast<uint16_t>(coord.second);
            if (i < numChars - 1) {
                outputStream << "\n";
            }
        }
    }

    // section 3: unit properties
    if (!impl->properties.empty()) {
        outputStream << "\n";

        const size_t numProperties = impl->properties.size();
        size_t i = 0;
        for (const auto& property : impl->properties) {
            outputStream << unit_keys::HAS_PROPERTY << " " << static_cast<uint16_t>(property);
            if (i < numProperties - 1) {
                outputStream << "\n";
            }
            ++i;
        }
    }

    return outputStream;
}