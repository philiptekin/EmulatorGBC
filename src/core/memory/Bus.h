#pragma once

#include <cstdint>

#include "Hram.h"
#include "Vram.h"
#include "Wram.h"

namespace core {
namespace memory {

class Bus {
 public:
  Bus();

  std::uint8_t Read(uint16_t address);
  void Write(uint16_t address, std::uint8_t value);

 private:
  Hram hram_;
  Vram vram_;
  Wram wram_;
};

}  // namespace memory
}  // namespace core
