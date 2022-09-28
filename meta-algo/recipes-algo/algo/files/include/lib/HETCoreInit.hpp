/*
    HET-CORE
    Copyright (c) 2022 Continental AG and subsidiaries. All rights reserved.

    All binaries and source code stored in this repository,
    except Third Party Software, remains PROPRIETARY and CONFIDENTIAL. 
    Any kind of distribution is prohibited unless explicitly a written consent is given by Continental AG. 
    This software is provided AS-IS without any warrenty nor without any liability.
*/

/**
    @file : HETCoreInit.hpp
    Interface to init the library algorithms and other module. 
*/

#ifndef _MODEL_LOADER_H
#define _MODEL_LOADER_H

#include "lib/SharedHETCore.hpp"
#include "common/HETCoreDataStructure.hpp"

/**
 *
 * @brief Init all the algorithms of the dmap het library with Default Config
 *
 * 
 */
HET_CORE_API void init_het_core (); 

/**
 *
 * @brief Init all the algorithms of the dmap het library with Config
 *
 * 
 */
HET_CORE_API void init_het_core (init_config config); 



#endif 