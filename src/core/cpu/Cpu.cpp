#include "Cpu.h"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "core/memory/Bus.h"

using namespace core::cpu;
using namespace core::memory;

Cpu::Cpu(Bus& bus, Registers registers) : bus_(bus), registers_(registers) {}

namespace {

uint8_t FetchByte(Bus& bus, Registers& registers) {
  const uint8_t value = bus.Read(registers.PC);
  registers.PC += 1;
  return value;
}

int32_t NOP() {
  std::cout << "Executing NOP instruction." << "\n";
  return 4;  // NOP takes 4 T-cycles and 1 M-cycle on the Game Boy CPU.
}
int32_t LD_R16_IMM16(uint16_t& destination, uint16_t value) {
  destination = value;
  return 3;
}

int32_t LD_BC_n16(Registers& regs, uint16_t value) {
  std::cout << "Executing LD BC, n16 instruction." << "\n";
  regs.SetBC(value);
  return 12;
}

int32_t LD_BC_A_IO(Bus& bus, Registers& regs) {
  std::cout << "Executing [BC], A instruction" << "\n";
  bus.Write(regs.GetBC() + 0xFF00, regs.A);
  return 8;
}

int32_t INC_BC(Registers& regs) {
  std::cout << "Executing INC BC" << "/n";
  regs.SetBC(regs.GetBC() + 1);
  return 8;
}
void Create_LD_BC_n16(Bus& bus, uint16_t address, uint16_t value) {
  const uint8_t high = static_cast<uint32_t>(value) >> 8U;
  const uint8_t low = value & 0xFFU;
  bus.Write(address, 0x01);
  bus.Write(address + 1, low);
  bus.Write(address + 2, high);
}

enum class Family : uint8_t {
  Block0 = 0x00,
  Block1 = 0x01,
  Block2 = 0x10,
  Block3 = 0x11
};

enum class Instructions : uint8_t {
  NOP = 0x00,
  LD_BC_n16 = 0x01,
  LD_BC_A_IO = 0x02,
  INC_BC = 0x03,
  HALT = 0x76,
};
enum class R8Operand : uint8_t {
  B = 0,
  C = 1,
  D = 2,
  E = 3,
  H = 4,
  L = 5,
  IndirectHL = 6,
  A = 7
};

enum class R16Pair : uint8_t { BC = 0, DE = 1, HL = 2, SP = 3 };

enum class R16MemoryMode : uint8_t {
  BC = 0,
  DE = 1,
  HLIncrement = 2,
  HLDecrement = 3
};

enum class Conditionals : uint8_t { NZ = 0, Z = 1, NC = 2, C = 3 };

uint8_t ReadR8Operand(R8Operand operand, const Registers& registers,
                      const Bus& bus) {
  switch (operand) {
    case (R8Operand::B): {
      return registers.B;
    }
    case (R8Operand::C): {
      return registers.C;
    }
    case (R8Operand::D): {
      return registers.D;
    }
    case (R8Operand::E): {
      return registers.E;
    }
    case (R8Operand::H): {
      return registers.H;
    }
    case (R8Operand::L): {
      return registers.L;
    }
    case (R8Operand::IndirectHL): {
      return bus.Read(registers.GetHL());
    }
    case (R8Operand::A): {
      return registers.A;
    }
    default: {
      throw std::invalid_argument(
          "The value does not exist in R8Operand: " +
          std::to_string(static_cast<int32_t>(operand)));
    }
  }
}
void WriteR8Operand(R8Operand operand, uint8_t value, Registers& registers,
                    Bus& bus) {
  switch (operand) {
    case R8Operand::B:
      registers.B = value;
      return;
    case R8Operand::C:
      registers.C = value;
      return;
    case R8Operand::D:
      registers.D = value;
      return;
    case R8Operand::E:
      registers.E = value;
      return;
    case R8Operand::H:
      registers.H = value;
      return;
    case R8Operand::L:
      registers.L = value;
      return;
    case R8Operand::IndirectHL:
      bus.Write(registers.GetHL(), value);
      return;
    case R8Operand::A:
      registers.A = value;
      return;
  }

  throw std::invalid_argument("Invalid 8-bit operand" +
                              std::to_string(static_cast<int32_t>(operand)));
}

uint16_t ReadR16Pair(R16Pair pair, const Registers& registers) {
  switch (pair) {
    case R16Pair::BC:
      return registers.GetBC();
    case R16Pair::DE:
      return registers.GetDE();
    case R16Pair::HL:
      return registers.GetHL();
    case R16Pair::SP:
      return registers.SP;
  }

  throw std::invalid_argument("Invalid 16-bit pair" +
                              std::to_string(static_cast<int32_t>(pair)));
}

void WriteR16Pair(R16Pair pair, uint16_t value, Registers& registers) {
  switch (pair) {
    case R16Pair::BC:
      registers.SetBC(value);
      return;
    case R16Pair::DE:
      registers.SetDE(value);
      return;
    case R16Pair::HL:
      registers.SetHL(value);
      return;
    case R16Pair::SP:
      registers.SP = value;
      return;
  }

  throw std::invalid_argument("Invalid 16-bit pair" +
                              std::to_string(static_cast<int32_t>(pair)));
}

uint16_t GetR16MemoryAddress(R16MemoryMode mode, const Registers& registers) {
  switch (mode) {
    case R16MemoryMode::BC:
      return registers.GetBC();
    case R16MemoryMode::DE:
      return registers.GetDE();
    case R16MemoryMode::HLIncrement:
    case R16MemoryMode::HLDecrement:
      return registers.GetHL();
  }

  throw std::invalid_argument("Invalid 16-bit memory mode" +
                              std::to_string(static_cast<int32_t>(mode)));
}

void UpdateR16MemoryMode(R16MemoryMode mode, Registers& registers) {
  if (mode == R16MemoryMode::HLIncrement) {
    registers.SetHL(static_cast<uint16_t>(registers.GetHL() + 1U));
  } else if (mode == R16MemoryMode::HLDecrement) {
    registers.SetHL(static_cast<uint16_t>(registers.GetHL() - 1U));
  }
}

uint8_t ReadR16Memory(R16MemoryMode mode, Registers& registers,
                      const Bus& bus) {
  const uint16_t address = GetR16MemoryAddress(mode, registers);
  const uint8_t value = bus.Read(address);
  UpdateR16MemoryMode(mode, registers);
  return value;
}

void WriteR16Memory(R16MemoryMode mode, uint8_t value, Registers& registers,
                    Bus& bus) {
  const uint16_t address = GetR16MemoryAddress(mode, registers);
  bus.Write(address, value);
  UpdateR16MemoryMode(mode, registers);
}

bool ReadConditionals(Conditionals conditional, const Registers& registers) {
  switch (conditional) {
    case Conditionals::NZ:
      return !registers.GetFlagZ();
    case Conditionals::Z:
      return registers.GetFlagZ();
    case Conditionals::NC:
      return !registers.GetFlagC();
    case Conditionals::C:
      return registers.GetFlagC();
  }

  throw std::invalid_argument(
      "Invalid conditional" +
      std::to_string(static_cast<int32_t>(conditional)));
}

// Block 0 instructions
void LDDestination16Source16(uint16_t& destination, uint16_t source) {
  destination = source;
}

void LDDestination8Source8(uint8_t& destination, uint8_t source) {
  destination = source;
}

}  // namespace

