#include <gtest/gtest.h>
#include <stdint.h>

#include <cstdint>
#include <initializer_list>

#include "core/cpu/Cpu.h"
#include "core/cpu/Registers.h"
#include "core/memory/Bus.h"

using core::cpu::Cpu;
using core::cpu::Registers;
using core::memory::Bus;

namespace {

constexpr uint16_t kProgramStart = 0xC000;
constexpr uint16_t kDataAddress = kProgramStart + 0x20;
constexpr uint16_t kSpDestination = kProgramStart + 0x100;

// Block 0
constexpr uint8_t kNop = 0x00;

constexpr uint8_t kLdBcImmediate16 = 0x01;
constexpr uint8_t kStoreAAtAddressInBc = 0x02;
constexpr uint8_t kLoadAFromAddressInBc = 0x0A;
constexpr uint8_t kStoreSpAtImmediateAddress = 0x08;
constexpr uint8_t kStoreAAtAddressInHlThenIncrement = 0x22;
constexpr uint8_t kStoreAAtAddressInHlThenDecrement = 0x32;

constexpr uint8_t kIncrementBC = 0x3;
constexpr uint8_t kDecrementBC = 0xB;
constexpr uint8_t kAddHlWithValueOfBC = 0x9;

constexpr uint8_t kIncrementB = 0x4;
constexpr uint8_t kIncrementAtAddressInHL = 0x34;

constexpr uint8_t kDecrementB = 0x5;
constexpr uint8_t kDecrementAtAddressInHL = 0x35;

constexpr uint8_t kStoreImmediate8AtB = 0x6;
constexpr uint8_t kStoreImmediate8AtAddressInHl = 0x36;
constexpr uint8_t kDaa = 0x27;
constexpr uint8_t kCpl = 0x2F;
constexpr uint8_t kStop = 0x10;
constexpr uint8_t kRlca = 0x07;
constexpr uint8_t kRrca = 0x0F;
constexpr uint8_t kRla = 0x17;
constexpr uint8_t kRra = 0x1F;
constexpr uint8_t kScf = 0x37;
constexpr uint8_t kCcf = 0x3F;
constexpr uint8_t kJr = 0x18;
constexpr uint8_t kJrNz = 0x20;
constexpr uint8_t kJrZ = 0x28;
constexpr uint8_t kJrNc = 0x30;
constexpr uint8_t kJrC = 0x38;

}  // namespace

class CpuTest : public ::testing::Test {
 protected:
  static Registers DefaultRegisters() {
    Registers registers{};
    registers.PC = kProgramStart;
    return registers;
  }

  Cpu MakeCpu(Registers registers = DefaultRegisters()) {
    return Cpu(bus_, registers);
  }

  void LoadProgram(std::initializer_list<uint8_t> bytes) {
    uint16_t address = kProgramStart;
    for (const uint8_t byte : bytes) {
      bus_.Write(address++, byte);
    }
  }

  Bus bus_;
};

