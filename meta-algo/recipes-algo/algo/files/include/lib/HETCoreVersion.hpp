/*
    HET-CORE
    Copyright (c) 2022 Continental AG and subsidiaries. All rights reserved.

    All binaries and source code stored in this repository,
    except Third Party Software, remains PROPRIETARY and CONFIDENTIAL. 
    Any kind of distribution is prohibited unless explicitly a written consent is given by Continental AG. 
    This software is provided AS-IS without any warrenty nor without any liability.
*/

/**
    @file : HETCoreVersion.hpp
    Interface to get the HET Core library version.
*/

#ifndef _HET_CORE_VERSION_HPP
#define _HET_CORE_VERSION_HPP
#include <cstdint>

#include "lib/SharedHETCore.hpp"

/**
 *
 * @brief Get the DMAP het core version
 *
 * @param het_core_version[out] : DMAP het core version [Major; Minor; Patch]
 */
HET_CORE_API void get_het_core_version(uint8_t* het_core_version); 

#endif