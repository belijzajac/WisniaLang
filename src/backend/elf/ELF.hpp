// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_ELF_HPP
#define WISNIALANG_ELF_HPP

#include <vector>
// Wisnia
#include "ByteArray.hpp"

namespace Wisnia {

constexpr uint64_t kVirtualStartAddress{0x400000};
constexpr uint64_t kDataVirtualStartAddress{0x600000};
constexpr uint64_t kAlignment{0x200000};
constexpr uint64_t kTextOffset{0x40 + (2 * 0x38)};

class ELF {
 public:
  ELF(const ByteArray &textSection, const ByteArray &dataSection);
  void compile();

 private:
  ByteArray assembleELF();

 private:
  ByteArray m_textSection{};
  ByteArray m_dataSection{};
};

}  // namespace Wisnia

#endif  // WISNIALANG_ELF_HPP
