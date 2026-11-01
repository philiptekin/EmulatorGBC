#include <gtest/gtest.h>
#include <stdint.h>

#include "core/cpu/Registers.h"

using core::cpu::Registers;

// General purpose register tests

TEST(RegistersTest, AFReturnsCorrectValue) {
  Registers registers;
  const uint16_t notExpectedAF = 0x1234;
  registers.SetAF(notExpectedAF);

  const uint16_t actualAF = registers.GetAF();

  // The expected value is 0x1234, but the F register should only contain the
  // upper nibble (0x30) of the lower byte.
  EXPECT_NE(notExpectedAF, actualAF);

  // The actual value should be 0x1230
  EXPECT_EQ(0x1230, actualAF);
}

TEST(RegistersTest, BCReturnsCorrectValue) {
  Registers registers;
  const uint16_t expectedBC = 0xABCD;
  registers.SetBC(expectedBC);

  const uint16_t actualBC = registers.GetBC();

  EXPECT_EQ(expectedBC, actualBC);
}

TEST(RegistersTest, DEReturnsCorrectValue) {
  Registers registers;
  const uint16_t expectedDE = 0x5678;
  registers.SetDE(expectedDE);

  const uint16_t actualDE = registers.GetDE();

  EXPECT_EQ(expectedDE, actualDE);
}

TEST(RegistersTest, HLReturnsCorrectValue) {
  Registers registers;
  const uint16_t expectedHL = 0x9ABC;
  registers.SetHL(expectedHL);

  const uint16_t actualHL = registers.GetHL();

  EXPECT_EQ(expectedHL, actualHL);
}

// Flag accessor tests

TEST(RegistersTest, GetZeroFlagReturnsCorrectValue) {
  Registers registers;
  registers.F = 0x80;  // Set the Zero flag (bit 7)

  EXPECT_EQ(1, registers.GetFlagZ());
}

TEST(RegistersTest, GetSubtractFlagReturnsCorrectValue) {
  Registers registers;
  registers.F = 0x40;  // Set the Subtract flag (bit 6)

  EXPECT_EQ(1, registers.GetFlagN());
}

TEST(RegistersTest, GetHalfCarryFlagReturnsCorrectValue) {
  Registers registers;
  registers.F = 0x20;  // Set the Half Carry flag (bit 5)

  EXPECT_EQ(1, registers.GetFlagH());
}

TEST(RegistersTest, GetCarryFlagReturnsCorrectValue) {
  Registers registers;
  registers.F = 0x10;  // Set the Carry flag (bit 4)

  EXPECT_EQ(1, registers.GetFlagC());
}

TEST(RegistersTest, GetAllFlagsReturnsSetValues) {
  Registers registers;
  registers.F = 0xF0;  // Set all flags (bits 7-4)

  EXPECT_EQ(1, registers.GetFlagZ());
  EXPECT_EQ(1, registers.GetFlagN());
  EXPECT_EQ(1, registers.GetFlagH());
  EXPECT_EQ(1, registers.GetFlagC());
}

TEST(RegistersTest, GetAllFlagsReturnsClearValuesWhenUnset) {
  Registers registers;
  registers.F = 0x00;  // Clear all flags

  EXPECT_EQ(0, registers.GetFlagZ());
  EXPECT_EQ(0, registers.GetFlagN());
  EXPECT_EQ(0, registers.GetFlagH());
  EXPECT_EQ(0, registers.GetFlagC());
}

TEST(RegistersTest, GetAllFlagsReturnsMixedValues) {
  Registers registers;
  registers.F = 0xA0;  // Set the Zero and Half Carry flags (bits 7 and 5)

  EXPECT_EQ(1, registers.GetFlagZ());
  EXPECT_EQ(0, registers.GetFlagN());
  EXPECT_EQ(1, registers.GetFlagH());
  EXPECT_EQ(0, registers.GetFlagC());
}
