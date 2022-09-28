#ifndef _FACE_DETECTION_H
#define _FACE_DETECTION_H

#include "common/HETCoreDataStructure.hpp"

#include <iostream>
#include <vector>

/**
 * @brief Using base class, we can call different face detection models to
 * predict faces on input images
 *
 * Attributes:
 *  p_image_data : pointer to input image
 *  output_array  : array to save predicted bbox coordinates on input image
 */
class FaceDetection 
{
public:
  /**
   * @brief virtual function to do inference on input data
   *        will be derived by the model class
   *
   * @param input_image[in] : input image
   * @param img_height[in] : input image height 
   * @param img_width[in]  : input image width
   * @param face_detected [out] : Face detection output
   */
  virtual void detect_face(uint8_t *input_image, const int16_t img_height,
                           const int16_t img_width, facedetection_output* face_detected) = 0;
};

#endif