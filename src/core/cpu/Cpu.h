#pragma once
#include "../memory/Bus.h"

namespace core {
namespace cpu {

class Cpu {
 public:
  Cpu(core::memory::Bus& bus);

  void Initialize();

 private:
  core::memory::Bus& bus_;
};

}  // namespace cpu
}  // namespace core
