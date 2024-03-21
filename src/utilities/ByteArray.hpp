// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_BYTE_ARRAY_HPP
#define WISNIALANG_BYTE_ARRAY_HPP

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
  constexpr ByteArray() = default;

  template <typename T>
  constexpr explicit ByteArray(T value) {
    if constexpr (std::is_same<T, uint32_t>()) {
      putValue<uint32_t>(value);
    } else if constexpr (std::is_same<T, uint64_t>()) {
      putValue<uint64_t>(value);
    } else if constexpr (std::is_same<T, std::byte>()) {
      putBytes(value);
    } else {
      assert(0 && "No matching type to initialize ByteArray with");
    }
  }

  constexpr ByteArray(std::initializer_list<std::byte> list) : m_byteData(list) {}

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

  template <typename T>
  constexpr void putValue(T) {
    assert(0 && "Unsupported type T");
  }

  template <std::same_as<uint32_t> T>
  constexpr void putValue(T value) {
    m_byteData.emplace_back(static_cast<std::byte>(value));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 8));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 16));
    m_byteData.emplace_back(static_cast<std::byte>(value >> 24));
  }

  template <std::same_as<uint64_t> T>
  constexpr void putValue(T value) {
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
      str << (((m_byteData[i] >> 4) == std::byte{0x0}) ? '0' : '\0') << m_byteData[i];
      if ((i + 1) % 8 == 0) str << '\n';
      else if (i + 1 < m_byteData.size()) str << ' ';
    }
    return str.str();
  }

 private:
  Bytes m_byteData{};
};

}  // namespace Wisnia

#endif  // WISNIALANG_BYTE_ARRAY_HPP
