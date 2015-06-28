#pragma once

class Car;

class Controller
{
public:
    Controller() = default;

    // Must return the flags the car owning it should update to
    virtual uint32_t updateFlags(Car * c) const = 0;
};
