#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "CpuTestFixture.h"
#include "core/cpu/Registers.h"
#include "gtest/gtest.h"

namespace {
constexpr uint8_t kAddImmediate8ToA = 0xC6;
constexpr uint8_t kAdcImmediate8ToA = 0xCE;
constexpr uint8_t kSubImmediate8ToA = 0xD6;
constexpr uint8_t kSbcImmediate8ToA = 0xDE;
constexpr uint8_t kAndImmediate8ToA = 0xE6;
constexpr uint8_t kXorImmediate8ToA = 0xEE;
constexpr uint8_t kOrImmediate8ToA = 0xF6;
constexpr uint8_t kCompareImmediate8ToA = 0xFE;
constexpr uint8_t kRet = 0xC9;
constexpr uint8_t kRetIfNzToImmediate16 = 0xC0;
constexpr uint8_t kRetIfZToImmediate16 = 0xC8;
constexpr uint8_t kRetIfNcToImmediate16 = 0xD0;
constexpr uint8_t kRetIfCToImmediate16 = 0xD8;
constexpr uint8_t kRetI = 0xD9;
constexpr uint8_t kJpToImmediate16 = 0xC3;
constexpr uint8_t kJpIfNzToImmediate16 = 0xC2;
constexpr uint8_t kJpIfZToImmediate16 = 0xCA;
constexpr uint8_t kJpIfNcToImmediate16 = 0xD2;
constexpr uint8_t kJpIfCToImmediate16 = 0xDA;
constexpr uint8_t kJpHl = 0xE9;

constexpr uint8_t kCallIfNzmmediate16 = 0xC4;
constexpr uint8_t kCallIfZmmediate16 = 0xCC;
constexpr uint8_t kCallIfNcmmediate16 = 0xD4;
constexpr uint8_t kCallIfCmmediate16 = 0xDC;
constexpr uint8_t kPopBc = 0xC1;
constexpr uint8_t kPushBc = 0xC5;
constexpr uint8_t kPopDe = 0xD1;
constexpr uint8_t kPushDe = 0xD5;
constexpr uint8_t kPopHl = 0xE1;
constexpr uint8_t kPushHl = 0xE5;
constexpr uint8_t kPopAf = 0xF1;
constexpr uint8_t kPushAf = 0xF5;

constexpr uint8_t kCallImmediate16 = 0xCD;
constexpr uint8_t kRst00 = 0xC7;
constexpr uint8_t kRst08 = 0xCF;
constexpr uint8_t kRst10 = 0xD7;
constexpr uint8_t kRst18 = 0xDF;
constexpr uint8_t kRst20 = 0xE7;
constexpr uint8_t kRst28 = 0xEF;
constexpr uint8_t kRst30 = 0xF7;
constexpr uint8_t kRst38 = 0xFF;

}  // namespace

struct ImmediateJpCase {
  uint16_t target;
  const char* name;
};

class ImmediateJpTest : public CpuTest,
                        public ::testing::WithParamInterface<ImmediateJpCase> {
};
struct ConditionalRetCase {
  uint8_t opcode;
  uint8_t low;
  uint8_t high;
  bool zeroFlag;
  bool carryFlag;
  bool taken;
  std::string name = "Ret";
  std::unordered_map<uint8_t, std::string> opcodeNamePair{
      {kRetIfNzToImmediate16, "RetNz"},
      {kRetIfZToImmediate16, "RetZ"},
      {kRetIfNcToImmediate16, "RetNc"},
      {kRetIfCToImmediate16, "RetC"}};
};

class ConditionalRetTest
    : public CpuTest,
      public ::testing::WithParamInterface<ConditionalRetCase> {};

struct ConditionalJpCase {
  uint8_t opcode;
  bool zeroFlag;
  bool carryFlag;
  bool taken;
  std::string name = "Jp";
  std::unordered_map<uint8_t, std::string> opcodeNamePair{
      {kJpIfNzToImmediate16, "JpNz"},
      {kJpIfZToImmediate16, "JpZ"},
      {kJpIfNcToImmediate16, "JpNc"},
      {kJpIfCToImmediate16, "JpC"}};
};

