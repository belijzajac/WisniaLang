// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <utility>
// Wisnia
#include "ELF.hpp"

using namespace Wisnia;

ELF::ELF(ByteArray textSection, ByteArray dataSection)
    : m_textSection{std::move(textSection)}, m_dataSection{std::move(dataSection)} {}

ByteArray ELF::generateELF() {
  ByteArray elfData{};

  const auto textSize = uint64_t(m_textSection.size());
  const auto dataSize = uint64_t(m_dataSection.size());
  const auto dataOffset = uint64_t(kTextOffset + textSize);
  const auto dataVirtualAddress = uint64_t(kDataVirtualStartAddress + dataOffset);

  elfData.putBytes(
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

  elfData.putValue<uint64_t>(kVirtualStartAddress + kTextOffset); // 64-bit virtual offset

  elfData.putBytes(
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

  // Build program header: text segment
  elfData.putBytes(
      std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // PT_LOAD
      std::byte{0x07}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}  // Flags
  );

  elfData.putValue<uint64_t>(0);                    // Offset from the beginning of the file
  elfData.putValue<uint64_t>(kVirtualStartAddress); // Virtual address
  elfData.putValue<uint64_t>(kVirtualStartAddress); // Physical address
  elfData.putValue<uint64_t>(textSize);             // Number of bytes in file image of segment
  elfData.putValue<uint64_t>(textSize);             // Number of bytes in memory image of segment
  elfData.putValue<uint64_t>(kAlignment);           // Alignment

  // Build program header: data segment
  elfData.putBytes(
      std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // PT_LOAD
      std::byte{0x07}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}  // Flags
  );

  elfData.putValue<uint64_t>(dataOffset);         // Offset address
  elfData.putValue<uint64_t>(dataVirtualAddress); // Virtual address
  elfData.putValue<uint64_t>(dataVirtualAddress); // Physical address
  elfData.putValue<uint64_t>(dataSize);           // Number of bytes in file image
  elfData.putValue<uint64_t>(dataSize);           // Number of bytes in memory image
  elfData.putValue<uint64_t>(kAlignment);         // Alignment

  elfData.putBytes(m_textSection); // Output the text segment
  elfData.putBytes(m_dataSection); // Output the data segment

  return elfData;
}

void ELF::writeELF(std::string_view filename) {
  namespace fs = std::filesystem;
  ByteArray elfData = generateELF();
  {
    std::ofstream file(filename.data(), std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char *>(elfData.data()), (std::streamsize)elfData.size());
  }
  fs::permissions(filename, fs::perms::all, fs::perm_options::add);
}
