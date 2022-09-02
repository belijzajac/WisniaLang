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

#ifndef WISNIALANG_BYTEARRAY_HPP
#define WISNIALANG_BYTEARRAY_HPP

#include <cassert>
#include <cstddef>
#include <sstream>
#include <vector>

namespace Wisnia {

namespace {
  std::ostream &operator<<(std::ostream &os, std::byte b) {
    return os << std::hex << std::to_integer<int>(b);
  }
}  // namespace

class ByteArray {
  using Bytes = std::vector<std::byte>;

 private:
  constexpr auto begin() const noexcept { return mByteData.begin(); }
  constexpr auto end() const noexcept { return mByteData.end(); }

 public:
  constexpr size_t size() const { return mByteData.size(); }
  constexpr const std::byte *data() const { return mByteData.data(); }

  constexpr const auto &operator[](std::size_t index) const {
    assert(index < size());
    return mByteData[index];
  }

  constexpr void insert(size_t index, std::byte value) {
    assert(index < size());
    mByteData[index] = value;
  }

  constexpr void putU32(uint32_t value) {
    mByteData.emplace_back(static_cast<std::byte>(value));
    mByteData.emplace_back(static_cast<std::byte>(value >> 8));
    mByteData.emplace_back(static_cast<std::byte>(value >> 16));
    mByteData.emplace_back(static_cast<std::byte>(value >> 24));
  }

  constexpr void putU64(uint64_t value) {
    mByteData.emplace_back(static_cast<std::byte>(value));
    mByteData.emplace_back(static_cast<std::byte>(value >> 8));
    mByteData.emplace_back(static_cast<std::byte>(value >> 16));
    mByteData.emplace_back(static_cast<std::byte>(value >> 24));
    mByteData.emplace_back(static_cast<std::byte>(value >> 32));
    mByteData.emplace_back(static_cast<std::byte>(value >> 40));
    mByteData.emplace_back(static_cast<std::byte>(value >> 48));
    mByteData.emplace_back(static_cast<std::byte>(value >> 56));
  }

  template <typename... Ts>
  requires (std::is_same_v<Ts, std::byte> || ...)
  constexpr void putBytes(Ts &&...t) {
    std::byte dummy[] = {std::byte{0}, (mByteData.emplace_back(std::forward<Ts>(t)), std::byte{0})...};
    (void)dummy;
  }

  constexpr void putBytes(const ByteArray &other) {
    mByteData.insert(mByteData.end(), other.begin(), other.end());
  }

  std::string getString() const {
    std::stringstream str{};
    for (size_t i = 0; i < mByteData.size(); i++) {
      str << "0x" << mByteData[i];
      if (i + 1 < mByteData.size()) str << ' ';
    }
    return str.str();
  }

 private:
  Bytes mByteData{};
};

}  // namespace Wisnia

#endif  // WISNIALANG_BYTEARRAY_HPP
