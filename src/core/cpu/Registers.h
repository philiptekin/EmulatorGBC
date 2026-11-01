#pragma once
#include <cstdint>

namespace core {
namespace cpu {

struct Registers {
  // 8 bit registers
  uint8_t A = 0;  // Accumulator
  uint8_t F = 0;  // Flags
  uint8_t B = 0;  // General purpose register
  uint8_t C = 0;  // General purpose register
  uint8_t D = 0;  // General purpose register
  uint8_t E = 0;  // General purpose register
  uint8_t H = 0;  // General purpose register
  uint8_t L = 0;  // General purpose register

  // 16 bit registers
  uint16_t PC = 0;  // Program Counter
  uint16_t SP = 0;  // Stack Pointer

  uint16_t GetAF() const {
    return static_cast<uint16_t>((static_cast<uint32_t>(A) << 8U) |
                                 static_cast<uint32_t>(F));
  }

  uint16_t GetBC() const {
    return static_cast<uint16_t>((static_cast<uint32_t>(B) << 8U) |
                                 static_cast<uint32_t>(C));
  }

  uint16_t GetDE() const {
    return static_cast<uint16_t>((static_cast<uint32_t>(D) << 8U) |
                                 static_cast<uint32_t>(E));
  }

  uint16_t GetHL() const {
    return static_cast<uint16_t>((static_cast<uint32_t>(H) << 8U) |
                                 static_cast<uint32_t>(L));
  }

  void SetAF(uint16_t value) {
    A = static_cast<uint8_t>(static_cast<uint32_t>(value) >> 8U);
    F = static_cast<uint8_t>(static_cast<uint32_t>(value) & 0xF0U);
  }

  void SetBC(uint16_t value) {
    B = static_cast<uint8_t>(static_cast<uint32_t>(value) >> 8U);
    C = static_cast<uint8_t>(static_cast<uint32_t>(value) & 0xFFU);
  }

  void SetDE(uint16_t value) {
    D = static_cast<uint8_t>(static_cast<uint32_t>(value) >> 8U);
    E = static_cast<uint8_t>(static_cast<uint32_t>(value) & 0xFFU);
  }

  void SetHL(uint16_t value) {
    H = static_cast<uint8_t>(static_cast<uint32_t>(value) >> 8U);
    L = static_cast<uint8_t>(static_cast<uint32_t>(value) & 0xFFU);
  }

  // Flag masks
  static constexpr uint8_t kZMask = 0x80U;
  static constexpr uint8_t kNMask = 0x40U;
  static constexpr uint8_t kHMask = 0x20U;
  static constexpr uint8_t kCMask = 0x10U;

  // Flag accessors
  //! @brief Get the value of the Zero flag (Z). It is bit 7 of the F register.
  //! 0x80 = 2^7
  //! @return 1 if the Zero flag is set, 0 otherwise.
  bool GetFlagZ() const {
    return static_cast<bool>((static_cast<uint32_t>(F) & kZMask) >> 7U);
  }

  //! @brief Get the value of the Subtract flag (N). It is bit 6 of the F
  //! register. 0x40 = 2^6
  //! @return 1 if the Subtract flag is set, 0 otherwise.
  bool GetFlagN() const {
    return static_cast<bool>((static_cast<uint32_t>(F) & kNMask) >> 6U);
  }

  //! @brief Get the value of the Half Carry flag (H). It is bit 5 of the F
  //! register. 0x20 = 2^5
  //! @return 1 if the Half Carry flag is set, 0 otherwise.
  bool GetFlagH() const {
    return static_cast<bool>((static_cast<uint32_t>(F) & kHMask) >> 5U);
  }

  //! @brief Get the value of the Carry flag (C). It is bit 4 of the F register.
  //! 0x10 = 2^4
  //! @return 1 if the Carry flag is set, 0 otherwise.
  bool GetFlagC() const {
    return static_cast<bool>((static_cast<uint32_t>(F) & kCMask) >> 4U);
  }

  // Flag setters

  //! @brief Helper for determening the values of setting a flag.
  //! @param Set the intended value to set the flag.
  //! @param mask the flags mask in the flags byte.
  uint8_t SetFlagGeneric(bool set, uint32_t mask) {
    const uint32_t flags = F;
    if (set) {
      return static_cast<uint8_t>(flags | mask);
    }
    return static_cast<uint8_t>(flags & ~mask);
  }

  //! @brief Set the value of the Zero flag (Z). It is bit 7 of the F register.
  //! 0x80 = 2^7
  //! @param Set the intended value to set the flag.
  void SetFlagZ(bool set) {
    F = SetFlagGeneric(set, static_cast<uint32_t>(kZMask));
  }

  //! @brief Set the value of the Subtract flag (N). It is bit 6 of the F
  //! register. 0x40 = 2^6
  //! @param Set the intended value to set the flag.
  void SetFlagN(bool set) {
    F = SetFlagGeneric(set, static_cast<uint32_t>(kNMask));
  }

  //! @brief Set the value of the Half Carry flag (H). It is bit 5 of the F
  //! register. 0x20 = 2^5
  //! @param Set the intended value to set the flag.
  void SetFlagH(bool set) {
    F = SetFlagGeneric(set, static_cast<uint32_t>(kHMask));
  }

  //! @brief Set the value of the Carry flag (C). It is bit 4 of the F register.
  //! 0x10 = 2^4
  //! @param Set the intended value to set the flag.
  void SetFlagC(bool set) {
    F = SetFlagGeneric(set, static_cast<uint32_t>(kCMask));
  }
};

}  // namespace cpu
}  // namespace core
