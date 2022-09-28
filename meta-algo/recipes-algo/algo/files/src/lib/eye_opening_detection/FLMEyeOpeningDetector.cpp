#include "lib/eye_opening_detection/FLMEyeOpeningDetector.hpp"

HETS::FLMEyeOpeningDetector::FLMEyeOpeningDetector() 
{
  init_internal();
}

HETS::FLMEyeOpeningDetector::~FLMEyeOpeningDetector(){}

void HETS::FLMEyeOpeningDetector::detect_eye_opening(uint8_t* image_data, resolution_data& image_res,
                                facedetection_output& fd_data, flm_output& flm_data, pose_output& pose_data,
                                eye_flm_output& eye_flm_data, eyelid_output& eyelid_data)
{
  
  bool_t res = prepare_eyeopening_inputs(image_data, image_res, fd_data, flm_data);

  if (res)
  {
    // Start left eye analysis only on valid dataset
    if((m_upper_eyelid_lms_left.size() > 0) && (m_lower_eyelid_lms_left.size() > 0) && (m_eye_corner_lms_left.size() > 0))
    {
      // For now, determine if the eyes are closed based on the EAR
       float32_t left_EAR  = calculate_EAR2d(m_upper_eyelid_lms_left,  m_lower_eyelid_lms_left,  m_eye_corner_lms_left);
      if (left_EAR < HETSDefines::EAR_EYE_CLOSED_THRESHOLD)
      {
        m_eyelid_data.is_eye_closed[0] = true;
        m_eyelid_data.is_eye_closed_valid[0] = true;
      }

      // Calculate the eye opening in millimeter for both eyes
      std::pair<float32_t,float32_t> left_aperture  = calculate_eye_opening(image_data, image_res, fd_data, flm_data, m_upper_eyelid_lms_left, m_lower_eyelid_lms_left);
      m_eyelid_data.eye_opening_mm[0] = left_aperture.first;
      m_eyelid_data.eye_opening_quality[0] = left_aperture.second;
      m_eyelid_data.is_eye_opening_valid[0] = true;
    }
    
    // Start right eye analysis only on valid dataset
    if((m_upper_eyelid_lms_right.size() > 0) && (m_lower_eyelid_lms_right.size() > 0) && (m_eye_corner_lms_right.size() > 0))
    {
      // For now, determine if the eyes are closed based on the EAR    
      float32_t right_EAR = calculate_EAR2d(m_upper_eyelid_lms_right, m_lower_eyelid_lms_right, m_eye_corner_lms_right);
      if (right_EAR < HETSDefines::EAR_EYE_CLOSED_THRESHOLD)
      {
        m_eyelid_data.is_eye_closed[1] = true;
        m_eyelid_data.is_eye_closed_valid[1] = true;
      }

      // Calculate the eye opening in millimeter for both eyes
      std::pair<float32_t,float32_t> right_aperture = calculate_eye_opening(image_data, image_res, fd_data, flm_data, m_upper_eyelid_lms_right, m_lower_eyelid_lms_right);
      m_eyelid_data.eye_opening_mm[1] = right_aperture.first;
      m_eyelid_data.eye_opening_quality[1] = right_aperture.second;
      m_eyelid_data.is_eye_opening_valid[1] = true;
    }

    // TODO: Calculate and fill these values!
    //m_eyelid_data.eye_opening_normal;
    //m_eyelid_data.eye_opening_percentage;    
    //m_eyelid_data.is_eye_opening_normal_valid;
    
  }
  
  res = fill_eyeopening_outputs(flm_data, eye_flm_data, eyelid_data);
}

void HETS::FLMEyeOpeningDetector::detect_blink(eye_flm_output& eye_flm_data, eyelid_output& eyelid_data, blink_output& blink_data, uint64_t timestamp_ns)
{
  //TODO: Actual Implementation
}


// -------------------------------------- Helper functions --------------------------------------

bool_t HETS::FLMEyeOpeningDetector::init_internal(void)
{
  std::memset(&m_eyelid_data,0,sizeof(m_eyelid_data));
  std::memset(&m_eye_flm_data,0,sizeof(m_eye_flm_data));
  std::memset(&m_blink_data,0,sizeof(m_blink_data));

  m_eye_corner_lms_left.clear();
  m_eye_corner_lms_right.clear();
  m_upper_eyelid_lms_left.clear();
  m_lower_eyelid_lms_left.clear();
  m_upper_eyelid_lms_right.clear();
  m_lower_eyelid_lms_right.clear();
  m_nose_lms.clear();

  return true;
}

