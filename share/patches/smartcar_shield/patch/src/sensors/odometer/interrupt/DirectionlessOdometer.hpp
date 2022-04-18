#pragma once

#include <cstdint>
#include <stdint.h>

#include "../../../runtime/Runtime.hpp"
#include "../Odometer.hpp"

class SmartcarOdometer;

class DirectionlessOdometer : public Odometer {
  protected:
    const SmartcarOdometer* udd;

    DirectionlessOdometer(uint16_t key);

  public:
    DirectionlessOdometer(Runtime&, uint8_t pulsePin, InterruptCallback, unsigned long)
        : DirectionlessOdometer(static_cast<uint16_t>(pulsePin)) {}
    ~DirectionlessOdometer() override = default;
    long getDistance() override;
    float getSpeed() override;
    bool isAttached() const override { return true; }
    bool providesDirection() const;
    virtual void reset();
    virtual void update() {}
};
