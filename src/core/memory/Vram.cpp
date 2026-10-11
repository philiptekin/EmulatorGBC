#include "Vram.h"

#include <stdexcept>

using namespace core::memory;

Vram::Vram() { data_.fill(0); }

std::uint8_t Vram::ReadByte(uint16_t address) {
  if (address < StartAddress_ || address >= StartAddress_ + Size_) {
    throw std::out_of_range("Address out of VRAM range");
  }
  return data_[address - StartAddress_];
}

void Vram::WriteByte(uint16_t address, std::uint8_t value) {
  if (address < StartAddress_ || address >= StartAddress_ + Size_) {
    throw std::out_of_range("Address out of VRAM range");
  }
  data_[address - StartAddress_] = value;
}
