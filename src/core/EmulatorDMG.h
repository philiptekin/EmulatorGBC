#pragma once

#include "cpu/Cpu.h"
#include "memory/Bus.h"

class EmulatorDMG {
 public:
  EmulatorDMG();

  void Initialize();

 private:
  core::memory::Bus bus_;
  core::cpu::Cpu cpu_;
};
