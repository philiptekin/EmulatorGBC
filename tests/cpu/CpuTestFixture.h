#pragma once

#include <gtest/gtest.h>

#include <cstdint>
#include <initializer_list>

#include "core/cpu/Cpu.h"
#include "core/cpu/Registers.h"
#include "core/memory/Bus.h"

using core::cpu::Cpu;
using core::cpu::Registers;
using core::memory::Bus;

constexpr uint16_t kProgramStart = 0xC000;
constexpr uint16_t kDataAddress = kProgramStart + 0x20;
constexpr uint16_t kSpDestination = kProgramStart + 0x100;

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
  bool IsFlagsSetSameValues(const Registers& initialRegisters,
                            const Registers& targetRegisters) {
    return (initialRegisters.GetFlagZ() == targetRegisters.GetFlagZ()) &&
           (initialRegisters.GetFlagC() == targetRegisters.GetFlagC()) &&
           (initialRegisters.GetFlagN() == targetRegisters.GetFlagN()) &&
           (initialRegisters.GetFlagH() == targetRegisters.GetFlagH());
  }

  Bus bus_;
};
