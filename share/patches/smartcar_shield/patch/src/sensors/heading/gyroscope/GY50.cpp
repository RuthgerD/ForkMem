#include "../../../utilities/Utilities.hpp"
#include "GY50.hpp"


constexpr uint8_t gyro_key = 0;

using namespace smartcarlib::utils;
using namespace smartcarlib::constants::gy50;



GY50::GY50(Runtime& runtime, int offset, unsigned long samplingInterval)
    : kOffset{offset}, kSamplingInterval{samplingInterval}, mRuntime{runtime},
      mPreviousSample{0}, mAttached{false}, mAngularDisplacement{0} {

}

int GY50::getHeading() {  }

void GY50::update() {  }

void GY50::attach() {
    mAttached = true;
}

int GY50::getOffset(int) { return 0; } // for now we are perfect

int GY50::getAngularVelocity() { return 0; }
int GY50::readL3G4200DRegister(uint8_t) { return 0; }
void GY50::writeL3G4200DRegister(uint8_t, uint8_t) {}
