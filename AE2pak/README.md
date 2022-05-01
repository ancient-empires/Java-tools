# AE2pak

AE2pak, the PAK archive utility for Ancient Empires 1 and 2 (Java version).

This is a remaster of the **AE2pak** utility initially published on [byblo](https://byblo.proboards.com/thread/25/tool-ae2pak-packer-unpacker-file) ([Archive](https://web.archive.org/web/20201101163511/https://byblo.proboards.com/thread/25/tool-ae2pak-packer-unpacker-file)). It is rewritten to eliminate unnecessary `goto`s and improve readability. The rewritten version can be compiled using [`gcc`](https://gcc.gnu.org/).

## How to use

### Compile
```bash
make
```

### Extract .pak archive
```bash
make extract pak=<path/to/pak.pak> dir=<path/to/extract/dir>
```

#### Shortcut: Extract .pak files for AE1 and AE2
```bash
make extract_ae1 # extract for AE1
make extract_ae2 # extract for AE2
make extract_all # extract for both series, which will call the two commands above automatically
```

**NOTE: The output directory must be created beforehand. Otherwise, you will be unable to obtain the extracted files.**

### Pack .pak archive from the file list .log file
```bash
make pack pak=<path/to/pak.pak> dir=<path/to/resource/dir> filelist=<path/to/file/list.log>
```

#### Shortcut: Pack .pak files for AE1 and AE2
```bash
make pack_ae1 # pack for AE1
make pack_ae2 # pack for AE2
make pack_all # pack for both series, which will call the two commands above automatically
```

### Clean up

#### Clean up extracted files
```bash
make clean_ae1 # clean up for AE1
make clean_ae2 # clean up for AE2
make clean_all_extracted # clean up for both series, which will call the two commands above automatically
```

#### Clean up executable files and object files
```bash
make clean
```
