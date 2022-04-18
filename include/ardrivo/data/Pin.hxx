#pragma once

#include <cstdint>
#include <boost/atomic/atomic.hpp>

class Pin {
  public:
    enum DataDirection {
        In,
        Out,
    };

  private:
    std::uint16_t pin_id;

    bool can_digital_read = true;
    bool can_digital_write = true;
    bool can_analog_read = true;
    bool can_analog_write = true;
    boost::atomic_uint16_t value{0};
    boost::atomic<DataDirection> data_direction;

  public:
    Pin(std::uint16_t id) : pin_id{id} {}

    std::uint16_t id() { return pin_id; }

    void write(std::uint16_t v) { value.store(v); }
    std::uint16_t read() { return value.load(); }

    bool can_read_digital() { return can_digital_read; }
    bool can_write_digital() { return can_digital_write; }
    bool can_read_analog() { return can_analog_read; }
    bool can_write_analog() { return can_analog_write; }

    void set_direction(DataDirection dir) { data_direction.store(dir); }
    DataDirection get_direction() { return data_direction.load(); }
};