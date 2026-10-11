#include <gtest/gtest.h>

#include "core/memory/Vram.h"

using namespace core::memory;

TEST(VramTest, ReadByte_ValidAddress_ReturnsCorrectValue) {
  Vram vram;

  // Write a value to VRAM
  uint16_t address = Vram::StartAddress_;
  std::uint8_t expectedValue = 42;
  vram.WriteByte(address, expectedValue);

  // Read the value back
  std::uint8_t actualValue = vram.ReadByte(address);

  EXPECT_EQ(expectedValue, actualValue);
}

TEST(VramTest, ReadByte_InvalidAddress_ThrowsOutOfRange) {
  Vram vram;

  // Test reading from an address below the valid range
  uint16_t invalidAddressLow = Vram::StartAddress_ - 1;
  EXPECT_THROW(vram.ReadByte(invalidAddressLow), std::out_of_range);

  // Test reading from an address above the valid range
  uint16_t invalidAddressHigh = Vram::StartAddress_ + Vram::Size_;
  EXPECT_THROW(vram.ReadByte(invalidAddressHigh), std::out_of_range);
}

TEST(VramTest, ReadByte_EmptyVram_ReturnsZero) {
  Vram vram;

  // Read from an address in VRAM without writing any value
  uint16_t address = Vram::StartAddress_;
  std::uint8_t actualValue = vram.ReadByte(address);

  EXPECT_EQ(0, actualValue);
}

TEST(VramTest, WriteByte_InvalidAddress_ThrowsOutOfRange) {
  Vram vram;

  // Test writing to an address below the valid range
  uint16_t invalidAddressLow = Vram::StartAddress_ - 1;
  EXPECT_THROW(vram.WriteByte(invalidAddressLow, 42), std::out_of_range);

  // Test writing to an address above the valid range
  uint16_t invalidAddressHigh = Vram::StartAddress_ + Vram::Size_;
  EXPECT_THROW(vram.WriteByte(invalidAddressHigh, 42), std::out_of_range);
}
