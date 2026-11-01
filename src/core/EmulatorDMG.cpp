#include "EmulatorDMG.h"

#include <iostream>

EmulatorDMG::EmulatorDMG() : bus_(), cpu_(bus_) {}

void EmulatorDMG::Initialize() {
  std::cout << "Initializing EmulatorDMG..." << "\n";
  cpu_.Initialize();
  const uint8_t time = cpu_.Step();
  std::cout << "CPU step time: " << static_cast<int>(time) << " cycles" << "\n";
}
