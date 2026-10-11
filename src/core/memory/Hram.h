#pragma once

#include <array>
#include <cstdint>

namespace core {
namespace memory {

class Hram {
 public:
  Hram();

  std::uint8_t ReadByte(uint16_t address);
  void WriteByte(uint16_t address, std::uint8_t value);

  static const uint16_t StartAddress_ = 0xFF80;
  static const uint16_t Size_ = 8 * 16 - 1;  // 127 bytes

 private:
  std::array<std::uint8_t, Size_> data_{};
};

}  // namespace memory
}  // namespace core
