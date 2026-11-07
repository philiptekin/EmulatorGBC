#include "CpuTestFixture.h"

namespace {
constexpr uint8_t kHalt = 0x76;
constexpr uint8_t kLoadBIntoB = 0x40;
constexpr uint8_t kLoadHLIntoB = 0x46;
}  // namespace

// Block 1

TEST_F(CpuTest, Halt) {
  LoadProgram({kHalt});
  Cpu cpu = MakeCpu(DefaultRegisters());
  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, LoadBIntoB) {
  LoadProgram({kLoadBIntoB});

  Registers initials = DefaultRegisters();
  initials.B = 0x5U;
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(0x5U, cpu.GetRegisters().B);
}

TEST_F(CpuTest, LoadHLIntoB) {
  LoadProgram({kLoadHLIntoB, 0x10});

  Registers initials = DefaultRegisters();
  initials.B = 0x5U;
  initials.SetHL(kProgramStart + 1);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(0x10U, cpu.GetRegisters().B);
  EXPECT_EQ(0x10U, bus_.Read(cpu.GetRegisters().GetHL()));
  EXPECT_EQ(cpu.GetRegisters().GetHL(), kProgramStart + 1);
}
