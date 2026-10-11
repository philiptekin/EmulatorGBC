#include "Bus.h"

#include <stdexcept>

using namespace core::memory;

Bus::Bus() = default;

void Bus::Write(uint16_t address, std::uint8_t value) {
  if (address >= Hram::StartAddress_ &&
      address < Hram::StartAddress_ + Hram::Size_) {
    hram_.WriteByte(address, value);
  } else if (address >= Vram::StartAddress_ &&
             address < Vram::StartAddress_ + Vram::Size_) {
    vram_.WriteByte(address, value);
  } else if (address >= Wram::StartAddress_ &&
             address < Wram::StartAddress_ + Wram::Size_) {
    wram_.WriteByte(address, value);
  } else {
    throw std::out_of_range("Address out of range");
  }
}

std::uint8_t Bus::Read(uint16_t address) {
  if (address >= Hram::StartAddress_ &&
      address < Hram::StartAddress_ + Hram::Size_) {
    return hram_.ReadByte(address);
  } else if (address >= Vram::StartAddress_ &&
             address < Vram::StartAddress_ + Vram::Size_) {
    return vram_.ReadByte(address);
  } else if (address >= Wram::StartAddress_ &&
             address < Wram::StartAddress_ + Wram::Size_) {
    return wram_.ReadByte(address);
  } else {
    throw std::out_of_range("Address out of range");
  }
}
