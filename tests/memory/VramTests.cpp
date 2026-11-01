#include <gtest/gtest.h>
#include <stdint.h>

#include <stdexcept>

#include "core/memory/Vram.h"

using core::memory::Vram;

TEST(VramTest, ReadByteValidAddressReturnsWrittenValue) {
  Vram vram;

  // Write a value to VRAM
  const uint16_t address = Vram::kStartAddress;
  const uint8_t expectedValue = 42;
  vram.WriteByte(address, expectedValue);

  // Read the value back
  const uint8_t actualValue = vram.ReadByte(address);

  EXPECT_EQ(expectedValue, actualValue);
}

TEST(VramTest, ReadByteInvalidAddressThrowsOutOfRange) {
  Vram vram;

  // Test reading from an address below the valid range
  const uint16_t invalidAddressLow = Vram::kStartAddress - 1;
  EXPECT_THROW(vram.ReadByte(invalidAddressLow), std::out_of_range);

  // Test reading from an address above the valid range
  const uint16_t invalidAddressHigh = Vram::kStartAddress + Vram::kSize;
  EXPECT_THROW(vram.ReadByte(invalidAddressHigh), std::out_of_range);
}

TEST(VramTest, ReadByteFromUninitializedMemoryReturnsZero) {
  Vram vram;

  // Read from an address in VRAM without writing any value
  const uint16_t address = Vram::kStartAddress;
  const uint8_t actualValue = vram.ReadByte(address);

  EXPECT_EQ(0, actualValue);
}

TEST(VramTest, WriteByteInvalidAddressThrowsOutOfRange) {
  Vram vram;

  // Test writing to an address below the valid range
  const uint16_t invalidAddressLow = Vram::kStartAddress - 1;
  EXPECT_THROW(vram.WriteByte(invalidAddressLow, 42), std::out_of_range);

  // Test writing to an address above the valid range
  const uint16_t invalidAddressHigh = Vram::kStartAddress + Vram::kSize;
  EXPECT_THROW(vram.WriteByte(invalidAddressHigh, 42), std::out_of_range);
}