float32_t HETS::FLMEyeOpeningDetector::calculate_EAR2d(std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm, std::vector<cv::Point2f> corner_lm)
{
  float32_t res = 0.0f;

  if(lower_lm.size() == upper_lm.size())
  {
    float32_t n = static_cast<float32_t>(lower_lm.size());
    std::vector<float32_t> ds;

    for (int i = 0; i < n ; i++)
    {
      ds.push_back(euclidian_distance2d(upper_lm.at(i),lower_lm.at(i)));
    }
    float32_t sum = std::accumulate(ds.begin(),ds.end(),0.0f);
    res = (sum / (n * euclidian_distance2d(corner_lm.at(0),corner_lm.at(1))));
    
  }
  else{
    //TODO: Call to logger instead of console
    std::cout << "[ERROR][HETS] Number of upper and lower landmarks not identical!" << std::endl;
    res = -1;
  }

  return res;
}


// -------------------------------------- Functions for Eye Opening calculation --------------------------------------

bool_t HETS::FLMEyeOpeningDetector::prepare_eyeopening_inputs(uint8_t* image_data, resolution_data& image_res, facedetection_output& fd_data, flm_output& flm_data)
{
  init_internal();

  // Check left and right eye landmark qualities
  bool_t is_left_eye_valid = true;
  bool_t is_right_eye_valid = true;

  if(flm_data.facial_landmark_validity)
  {
    // Left eye FLM quality check
    std::vector<float32_t> left_eye_flm_points;
    left_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_LEFTEYE_UPPER_APEX]);
    left_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_LEFTEYE_LOWER_APEX]);    
    left_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_LEFTEYE_OUTER_CORNER]);
    left_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_LEFTEYE_INNER_CORNER]);
    is_left_eye_valid = std::all_of(std::begin(left_eye_flm_points), std::end(left_eye_flm_points), [](float32_t pt_quality){return pt_quality >= HETSDefines::FLM_QUALITY_THRESHOLD;});  
    
    // Right eye FLM quality check
    std::vector<float32_t> right_eye_flm_points;
    right_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_RIGHTEYE_UPPER_APEX]);
    right_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_RIGHTEYE_LOWER_APEX]);    
    right_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_RIGHTEYE_OUTER_CORNER]);
    right_eye_flm_points.push_back( flm_data.facial_landmark_quality[HETSDefines::IDX_RIGHTEYE_INNER_CORNER]);
    is_right_eye_valid = std::all_of(std::begin(right_eye_flm_points), std::end(right_eye_flm_points), [](float32_t pt_quality){return pt_quality >= HETSDefines::FLM_QUALITY_THRESHOLD;});  
  }
  else
  {
    is_left_eye_valid = false;
    is_right_eye_valid = false;
  }

  if (is_left_eye_valid == 1)
  {
    // Construct left eye dataset from input landmarks
    cv::Point2f upper_apex_left   = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_UPPER_APEX][0]  , flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_UPPER_APEX][1]);
    cv::Point2f lower_apex_left   = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_LOWER_APEX][0]  , flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_LOWER_APEX][1]);
    cv::Point2f outer_corner_left = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_OUTER_CORNER][0], flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_OUTER_CORNER][1]);
    cv::Point2f inner_corner_left = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_INNER_CORNER][0], flm_data.facial_landmarks_2d[HETSDefines::IDX_LEFTEYE_INNER_CORNER][1]);
    m_upper_eyelid_lms_left.push_back(upper_apex_left);
    m_lower_eyelid_lms_left.push_back(lower_apex_left);
    m_eye_corner_lms_left.push_back(outer_corner_left);
    m_eye_corner_lms_left.push_back(inner_corner_left);
  }

  if (is_right_eye_valid == 1)
  {
    // Construct right eye dataset from input landmarks
    cv::Point2f upper_apex_right   = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_UPPER_APEX][0]  , flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_UPPER_APEX][1]);
    cv::Point2f lower_apex_right   = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_LOWER_APEX][0]  , flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_LOWER_APEX][1]);
    cv::Point2f outer_corner_right = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_OUTER_CORNER][0], flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_OUTER_CORNER][1]);
    cv::Point2f inner_corner_right = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_INNER_CORNER][0], flm_data.facial_landmarks_2d[HETSDefines::IDX_RIGHTEYE_INNER_CORNER][1]);
    m_upper_eyelid_lms_right.push_back(upper_apex_right);
    m_lower_eyelid_lms_right.push_back(lower_apex_right);
    m_eye_corner_lms_right.push_back(outer_corner_right);
    m_eye_corner_lms_right.push_back(inner_corner_right);
  }

  // Construct nose dataset from input landmarks
  cv::Point2f nose_top    = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_NOSE_TOP][0]   , flm_data.facial_landmarks_2d[HETSDefines::IDX_NOSE_TOP][1]);
  cv::Point2f nose_bottom = cv::Point2f(flm_data.facial_landmarks_2d[HETSDefines::IDX_NOSE_BOTTOM][0], flm_data.facial_landmarks_2d[HETSDefines::IDX_NOSE_BOTTOM][1]);
  m_nose_lms.push_back(nose_top);
  m_nose_lms.push_back(nose_bottom);

  // Copy relevant landmark data to process image
  for (int i = 0 ; i < HETSDefines::NUM_EYE_LM_IDX ; i++)
  {
    auto left_idx = HETSDefines::LEFT_EYE_INDEXES[i];
    auto right_idx = HETSDefines::RIGHT_EYE_INDEXES[i];

    m_eye_flm_data.eye_landmark_left_quality[i] = flm_data.facial_landmark_quality[left_idx];
    m_eye_flm_data.eye_landmark_right_quality[i] = flm_data.facial_landmark_quality[right_idx];
    m_eye_flm_data.eye_landmark_left_2d[i][0] = flm_data.facial_landmarks_2d[left_idx][0];
    m_eye_flm_data.eye_landmark_left_2d[i][1] = flm_data.facial_landmarks_2d[left_idx][1];
    m_eye_flm_data.eye_landmark_right_2d[i][0] = flm_data.facial_landmarks_2d[right_idx][0];
    m_eye_flm_data.eye_landmark_right_2d[i][1] = flm_data.facial_landmarks_2d[right_idx][1];

    m_eye_flm_data.eye_landmark_left_valid[i] = flm_data.facial_landmark_validity;
    m_eye_flm_data.eye_landmark_right_valid[i] = flm_data.facial_landmark_validity;
  }

  return (is_left_eye_valid || is_right_eye_valid);
}

