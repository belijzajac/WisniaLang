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

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Wisnia {

class ByteArray {
  using Bytes = std::vector<std::byte>;

 private:
  constexpr auto begin() const noexcept { return mByteData.begin(); }
  constexpr auto end() const noexcept { return mByteData.end(); }

 public:
  size_t size() const { return mByteData.size(); }
  const std::byte *data() const { return mByteData.data(); }

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

 private:
  Bytes mByteData{};
};

}  // namespace Wisnia

#endif  // WISNIALANG_BYTEARRAY_HPP
