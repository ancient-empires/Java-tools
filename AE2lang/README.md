# AE2lang

AE2lang, the language file utility for Ancient Empires 1 and 2 (Java version).

This is a remaster of the **AE2lang** utility initially published on [byblo](https://byblo.proboards.com/thread/23/tool-ae2lang-language-file-converter) ([Archive](https://web.archive.org/web/20201102050928/https://byblo.proboards.com/thread/23/tool-ae2lang-language-file-converter)). It is rewritten to eliminate unnecessary `goto`s and improve readability. The rewritten version can be compiled using [`gcc`](https://gcc.gnu.org/).

## How to use

### Compile
```bash
make
```

### Convert from .dat to .txt
```bash
make dat2txt dat=<path/to/dat.dat> txt=<path/to/txt.txt>
```

#### Shortcut: Convert .dat to .txt for AE1 and AE2
```bash
make dat2txt_ae1 # convert for AE1
make dat2txt_ae2 # convert for AE2
make dat2txt_all # convert for both series, which will call the two commands above automatically
```

### Convert from .txt to .dat
```bash
make txt2dat dat=<path/to/dat.dat> txt=<path/to/txt.txt>
```

#### Shortcut: Convert .txt to .dat for AE1 and AE2
```bash
make txt2dat_ae1 # convert for AE1
make txt2dat_ae2 # convert for AE2
make txt2dat_all # convert for both series, which will call the two commands above automatically
```

### Clean up
```bash
$ make clean
```