bool_t HETS::FLMEyeOpeningDetector::fill_eyeopening_outputs(flm_output& flm_data, eye_flm_output& eye_flm_data, eyelid_output& eyelid_data)
{
  std::memcpy(&eyelid_data, &m_eyelid_data, sizeof(m_eyelid_data));
  std::memcpy(&m_eye_flm_data, &m_eye_flm_data, sizeof(m_eye_flm_data));

  // TODO: For now, return always true. In the future, post-calculation sanity checks shall be done here.
  return true;
}

std::pair<float32_t,float32_t> HETS::FLMEyeOpeningDetector::calculate_eye_opening(uint8_t* image_data, resolution_data& image_res, facedetection_output& fd_data, flm_output& flm_data, std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm)
{
  //TODO: Try to get accurate millimeter mapping from iris. If not possible, fall back to HPE Nose size reference
  std::pair<float32_t,float32_t> eye_opening_from_hpe = calculate_eye_opening_HPE_based(upper_lm, lower_lm, m_nose_lms);

  return eye_opening_from_hpe;
}

std::pair<float32_t,float32_t> HETS::FLMEyeOpeningDetector::calculate_eye_opening_HPE_based(std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm, std::vector<cv::Point2f> nose_lm)
{
    // Get Nose Top/Bottom distance from HPE mask
    // TODO: Somehow get these magic numbers from HPE module
    cv::Point3f nose_top_3d = cv::Point3f(0.0211f, 0.0f,    0.0136f);
    cv::Point3f nose_bottom_3d = cv::Point3f(0.0289f, 0.0f,   -0.0400f);    
    float32_t nose_size_mm = euclidian_distance3d(nose_top_3d, nose_bottom_3d) * 1000.f;

    // Get pixel distance of nose landmarks
    float32_t nose_size_px = euclidian_distance2d(m_nose_lms.at(0), m_nose_lms.back());
    
    // Get mm per pixel mapping in current frame
    float32_t mm_per_px = nose_size_mm / nose_size_px;

    // Get Eylid distance in reference to nose size
    float32_t eyelid_dist_px = euclidian_distance2d(upper_lm.at(0), lower_lm.at(0));
    float32_t eyelid_dist_mm = eyelid_dist_px * mm_per_px;

    // TODO: Calculate the confidence value from HPE quality
    return std::make_pair(eyelid_dist_mm, 1.0f);
}

std::pair<float32_t,float32_t> HETS::FLMEyeOpeningDetector::calculate_eye_opening_iris_based(uint8_t* image_data, resolution_data& image_res, flm_output& flm_data, std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm)
{
    // TODO: Port implementation from python to C++ 
    return std::make_pair(0.0f, 0.0f);
}


// -------------------------------------- Functions for Blink Detection --------------------------------------

bool_t HETS::FLMEyeOpeningDetector::prepare_blink_inputs(eye_flm_output& eye_flm_data, eyelid_output& eyelid_data, blink_output& blink_data, uint64_t timestamp_ns)
{
  // TODO: Actual implementation
  return false;
}

bool_t HETS::FLMEyeOpeningDetector::fill_blink_outputs(blink_output& blink_data)
{
  std::memcpy(&m_blink_data, &m_blink_data, sizeof(m_blink_data));

  // TODO: For now, return always false since there is no calculation. In the future, post-calculation sanity checks shall be done here.
  return false;
}