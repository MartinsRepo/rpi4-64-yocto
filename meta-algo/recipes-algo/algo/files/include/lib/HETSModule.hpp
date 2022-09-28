/*
    HET-CORE
    Copyright (c) 2022 Continental AG and subsidiaries. All rights reserved.

    All binaries and source code stored in this repository,
    except Third Party Software, remains PROPRIETARY and CONFIDENTIAL. 
    Any kind of distribution is prohibited unless explicitly a written consent is given by Continental AG. 
    This software is provided AS-IS without any warrenty nor without any liability.
*/

/**
    @file : HETSModule.hpp
    Interfaces for the Eye Opening and Blink Detection module.
    This module contains the following features :
        - Eye Opening Detection
        - Blink Detection
*/

#ifndef _HETS_MODULE_HPP
#define _HETS_MODULE_HPP

#include "lib/SharedHETCore.hpp"
#include "common/HETCoreDataStructure.hpp"

namespace HETS
{

    /**
     *
     * @brief Call Eye Opening and Blink Detection
     *
     * @param image_data[in] : Pointer to image data
     * @param image_res [in] : Image resolution structure 
     * @param fd_data   [in] : Face Detection Output 
     * @param flm_data  [in] : Face Landmark Detection Output
     * @param pose_data [in] : Head Pose Output
     * @param timestamp_ns [in] : Timestamp in nanoseconds
     * @param eye_flm_data [out] : Struct for Eye Landmark output
     * @param eyelid_data  [out] : Struct for Eyelid Data Output
     * @param blink_data   [out] : Struct for Blink Data Output
     */
    HET_CORE_API void het_find_eyeopening(uint8_t* image_data, resolution_data& image_res,
                      facedetection_output& fd_data, flm_output& flm_data, pose_output& pose_data, uint64_t timestamp_ns,
                      eye_flm_output& eye_flm_data, eyelid_output& eyelid_data, blink_output& blink_data);

}
#endif