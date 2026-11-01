#include "Bus.h"

#include <stdexcept>
#include <string>

using namespace core::memory;

Bus::Bus() = default;

void Bus::Write(uint16_t address, std::uint8_t value) {
  if (address >= Hram::kStartAddress &&
      address < Hram::kStartAddress + Hram::kSize) {
    hram_.WriteByte(address, value);
  } else if (address >= Vram::kStartAddress &&
             address < Vram::kStartAddress + Vram::kSize) {
    vram_.WriteByte(address, value);
  } else if (address >= Wram::kStartAddress &&
             address < Wram::kStartAddress + Wram::kSize) {
    wram_.WriteByte(address, value);
  } else {
    throw std::out_of_range("Address out of range: " + std::to_string(address));
  }
}

uint8_t Bus::Read(uint16_t address) const {
  if (address >= Hram::kStartAddress &&
      address < Hram::kStartAddress + Hram::kSize) {
    return hram_.ReadByte(address);
  }
  if (address >= Vram::kStartAddress &&
      address < Vram::kStartAddress + Vram::kSize) {
    return vram_.ReadByte(address);
  }
  if (address >= Wram::kStartAddress &&
      address < Wram::kStartAddress + Wram::kSize) {
    return wram_.ReadByte(address);
  }
  throw std::out_of_range("Address out of range: " + std::to_string(address));
}
