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
  constexpr auto begin() const noexcept { return m_byteData.begin(); }
  constexpr auto end() const noexcept { return m_byteData.end(); }

 public:
  ByteArray() = default;
  ByteArray(std::initializer_list<std::byte> list) : m_byteData(list) {}

  constexpr size_t size() const { return m_byteData.size(); }
  constexpr const std::byte *data() const { return m_byteData.data(); }

  constexpr const auto &operator[](std::size_t index) const {
    assert(index < size());
    return m_byteData[index];
  }

  constexpr void insert(size_t index, std::byte value) {
    assert(index < size());
    m_byteData[index] = value;
  }

  constexpr void putU32(uint32_t value) {
    m_byteData.emplace_back(static_cast<std::byte>(value));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 8));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 16));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 24));
  }

  constexpr void putU64(uint64_t value) {
    m_byteData.emplace_back(static_cast<std::byte>(value));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 8));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 16));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 24));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 32));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 40));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 48));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 56));
  }

  template <typename... Ts>
  requires (std::is_same_v<Ts, std::byte> || ...)
  constexpr void putBytes(Ts &&...t) {
    std::byte dummy[] = {std::byte{0}, (m_byteData.emplace_back(std::forward<Ts>(t)), std::byte{0})...};
    (void)dummy;
  }

  constexpr void putBytes(const ByteArray &other) {
    m_byteData.insert(m_byteData.end(), other.begin(), other.end());
  }

  std::string getString() const {
    std::stringstream str{};
    for (size_t i = 0; i < m_byteData.size(); i++) {
      str << "0x" << m_byteData[i];
      if (i + 1 < m_byteData.size()) str << ' ';
    }
    return str.str();
  }

 private:
  Bytes m_byteData{};
};

}  // namespace Wisnia

#endif  // WISNIALANG_BYTEARRAY_HPP
