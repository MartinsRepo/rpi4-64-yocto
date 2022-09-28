#ifndef _PNPHEADPOSEESTIMATOR_H
#define _PNPHEADPOSEESTIMATOR_H

#include "lib/headpose_estimation/HeadPoseEstimator.hpp"
#include <opencv2/opencv.hpp>

#include "common/Utils.hpp"
#include "common/HETCoreDataStructure.hpp"

#include "HeadPoseEstimatorDefinitions.hpp"


class PnPHeadPoseEstimator : public HeadPoseEstimator {
public:
  /**
   * @brief Construct a new PnPHeadPoseEstimator::PnPHeadPoseEstimator
   * 
   */
  PnPHeadPoseEstimator();

  /**
   * @brief Public Destructor
   * 
   */
  
  ~PnPHeadPoseEstimator();

  /**
   * @brief estimate the head postion 
   *  
   * @param camera_intrinsic[in] : Camera intrinsic structure
   * @param image_res[in] : Image resolution structure 
   * @param face_landmarks [in] : Facial landmarks 
   * @param head_pose [out] : Head pose estimation output 
   */
  void estimate_headpose(intrinsic_camera_parameters camera_intrinsic, resolution_data image_res, flm_output face_landmarks, pose_output* head_pose);

  

private:

  /**
   * @brief Checks the input data, whether it is suitable for a meaningful HPE execution
   *  
   * @param camera_intrinsic[in] : Camera intrinsic structure
   * @param image_res[in] : Image resolution structure 
   * @param face_landmarks [in] : Facial landmarks
   * @return bool_t : TRUE, if all input data is deemed to be usable by HPE, FALSE otherwise
   */
  bool_t is_input_valid(const intrinsic_camera_parameters& camera_intrinsic, const resolution_data& image_res, const flm_output& face_landmarks);

  /**
   * @brief Checks the HPE output data, whether it is rational
   *  
   * @param head_pose[in] : Head pose estimation output
   * @return bool_t : TRUE, if HPE translation and rotation output data is deemed to be rational, FALSE otherwise
   */
  bool_t is_output_valid(const pose_output& head_pose);

};

#endif // !