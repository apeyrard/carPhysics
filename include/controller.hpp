#pragma once

class Controller
{
public:
    Controller(){};
    // Must return the flags the car owning it should update to
    virtual uint32_t updateFlags() const =0;
};
