/*
  Header file to contain the unit information in Ancient Empires 2.
  In the original game the unit information is hard-coded.
  Hence it is impossible to add or remove units from the game.
*/

#ifndef UNITS_HPP
#define UNITS_HPP

#include <string>
#include <vector>

// Unit names are taken from Ancient Empires 1, for compatibility purposes.
const std::vector<std::string> UNIT_NAMES = {
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
};

const std::string UNIT_EXT = ".unit";

const size_t NUM_UNITS = UNIT_NAMES.size();

#endif
