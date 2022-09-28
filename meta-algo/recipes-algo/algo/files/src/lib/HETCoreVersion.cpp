#include "lib/HETCoreVersion.hpp"
#include "version.h"

#include <iostream>
#include <string>

void get_het_core_version(uint8_t* het_core_version)
{
    if (het_core_version!=nullptr)
    {
        het_core_version[0]=std::stoi(PROJECT_VER_MAJOR);
        het_core_version[1]=std::stoi(PROJECT_VER_MINOR);
        het_core_version[2]=std::stoi(PROJECT_VER_PATCH);
    }
}
