# AE2units
This tool is for extracting / packing the `units.bin` file in Ancient Empires 2. This file contains the information for all units.

Extracted files are stored as text format with `.unit` extension.

[Ancient Empires 2 Revolution Mod](https://web.archive.org/web/20201214010840/http://projectd8.org/Ancient_Empires_II_RM) currently uses the `*.unit` file format. Note that the unit names are taken from Ancient Empires 1 for the purpose of compatibility.

## Compile
```bash
make
```

## Extract `units.bin` to `*.unit` files
```bash
./AE2units.out -e <path/to/units.bin> <extract/directory>
```

**In the original game the number of unit types was hard-coded to 12.** Hence it is impossible to add or remove units from this game.

The generated files are (according to how the units are ordered in the `units.bin` file):

1. `soldier.unit`
2. `archer.unit`
3. `lizard.unit`
4. `wizard.unit`
5. `wisp.unit`
6. `spider.unit`
7. `golem.unit`
8. `catapult.unit`
9. `wyvern.unit`
10. `king.unit`
11. `skeleton.unit`
12. `crystall.unit`

## Pack `*.unit` files into `units.bin`
```bash
./AE2units.out -p <path/to/units.bin> <pack/directory>
```

The program will look in the packing directory for the 12 `*.unit` files with filenames *exactly equal to* the names listed above. **Missing any of the text files will cause a runtime error.**

## Clean up
```bash
make clean
```
