# AE2units

This tool is for extracting / packing the `units.bin` file in Ancient Empires 2. This file contains the information for all units.

Extracted files are stored as text format with `.unit` extension.

[Ancient Empires 2 Revolution Mod](https://web.archive.org/web/20201214010840/http://projectd8.org/Ancient_Empires_II_RM) currently uses the `*.unit` file format. Note that the unit names are taken from Ancient Empires 1 for compatibility issues.

## How to use

### Compile
```
$ make
```

### Extract `units.bin` to `*.unit` files
```
$ ./AE2units.out -e <path/to/units.bin> <extract/directory>
```
**In the original game the number of unit types was hard-coded to 12.** Hence it is impossible to add or remove units from this game.

The generated files are (according to how the units are ordered in the `units.bin` file):

* `soldier.unit`
* `archer.unit`
* `lizard.unit`
* `wizard.unit`
* `wisp.unit`
* `spider.unit`
* `golem.unit`
* `catapult.unit`
* `wyvern.unit`
* `king.unit`
* `skeleton.unit`
* `crystall.unit`

### Pack `*.unit` files into `units.bin`
```
$ ./AE2units.out -p <path/to/units.bin> <pack/directory>
```

The program will look in the packing directory for the 12 `*.unit` files with filenames *exactly equal to* the names listed above. **Missing any of the text files will cause a runtime error.**

### Clean up
```
$ make clean
```
