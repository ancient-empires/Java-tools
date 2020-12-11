#include <string>
#include <vector>

#ifndef UNITS_HPP
#define UNITS_HPP

/*
	Header file to contain the unit information in Ancient Empires 2.
	In the original game the unit information is hard-coded.
	Hence it is impossible to add or remove units from the game.
*/

// Unit names are taken from Ancient Empires 1, for compatibility purposes.
static const std::vector<std::string> unitNames = {
	"soldier",
	"archer",
	"lizard",   // Elemental
	"wizard",   // Sorceress
	"wisp",
	"spider",   // Dire Wolf
	"golem",
	"catapult",
	"wyvern",   // Dragon
	"king",     // Galamar / Valadorn / Demon Lord / Saeth
	"skeleton",
	"crystall", // new unit in Ancient Empires 2
};

static const unsigned int numUnits = unitNames.size();

#endif