int32_t Cpu::ExecuteBlock0(const uint8_t opcode) {
  if (opcode == static_cast<uint8_t>(Instructions::NOP)) {
    return NOP();
  }

  // fetch last 4 bits, 0xF = 0b1111
  const uint8_t lastFour = opcode & 0xFU;

  // LD r16, imm16
  if (lastFour == 0x1) {
    const uint8_t low = FetchByte(bus_, registers_);
    const uint8_t high = FetchByte(bus_, registers_);
    const uint16_t immValue =
        static_cast<uint16_t>((high) << 8U) | static_cast<uint16_t>(low);

    // 0x30U = 0b110000
    const uint16_t destinationRegisterValue = (opcode & 0x30U) >> 4U;

    WriteR16Pair(static_cast<R16Pair>(destinationRegisterValue), immValue,
                 registers_);
    return 12;
  }

  // LD [r16mem], a
  if (lastFour == 0x2) {
    // 0x30U = 0b110000
    const uint16_t destinationRegisterValue = (opcode & 0x30U) >> 4U;

    WriteR16Memory(static_cast<R16MemoryMode>(destinationRegisterValue),
                   registers_.A, registers_, bus_);

    return 8;
  }

  // LD a, [r16mem]
  if (lastFour == 0xA) {
    // 0x30U = 0b110000
    const uint16_t sourceRegisterValue = (opcode & 0x30U) >> 4U;

    const uint8_t sourceRegisterMemoryValue = ReadR16Memory(
        static_cast<R16MemoryMode>(sourceRegisterValue), registers_, bus_);
    LDDestination8Source8(registers_.A, sourceRegisterMemoryValue);
    return 8;
  }

  // LD [imm16], sp
  if (opcode == 0x08) {
    const uint8_t low = FetchByte(bus_, registers_);
    const uint8_t high = FetchByte(bus_, registers_);
    const uint16_t immValue =
        static_cast<uint16_t>(static_cast<uint16_t>(high) << 8U) |
        static_cast<uint16_t>(low);

    uint32_t sp = registers_.SP;

    const uint8_t lowSP = sp & 0xFFU;
    const uint8_t highSP = sp >> 8U;

    bus_.Write(immValue, lowSP);
    bus_.Write(immValue + 1, highSP);
    return 20;
  }

  // inc r16
  if (lastFour == 0x3) {
    const uint16_t pair = (opcode & 0x30U) >> 4U;
    WriteR16Pair(static_cast<R16Pair>(pair),
                 ReadR16Pair(static_cast<R16Pair>(pair), registers_) + 1,
                 registers_);
    return 8;
  }

  // dec r16
  if (lastFour == 0xB) {
    const uint16_t pair = (opcode & 0x30U) >> 4U;
    WriteR16Pair(static_cast<R16Pair>(pair),
                 ReadR16Pair(static_cast<R16Pair>(pair), registers_) - 1,
                 registers_);
    return 8;
  }

  // add hl, r16
  if (lastFour == 0x9) {
    const uint16_t pair = (opcode & 0x30U) >> 4U;
    const uint16_t currentHLValue = ReadR16Pair(R16Pair::HL, registers_);
    const uint16_t customPairValue =
        ReadR16Pair(static_cast<R16Pair>(pair), registers_);
    const uint32_t additionResult = customPairValue + currentHLValue;
    const uint16_t additionResultWrap = additionResult & 0xFFFFU;
    WriteR16Pair(R16Pair::HL, additionResultWrap, registers_);

    // Set flags - 0 H C
    const bool carry = additionResult > 0xFFFFU;
    registers_.SetFlagC(carry);

    registers_.SetFlagN(false);

    // Check if addition causes spill to high nibble
    const bool halfCarry =
        (((customPairValue & 0xFFFU) + (currentHLValue & 0xFFFU) > 0xFFFU));
    registers_.SetFlagH(halfCarry);

    return 8;
  }

  const uint8_t lastThree = opcode & 0x7U;

  // inc r8
  if (lastThree == 0x4) {
    const uint8_t operandValue = opcode >> 3U;
    const R8Operand operand = static_cast<R8Operand>(operandValue);

    const uint8_t currentValue = ReadR8Operand(operand, registers_, bus_);
    WriteR8Operand(operand, currentValue + 1, registers_, bus_);

    // Set flags Z 0 H -
    const bool halfCarry = ((currentValue & 0xFU) + (1U & 0xFU) > 0xFU);
    registers_.SetFlagH(halfCarry);
    registers_.SetFlagN(false);
    registers_.SetFlagZ(static_cast<uint8_t>(currentValue + 1) == 0U);

    // IndirectHL requires a bus read and write which is an additional 8
    // T-Cycles
    if (operand == R8Operand::IndirectHL) {
      return 12;
    }
    return 4;
  }

  // dec r8
  if (lastThree == 0x5) {
    const uint8_t operandValue = opcode >> 3U;
    const R8Operand operand = static_cast<R8Operand>(operandValue);

    const uint8_t currentValue = ReadR8Operand(operand, registers_, bus_);
    WriteR8Operand(operand, currentValue - 1, registers_, bus_);

    // Set flags Z 1 H -
    const bool halfCarry = ((currentValue & 0xFU) - (1U & 0xFU) > 0xFU);
    registers_.SetFlagH(halfCarry);
    registers_.SetFlagN(true);
    registers_.SetFlagZ(static_cast<uint8_t>(currentValue - 1) == 0U);

    // IndirectHL requires a bus read and write which is an additional 8
    // T-Cycles
    if (operand == R8Operand::IndirectHL) {
      return 12;
    }
    return 4;
  }

  // ld r8 imm8
  if (lastThree == 0x6) {
    const uint8_t operandValue = opcode >> 3U;
    const R8Operand operand = static_cast<R8Operand>(operandValue);
    const uint8_t immValue = FetchByte(bus_, registers_);

    WriteR8Operand(operand, immValue, registers_, bus_);

    // IndirectHL requires a bus read and write which is an additional 8
    // T-Cycles
    if (operand == R8Operand::IndirectHL) {
      return 12;
    }
    return 8;
  }

  // rlca
  if (opcode == 0x7) {
    // rlca is an instruction that rotates all bits in the register A to the
    // left, bit 7 gets put in the carry flag C and at bit 0.
    const uint8_t bit7 = registers_.A >> 7;
    registers_.SetFlagC(static_cast<bool>(bit7));
    registers_.SetFlagN(false);
    registers_.SetFlagZ(false);
    registers_.SetFlagH(false);

    // The idea is to reset bit 1 to 0 in register A and then we can safely
    // or the bit7 value.
    registers_.A = registers_.A << 1;
    registers_.A = static_cast<uint8_t>((registers_.A & ~0x1U)) | bit7;
    return 4;
  }

  // rrca
  if (opcode == 0xF) {
    // rlca is an instruction that rotates all bits in the register A to the
    // right, bit 0 gets put in the carry flag C and at bit 7.
    const uint8_t bit0 = registers_.A & 0x1;
    registers_.SetFlagC(static_cast<bool>(bit0));
    registers_.SetFlagN(false);
    registers_.SetFlagZ(false);
    registers_.SetFlagH(false);

    registers_.A = registers_.A >> 1;
    // 0x80 = 2^7
    const uint8_t bit7Mask = 0x80U;
    registers_.A =
        static_cast<uint8_t>((registers_.A & ~bit7Mask)) | (bit0 << 7);
    return 4;
  }
  // rla
  if (opcode == 0x17) {
    // rla shifts register A to the left, bit 7 gets placed in flag C, flag C
    // gets placed in bit 0.
    const uint8_t bit7 = registers_.A >> 7;

    // The idea is to reset bit 1 to 0 in register A and then we can safely
    // or the bit7 value.
    registers_.A = registers_.A << 1;
    registers_.A = static_cast<uint8_t>((registers_.A & ~0x1U)) |
                   static_cast<uint8_t>(registers_.GetFlagC());

    registers_.SetFlagC(static_cast<bool>(bit7));
    registers_.SetFlagN(false);
    registers_.SetFlagZ(false);
    registers_.SetFlagH(false);
    return 4;
  }
  // rra
  if (opcode == 0x1F) {
    // rra shifts register A to the right, bit 0 gets placed in flag C, flag C
    // gets placed in bit 7.
    const uint8_t bit0 = registers_.A & 0x1;

    registers_.A = registers_.A >> 1;
    // 0x80 = 2^7
    const uint8_t bit7Mask = 0x80U;
    registers_.A = static_cast<uint8_t>((registers_.A & ~bit7Mask)) |
                   (static_cast<uint8_t>(registers_.GetFlagC()) << 7);

    registers_.SetFlagC(static_cast<bool>(bit0));
    registers_.SetFlagN(false);
    registers_.SetFlagZ(false);
    registers_.SetFlagH(false);
    return 4;
  }
  // daa
  if (opcode == 0x27) {
    // daa or decimal adjust accumulator
    /*
    Designed to be used after performing an arithmetic instruction (ADD, ADC,
    SUB, SBC) whose inputs were in Binary-Coded Decimal (BCD), adjusting the
    result to likewise be in BCD.

    The exact behavior of this instruction depends on the state of the subtract
    flag N:

    If the subtract flag N is set:
      Initialize the adjustment to 0.
      If the half-carry flag H is set, then add $6 to the adjustment.
      If the carry flag is set, then add $60 to the adjustment.
      Subtract the adjustment from A.

    If the subtract flag N is not set:
      Initialize the adjustment to 0.
      If the half-carry flag H is set or A & $F > $9, then add $6 to the
      adjustment. If the carry flag is set or A > $99, then add $60 to the
      adjustment and set the carry flag. Add the adjustment to A.
      */

    uint8_t adjustment = 0;
    uint8_t registerAInitialValue = registers_.A;
    if (registers_.GetFlagN()) {
      if (registers_.GetFlagH()) {
        adjustment += 0x6;
      }
      if (registers_.GetFlagC()) {
        adjustment += 0x60;
      }
      registers_.A -= adjustment;
    } else {
      if (registers_.GetFlagH() || (registers_.A & 0xFU) > 0x9U) {
        adjustment += 0x6;
      }
      if (registers_.GetFlagC() || registers_.A > 0x99) {
        adjustment += 0x60;
        registers_.SetFlagC(true);
      }
      registers_.A += adjustment;
      if (static_cast<uint32_t>(registerAInitialValue) +
              static_cast<uint32_t>(adjustment) >
          0xFFU) {
        registers_.SetFlagC(true);
      }
    }
    registers_.SetFlagH(false);
    registers_.SetFlagZ(registers_.A == 0);

    return 4;
  }

  // cpl
  if (opcode == 0x2F) {
    // cpl Complement accumulator (A = ~A); also called bitwise NOT.
    registers_.A = static_cast<uint8_t>(~static_cast<uint32_t>(registers_.A));
    registers_.SetFlagN(true);
    registers_.SetFlagH(true);
    return 4;
  }
  // scf
  if (opcode == 0x37) {
    // scf Set carry flag instruction
    registers_.SetFlagC(true);
    registers_.SetFlagN(false);
    registers_.SetFlagH(false);
    return 4;
  }

  // ccf
  if (opcode == 0x3F) {
    // ccf Invert carry flag
    registers_.SetFlagC(!registers_.GetFlagC());
    registers_.SetFlagN(false);
    registers_.SetFlagH(false);
    return 4;
  }

  // jr imm8
  if (opcode == 0x18) {
    // have to read imm8 as a signed value so we can jump forward and backwards
    const int8_t immValue = static_cast<int8_t>(FetchByte(bus_, registers_));
    registers_.PC += immValue;
    return 12;
  }

  // jr cond, imm8
  if (lastThree == 0x0 && (opcode >> 5) == 0x1) {
    const uint8_t conditional = (opcode & 0x18) >> 3;
    const bool isConditionMet =
        ReadConditionals(static_cast<Conditionals>(conditional), registers_);
    const int8_t immValue = static_cast<int8_t>(FetchByte(bus_, registers_));
    if (isConditionMet) {
      registers_.PC += immValue;
      return 12;
    }
    return 8;
  }

  // stop
  if (opcode == 0x10) {
    FetchByte(bus_, registers_);
    stopped_ = true;
    return 4;
    return 8;
  }

  throw std::invalid_argument("Unknown opcode for Block0: " +
                              std::to_string(opcode));
}

