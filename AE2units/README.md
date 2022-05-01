# AE2units
This tool is for extracting / packing the `units.bin` file in Ancient Empires 2. This file contains the information for all units.

Extracted files are stored as text format with `.unit` extension.

[Ancient Empires 2 Revolution Mod](https://web.archive.org/web/20201214010840/http://projectd8.org/Ancient_Empires_II_RM) currently uses the `*.unit` file format. Note that the unit names are taken from Ancient Empires 1 for the purpose of compatibility.

## Compile
```bash
make
```

## Extract `units.bin` to `*.unit` files
Running
```bash
make extract
```
will extract the `data/units.bin` file into 12 `*.unit` files, which will be put in the `data` directory. The generated files are:

0. `soldier.unit`
1. `archer.unit`
2. `lizard.unit`
3. `wizard.unit`
4. `wisp.unit`
5. `spider.unit`
6. `golem.unit`
7. `catapult.unit`
8. `wyvern.unit`
9. `king.unit`
10. `skeleton.unit`
11. `crystall.unit`

**Note:** *In the original game the number of unit types was hard-coded to 12.* Hence it is impossible to add or remove units from this game.

## Pack `*.unit` files into `units.bin`
After you modify the .unit files in the `data` directory, run
```bash
make pack
```
to pack them up into `data/units.bin`.

The program will look in the `data` directory for the 12 `*.unit` files with filenames *exactly equal to* the names listed above. **Missing any of the text files will cause a runtime error.**

## Clean up
```bash
make clean
```
