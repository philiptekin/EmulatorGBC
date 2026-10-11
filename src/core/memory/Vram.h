#pragma once

#include <array>
#include <cstdint>
namespace core {
namespace memory {

class Vram {
 public:
  Vram();

  std::uint8_t ReadByte(uint16_t address);
  void WriteByte(uint16_t address, std::uint8_t value);

  static const uint16_t StartAddress_ = 0x8000;
  static const uint16_t Size_ = 8 * 1024;  // 8 KB
 private:
  std::array<std::uint8_t, Size_> data_{};
};

}  // namespace memory
}  // namespace core
