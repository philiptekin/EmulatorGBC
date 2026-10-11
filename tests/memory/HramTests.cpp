#include <gtest/gtest.h>

#include "core/memory/Hram.h"

using namespace core::memory;

TEST(HramTest, ReadByte_ValidAddress_ReturnsCorrectValue) {
  Hram hram;

  // Write a value to HRAM
  uint16_t address = Hram::StartAddress_;
  std::uint8_t expectedValue = 42;
  hram.WriteByte(address, expectedValue);

  // Read the value back
  std::uint8_t actualValue = hram.ReadByte(address);

  EXPECT_EQ(expectedValue, actualValue);
}

TEST(HramTest, ReadByte_InvalidAddress_ThrowsOutOfRange) {
  Hram hram;

  // Test reading from an address below the valid range
  uint16_t invalidAddressLow = Hram::StartAddress_ - 1;
  EXPECT_THROW(hram.ReadByte(invalidAddressLow), std::out_of_range);

  // Test reading from an address above the valid range
  uint16_t invalidAddressHigh = Hram::StartAddress_ + Hram::Size_;
  EXPECT_THROW(hram.ReadByte(invalidAddressHigh), std::out_of_range);
}

TEST(HramTest, ReadByte_EmptyHram_ReturnsZero) {
  Hram hram;

  // Read from an address in HRAM without writing any value
  uint16_t address = Hram::StartAddress_;
  std::uint8_t actualValue = hram.ReadByte(address);

  EXPECT_EQ(0, actualValue);
}

TEST(HramTest, WriteByte_InvalidAddress_ThrowsOutOfRange) {
  Hram hram;

  // Test writing to an address below the valid range
  uint16_t invalidAddressLow = Hram::StartAddress_ - 1;
  EXPECT_THROW(hram.WriteByte(invalidAddressLow, 42), std::out_of_range);

  // Test writing to an address above the valid range
  uint16_t invalidAddressHigh = Hram::StartAddress_ + Hram::Size_;
  EXPECT_THROW(hram.WriteByte(invalidAddressHigh, 42), std::out_of_range);
}