int32_t Cpu::ExecuteBlock1(const uint8_t opcode) {
  // Implementation for Block1 instructions
  // Halt
  if (opcode == static_cast<uint8_t>(Instructions::HALT)) {
    /*
      halt is an instruction that pauses the CPU (during which less power is
    consumed) when executed. The CPU wakes up as soon as an interrupt is
    pending, that is, when the bitwise AND of IE and IF is non-zero.

    Most commonly, IME is set. In this case, the CPU simply wakes up, and before
    executing the instruction after the halt, the interrupt handler is called
    normally.

    If IME is not set, there are two distinct cases, depending on whether an
    interrupt is pending as the halt instruction is first executed.

    If no interrupt is pending, halt executes as normal, and the CPU resumes
    regular execution as soon as an interrupt becomes pending. However, since
    IME=0, the interrupt is not handled. If an interrupt is pending, halt
    immediately exits, as expected, however the “halt bug”, explained below, is
    triggered. halt bug When a halt instruction is executed with IME = 0 and
    [IE] & [IF] != 0, the halt instruction ends immediately, but pc fails to be
    normally incremented.

    Under most circumstances, this causes the byte after the halt to be read a
    second time (and this behaviour can repeat if said byte executes another
    halt instruction). But, if the halt is immediately followed by a jump to
    elsewhere, then the behaviour will be slightly different; this is possible
    in only one of two ways:

    The halt comes immediately after a ei instruction (whose effect is typically
    delayed by one instruction, hence IME still being zero for the halt): the
    interrupt is serviced and the handler called, but the interrupt returns to
    the halt, which is executed again, and thus waits for another interrupt.
    (Source) The halt is immediately followed by a rst instruction: the rst
    instruction’s return address will point at the rst itself, instead of the
    byte after it. Notably, a ret would return to the rst an execute it again.
    If the bugged halt is preceded by a ei and followed by a rst, the former
    “wins”.
    */
    std::cout << "Executing HALT instruction." << "\n";
    return 4;
  }
  // ld r8, r8
  const uint8_t sourceOperandValue = opcode & 0x7U;
  const uint8_t destinationOperandValue = (opcode & 0x38U) >> 3U;
  const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
  const R8Operand destinationOperand =
      static_cast<R8Operand>(destinationOperandValue);
  const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);
  WriteR8Operand(destinationOperand, sourceValue, registers_, bus_);

  if (sourceOperand == R8Operand::IndirectHL ||
      destinationOperand == R8Operand::IndirectHL) {
    return 8;
  }

  return 4;
}

