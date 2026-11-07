#include "CpuTestFixture.h"

namespace {
constexpr uint8_t kAddBToA = 0x80;
constexpr uint8_t kAddAddressInHLToA = 0x86;
constexpr uint8_t kAdcBToA = 0x88;
constexpr uint8_t kAdcAddressInHLToA = 0x8E;
constexpr uint8_t kSubBToA = 0x90;
constexpr uint8_t kSubAddressInHLToA = 0x96;
constexpr uint8_t kSbcBToA = 0x98;
constexpr uint8_t kAndBToA = 0xA0;
constexpr uint8_t kXorBToA = 0xA8;
constexpr uint8_t kOrBToA = 0xB0;
constexpr uint8_t kCompareBToA = 0xB8;
}  // namespace

// Block 2

TEST_F(CpuTest, AddBToA) {
  LoadProgram({kAddBToA});

  Registers initials = DefaultRegisters();
  initials.B = 0x10U;
  initials.A = 0x05U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x15U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AddBToAHalfFlag) {
  LoadProgram({kAddBToA});

  Registers initials = DefaultRegisters();
  initials.B = 0xFU;
  initials.A = 0x01U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x10U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AddBToACarryFlag) {
  LoadProgram({kAddBToA});

  Registers initials = DefaultRegisters();
  // 0b1000 0000
  // 0b1000 0000
  // result:
  // 0b0000 0000
  initials.B = 0x80U;
  initials.A = 0x80U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AddBToACarryFlagAndHalfFlag) {
  LoadProgram({kAddBToA});

  Registers initials = DefaultRegisters();
  initials.B = 0xFFU;
  initials.A = 0x01U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AddAddressInHLToA) {
  LoadProgram({kAddAddressInHLToA, 0x10});

  Registers initials = DefaultRegisters();
  initials.SetHL(kProgramStart + 1);
  initials.A = 0x01U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x11U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcBToA) {
  LoadProgram({kAdcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x05U;
  initials.B = 0x10U;
  initials.SetFlagH(true);
  initials.SetFlagC(false);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x15U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcBToAIncludesCarryIn) {
  LoadProgram({kAdcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x10U;
  initials.B = 0x20U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x31U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcBToAHalfCarryIncludesCarryIn) {
  LoadProgram({kAdcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x0FU;
  initials.B = 0x00U;
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x10U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcBToACarryWithoutHalfCarry) {
  LoadProgram({kAdcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xF0U;
  initials.B = 0x10U;
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x01U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcBToACarryAndHalfCarrySetZero) {
  LoadProgram({kAdcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xFFU;
  initials.B = 0x00U;
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcAddressInHLToAUsesCarryInAndMemoryCycleCount) {
  LoadProgram({kAdcAddressInHLToA});

  Registers initial = DefaultRegisters();
  initial.A = 0x0FU;
  initial.SetHL(kDataAddress);
  initial.SetFlagC(true);
  bus_.Write(kDataAddress, 0x00U);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(0x10U, cpu.GetRegisters().A);
  EXPECT_TRUE(cpu.GetRegisters().GetFlagH());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagC());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagN());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, SubBToA) {
  LoadProgram({kSubBToA});

  Registers initials = DefaultRegisters();
  initials.B = 0x10U;
  initials.A = 0x11U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x01U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SubBToAZFlag) {
  LoadProgram({kSubBToA});

  Registers initials = DefaultRegisters();
  initials.B = 0x11U;
  initials.A = 0x11U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(false);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SubBToAHalfCarryFlag) {
  LoadProgram({kSubBToA});

  Registers initials = DefaultRegisters();
  initials.B = 0x08U;
  initials.A = 0x11U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x09U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SubBToACarryAndHalfCarryFlag) {
  LoadProgram({kSubBToA});

  Registers initials = DefaultRegisters();
  initials.B = 0x08U;
  initials.A = 0x01U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0xF9U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SubAddressInHLToA) {
  LoadProgram({kSubAddressInHLToA, 0x01});

  Registers initials = DefaultRegisters();
  initials.SetHL(kProgramStart + 1);
  initials.A = 0x01U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0U, cpu.GetRegisters().A);
  EXPECT_EQ(kProgramStart + 0x01U, cpu.GetRegisters().GetHL());
}

TEST_F(CpuTest, SbcBToA) {
  LoadProgram({kSbcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x12U;
  initials.B = 0x01U;
  initials.SetFlagH(true);
  initials.SetFlagC(false);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x11U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SbcBToAIncludesCarryIn) {
  LoadProgram({kSbcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x12U;
  initials.B = 0x01U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x10U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SbcBToAHalfBorrowFromCarryIn) {
  LoadProgram({kSbcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x10U;
  initials.B = 0x00U;
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0FU, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SbcBToACarryAndHalfBorrow) {
  LoadProgram({kSbcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x00U;
  initials.B = 0x01U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0xFFU, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SbcBToAZeroSourceWithCarryInDoesNotSetHalfBorrow) {
  LoadProgram({kSbcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x11U;
  initials.B = 0x00U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x10U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SbcBToACarryAndHalfBorrowSetZero) {
  LoadProgram({kSbcBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x00U;
  initials.B = 0xFFU;
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AndBToA) {
  LoadProgram({kAndBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xF3U;
  initials.B = 0x5AU;
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x52U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AndBToAZeroFlag) {
  LoadProgram({kAndBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x0FU;
  initials.B = 0xF0U;
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, XorBToA) {
  LoadProgram({kXorBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xAAU;
  initials.B = 0x0FU;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0xA5U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, XorBToAZeroFlag) {
  LoadProgram({kXorBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x5AU;
  initials.B = 0x5AU;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, OrBToA) {
  LoadProgram({kOrBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x50U;
  initials.B = 0x0FU;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x5FU, cpu.GetRegisters().A);
}

TEST_F(CpuTest, OrBToAZeroFlag) {
  LoadProgram({kOrBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x00U;
  initials.B = 0x00U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, CompareBToA) {
  LoadProgram({kCompareBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x20U;
  initials.B = 0x10U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x20U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, CompareBToACarryAndHalfCarry) {
  LoadProgram({kCompareBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x10U;
  initials.B = 0x11U;
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x10U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, CompareBToAZeroFlag) {
  LoadProgram({kCompareBToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x22U;
  initials.B = 0x22U;
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x22U, cpu.GetRegisters().A);
}
