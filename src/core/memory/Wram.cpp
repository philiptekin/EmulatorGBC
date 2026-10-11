#include "Wram.h"

#include <stdexcept>

using namespace core::memory;

Wram::Wram() { data_.fill(0); }

std::uint8_t Wram::ReadByte(uint16_t address) {
  if (address < StartAddress_ || address >= StartAddress_ + Size_) {
    throw std::out_of_range("Address out of WRAM range");
  }
  return data_[address - StartAddress_];
}

void Wram::WriteByte(uint16_t address, std::uint8_t value) {
  if (address < StartAddress_ || address >= StartAddress_ + Size_) {
    throw std::out_of_range("Address out of WRAM range");
  }
  data_[address - StartAddress_] = value;
}
