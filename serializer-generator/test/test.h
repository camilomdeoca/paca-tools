#pragma once

#include <cstdint>
#include <string>

#include "Serializer.hpp"

struct Another {
    std::string name;
};

struct TestStruct : public Another {
    TestStruct(std::string name, float volume, uint32_t count)
        : Another(name), volume(volume), count(count)
    {}

    void testFunc() {}

    float volume;
    uint32_t count;
    AUTO_SERIALIZE;
};