TEST_F(CpuTest, NopConsumesFourTCyclesAndAdvancesPC) {
  LoadProgram({kNop});
  Cpu cpu = MakeCpu();

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, LoadBCImmediateUsesLittleEndianAndAdvancesPC) {
  LoadProgram({kLdBcImmediate16, 0x34, 0x12});
  Cpu cpu = MakeCpu();

  EXPECT_EQ(12, cpu.Step());
  EXPECT_EQ(0x1234, cpu.GetRegisters().GetBC());
  EXPECT_EQ(kProgramStart + 3, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, StoreAAtAddressInBC) {
  LoadProgram({kStoreAAtAddressInBc});

  Registers initial = DefaultRegisters();
  initial.A = 0x5A;
  initial.SetBC(kDataAddress);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(0x5A, bus_.Read(kDataAddress));
  EXPECT_EQ(0x5A, cpu.GetRegisters().A);
  EXPECT_EQ(kDataAddress, cpu.GetRegisters().GetBC());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, LoadAFromAddressInBC) {
  LoadProgram({kLoadAFromAddressInBc});

  constexpr uint8_t valueAtAddress = 0xA5;
  bus_.Write(kDataAddress, valueAtAddress);
  Registers initial = DefaultRegisters();
  initial.A = 0x00;
  initial.SetBC(kDataAddress);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(valueAtAddress, cpu.GetRegisters().A);
  EXPECT_EQ(kDataAddress, cpu.GetRegisters().GetBC());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, StoreAAtAddressInHLThenIncrementHL) {
  LoadProgram({kStoreAAtAddressInHlThenIncrement});

  constexpr uint8_t value = 0x5A;
  Registers initial = DefaultRegisters();
  initial.A = value;
  initial.SetHL(kDataAddress);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(value, bus_.Read(kDataAddress));
  EXPECT_EQ(value, cpu.GetRegisters().A);
  EXPECT_EQ(kDataAddress + 1, cpu.GetRegisters().GetHL());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, StoreAAtAddressInHLThenDecrementHL) {
  LoadProgram({kStoreAAtAddressInHlThenDecrement});

  constexpr uint8_t value = 0xA5;
  Registers initial = DefaultRegisters();
  initial.A = value;
  initial.SetHL(kDataAddress);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(value, bus_.Read(kDataAddress));
  EXPECT_EQ(value, cpu.GetRegisters().A);
  EXPECT_EQ(kDataAddress - 1, cpu.GetRegisters().GetHL());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, StoreSPAtImmediateAddressInLittleEndianOrder) {
  LoadProgram({kStoreSpAtImmediateAddress,
               static_cast<uint8_t>(kSpDestination & 0xFFU),
               static_cast<uint8_t>(kSpDestination >> 8U)});

  constexpr uint16_t initialSP = 0xABCD;
  Registers initial = DefaultRegisters();
  initial.SP = initialSP;
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(20, cpu.Step());
  EXPECT_EQ(0xCD, bus_.Read(kSpDestination));
  EXPECT_EQ(0xAB, bus_.Read(kSpDestination + 1));
  EXPECT_EQ(initialSP, cpu.GetRegisters().SP);
  EXPECT_EQ(kProgramStart + 3, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, IncrementBC) {
  LoadProgram({kIncrementBC});

  Registers initial = DefaultRegisters();
  initial.SetBC(0x1);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(0x2, cpu.GetRegisters().GetBC());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, DecrementBC) {
  LoadProgram({kDecrementBC});

  Registers initial = DefaultRegisters();
  initial.SetBC(0x1);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(0x0, cpu.GetRegisters().GetBC());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, AddHlWithValueOfBC) {
  LoadProgram({kAddHlWithValueOfBC});

  Registers initial = DefaultRegisters();
  initial.SetBC(0x1);
  initial.SetHL(0x5);
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(0x6, cpu.GetRegisters().GetHL());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
}

TEST_F(CpuTest, AddHlWithValueOfBCHalfBitOverflow) {
  LoadProgram({kAddHlWithValueOfBC});

  Registers initial = DefaultRegisters();
  initial.SetBC(0x1);
  initial.SetHL(0xFFF);
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(0x1000, cpu.GetRegisters().GetHL());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
}

TEST_F(CpuTest, AddHlWithValueOfBCCarryBitOVerflow) {
  LoadProgram({kAddHlWithValueOfBC});

  Registers initial = DefaultRegisters();
  initial.SetBC(0x1001);
  initial.SetHL(0xF000);
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  // Expect wraparound to 1
  EXPECT_EQ(0x1, cpu.GetRegisters().GetHL());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
}

TEST_F(CpuTest, AddHlWithValueOfBCBothCarryBitsOverflow) {
  LoadProgram({kAddHlWithValueOfBC});

  Registers initial = DefaultRegisters();
  initial.SetBC(0x2);
  initial.SetHL(0xFFFF);
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  // Expect wraparound to 1
  EXPECT_EQ(0x1, cpu.GetRegisters().GetHL());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
}

TEST_F(CpuTest, IncrementB) {
  LoadProgram({kIncrementB});

  Registers initial = DefaultRegisters();
  initial.B = 0x1;
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x2, cpu.GetRegisters().B);
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
}

TEST_F(CpuTest, IncrementAtAddressInHl) {
  LoadProgram({kIncrementAtAddressInHL});

  Registers initial = DefaultRegisters();
  initial.SetHL(kProgramStart + 20);
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(12, cpu.Step());

  // We do not set the value of HL, but the memory value of HL
  EXPECT_EQ(kProgramStart + 20, cpu.GetRegisters().GetHL());
  EXPECT_EQ(0x1, bus_.Read(cpu.GetRegisters().GetHL()));
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
}

TEST_F(CpuTest, IncrementBHalfCarrySet) {
  LoadProgram({kIncrementB});

  Registers initial = DefaultRegisters();
  initial.B = 0xF;
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x10, cpu.GetRegisters().B);
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
}

