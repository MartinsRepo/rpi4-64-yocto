#include "lib/HETModule.hpp"

#include "common/AlgoFactory.hpp"

#include <algorithm>

void het_find_landmark(uint8_t *image_data, resolution_data image_res,
                       facedetection_output face_detection_output, flm_output *output_flm)
{
  //check whether landmarks detected or not
  //update output vector
  if (face_detection_output.valid==false)
  {
    
      //when face not detected
    flm_output landmark_detected = 
    {
                    false,
                    0,
                    0,
                    false,
                    {0,0,0,0,0,0,0,0,0,0,0,0},
                    {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}}
    };
      //update output
      *output_flm = landmark_detected;
  }
  else 
  {
    std::shared_ptr<FacialLandmarkDetector> detector = AlgoFactory::get_instance()->get_facial_landmark_detector();

    //do inference on input image
    detector->detect_landmark(image_data, image_res, face_detection_output, output_flm);
  }
}

void het_find_headposition(intrinsic_camera_parameters camera_intrinsic, resolution_data image_res,
                       flm_output face_landmarks, pose_output *out_ptr)
{
  if(face_landmarks.facial_landmark_validity==false)
  {
    pose_output out_pos = 
    {
      false,
      0,
      {0,0,0,0,0,0,0,0,0},
      {0,0,0},
      {0,0,0}
    };

    *out_ptr = out_pos;
  }
  else
  {
    //create detector
    std::shared_ptr<HeadPoseEstimator> detector = AlgoFactory::get_instance()->get_head_pose_estimator();
    //do inference on input image
    detector->estimate_headpose(camera_intrinsic, image_res, face_landmarks,out_ptr);
  }
}