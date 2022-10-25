# WisniaLang

[![Ubuntu](https://github.com/belijzajac/WisniaLang/actions/workflows/ubuntu.yml/badge.svg?branch=master)](https://github.com/belijzajac/WisniaLang/actions/workflows/ubuntu.yml)
[![codecov](https://codecov.io/gh/belijzajac/WisniaLang/branch/master/graph/badge.svg?token=SN5094ZY23)](https://codecov.io/gh/belijzajac/WisniaLang)
[![license](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

## Dependencies

* C++20 compatible compiler
* CMake

## Building from source

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc) wisnia
```

## Debugging binaries

```gdb
(gdb) set disassembly-flavor intel
(gdb) layout asm
(gdb) layout reg
(gdb) b *0x4000b0
(gdb) r
```

## License

This project is available under the [GPLv3 license](LICENSE).
