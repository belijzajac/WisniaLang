#!/bin/bash

print_header () {
  echo "------------------------------------------------------------------------------"
  echo "$1"
  echo "------------------------------------------------------------------------------"
}

print_binary_size () {
  echo "Binary size           :" $(wc -c "$1" | awk '{printf "%.3f MiB, %.3f KiB, %d B\n", $1/(1024*1024), $1/1024, $1}')
  strip "$1" >/dev/null 2>&1
  echo "Binary size (stripped):" $(wc -c "$1" | awk '{printf "%.3f MiB, %.3f KiB, %d B\n", $1/(1024*1024), $1/1024, $1}')
}

print_header "WisniaLang"
hyperfine --runs 20 --warmup 1 'wisnia calculate.wsn'
print_binary_size "a.out"

print_header "C++ (gcc)"
hyperfine --runs 20 --warmup 1 'g++ -std=c++23 -O3 calculate.cpp'
print_binary_size "a.out"

print_header "C++ (clang)"
hyperfine --runs 20 --warmup 1 'clang++ -std=c++2b -O3 calculate.cpp'
print_binary_size "a.out"

print_header "Rust"
hyperfine --runs 20 --warmup 1 'rustc -C opt-level=3 calculate.rs'
print_binary_size "calculate"
