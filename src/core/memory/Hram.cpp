#include "Hram.h"

#include <stdexcept>

using namespace core::memory;

Hram::Hram() { data_.fill(0); }

std::uint8_t Hram::ReadByte(uint16_t address) {
  if (address < StartAddress_ || address >= StartAddress_ + Size_) {
    throw std::out_of_range("Address out of HRAM range");
  }
  return data_[address - StartAddress_];
}

void Hram::WriteByte(uint16_t address, std::uint8_t value) {
  if (address < StartAddress_ || address >= StartAddress_ + Size_) {
    throw std::out_of_range("Address out of HRAM range");
  }
  data_[address - StartAddress_] = value;
}