int32_t Cpu::ExecuteBlock2(const uint8_t opcode) {
  // Implementation for Block2 instructions
  // add a, r8
  if ((opcode & 0xF8U) == 0x80U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);

    const uint32_t result = static_cast<uint32_t>(registers_.A) +
                            static_cast<uint32_t>(sourceValue);
    const uint8_t resultWrap = result & 0xFFU;

    registers_.SetFlagZ(resultWrap == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(((registers_.A & 0xFU) + (sourceValue & 0xFU)) > 0xFU);
    registers_.SetFlagC(result > 0xFFU);

    registers_.A = resultWrap;

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  // adc a, r8
  if ((opcode & 0xF8U) == 0x88U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);

    const uint32_t result = static_cast<uint32_t>(registers_.A) +
                            static_cast<uint32_t>(sourceValue) +
                            static_cast<uint32_t>(registers_.GetFlagC());
    const uint8_t resultWrap = result & 0xFFU;

    registers_.SetFlagZ(resultWrap == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(((registers_.A & 0xFU) + (sourceValue & 0xFU) +
                         static_cast<uint32_t>(registers_.GetFlagC())) > 0xFU);
    registers_.SetFlagC(result > 0xFFU);

    registers_.A = resultWrap;

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  // sub a, r8
  if ((opcode & 0xF8U) == 0x90U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);

    const uint32_t result = static_cast<uint32_t>(registers_.A) -
                            static_cast<uint32_t>(sourceValue);
    const uint8_t resultWrap = result & 0xFFU;

    registers_.SetFlagZ(resultWrap == 0U);
    registers_.SetFlagN(true);
    registers_.SetFlagH(((registers_.A & 0xFU) < (sourceValue & 0xFU)));
    registers_.SetFlagC(result > 0xFFU);

    registers_.A = resultWrap;

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  // sbc a, r8
  if ((opcode & 0xF8U) == 0x98U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);

    const uint32_t result = static_cast<uint32_t>(registers_.A) -
                            static_cast<uint32_t>(sourceValue) -
                            static_cast<uint32_t>(registers_.GetFlagC());
    const uint8_t resultWrap = result & 0xFFU;

    registers_.SetFlagZ(resultWrap == 0U);
    registers_.SetFlagN(true);
    registers_.SetFlagH((
        (registers_.A & 0xFU) <
        ((sourceValue & 0xFU) - static_cast<uint32_t>(registers_.GetFlagC()))));
    registers_.SetFlagC(result > 0xFFU);

    registers_.A = resultWrap;

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  // and a, r8
  if ((opcode & 0xF8U) == 0xA0U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);

    registers_.A &= sourceValue;
    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(true);
    registers_.SetFlagC(false);

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  // xor a, r8
  if ((opcode & 0xF8U) == 0xA8U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);
    registers_.A ^= sourceValue;
    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(false);
    registers_.SetFlagC(false);

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  // or a, r8
  if ((opcode & 0xF8U) == 0xB0U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);

    registers_.A |= sourceValue;
    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(false);
    registers_.SetFlagC(false);

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  // cp a, r8
  if ((opcode & 0xF8U) == 0xB8U) {
    const uint8_t sourceOperandValue = opcode & 0x7U;
    const R8Operand sourceOperand = static_cast<R8Operand>(sourceOperandValue);
    const uint8_t sourceValue = ReadR8Operand(sourceOperand, registers_, bus_);

    const uint8_t result = registers_.A - sourceValue;
    registers_.SetFlagZ(result == 0U);
    registers_.SetFlagN(true);
    registers_.SetFlagH((registers_.A & 0xFU) < (sourceValue & 0xFU));
    registers_.SetFlagC(registers_.A < sourceValue);

    if (sourceOperand == R8Operand::IndirectHL) {
      return 8;
    }
    return 4;
  }

  throw std::invalid_argument("Unknown opcode for Block2: " +
                              std::to_string(opcode));
}

int32_t Cpu::ExecuteBlock3(const uint8_t opcode) {
  // Implementation for Block3 instructions
  // add a, imm8
  if (opcode == 0xC6) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    const uint8_t originalA = registers_.A;
    const uint32_t result =
        static_cast<uint32_t>(registers_.A) + static_cast<uint32_t>(immValue);
    registers_.A = static_cast<uint8_t>(result & 0xFFU);
    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(((originalA & 0xFU) + (immValue & 0xFU)) > 0xFU);
    registers_.SetFlagC(result > 0xFFU);
    return 8;
  }

  // adc a, imm8
  if (opcode == 0xCE) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    const uint8_t originalA = registers_.A;
    const uint32_t result = static_cast<uint32_t>(registers_.A) +
                            static_cast<uint32_t>(immValue) +
                            static_cast<uint32_t>(registers_.GetFlagC());
    registers_.A = static_cast<uint8_t>(result & 0xFFU);
    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(((originalA & 0xFU) + (immValue & 0xFU) +
                         static_cast<uint32_t>(registers_.GetFlagC())) > 0xFU);
    registers_.SetFlagC(result > 0xFFU);
    return 8;
  }

  // sub a, imm8
  if (opcode == 0xC6) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    const uint32_t result =
        static_cast<uint32_t>(registers_.A) - static_cast<uint32_t>(immValue);
    const uint8_t resultWrap = result & 0xFFU;

    registers_.SetFlagZ(resultWrap == 0U);
    registers_.SetFlagN(true);
    registers_.SetFlagH(((registers_.A & 0xFU) < (immValue & 0xFU)));
    registers_.SetFlagC(result > 0xFFU);

    registers_.A = resultWrap;

    return 8;
  }

  // sbc a, imm8
  if (opcode == 0xDE) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    const uint32_t result = static_cast<uint32_t>(registers_.A) -
                            static_cast<uint32_t>(immValue) -
                            static_cast<uint32_t>(registers_.GetFlagC());
    const uint8_t resultWrap = result & 0xFFU;

    registers_.SetFlagZ(resultWrap == 0U);
    registers_.SetFlagN(true);
    registers_.SetFlagH(
        ((registers_.A & 0xFU) <
         ((immValue & 0xFU) - static_cast<uint32_t>(registers_.GetFlagC()))));
    registers_.SetFlagC(result > 0xFFU);

    registers_.A = resultWrap;

    return 8;
  }

  // and a, imm8
  if (opcode == 0xE6) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    registers_.A &= immValue;

    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(true);
    registers_.SetFlagC(false);

    return 8;
  }

  // xor a, imm8
  if (opcode == 0xEE) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    registers_.A ^= immValue;

    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(false);
    registers_.SetFlagC(false);
    return 8;
  }

  // or a, imm8
  if (opcode == 0xF6) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    registers_.A |= immValue;

    registers_.SetFlagZ(registers_.A == 0U);
    registers_.SetFlagN(false);
    registers_.SetFlagH(false);
    registers_.SetFlagC(false);
    return 8;
  }

  // cp a, imm8
  if (opcode == 0xFE) {
    const uint8_t immValue = FetchByte(bus_, registers_);
    const uint32_t result =
        static_cast<uint32_t>(registers_.A) - static_cast<uint32_t>(immValue);

    registers_.SetFlagZ(result == 0U);
    registers_.SetFlagN(true);
    registers_.SetFlagH((registers_.A & 0xF) < (immValue & 0xF));
    registers_.SetFlagC(immValue > registers_.A);

    return 8;
  }

  // ret cond

  // For now, just print the opcode and return a default cycle count
  std::cout << "Executing Block3 opcode: " << static_cast<int>(opcode) << "\n";
  return 4;  // Example: assume each instruction takes 4 cycles
}

