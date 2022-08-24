/***

  WisniaLang - A Compiler for an Experimental Programming Language
  Copyright (C) 2022 Tautvydas Povilaitis (belijzajac) and contributors

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

***/

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ostream>
#include <filesystem>
// Wisnia
#include "ELF.hpp"

using namespace Wisnia;

constexpr const uint64_t kVirtualStartAddress{0x400000};
constexpr const uint64_t kDataVirtualStartAddress{0x600000};
constexpr const uint64_t kAlignment{0x200000};
constexpr const uint64_t kTextOffset{0x40 + (2 * 0x38)};

ELF::ELF(const ELF::Bytes &textSection, const ELF::Bytes &dataSection)
    : mTextSection{textSection}, mDataSection{dataSection} {}

// todo: put into your own class wrapping byte[] + myClass.insert(byte ...)
std::ostream& operator<< (std::ostream &os, std::byte b) {
  return os << std::to_integer<int>(b);
}

ELF::Bytes ELF::assembleELF() {
  Bytes elf{};

  auto textSize = uint64_t(mTextSection.size());
  auto dataSize = uint64_t(mDataSection.size());
  auto dataOffset = uint64_t(kTextOffset + textSize);
  auto dataVirtualAddress = uint64_t(kDataVirtualStartAddress + dataOffset);

  // ELF magic value
  elf.emplace_back(std::byte{0x7f});
  elf.emplace_back(std::byte{0x45});
  elf.emplace_back(std::byte{0x4c});
  elf.emplace_back(std::byte{0x46});

  elf.emplace_back(std::byte{0x02}); // 64-bit executable
  elf.emplace_back(std::byte{0x01}); // Little endian
  elf.emplace_back(std::byte{0x01}); // ELF version
  elf.emplace_back(std::byte{0x00}); // Target OS ABI
  elf.emplace_back(std::byte{0x00}); // Further specify ABI version

  // Unused bytes
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // Executable type
  elf.emplace_back(std::byte{0x02});
  elf.emplace_back(std::byte{0x00});

  // x86-64 target architecture
  elf.emplace_back(std::byte{0x3e});
  elf.emplace_back(std::byte{0x00});

  // ELF version
  elf.emplace_back(std::byte{0x01});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // 64-bit virtual offset
  auto val = static_cast<uint64_t>(kVirtualStartAddress + kTextOffset);
  elf.emplace_back(std::byte{(std::byte)(val)});
  elf.emplace_back(std::byte{(std::byte)(val >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val >> 56)});

  // Offset from file to program header
  elf.emplace_back(std::byte{0x40});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // Start of section header table
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // Flags
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // Size of this header
  elf.emplace_back(std::byte{0x40});
  elf.emplace_back(std::byte{0x00});

  // Size of a program header table entry
  elf.emplace_back(std::byte{0x38});
  elf.emplace_back(std::byte{0x00});

  // Length of sections: data and text
  elf.emplace_back(std::byte{0x02});
  elf.emplace_back(std::byte{0x00});

  // Size of section header
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // Number of entries section header
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // Index of section header table entry
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});

  // Build Program Header
  // Text Segment
  // PT_LOAD
  elf.emplace_back(std::byte{0x01});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  // Flags
  elf.emplace_back(std::byte{0x07});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  // Offset from the beginning of the file
  auto val1 = static_cast<uint64_t>(0);
  elf.emplace_back(std::byte{(std::byte)(val1)});
  elf.emplace_back(std::byte{(std::byte)(val1 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val1 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val1 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val1 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val1 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val1 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val1 >> 56)});
  //...
  auto val2 = static_cast<uint64_t>(kVirtualStartAddress);
  elf.emplace_back(std::byte{(std::byte)(val2)});
  elf.emplace_back(std::byte{(std::byte)(val2 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val2 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val2 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val2 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val2 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val2 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val2 >> 56)});
  // Physical address
  auto val3 = static_cast<uint64_t>(kVirtualStartAddress);
  elf.emplace_back(std::byte{(std::byte)(val3)});
  elf.emplace_back(std::byte{(std::byte)(val3 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val3 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val3 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val3 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val3 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val3 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val3 >> 56)});
  // Number of bytes in file image of segment
  auto val4 = static_cast<uint64_t>(textSize);
  elf.emplace_back(std::byte{(std::byte)(val4)});
  elf.emplace_back(std::byte{(std::byte)(val4 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val4 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val4 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val4 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val4 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val4 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val4 >> 56)});
  // Number of bytes in memory image of segment
  auto val5 = static_cast<uint64_t>(textSize);
  elf.emplace_back(std::byte{(std::byte)(val5)});
  elf.emplace_back(std::byte{(std::byte)(val5 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val5 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val5 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val5 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val5 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val5 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val5 >> 56)});
  //...
  auto val6 = static_cast<uint64_t>(kAlignment);
  elf.emplace_back(std::byte{(std::byte)(val6)});
  elf.emplace_back(std::byte{(std::byte)(val6 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val6 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val6 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val6 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val6 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val6 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val6 >> 56)});

  // Build Program Header
  // Data Segment
  // PT_LOAD
  elf.emplace_back(std::byte{0x01});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  // Flags
  elf.emplace_back(std::byte{0x07});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  elf.emplace_back(std::byte{0x00});
  // Offset address
  auto val7 = static_cast<uint64_t>(dataOffset);
  elf.emplace_back(std::byte{(std::byte)(val7)});
  elf.emplace_back(std::byte{(std::byte)(val7 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val7 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val7 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val7 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val7 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val7 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val7 >> 56)});
  // Virtual address
  auto val8 = static_cast<uint64_t>(dataVirtualAddress);
  elf.emplace_back(std::byte{(std::byte)(val8)});
  elf.emplace_back(std::byte{(std::byte)(val8 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val8 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val8 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val8 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val8 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val8 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val8 >> 56)});
  // Physical address
  auto val9 = static_cast<uint64_t>(dataVirtualAddress);
  elf.emplace_back(std::byte{(std::byte)(val9)});
  elf.emplace_back(std::byte{(std::byte)(val9 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val9 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val9 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val9 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val9 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val9 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val9 >> 56)});
  // Number of bytes in file image
  auto val10 = static_cast<uint64_t>(dataSize);
  elf.emplace_back(std::byte{(std::byte)(val10)});
  elf.emplace_back(std::byte{(std::byte)(val10 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val10 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val10 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val10 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val10 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val10 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val10 >> 56)});
  // Number of bytes in memory image
  auto val11 = static_cast<uint64_t>(dataSize);
  elf.emplace_back(std::byte{(std::byte)(val11)});
  elf.emplace_back(std::byte{(std::byte)(val11 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val11 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val11 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val11 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val11 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val11 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val11 >> 56)});
  //...
  auto val12 = static_cast<uint64_t>(kAlignment);
  elf.emplace_back(std::byte{(std::byte)(val12)});
  elf.emplace_back(std::byte{(std::byte)(val12 >> 8)});
  elf.emplace_back(std::byte{(std::byte)(val12 >> 16)});
  elf.emplace_back(std::byte{(std::byte)(val12 >> 24)});
  elf.emplace_back(std::byte{(std::byte)(val12 >> 32)});
  elf.emplace_back(std::byte{(std::byte)(val12 >> 40)});
  elf.emplace_back(std::byte{(std::byte)(val12 >> 48)});
  elf.emplace_back(std::byte{(std::byte)(val12 >> 56)});

  // Output the text segment
  for (const auto byte : mTextSection) {
    elf.emplace_back(std::byte{byte});
  }
  // Output the data segment
  for (const auto byte : mDataSection) {
    elf.emplace_back(std::byte{byte});
  }

  return elf;
}

void ELF::compile() {
  namespace fs = std::filesystem;
  const auto filename{"a.out"};
  Bytes data = assembleELF();
  {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char *>(data.data()), (std::streamsize)data.size());
  }
  fs::permissions(filename, fs::perms::all, fs::perm_options::add);
}
