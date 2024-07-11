#include "test.h"

#include <iostream>
#include <ostream>
#include <unistd.h>
#include <fstream>

int main (int argc, char *argv[]) {
    int error = -1;
    TestStruct testBefore {
        "string",
        2.56f,
        200
    };
    {
        std::ofstream ofs("binary_out");
        testBefore.serialize(ofs);
    }
    std::cout << "SWITCH" << std::endl;
    {
        TestStruct test{"", 0.0f, 0};
        std::ifstream ifs("binary_out");
        test.unserialize(ifs);
        if (   testBefore.name == test.name
            && testBefore.volume == test.volume
            && testBefore.count == test.count)
        {
            std::cout << "The struct was serialized and unserialized correctly" << std::endl;
            error = 0;
        }   
        else
        {
            std::cout << "The unserialized struct differs from the serialized struct" << std::endl;
            error = 1;
        }
    }
    std::remove("binary_out");

    return error;
}
