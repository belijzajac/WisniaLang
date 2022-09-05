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
