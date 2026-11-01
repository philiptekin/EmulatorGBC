#include <gtest/gtest.h>
#include <stdint.h>

#include <stdexcept>

#include "core/memory/Hram.h"

using core::memory::Hram;

TEST(HramTest, ReadByteValidAddressReturnsWrittenValue) {
  Hram hram;

  // Write a value to HRAM
  const uint16_t address = Hram::kStartAddress;
  const uint8_t expectedValue = 42;
  hram.WriteByte(address, expectedValue);

  // Read the value back
  const uint8_t actualValue = hram.ReadByte(address);

  EXPECT_EQ(expectedValue, actualValue);
}

TEST(HramTest, ReadByteInvalidAddressThrowsOutOfRange) {
  Hram hram;

  // Test reading from an address below the valid range
  const uint16_t invalidAddressLow = Hram::kStartAddress - 1;
  EXPECT_THROW(hram.ReadByte(invalidAddressLow), std::out_of_range);

  // Test reading from an address above the valid range
  const uint16_t invalidAddressHigh = Hram::kStartAddress + Hram::kSize;
  EXPECT_THROW(hram.ReadByte(invalidAddressHigh), std::out_of_range);
}

TEST(HramTest, ReadByteFromUninitializedMemoryReturnsZero) {
  Hram hram;

  // Read from an address in HRAM without writing any value
  const uint16_t address = Hram::kStartAddress;
  const uint8_t actualValue = hram.ReadByte(address);

  EXPECT_EQ(0, actualValue);
}

TEST(HramTest, WriteByteInvalidAddressThrowsOutOfRange) {
  Hram hram;

  // Test writing to an address below the valid range
  const uint16_t invalidAddressLow = Hram::kStartAddress - 1;
  EXPECT_THROW(hram.WriteByte(invalidAddressLow, 42), std::out_of_range);

  // Test writing to an address above the valid range
  const uint16_t invalidAddressHigh = Hram::kStartAddress + Hram::kSize;
  EXPECT_THROW(hram.WriteByte(invalidAddressHigh, 42), std::out_of_range);
}
