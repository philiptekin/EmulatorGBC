#pragma once
#include <cstdint>

#include "../memory/Bus.h"
#include "Registers.h"

namespace core {
namespace cpu {

class Cpu {
 public:
  Cpu(core::memory::Bus& bus, Registers initialRegisters = {});

  void Initialize();

  //! @brief Step the CPU by executing one instruction.
  //! @return Number of T-cycles taken to execute the instruction.
  int32_t Step();

  const Registers& GetRegisters() const;

 private:
  int32_t ExecuteBlock0(const uint8_t opcode);
  core::memory::Bus& bus_;
  Registers registers_;
  bool stopped_ = false;
};

}  // namespace cpu
}  // namespace core
