#include "Cpu.h"

#include <iostream>

using namespace core::cpu;

Cpu::Cpu(core::memory::Bus& bus) : bus_(bus) {}

void Cpu::Initialize() {
  bus_.Write(0xC000,
             0xFF);  // Example initialization write to a specific address
  uint8_t value =
      bus_.Read(0xC000);  // Example initialization read from a specific address

  std::cout << "CPU initialized. Read value: " << static_cast<int>(value)
            << "\n";

  // Initialize CPU state here
}