void Cpu::Initialize() {
  // Set pc to start at 0xC000 for now, just for testing.
  registers_.PC = 0xC000;
  Create_LD_BC_n16(bus_, registers_.PC, 0x010F);

  int32_t cycles = Step();

  std::cout << "BC value: " << registers_.GetBC() << "\n";

  std::cout << "Executed instruction in " << cycles << " cycles.\n";
  // Initialize CPU state here
}

int32_t Cpu::Step() {
  if (stopped_) {
    return 0;
  }

  const uint8_t opcode = FetchByte(bus_, registers_);

  // Decode and execute the instruction based on the opcode
  // For simplicity, let's just print the opcode for now
  std::cout << "Executing opcode: " << static_cast<int>(opcode) << "\n";

  // Determine family
  const uint8_t familyType = opcode >> 6U;

  // Block 0
  if (familyType == static_cast<uint8_t>(Family::Block0)) {
    return ExecuteBlock0(opcode);
  }

  // Block 1
  if (familyType == static_cast<uint8_t>(Family::Block1)) {
    return ExecuteBlock1(opcode);
  }

  // Block 2
  if (familyType == static_cast<uint8_t>(Family::Block2)) {
    return ExecuteBlock2(opcode);
  }

  // Block 3
  if (familyType == static_cast<uint8_t>(Family::Block3)) {
    return ExecuteBlock3(opcode);
  }

  switch (opcode) {
    case static_cast<uint8_t>(Instructions::LD_BC_n16): {
      const uint8_t low = FetchByte(bus_, registers_);
      const uint8_t high = FetchByte(bus_, registers_);
      const uint16_t immediateValue =
          static_cast<uint16_t>(static_cast<uint32_t>(high) << 8U) |
          static_cast<uint16_t>(low);
      return LD_BC_n16(registers_, immediateValue);
    }
    case static_cast<uint8_t>(Instructions::LD_BC_A_IO): {
      return LD_BC_A_IO(bus_, registers_);
    }
    case static_cast<uint8_t>(Instructions::INC_BC):
      return INC_BC(registers_);
    default: {
      std::cerr << "Unhandled opcode: " << static_cast<int>(opcode) << "\n";
      return 4;  // Example: assume each instruction takes 4 cycles
    }
  }
}

const Registers& Cpu::GetRegisters() const { return registers_; }