class ConditionalJpTest
    : public CpuTest,
      public ::testing::WithParamInterface<ConditionalJpCase> {};

struct ConditionalCallCase {
  uint8_t opcode;
  uint16_t target;
  bool zeroFlag;
  bool carryFlag;
  bool taken;
  std::string name = "Call";
  std::unordered_map<uint8_t, std::string> opcodeNamePair{
      {kCallIfNzmmediate16, "CallNz"},
      {kCallIfZmmediate16, "CallZ"},
      {kCallIfNcmmediate16, "CallNc"},
      {kCallIfCmmediate16, "CallC"}};
};

class ConditionalCallTest
    : public CpuTest,
      public ::testing::WithParamInterface<ConditionalCallCase> {};

struct RstCase {
  uint8_t opcode;
  uint16_t target;
};

class RstTest : public CpuTest,
                public ::testing::WithParamInterface<RstCase> {};

struct R16StackCase {
  uint8_t pushOpcode;
  uint8_t popOpcode;
  uint16_t value;
  const char* name;
};

class PushR16StackTest : public CpuTest,
                         public ::testing::WithParamInterface<R16StackCase> {};

class PopR16StackTest : public CpuTest,
                        public ::testing::WithParamInterface<R16StackCase> {};

template <typename T>
std::string ConditionalNameResolution(const ::testing::TestParamInfo<T>& info) {
  std::string returnName;
  if (info.param.opcodeNamePair.count(info.param.opcode) == 0) {
    returnName += "Unknown";
    returnName += info.param.name;
    returnName += info.param.taken ? "Taken" : "NotTaken";

    return returnName;
  }
  returnName += info.param.opcodeNamePair.find(info.param.opcode)->second;

  returnName += info.param.taken ? "Taken" : "NotTaken";
  return returnName;
}

// Block 3

