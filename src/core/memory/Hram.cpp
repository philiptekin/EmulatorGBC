#include "Hram.h"

#include <stdexcept>

using namespace core::memory;

Hram::Hram() { data_.fill(0); }

std::uint8_t Hram::ReadByte(uint16_t address) const {
  if (address < kStartAddress || address >= kStartAddress + kSize) {
    throw std::out_of_range("Address out of HRAM range");
  }
  return data_[address - kStartAddress];
}

void Hram::WriteByte(uint16_t address, std::uint8_t value) {
  if (address < kStartAddress || address >= kStartAddress + kSize) {
    throw std::out_of_range("Address out of HRAM range");
  }
  data_[address - kStartAddress] = value;
}
