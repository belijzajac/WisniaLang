# WisniaLang

[![tests](https://github.com/belijzajac/WisniaLang/actions/workflows/tests.yml/badge.svg)](https://github.com/belijzajac/WisniaLang/actions/workflows/tests.yml)
[![codecov](https://codecov.io/gh/belijzajac/WisniaLang/branch/master/graph/badge.svg?token=SN5094ZY23)](https://codecov.io/gh/belijzajac/WisniaLang)
[![license](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![online](https://img.shields.io/badge/try_it_online-ryugod.com-pink)](https://ryugod.com/pages/ide/wisnia)

## Prerequisites

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

## Example programs

You can find example programs in the [tests/programs/ProgramTest.cpp](tests/programs/ProgramTest.cpp) file.

Alternatively, [try them online](https://ryugod.com/pages/ide/wisnia) (please note that the compiler binary may not be the latest version).

## Benchmarks

[![benchmarks](docs/benchmark-1.png)](https://belijzajac.dev/wisnialang-compiler-project/)

> [!NOTE]
> The runtime range for WisniaLang was from 84.0 µs to 736.3 µs over 1000 program runs, indicating ambiguous results due to benchmarking a 17-line program that executes 3 lines of code 45 times. However, this does demonstrate the speed at which we can compile small programs.

[![benchmarks](docs/benchmark-2.png)](https://belijzajac.dev/wisnialang-compiler-project/)

## License

This project is available under the [GPLv3 license](LICENSE).
