#ifndef _HEAD_POSE_ESTIMATOR_H
#define _HEAD_POSE_ESTIMATOR_H

#include "common/Utils.hpp"
#include "common/HETCoreDataStructure.hpp"

#include <iostream>
#include <vector>

/**
 * @brief Using base class, we can call different head pose estimation algorithms to
 * to compute de position of the face
 *
 * Attributes:
 *  output_array  : define the output according to which algorithm is going to be used
 */
class HeadPoseEstimator 
{
public:
  /**
   * @brief virtual function to do inference on input data
   *        will be derived by the model class
   *
   * @param camera_intrinsic[in] : Camera intrinsic structure
   * @param image_res[in] : Image resolution structure 
   * @param face_landmarks [in] : Facial landmarks 
   * @param head_pose [out] : Head pose estimation output 
   */
  virtual void estimate_headpose(intrinsic_camera_parameters camera_intrinsic, resolution_data image_res, flm_output face_landmarks, pose_output* head_pose) = 0;

};

#endif