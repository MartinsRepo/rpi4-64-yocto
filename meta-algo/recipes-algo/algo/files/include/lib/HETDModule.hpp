/*
    HET-CORE
    Copyright (c) 2022 Continental AG and subsidiaries. All rights reserved.

    All binaries and source code stored in this repository,
    except Third Party Software, remains PROPRIETARY and CONFIDENTIAL. 
    Any kind of distribution is prohibited unless explicitly a written consent is given by Continental AG. 
    This software is provided AS-IS without any warrenty nor without any liability.
*/

/**
    @file : HETDModule.hpp
    Interfaces for the Head Eye Tracker Detector module.
    This module contains the following feature :
        - Face Detection 
*/

#ifndef _HETD_MODULE_HPP
#define _HETD_MODULE_HPP

#include "lib/SharedHETCore.hpp"
#include "common/HETCoreDataStructure.hpp"

/**
 *
 * @brief call detect face function to do inference on input images
 *
 * @param image_data[in] : input image
 * @param image_res[in]  : image resolution i.e rows and columns
 * @param output_ptr[out] : output of face detection on input image
 */
HET_CORE_API void hetd_find_face(uint8_t *image_data, resolution_data image_res,
                       facedetection_output *output_ptr);

#endif