TEST_F(CpuTest, IncrementBZeroSet) {
  LoadProgram({kIncrementB});

  Registers initial = DefaultRegisters();
  initial.B = 0xFF;
  initial.SetFlagN(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  // Wraparound to 0
  EXPECT_EQ(0x0, cpu.GetRegisters().B);
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());

  // A test where only Z flag is set is not possible in increment, that is
  // because we can only add 1 to the register vs a real add instruction so 0xFF
  // is the only scenario for setting Z flag.
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
}

TEST_F(CpuTest, DecrementB) {
  LoadProgram({kDecrementB});

  Registers initial = DefaultRegisters();
  initial.B = 0x1;
  initial.SetFlagN(false);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x0, cpu.GetRegisters().B);
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagZ());
}

TEST_F(CpuTest, DecrementAtAddressInHl) {
  LoadProgram({kDecrementAtAddressInHL});

  Registers initial = DefaultRegisters();
  initial.SetHL(kProgramStart + 20);
  initial.SetFlagN(false);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(12, cpu.Step());

  // We do not set the value of HL, but the memory value of HL
  EXPECT_EQ(kProgramStart + 20, cpu.GetRegisters().GetHL());

  // Expect wraparound to max
  EXPECT_EQ(UINT8_MAX, bus_.Read(cpu.GetRegisters().GetHL()));
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
}

TEST_F(CpuTest, DecrementBHalfCarrySet) {
  LoadProgram({kDecrementB});

  Registers initial = DefaultRegisters();
  initial.B = 0x10;
  initial.SetFlagN(false);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x0F, cpu.GetRegisters().B);
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagN());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(true, cpu.GetRegisters().GetFlagH());
  EXPECT_EQ(false, cpu.GetRegisters().GetFlagZ());
}

TEST_F(CpuTest, StoreImmediate8AtB) {
  LoadProgram({kStoreImmediate8AtB, 0x11});

  Registers initial = DefaultRegisters();
  initial.B = 0x10;
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(8, cpu.Step());
  EXPECT_EQ(0x11, cpu.GetRegisters().B);
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, StoreImmediate8AtAddressInHl) {
  LoadProgram({kStoreImmediate8AtAddressInHl, 0x11});

  Registers initial = DefaultRegisters();
  initial.SetHL(kProgramStart + 20);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(12, cpu.Step());
  EXPECT_EQ(0x11, bus_.Read(cpu.GetRegisters().GetHL()));
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, ComplementAccumulatorPreservesZAndC) {
  LoadProgram({kCpl});

  Registers initial = DefaultRegisters();
  initial.A = 0x55;
  initial.SetFlagZ(true);
  initial.SetFlagC(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0xAA, cpu.GetRegisters().A);
  EXPECT_TRUE(cpu.GetRegisters().GetFlagZ());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagN());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagH());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagC());
}

TEST_F(CpuTest, DaaSetsZeroFlagWhenAdjustedResultIsZero) {
  LoadProgram({kDaa});

  Registers initial = DefaultRegisters();
  initial.A = 0x9A;
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x00, cpu.GetRegisters().A);
  EXPECT_TRUE(cpu.GetRegisters().GetFlagZ());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagN());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagH());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagC());
}

TEST_F(CpuTest, DaaClearsZeroFlagWhenAdjustedResultIsNonzero) {
  LoadProgram({kDaa});

  Registers initial = DefaultRegisters();
  initial.A = 0x01;
  initial.SetFlagZ(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x01, cpu.GetRegisters().A);
  EXPECT_FALSE(cpu.GetRegisters().GetFlagZ());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagH());
}

TEST_F(CpuTest, DaaCorrectsSubtractionResult) {
  LoadProgram({kDaa});

  Registers initial = DefaultRegisters();
  initial.A = 0x0F;
  initial.SetFlagN(true);
  initial.SetFlagH(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(0x09, cpu.GetRegisters().A);
  EXPECT_FALSE(cpu.GetRegisters().GetFlagZ());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagN());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagH());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagC());
}

