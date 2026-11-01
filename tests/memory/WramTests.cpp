#include <gtest/gtest.h>
#include <stdint.h>

#include <stdexcept>

#include "core/memory/Wram.h"

using core::memory::Wram;

TEST(WramTest, ReadByteValidAddressReturnsWrittenValue) {
  Wram wram;

  // Write a value to WRAM
  const uint16_t address = Wram::kStartAddress;
  const uint8_t expectedValue = 42;
  wram.WriteByte(address, expectedValue);

  // Read the value back
  const uint8_t actualValue = wram.ReadByte(address);

  EXPECT_EQ(expectedValue, actualValue);
}

TEST(WramTest, ReadByteInvalidAddressThrowsOutOfRange) {
  Wram wram;

  // Test reading from an address below the valid range
  const uint16_t invalidAddressLow = Wram::kStartAddress - 1;
  EXPECT_THROW(wram.ReadByte(invalidAddressLow), std::out_of_range);

  // Test reading from an address above the valid range
  const uint16_t invalidAddressHigh = Wram::kStartAddress + Wram::kSize;
  EXPECT_THROW(wram.ReadByte(invalidAddressHigh), std::out_of_range);
}

TEST(WramTest, ReadByteFromUninitializedMemoryReturnsZero) {
  Wram wram;

  // Read from an address in WRAM without writing any value
  const uint16_t address = Wram::kStartAddress;
  const uint8_t actualValue = wram.ReadByte(address);

  EXPECT_EQ(0, actualValue);
}

TEST(WramTest, WriteByteInvalidAddressThrowsOutOfRange) {
  Wram wram;

  // Test writing to an address below the valid range
  const uint16_t invalidAddressLow = Wram::kStartAddress - 1;
  EXPECT_THROW(wram.WriteByte(invalidAddressLow, 42), std::out_of_range);

  // Test writing to an address above the valid range
  const uint16_t invalidAddressHigh = Wram::kStartAddress + Wram::kSize;
  EXPECT_THROW(wram.WriteByte(invalidAddressHigh, 42), std::out_of_range);
}
