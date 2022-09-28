#ifndef _HET_CORE_DATA_STRUCTURE_HPP
#define _HET_CORE_DATA_STRUCTURE_HPP

#include "common/HETCoreTypes.hpp"

static constexpr const uint8_t NUM_FACE_LANDMARKS = 12;
static constexpr const uint8_t NUM_EYE_LANDMARKS = 4;

struct logger_config
{
  //Logger Config
  LogLevel log_level;        //! > Log Level
  LogType log_type;         //! > Log Type
  std::string log_file_name;  //!> Log File Name
}; 

struct init_config
{
  logger_config log_config;
};
struct resolution_data 
{
  int16_t image_rows; //! > input image rows
  int16_t image_cols; //! > input image cols
};

struct facedetection_output 
{
  bool valid;  //! > whether face is detected in input image or not
  float32_t face_rect[4];  //! > array to store detected bbox coordinate on input image
};

struct flm_output 
{
  bool face_tracked; //! if you want to track the face
  float32_t tracking_quality; //! quality of tracker from 0.0 to 1.0
  int32_t facial_roi; //! ROI of face image
  bool facial_landmark_validity;  //! validity of detected landmarks
  float32_t facial_landmark_quality[NUM_FACE_LANDMARKS]; //! quality of facial landmarks
  float32_t facial_landmarks_2d[NUM_FACE_LANDMARKS][2];  //! 2 Dimentional detected facial landmarks
};

struct intrinsic_camera_parameters
{
  float32_t focal_x; //! x coordinate of the focal distance of the camera
  float32_t focal_y; //! y coordinate of the focal distance of the camera
  float32_t center_x; //! x coordinate of the center of the camera
  float32_t center_y; //! x coordinate of the center of the camera
  float32_t skew; //! skew of the camera
  float32_t distortion_coefficients[5]; //! Positions: 0=RadialDistortion1, 1=RadialDistortion2, 2=TangentialDistortion1, 3=TangentialDistortion2, 4=RadialDistortion3
};

struct pose_output
{
  bool head_pose_valid; //! head pose output is valid or not
  float32_t head_pose_quality; //! quality of head pose estimation from 0.0 to 1.0
  float32_t R[9]; //! Rotation matrix 
  float32_t T[3];  //! Translation vector
  float32_t euler_angles[3]; //! Euler angles
  float32_t R_vcs[9]; //! Rotation matrix in vehicle coordinate system
  float32_t T_vcs[3];  //! Translation vector in vehicle coordinate system
  float32_t euler_angles_vcs[3]; //! Euler angles  in vehicle coordinate system
};

struct eye_flm_output
{
  float32_t eye_landmark_left_2d[NUM_EYE_LANDMARKS][2];       // Landmark positions in x,y pixel coordinates
  float32_t eye_landmark_left_valid[NUM_EYE_LANDMARKS];       // Validity signal for all landmarks
  float32_t eye_landmark_left_quality[NUM_EYE_LANDMARKS];     // Quality value per landmark
  float32_t eye_landmark_right_2d[NUM_EYE_LANDMARKS][2];      // Landmark positions in x,y pixel coordinates
  float32_t eye_landmark_right_valid[NUM_EYE_LANDMARKS];      // Validity signal for all landmarks
  float32_t eye_landmark_right_quality[NUM_EYE_LANDMARKS];    // Quality value per landmark
};

struct eyelid_output
{
  float32_t eye_opening_mm[2];              // Eye opening value in millimeter for both eyes (Index 0=Left, 1=Right)
  float32_t eye_opening_quality[2];         // Eye opening quality for both eyes (Index 0=Left, 1=Right, Range 0.0 - 1.0)
  bool_t    is_eye_opening_valid[2];        // Validity flag for both eye openings (Index 0=Left, 1=Right)
  float32_t eye_opening_normal[2];          // Reference eye opening value for fully opened eye in millimeter (Index 0=Left, 1=Right)
  bool_t    is_eye_opening_normal_valid[2]; // Eye opening validity for reference value (Index 0=Left, 1=Right)
  bool_t    is_eye_closed[2];               // Flag for eye closure (Index 0=Left, 1=Right)
  bool_t    is_eye_closed_valid[2];         // Eye closure validity flags for both eyes (Index 0=Left, 1=Right)
};

struct blink_output
{
  bool_t    is_blink_detected[2];                   // Flag for detected blink (Index 0=Left, 1=Right)
  bool_t    is_blink_detected_valid[2];             // Valid flag for blink detection (Index 0=Left, 1=Right)
  float32_t blink_detected_quality[2];              // Quality value of blink detection (Index 0=Left, 1=Right, Range 0.0 - 1.0)
  float32_t eye_average_closing_speed[2];           // Average eye closing speed calculated over eye_average_speed_window in mm/sec (Index 0=Left, 1=Right)
  bool_t    is_eye_average_closing_speed_valid[2];  // Valid flag for eye closing speed (Index 0=Left, 1=Right)
  float32_t eye_average_opening_speed[2];           // Average eye opening speed calculated over eye_average_speed_window in mm/sec (Index 0=Left, 1=Right)
  bool_t    is_eye_average_opening_speed_valid[2];  // Valid flag for eye opening speed (Index 0=Left, 1=Right)
  float32_t eye_average_blink_frequency[2];         // Average number of blinks per minute calculated over blink_frequency_window in blink/min (Index 0=Left, 1=Right)
  bool_t    is_eye_average_blink_frequency_valid[2];// Valid flag for average blink frequency (Index 0=Left, 1=Right)
};

#endif