#include <gtest/gtest.h>

#include "core/memory/Wram.h"

using namespace core::memory;

TEST(WramTest, ReadByte_ValidAddress_ReturnsCorrectValue) {
  Wram wram;

  // Write a value to WRAM
  uint16_t address = Wram::StartAddress_;
  std::uint8_t expectedValue = 42;
  wram.WriteByte(address, expectedValue);

  // Read the value back
  std::uint8_t actualValue = wram.ReadByte(address);

  EXPECT_EQ(expectedValue, actualValue);
}

TEST(WramTest, ReadByte_InvalidAddress_ThrowsOutOfRange) {
  Wram wram;

  // Test reading from an address below the valid range
  uint16_t invalidAddressLow = Wram::StartAddress_ - 1;
  EXPECT_THROW(wram.ReadByte(invalidAddressLow), std::out_of_range);

  // Test reading from an address above the valid range
  uint16_t invalidAddressHigh = Wram::StartAddress_ + Wram::Size_;
  EXPECT_THROW(wram.ReadByte(invalidAddressHigh), std::out_of_range);
}

TEST(WramTest, ReadByte_EmptyWram_ReturnsZero) {
  Wram wram;

  // Read from an address in WRAM without writing any value
  uint16_t address = Wram::StartAddress_;
  std::uint8_t actualValue = wram.ReadByte(address);

  EXPECT_EQ(0, actualValue);
}

TEST(WramTest, WriteByte_InvalidAddress_ThrowsOutOfRange) {
  Wram wram;

  // Test writing to an address below the valid range
  uint16_t invalidAddressLow = Wram::StartAddress_ - 1;
  EXPECT_THROW(wram.WriteByte(invalidAddressLow, 42), std::out_of_range);

  // Test writing to an address above the valid range
  uint16_t invalidAddressHigh = Wram::StartAddress_ + Wram::Size_;
  EXPECT_THROW(wram.WriteByte(invalidAddressHigh, 42), std::out_of_range);
}
