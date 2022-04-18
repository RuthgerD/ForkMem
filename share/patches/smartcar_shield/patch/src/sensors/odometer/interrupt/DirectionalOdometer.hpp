#pragma once

#include <cstdint>
#include <iostream>
#include "DirectionlessOdometer.hpp"

struct DirectionalOdometerPins {
    DirectionalOdometerPins(uint8_t pulsePin, uint8_t forwardWhenLowPin)
        : pulse{pulsePin}, direction{forwardWhenLowPin} {}

    const uint8_t pulse;
    const uint8_t direction;
};

class DirectionalOdometer : public DirectionlessOdometer {
  public:
    DirectionalOdometer(Runtime&, uint8_t pulsePin, uint8_t forwardWhenLowPin, InterruptCallback,
                        unsigned long)
        : DirectionlessOdometer{static_cast<uint16_t>((static_cast<uint16_t>(forwardWhenLowPin) << 8) +
                                                      static_cast<uint16_t>(pulsePin))} {
        std::cerr << "fuck " << (int)forwardWhenLowPin << " " << (int)pulsePin << " "
                  << (int)static_cast<uint16_t>(forwardWhenLowPin) << 8 << " "
                  << (int)static_cast<uint16_t>(pulsePin) << "\n";
    }

    DirectionalOdometer(Runtime& r, DirectionalOdometerPins pins, InterruptCallback c, unsigned long a)
        : DirectionalOdometer(r, pins.pulse, pins.direction, c, a) {}

    int8_t getDirection() const;
};
