// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <gtest/gtest.h>
// Wisnia
#include "ByteArray.hpp"

using namespace Wisnia;

TEST(ByteArrayTest, PutValueUInt8) {
  ByteArray array{};
  array.putValue<uint8_t>(42);
  ASSERT_EQ(array.size(), 1);
  EXPECT_EQ(array[0], std::byte{42});
}

TEST(ByteArrayTest, PutValueUInt16) {
  ByteArray array{};
  array.putValue<uint16_t>(0xABCD);
  ASSERT_EQ(array.size(), 2);
  EXPECT_EQ(array[0], std::byte{0xCD});
  EXPECT_EQ(array[1], std::byte{0xAB});
}

TEST(ByteArrayTest, PutValueUInt32) {
  ByteArray array{};
  array.putValue<uint32_t>(0x12345678);
  ASSERT_EQ(array.size(), 4);
  EXPECT_EQ(array[0], std::byte{0x78});
  EXPECT_EQ(array[1], std::byte{0x56});
  EXPECT_EQ(array[2], std::byte{0x34});
  EXPECT_EQ(array[3], std::byte{0x12});
}

TEST(ByteArrayTest, PutValueUInt64) {
  ByteArray array{};
  array.putValue<uint64_t>(0x0123456789ABCDEF);
  ASSERT_EQ(array.size(), 8);
  EXPECT_EQ(array[0], std::byte{0xEF});
  EXPECT_EQ(array[1], std::byte{0xCD});
  EXPECT_EQ(array[2], std::byte{0xAB});
  EXPECT_EQ(array[3], std::byte{0x89});
  EXPECT_EQ(array[4], std::byte{0x67});
  EXPECT_EQ(array[5], std::byte{0x45});
  EXPECT_EQ(array[6], std::byte{0x23});
  EXPECT_EQ(array[7], std::byte{0x01});
}

TEST(ByteArrayTest, Endianness) {
  ByteArray array{};
  array.putValue<uint16_t>(0x1234);
  ASSERT_EQ(array.size(), 2);
  EXPECT_EQ(array[0], std::byte{0x34});
  EXPECT_EQ(array[1], std::byte{0x12});
}

TEST(ByteArrayTest, InvalidType) {
  ByteArray array{};
  ASSERT_DEATH(array.putValue<float>(3.14f), "Unsupported type T");
  ASSERT_DEATH(array.putValue<int>(42), "Unsupported type T");
}

TEST(ByteArrayTest, MultipleInvocations) {
  ByteArray array{};
  array.putValue<uint8_t>(42);
  array.putValue<uint16_t>(0xABCD);
  array.putValue<uint32_t>(0x12345678);
  array.putValue<uint64_t>(0x0123456789ABCDEF);
  ASSERT_EQ(array.size(), 15);
  // uint8_t
  EXPECT_EQ(array[0], std::byte{42});
  // uint16_t
  EXPECT_EQ(array[1], std::byte{0xCD});
  EXPECT_EQ(array[2], std::byte{0xAB});
  // uint32_t
  EXPECT_EQ(array[3], std::byte{0x78});
  EXPECT_EQ(array[4], std::byte{0x56});
  EXPECT_EQ(array[5], std::byte{0x34});
  EXPECT_EQ(array[6], std::byte{0x12});
  // uint64_t
  EXPECT_EQ(array[7], std::byte{0xEF});
  EXPECT_EQ(array[8], std::byte{0xCD});
  EXPECT_EQ(array[9], std::byte{0xAB});
  EXPECT_EQ(array[10], std::byte{0x89});
  EXPECT_EQ(array[11], std::byte{0x67});
  EXPECT_EQ(array[12], std::byte{0x45});
  EXPECT_EQ(array[13], std::byte{0x23});
  EXPECT_EQ(array[14], std::byte{0x01});
}
