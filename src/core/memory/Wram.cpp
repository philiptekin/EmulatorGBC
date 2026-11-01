#include "Wram.h"

#include <stdexcept>

using namespace core::memory;

Wram::Wram() { data_.fill(0); }

std::uint8_t Wram::ReadByte(uint16_t address) const {
  if (address < kStartAddress || address >= kStartAddress + kSize) {
    throw std::out_of_range("Address out of WRAM range");
  }
  return data_[address - kStartAddress];
}

void Wram::WriteByte(uint16_t address, std::uint8_t value) {
  if (address < kStartAddress || address >= kStartAddress + kSize) {
    throw std::out_of_range("Address out of WRAM range");
  }
  data_[address - kStartAddress] = value;
}
