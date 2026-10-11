#include "EmulatorDMG.h"

#include <iostream>

EmulatorDMG::EmulatorDMG() : bus_(), cpu_(bus_) {}

void EmulatorDMG::Initialize() {
  std::cout << "Initializing EmulatorDMG..." << "\n";
  cpu_.Initialize();
}
