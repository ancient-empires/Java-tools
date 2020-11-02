# AE2lang

AE2lang, the language file utility for Ancient Empires 2 (Java version).

This is a remaster of the AE2lang utility initially publish on [byblo](https://byblo.proboards.com/thread/23/tool-ae2lang-language-file-converter) ([Archive](https://web.archive.org/web/20200512182837/https://byblo.proboards.com/thread/101/tool-ae1map-editor-version-20090913)). It is rewritten to eliminate unnecessary `goto`s and improve readability. The written version can be compiled using [`gcc`](https://gcc.gnu.org/).

## How to use

### Compile
```
$ make
```

### Convert from DAT to TXT
```
$ make dat2txt DAT=<path/to/dat.dat> TXT=<path/to/txt.txt>
```

### Convert from TXT to DAT
```
$ make txt2dat DAT=<path/to/dat.dat> TXT=<path/to/txt.txt>
```

### Clean up
```
$ make clean
```
