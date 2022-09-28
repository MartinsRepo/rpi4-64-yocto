/*
    HET-CORE
    Copyright (c) 2022 Continental AG and subsidiaries. All rights reserved.

    All binaries and source code stored in this repository,
    except Third Party Software, remains PROPRIETARY and CONFIDENTIAL. 
    Any kind of distribution is prohibited unless explicitly a written consent is given by Continental AG. 
    This software is provided AS-IS without any warrenty nor without any liability.
*/

/**
    @file : HETModule.hpp
    Interfaces for the Head Eye Tracker module.
    This module contains the following feature :
        - Facial Landmark Detection 
*/

#ifndef _HET_MODULE_HPP
#define _HET_MODULE_HPP

#include "lib/SharedHETCore.hpp"
#include "common/HETCoreDataStructure.hpp"

/**
 *
 * @brief call facial landmark finder inference pipeline
 *
 * @param image_data[in] : input image
 * @param image_res[in]  : image resolution i.e rows and columns
 * @param detected_face[in] : input bounding box
 * @param flm_output[out] : output of structure of facial landmark detection
 */
HET_CORE_API void het_find_landmark(uint8_t *image_data, resolution_data image_res, facedetection_output detected_face, flm_output *output_flm);

/**
 *
 * @brief call head position estimation inference pipeline
 *
 * @param camera_intrinsic[in] : intrinsic parametres of the camera
 * @param image_res[in]  : image resolution i.e rows and columns
 * @param face_landmarks[in] : output of face landmark detection 
 * @param out_ptr[out] : output of structure of facial landmark detection
 */
HET_CORE_API void het_find_headposition(intrinsic_camera_parameters camera_intrinsic, resolution_data image_res, flm_output face_landmarks, pose_output *out_ptr);


#endif