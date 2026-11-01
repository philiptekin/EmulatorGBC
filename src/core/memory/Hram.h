#pragma once

#include <array>
#include <cstdint>

namespace core {
namespace memory {

class Hram {
 public:
  Hram();

  std::uint8_t ReadByte(uint16_t address) const;
  void WriteByte(uint16_t address, std::uint8_t value);

  static constexpr uint16_t kStartAddress = 0xFF80;
  static constexpr uint16_t kSize = 8 * 16 - 1;  // 127 bytes

 private:
  std::array<std::uint8_t, kSize> data_{};
};

}  // namespace memory
}  // namespace core
