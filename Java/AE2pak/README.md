# AE2pak

AE2pak, the PAK archive utility for Ancient Empires 1 and 2 (Java version).

This is a remaster of the **AE2pak** utility initially published on [byblo](https://byblo.proboards.com/thread/25/tool-ae2pak-packer-unpacker-file) ([Archive](https://web.archive.org/web/20201101163511/https://byblo.proboards.com/thread/25/tool-ae2pak-packer-unpacker-file)). It is rewritten to eliminate unnecessary `goto`s and improve readability. The rewritten version can be compiled using [`gcc`](https://gcc.gnu.org/).

## How to use

### Compile
```
$ make
```

### Extract .pak archive
```
$ make extract pak=<path/to/pak.pak> dir=<path/to/extract/dir>
```

**NOTE: The output directory must be created beforehand. Otherwise, you will be unable to see the extracted files.**