TEST_F(CpuTest, AddImmediate8ToA) {
  LoadProgram({kAddImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x1U;
  bus_.Write(kProgramStart + 1, 0x2U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x3U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AddImmediate8ToAHalfCarry) {
  LoadProgram({kAddImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xFU;
  bus_.Write(kProgramStart + 1, 0x2U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x11U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AddImmediate8ToACarry) {
  LoadProgram({kAddImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xFAU;
  bus_.Write(kProgramStart + 1, 0x6U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcImmediate8ToA) {
  LoadProgram({kAdcImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x1U;
  initials.SetFlagC(true);
  bus_.Write(kProgramStart + 1, 0x2U);
  initials.SetFlagH(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x4U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AdcImmediate8ToACarryFlagsAndZero) {
  LoadProgram({kAdcImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xFDU;
  initials.SetFlagC(true);

  bus_.Write(kProgramStart + 1, 0x2U);
  initials.SetFlagH(false);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SubImmediate8ToA) {
  LoadProgram({kSubImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x6U;
  bus_.Write(kProgramStart + 1, 0x6U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SubImmediate8ToACarry) {
  LoadProgram({kSubImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x7U;
  bus_.Write(kProgramStart + 1, 0x8U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(false);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0xFFU, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SbcImmediate8ToA) {
  LoadProgram({kSbcImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x6U;
  initials.SetFlagC(true);
  bus_.Write(kProgramStart + 1, 0x5U);
  initials.SetFlagH(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x0U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, SbcImmediate8ToACarry) {
  LoadProgram({kSbcImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x7U;
  initials.SetFlagC(true);
  bus_.Write(kProgramStart + 1, 0x7U);
  initials.SetFlagH(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(false);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0xFFU, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AndImmediate8ToA) {
  LoadProgram({kAndImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xF3U;
  bus_.Write(kProgramStart + 1, 0x5AU);
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x52U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, AndImmediate8ToAZeroFlag) {
  LoadProgram({kAndImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x0FU;
  bus_.Write(kProgramStart + 1, 0xF0U);
  initials.SetFlagH(false);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, XorImmediate8ToA) {
  LoadProgram({kXorImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0xAAU;
  bus_.Write(kProgramStart + 1, 0x0FU);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0xA5U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, XorImmediate8ToAZeroFlag) {
  LoadProgram({kXorImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x5AU;
  bus_.Write(kProgramStart + 1, 0x5AU);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, OrImmediate8ToA) {
  LoadProgram({kOrImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x50U;
  bus_.Write(kProgramStart + 1, 0x0FU);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x5FU, cpu.GetRegisters().A);
}

TEST_F(CpuTest, OrImmediate8ToAZeroFlag) {
  LoadProgram({kOrImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x00U;
  bus_.Write(kProgramStart + 1, 0x00U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(true);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x00U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, CompareImmediate8ToA) {
  LoadProgram({kCompareImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x20U;
  bus_.Write(kProgramStart + 1, 0x10U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x20U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, CompareImmediate8ToACarryAndHalfCarry) {
  LoadProgram({kCompareImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x10U;
  bus_.Write(kProgramStart + 1, 0x11U);
  initials.SetFlagH(false);
  initials.SetFlagC(false);
  initials.SetFlagN(false);
  initials.SetFlagZ(true);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x10U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, CompareImmediate8ToAZeroFlag) {
  LoadProgram({kCompareImmediate8ToA});

  Registers initials = DefaultRegisters();
  initials.A = 0x22U;
  bus_.Write(kProgramStart + 1, 0x22U);
  initials.SetFlagH(true);
  initials.SetFlagC(true);
  initials.SetFlagN(false);
  initials.SetFlagZ(false);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(0x22U, cpu.GetRegisters().A);
}

TEST_F(CpuTest, Ret) {
  LoadProgram({kRet});

  Registers initials = DefaultRegisters();
  initials.SP = kProgramStart + 40;
  bus_.Write(initials.SP, 0x5);
  bus_.Write(initials.SP + 1, 0xA);

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(16, cpu.Step());
  EXPECT_EQ(0x0A05, cpu.GetRegisters().PC);
}

TEST_P(ConditionalRetTest, RetConditional) {
  const ConditionalRetCase& testCase = GetParam();
  LoadProgram({testCase.opcode});

  Registers initials = DefaultRegisters();
  initials.SetFlagZ(testCase.zeroFlag);
  initials.SetFlagC(testCase.carryFlag);
  initials.SP = kProgramStart + 40;
  bus_.Write(initials.SP, testCase.low);
  bus_.Write(initials.SP + 1, testCase.high);
  Cpu cpu = MakeCpu(initials);

  SCOPED_TRACE(static_cast<int>(testCase.opcode));
  EXPECT_EQ(testCase.taken ? 20 : 8, cpu.Step());
  const uint16_t programCounterExpectedValue =
      static_cast<uint16_t>((static_cast<int32_t>(testCase.high) << 8) |
                            static_cast<int32_t>(testCase.low));
  EXPECT_EQ(testCase.taken ? programCounterExpectedValue : kProgramStart + 1,
            cpu.GetRegisters().PC);
  EXPECT_TRUE(IsFlagsSetSameValues(initials, cpu.GetRegisters()));
}

INSTANTIATE_TEST_SUITE_P(
    CpuTest, ConditionalRetTest,
    ::testing::Values(
        ConditionalRetCase{kRetIfNzToImmediate16, 0xA, 0x8, true, false, false},
        ConditionalRetCase{kRetIfNzToImmediate16, 0xA, 0x8, false, true, true},
        ConditionalRetCase{kRetIfZToImmediate16, 0xA, 0x8, true, false, true},
        ConditionalRetCase{kRetIfZToImmediate16, 0xA, 0x8, false, true, false},
        ConditionalRetCase{kRetIfNcToImmediate16, 0xA, 0x8, true, false, true},
        ConditionalRetCase{kRetIfNcToImmediate16, 0xA, 0x8, false, true, false},
        ConditionalRetCase{kRetIfCToImmediate16, 0xA, 0x8, true, false, false},
        ConditionalRetCase{kRetIfCToImmediate16, 0xA, 0x8, false, true, true}),
    ConditionalNameResolution<ConditionalRetCase>);

TEST_F(CpuTest, RetI) {
  LoadProgram({kRetI});

  Registers initials = DefaultRegisters();
  initials.SP = kProgramStart + 40;
  bus_.Write(initials.SP, 0x5);
  bus_.Write(initials.SP + 1, 0xA);

  Cpu cpu = MakeCpu(initials);
  cpu.SetIme(false);

  EXPECT_EQ(16, cpu.Step());
  EXPECT_EQ(0x0A05, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetIme());
}

TEST_F(CpuTest, JpOutsideMemory) {
  LoadProgram({kJpIfNzToImmediate16, 0xA0, 0xFE});
  Registers initials = DefaultRegisters();
  initials.SetFlagZ(false);

  Cpu cpu = MakeCpu(initials);
  cpu.Step();
  EXPECT_EQ(0xFEA0, cpu.GetRegisters().PC);

  EXPECT_THROW(cpu.Step(), std::out_of_range);
}

TEST_P(ConditionalJpTest, UsesConditionForTargetAndTiming) {
  const ConditionalJpCase& testCase = GetParam();
  LoadProgram({testCase.opcode, 0x0A, 0xC0});

  Registers initial = DefaultRegisters();
  initial.SetFlagZ(testCase.zeroFlag);
  initial.SetFlagC(testCase.carryFlag);
  initial.SetFlagN(true);
  initial.SetFlagH(true);
  Cpu cpu = MakeCpu(initial);

  SCOPED_TRACE(static_cast<int>(testCase.opcode));
  EXPECT_EQ(testCase.taken ? 16 : 12, cpu.Step());
  EXPECT_EQ(testCase.taken ? 0xC00A : kProgramStart + 3, cpu.GetRegisters().PC);
  EXPECT_EQ(testCase.zeroFlag, cpu.GetRegisters().GetFlagZ());
  EXPECT_EQ(testCase.carryFlag, cpu.GetRegisters().GetFlagC());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagN());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagH());
}

INSTANTIATE_TEST_SUITE_P(
    CpuTest, ConditionalJpTest,
    ::testing::Values(
        ConditionalJpCase{kJpIfNzToImmediate16, false, true, true},
        ConditionalJpCase{kJpIfNzToImmediate16, true, false, false},
        ConditionalJpCase{kJpIfZToImmediate16, true, false, true},
        ConditionalJpCase{kJpIfZToImmediate16, false, true, false},
        ConditionalJpCase{kJpIfNcToImmediate16, true, false, true},
        ConditionalJpCase{kJpIfNcToImmediate16, false, true, false},
        ConditionalJpCase{kJpIfCToImmediate16, false, true, true},
        ConditionalJpCase{kJpIfCToImmediate16, true, false, false}),
    ConditionalNameResolution<ConditionalJpCase>);

TEST_P(ImmediateJpTest, JumpsToLittleEndianImmediateAddress) {
  const uint16_t target = GetParam().target;
  LoadProgram({kJpToImmediate16, static_cast<uint8_t>(target & 0xFFU),
               static_cast<uint8_t>(target >> 8U)});

  Registers initial = DefaultRegisters();
  initial.SetFlagZ(true);
  initial.SetFlagN(true);
  initial.SetFlagH(true);
  initial.SetFlagC(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(16, cpu.Step());
  EXPECT_EQ(target, cpu.GetRegisters().PC);
  EXPECT_TRUE(cpu.GetRegisters().GetFlagZ());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagN());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagH());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagC());
}

INSTANTIATE_TEST_SUITE_P(
    CpuTest, ImmediateJpTest,
    ::testing::Values(ImmediateJpCase{0x0000, "Target0000"},
                      ImmediateJpCase{0x1234, "Target1234"},
                      ImmediateJpCase{0xC00A, "TargetC00A"},
                      ImmediateJpCase{0xFFFE, "TargetFFFE"}),
    [](const ::testing::TestParamInfo<ImmediateJpCase>& info) {
      return std::string(info.param.name);
    });

TEST_F(CpuTest, JpHl) {
  LoadProgram({kJpHl});
  Registers initials = DefaultRegisters();
  initials.SetHL(0x10);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x10, cpu.GetRegisters().PC);
}

TEST_P(ConditionalCallTest, CallImmediate16Conditional) {
  const uint16_t target = GetParam().target;
  const bool isConditionalTaken = GetParam().taken;
  LoadProgram({GetParam().opcode, static_cast<uint8_t>(target & 0xFFU),
               static_cast<uint8_t>(target >> 8U)});

  Registers initials = DefaultRegisters();
  initials.SP = kProgramStart + 40;
  initials.SetFlagZ(GetParam().zeroFlag);
  initials.SetFlagC(GetParam().carryFlag);

  Cpu cpu = MakeCpu(initials);
  EXPECT_EQ(isConditionalTaken ? 24 : 12, cpu.Step());
  EXPECT_EQ(isConditionalTaken ? target : kProgramStart + 3,
            cpu.GetRegisters().PC);
  EXPECT_EQ(isConditionalTaken ? initials.SP - 0x2U : initials.SP,
            cpu.GetRegisters().SP);
  if (isConditionalTaken) {
    EXPECT_EQ(static_cast<uint8_t>((kProgramStart + 3U) & 0xFFU),
              bus_.Read(initials.SP - 2U));
    EXPECT_EQ(static_cast<uint8_t>((kProgramStart + 3U) >> 8U),
              bus_.Read(initials.SP - 1U));
  }
}

INSTANTIATE_TEST_SUITE_P(
    CpuTest, ConditionalCallTest,
    ::testing::Values(
        ConditionalCallCase{kCallIfNcmmediate16, 0xAABB, true, false, true},
        ConditionalCallCase{kCallIfNcmmediate16, 0xAABB, false, true, false},
        ConditionalCallCase{kCallIfCmmediate16, 0xAABB, false, true, true},
        ConditionalCallCase{kCallIfCmmediate16, 0xAABB, false, false, false},
        ConditionalCallCase{kCallIfZmmediate16, 0xAABB, true, false, true},
        ConditionalCallCase{kCallIfZmmediate16, 0xAABB, false, false, false},
        ConditionalCallCase{kCallIfNzmmediate16, 0xAABB, true, true, false},
        ConditionalCallCase{kCallIfNzmmediate16, 0xAABB, false, true, true}),
    ConditionalNameResolution<ConditionalCallCase>);

TEST_F(CpuTest, CallImmediate16) {
  LoadProgram({kCallImmediate16, 0xAA, 0xBB});

  Registers initials = DefaultRegisters();
  initials.SP = kProgramStart + 40;

  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(24, cpu.Step());
  EXPECT_TRUE(IsFlagsSetSameValues(initials, cpu.GetRegisters()));
  EXPECT_EQ(initials.SP - 0x2U, cpu.GetRegisters().SP);
  EXPECT_EQ(0xBBAA, cpu.GetRegisters().PC);
  EXPECT_EQ(static_cast<uint8_t>((kProgramStart + 3U) & 0xFFU),
            bus_.Read(initials.SP - 2U));
  EXPECT_EQ(static_cast<uint8_t>((kProgramStart + 3U) >> 8U),
            bus_.Read(initials.SP - 1U));
}

TEST_P(RstTest, RstTargetsAndPushesReturnAddress) {
  LoadProgram({GetParam().opcode});

  Registers initials = DefaultRegisters();
  initials.SP = kProgramStart + 40;
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(16, cpu.Step());
  EXPECT_EQ(GetParam().target, cpu.GetRegisters().PC);
  EXPECT_EQ(initials.SP - 2U, cpu.GetRegisters().SP);
  EXPECT_EQ(static_cast<uint8_t>((kProgramStart + 1U) & 0xFFU),
            bus_.Read(initials.SP - 2U));
  EXPECT_EQ(static_cast<uint8_t>((kProgramStart + 1U) >> 8U),
            bus_.Read(initials.SP - 1U));
}

INSTANTIATE_TEST_SUITE_P(
    CpuTest, RstTest,
    ::testing::Values(RstCase{kRst00, 0x00}, RstCase{kRst08, 0x08},
                      RstCase{kRst10, 0x10}, RstCase{kRst18, 0x18},
                      RstCase{kRst20, 0x20}, RstCase{kRst28, 0x28},
                      RstCase{kRst30, 0x30}, RstCase{kRst38, 0x38}),
    [](const ::testing::TestParamInfo<RstCase>& info) {
      return "Rst" + std::to_string(static_cast<uint32_t>(info.param.target));
    });

TEST_P(PushR16StackTest, PushesR16StackPairCorrectly) {
  LoadProgram({GetParam().pushOpcode});

  Registers initials = DefaultRegisters();
  initials.SP = kProgramStart + 40;
  initials.SetBC(0x1234);
  initials.SetDE(0x5678);
  initials.SetHL(0x9ABC);
  initials.SetAF(0xDEF0);
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(16, cpu.Step());
  EXPECT_EQ(initials.SP - 2U, cpu.GetRegisters().SP);
  EXPECT_EQ(static_cast<uint8_t>(GetParam().value & 0xFFU),
            bus_.Read(initials.SP - 2U));
  EXPECT_EQ(static_cast<uint8_t>(GetParam().value >> 8U),
            bus_.Read(initials.SP - 1U));
}

INSTANTIATE_TEST_SUITE_P(
    CpuTest, PushR16StackTest,
    ::testing::Values(R16StackCase{kPushBc, kPopBc, 0x1234, "Bc"},
                      R16StackCase{kPushDe, kPopDe, 0x5678, "De"},
                      R16StackCase{kPushHl, kPopHl, 0x9ABC, "Hl"},
                      R16StackCase{kPushAf, kPopAf, 0xDEF0, "Af"}),
    [](const ::testing::TestParamInfo<R16StackCase>& info) {
      return std::string(info.param.name);
    });

TEST_P(PopR16StackTest, PopsR16StackPairCorrectly) {
  LoadProgram({GetParam().popOpcode});

  Registers initials = DefaultRegisters();
  initials.SP = kProgramStart + 40;
  bus_.Write(initials.SP, static_cast<uint8_t>(GetParam().value & 0xFFU));
  bus_.Write(initials.SP + 1U, static_cast<uint8_t>(GetParam().value >> 8U));
  Cpu cpu = MakeCpu(initials);

  EXPECT_EQ(12, cpu.Step());
  EXPECT_EQ(initials.SP + 2U, cpu.GetRegisters().SP);

  switch (GetParam().popOpcode) {
    case kPopBc:
      EXPECT_EQ(GetParam().value, cpu.GetRegisters().GetBC());
      break;
    case kPopDe:
      EXPECT_EQ(GetParam().value, cpu.GetRegisters().GetDE());
      break;
    case kPopHl:
      EXPECT_EQ(GetParam().value, cpu.GetRegisters().GetHL());
      break;
    case kPopAf:
      EXPECT_EQ(GetParam().value & 0xFFF0U, cpu.GetRegisters().GetAF());
      break;
  }
}

INSTANTIATE_TEST_SUITE_P(
    CpuTest, PopR16StackTest,
    ::testing::Values(R16StackCase{kPushBc, kPopBc, 0x1234, "Bc"},
                      R16StackCase{kPushDe, kPopDe, 0x5678, "De"},
                      R16StackCase{kPushHl, kPopHl, 0x9ABC, "Hl"},
                      R16StackCase{kPushAf, kPopAf, 0xDEF0, "Af"}),
    [](const ::testing::TestParamInfo<R16StackCase>& info) {
      return std::string(info.param.name);
    });