TEST_F(CpuTest, StopConsumesPaddingByteAndPreventsFurtherExecution) {
  LoadProgram({kStop, 0x00, kNop});
  Cpu cpu = MakeCpu();

  EXPECT_EQ(4, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
  EXPECT_EQ(0, cpu.Step());
  EXPECT_EQ(kProgramStart + 2, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, AccumulatorRotatesWrapThroughExpectedBitsAndSetFlags) {
  struct RotateCase {
    uint8_t opcode;
    uint8_t initialA;
    bool initialCarry;
    uint8_t expectedA;
    bool expectedCarry;
  };

  const RotateCase cases[] = {
      {kRlca, 0x85, false, 0x0B, true},
      {kRrca, 0x81, false, 0xC0, true},
      {kRla, 0x00, true, 0x01, false},
      {kRra, 0x00, true, 0x80, false},
  };

  for (const RotateCase& testCase : cases) {
    LoadProgram({testCase.opcode});

    Registers initial = DefaultRegisters();
    initial.A = testCase.initialA;
    initial.SetFlagZ(true);
    initial.SetFlagN(true);
    initial.SetFlagH(true);
    initial.SetFlagC(testCase.initialCarry);
    Cpu cpu = MakeCpu(initial);

    SCOPED_TRACE(static_cast<int>(testCase.opcode));
    EXPECT_EQ(4, cpu.Step());
    EXPECT_EQ(testCase.expectedA, cpu.GetRegisters().A);
    EXPECT_FALSE(cpu.GetRegisters().GetFlagZ());
    EXPECT_FALSE(cpu.GetRegisters().GetFlagN());
    EXPECT_FALSE(cpu.GetRegisters().GetFlagH());
    EXPECT_EQ(testCase.expectedCarry, cpu.GetRegisters().GetFlagC());
    EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
  }
}

TEST_F(CpuTest, ScfSetsCarryClearsNHAndPreservesZ) {
  LoadProgram({kScf});

  Registers initial = DefaultRegisters();
  initial.SetFlagZ(true);
  initial.SetFlagN(true);
  initial.SetFlagH(true);
  initial.SetFlagC(false);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagZ());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagN());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagH());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagC());
  EXPECT_EQ(kProgramStart + 1, cpu.GetRegisters().PC);
}

TEST_F(CpuTest, CcfInvertsCarryAndPreservesZ) {
  LoadProgram({kCcf});

  Registers initial = DefaultRegisters();
  initial.SetFlagZ(true);
  initial.SetFlagN(true);
  initial.SetFlagH(true);
  initial.SetFlagC(true);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagZ());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagN());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagH());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagC());
}

TEST_F(CpuTest, CcfSetsCarryWhenItWasClear) {
  LoadProgram({kCcf});

  Registers initial = DefaultRegisters();
  initial.SetFlagZ(false);
  initial.SetFlagC(false);
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(4, cpu.Step());
  EXPECT_FALSE(cpu.GetRegisters().GetFlagZ());
  EXPECT_TRUE(cpu.GetRegisters().GetFlagC());
}

TEST_F(CpuTest, JrAppliesPositiveAndNegativeSignedOffsets) {
  LoadProgram({kJr, 0x02});
  Cpu forwardCpu = MakeCpu();

  EXPECT_EQ(12, forwardCpu.Step());
  EXPECT_EQ(kProgramStart + 4, forwardCpu.GetRegisters().PC);

  LoadProgram({kJr, 0xFE});
  Cpu backwardCpu = MakeCpu();

  EXPECT_EQ(12, backwardCpu.Step());
  EXPECT_EQ(kProgramStart, backwardCpu.GetRegisters().PC);
}

TEST_F(CpuTest, ConditionalJrUsesAllConditionsAndTakenCycles) {
  struct ConditionalJrCase {
    uint8_t opcode;
    bool zeroFlag;
    bool carryFlag;
    bool taken;
  };

  const ConditionalJrCase cases[] = {
      {kJrNz, false, false, true}, {kJrNz, true, false, false},
      {kJrZ, true, false, true},   {kJrZ, false, false, false},
      {kJrNc, false, false, true}, {kJrNc, false, true, false},
      {kJrC, false, true, true},   {kJrC, false, false, false},
  };

  for (const ConditionalJrCase& testCase : cases) {
    LoadProgram({testCase.opcode, 0x02});

    Registers initial = DefaultRegisters();
    initial.SetFlagZ(testCase.zeroFlag);
    initial.SetFlagC(testCase.carryFlag);
    Cpu cpu = MakeCpu(initial);

    SCOPED_TRACE(static_cast<int>(testCase.opcode));
    EXPECT_EQ(testCase.taken ? 12 : 8, cpu.Step());
    EXPECT_EQ(testCase.taken ? kProgramStart + 4 : kProgramStart + 2,
              cpu.GetRegisters().PC);
  }
}

TEST_F(CpuTest, JrWrapsProgramCounterAtSixteenBitBoundary) {
  constexpr uint16_t startAddress = 0xFFFD;
  bus_.Write(startAddress, kJr);
  bus_.Write(startAddress + 1, 0x01);

  Registers initial = DefaultRegisters();
  initial.PC = startAddress;
  Cpu cpu = MakeCpu(initial);

  EXPECT_EQ(12, cpu.Step());
  EXPECT_EQ(0x0000, cpu.GetRegisters().PC);
}
