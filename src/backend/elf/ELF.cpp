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
#include <filesystem>
#include <fstream>
// Wisnia
#include "ELF.hpp"

using namespace Wisnia;

ELF::ELF(const ByteArray &textSection, const ByteArray &dataSection)
    : mTextSection{textSection}, mDataSection{dataSection} {}

ByteArray ELF::assembleELF() {
  ByteArray elf{};

  const auto textSize = uint64_t(mTextSection.size());
  const auto dataSize = uint64_t(mDataSection.size());
  const auto dataOffset = uint64_t(kTextOffset + textSize);
  const auto dataVirtualAddress = uint64_t(kDataVirtualStartAddress + dataOffset);

  elf.putBytes(
      // ELF magic value
      std::byte{0x7f}, std::byte{0x45}, std::byte{0x4c}, std::byte{0x46},

      std::byte{0x02}, // 64-bit executable
      std::byte{0x01}, // Little endian
      std::byte{0x01}, // ELF version
      std::byte{0x00}, // Target OS ABI
      std::byte{0x00}, // Further specify ABI version

      // Unused bytes
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00},

      std::byte{0x02}, std::byte{0x00}, // Executable type
      std::byte{0x3e}, std::byte{0x00}, // x86-64 target architecture

      // ELF version
      std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}
  );

  elf.putU64(kVirtualStartAddress + kTextOffset); // 64-bit virtual offset

  elf.putBytes(
      // Offset from file to program header
      std::byte{0x40}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      // Start of section header table
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      // Flags
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},

      std::byte{0x40}, std::byte{0x00}, // Size of this header
      std::byte{0x38}, std::byte{0x00}, // Size of a program header table entry
      std::byte{0x02}, std::byte{0x00}, // Length of data and text sections
      std::byte{0x00}, std::byte{0x00}, // Size of section header
      std::byte{0x00}, std::byte{0x00}, // Number of entries in section header
      std::byte{0x00}, std::byte{0x00}  // Index of section header table entry
  );

  // Build Program Header. Text Segment
  elf.putBytes(
      std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // PT_LOAD
      std::byte{0x07}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}  // Flags
  );

  elf.putU64(0);                    // Offset from the beginning of the file
  elf.putU64(kVirtualStartAddress); // Virtual address
  elf.putU64(kVirtualStartAddress); // Physical address
  elf.putU64(textSize);             // Number of bytes in file image of segment
  elf.putU64(textSize);             // Number of bytes in memory image of segment
  elf.putU64(kAlignment);           // Alignment

  // Build Program Header. Data Segment
  elf.putBytes(
      std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // PT_LOAD
      std::byte{0x07}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}  // Flags
  );

  elf.putU64(dataOffset);         // Offset address
  elf.putU64(dataVirtualAddress); // Virtual address
  elf.putU64(dataVirtualAddress); // Physical address
  elf.putU64(dataSize);           // Number of bytes in file image
  elf.putU64(dataSize);           // Number of bytes in memory image
  elf.putU64(kAlignment);         // Alignment

  elf.putBytes(mTextSection); // Output the text segment
  elf.putBytes(mDataSection); // Output the data segment

  return elf;
}

void ELF::compile() {
  namespace fs = std::filesystem;
  const auto filename{"a.out"};
  ByteArray data = assembleELF();
  {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char *>(data.data()), (std::streamsize)data.size());
  }
  fs::permissions(filename, fs::perms::all, fs::perm_options::add);
}
