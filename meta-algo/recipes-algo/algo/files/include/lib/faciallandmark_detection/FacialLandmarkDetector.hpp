#ifndef _FacialLandmarkDetector_H
#define _FacialLandmarkDetector_H

#include "common/Utils.hpp"
#include "common/HETCoreDataStructure.hpp"

#include <cstdint>
#include <iostream>
#include <vector>



/**
 * @brief Using base class, we can call different landmark detection model on input images
 *
 * Attributes:
 *  ladmarks : list of landmarks
 */
class FacialLandmarkDetector 
{
public:
  /**
   * @brief virtual function to do inference on input data
   *        will be derived by the model class
   *
   * @param input_image : input image
   * @param image_res[in] : image_resolution 
   * @param detected_face[in] : face bbox
   * @param facial_landmarks[out] : Facial Landmark output 
  
   */
  virtual void detect_landmark(uint8_t *input_image, resolution_data image_res, facedetection_output detected_face, flm_output* facial_landmarks) = 0;
};

#endif