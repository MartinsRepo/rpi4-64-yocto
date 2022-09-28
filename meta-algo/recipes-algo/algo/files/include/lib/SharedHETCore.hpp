/*
    HET-CORE
    Copyright (c) 2022 Continental AG and subsidiaries. All rights reserved.

    All binaries and source code stored in this repository,
    except Third Party Software, remains PROPRIETARY and CONFIDENTIAL. 
    Any kind of distribution is prohibited unless explicitly a written consent is given by Continental AG. 
    This software is provided AS-IS without any warrenty nor without any liability.
*/

/**
    @file : SharedHETCore.hpp
    Definition for library import and export. 
*/


#ifndef _SHARED_HET_CORE_HPP
#define _SHARED_HET_CORE_HPP

#ifdef HET_CORE_EXPORTS 
#   ifdef __GNUC__
#       if __GNUC__ >=4
#           define HET_CORE_API __attribute__((visibility("default")))
#       else 
#           define HET_CORE_API __attribute__((dllexport))
#       endif
#   endif
#else 
#   ifdef __GNUC__
#       define HET_CORE_API
#   endif
#endif

#endif