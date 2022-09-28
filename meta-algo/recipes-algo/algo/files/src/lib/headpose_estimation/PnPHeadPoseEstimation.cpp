#include "lib/headpose_estimation/PnPHeadPoseEstimator.hpp"


#include "common/Utils.hpp"
#include "common/ImageProcessing.hpp"
#include "common/CppLogger.hpp"

#include <math.h>
#include <numeric>

PnPHeadPoseEstimator::PnPHeadPoseEstimator() 
{
  
}

PnPHeadPoseEstimator::~PnPHeadPoseEstimator()
{
  
}

void PnPHeadPoseEstimator::estimate_headpose(intrinsic_camera_parameters camera_intrinsic, resolution_data image_res, flm_output face_landmarks, pose_output* head_pose)
{
  if (is_input_valid(camera_intrinsic, image_res, face_landmarks))
  {

    // 2D image points. If you change the image, you need to change vector
    std::vector<cv::Point2f> image_points;
    image_points.push_back( cv::Point2f(face_landmarks.facial_landmarks_2d[HPEDefines::IDX_NOSE_BOTTOM][0],           face_landmarks.facial_landmarks_2d[HPEDefines::IDX_NOSE_BOTTOM][1]));    // Nose bottom
    image_points.push_back( cv::Point2f(face_landmarks.facial_landmarks_2d[HPEDefines::IDX_NOSE_TOP][0],              face_landmarks.facial_landmarks_2d[HPEDefines::IDX_NOSE_TOP][1]) );    // nose top
    image_points.push_back( cv::Point2f(face_landmarks.facial_landmarks_2d[HPEDefines::IDX_LEFTEYE_LEFT_CORNER][0],   face_landmarks.facial_landmarks_2d[HPEDefines::IDX_LEFTEYE_LEFT_CORNER][1]));     // Left eye left corner
    image_points.push_back( cv::Point2f(face_landmarks.facial_landmarks_2d[HPEDefines::IDX_RIGHTEYE_RIGHT_CORNER][0], face_landmarks.facial_landmarks_2d[HPEDefines::IDX_RIGHTEYE_RIGHT_CORNER][1]));    // Right eye right corner
    image_points.push_back( cv::Point2f(face_landmarks.facial_landmarks_2d[HPEDefines::IDX_MOUTH_LEFT_CORNER][0],     face_landmarks.facial_landmarks_2d[HPEDefines::IDX_MOUTH_LEFT_CORNER][1]));    // Left Mouth corner
    image_points.push_back( cv::Point2f(face_landmarks.facial_landmarks_2d[HPEDefines::IDX_MOUTH_RIGHT_CORNER][0],    face_landmarks.facial_landmarks_2d[HPEDefines::IDX_MOUTH_RIGHT_CORNER][1]));    // Right mouth corner

    // 3D model points in meter and in SM HCS (Origin between eyes, x-Axis out front,y-axis through left eye, z-Axis up)
    // Angle betwen Nose Bottom and Origin ~36deg
    std::vector<cv::Point3f> model_points;
    model_points.push_back(cv::Point3f(0.0289f, 0.0f,   -0.0400f)); // Nose bottom
    model_points.push_back(cv::Point3f(0.0211f, 0.0f,    0.0136f)); // nose top
    model_points.push_back(cv::Point3f(0.0f,    0.0445f, 0.0f));    // Left eye right corner
    model_points.push_back(cv::Point3f(0.0f,   -0.0445f, 0.0f));    // Right eye left corner
    model_points.push_back(cv::Point3f(0.0109f, 0.0232f,-0.0699f)); // Left mouth corner
    model_points.push_back(cv::Point3f(0.0109f,-0.0232f,-0.0699f)); // Right Mouth corner

    // Quality
    std::vector<float32_t> landmark_qualities;
    landmark_qualities.push_back(face_landmarks.facial_landmark_quality[10]);    // Nose bottom
    landmark_qualities.push_back(face_landmarks.facial_landmark_quality[11]);    // nose top
    landmark_qualities.push_back(face_landmarks.facial_landmark_quality[1]);     // Left eye left corner
    landmark_qualities.push_back(face_landmarks.facial_landmark_quality[3]);    // Right eye right corner
    landmark_qualities.push_back(face_landmarks.facial_landmark_quality[9]);    // Left Mouth corner
    landmark_qualities.push_back(face_landmarks.facial_landmark_quality[8]);    // Right mouth corner

    
    
    // Camera internals
    cv::Mat camera_matrix = (cv::Mat_<float32_t>(3,3) << camera_intrinsic.focal_x, 0, camera_intrinsic.center_x, 0 , 
                                              camera_intrinsic.focal_y, camera_intrinsic.center_y, 0, 0, 1);


    // Extrinsic calibration (column, row) - Hardcoded for seating buck recordings
    cv::Mat car_T_cam(4,4,CV_32F);   
    car_T_cam.at<float32_t>(0,0) = 0.0259f;
    car_T_cam.at<float32_t>(0,1) = -0.3646f;
    car_T_cam.at<float32_t>(0,2) = -0.9308f;
    car_T_cam.at<float32_t>(0,3) = -0.8966f;
    car_T_cam.at<float32_t>(1,0) = 0.9996f;
    car_T_cam.at<float32_t>(1,1) = 0.017f;
    car_T_cam.at<float32_t>(1,2) = 0.0211f;
    car_T_cam.at<float32_t>(1,3) = 0.3942f;
    car_T_cam.at<float32_t>(2,0) = 0.0081f;
    car_T_cam.at<float32_t>(2,1) = -0.931f;
    car_T_cam.at<float32_t>(2,2) = 0.3649f;
    car_T_cam.at<float32_t>(2,3) = 0.7653f;
    car_T_cam.at<float32_t>(3,0) = 0.0f;
    car_T_cam.at<float32_t>(3,1) = 0.0f;
    car_T_cam.at<float32_t>(3,2) = 0.0f;
    car_T_cam.at<float32_t>(3,3) = 1.0f;
    
    cv::Mat dist_coeffs = (cv::Mat_<float32_t>(5,1) << camera_intrinsic.distortion_coefficients[0], camera_intrinsic.distortion_coefficients[1],
                                                       camera_intrinsic.distortion_coefficients[2], camera_intrinsic.distortion_coefficients[3],
                                                       camera_intrinsic.distortion_coefficients[4]);
    
    // Output rotation and translation
    cv::Mat rotation_vector(3,1,CV_32F); // Rotation in axis-angle form
    cv::Mat translation_vector(3,1,CV_32F);
    cv::Mat rotation_matrix(3,3,CV_32F);
    cv::Mat calculated_cam_matrix,calculated_rot_matrix, calculated_trans_matrix;
    cv::Mat rot_matrix_x,rot_matrix_y,rot_matrix_z;
    cv::Mat euler_angles(3,1,CV_32F);
    cv::Mat projection_matrix(3,4,CV_32F);
    cv::Mat calculated_cam_matrix_vcs,calculated_rot_matrix_vcs, calculated_trans_matrix_vcs;
    cv::Mat rot_matrix_x_vcs,rot_matrix_y_vcs,rot_matrix_z_vcs;
    cv::Mat euler_angles_vcs(3,1,CV_32F);
    cv::Mat projection_matrix_vcs(3,4,CV_32F);
    try
    {
      cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector,false,cv::SOLVEPNP_EPNP);
      cv::Rodrigues(rotation_vector,rotation_matrix);
      cv::hconcat(rotation_matrix,translation_vector,projection_matrix);
      
      // Construct 4x4 transformation matrix, verbose right now for debugging
      cv::Mat ccs_head_pose(4,4,CV_32F);
      ccs_head_pose.at<float32_t>(0,0) = rotation_matrix.at<float32_t>(0,0);
      ccs_head_pose.at<float32_t>(1,0) = rotation_matrix.at<float32_t>(1,0);
      ccs_head_pose.at<float32_t>(2,0) = rotation_matrix.at<float32_t>(2,0);
      ccs_head_pose.at<float32_t>(3,0) = 0;
      ccs_head_pose.at<float32_t>(0,1) = rotation_matrix.at<float32_t>(0,1);
      ccs_head_pose.at<float32_t>(1,1) = rotation_matrix.at<float32_t>(1,1);
      ccs_head_pose.at<float32_t>(2,1) = rotation_matrix.at<float32_t>(2,1);
      ccs_head_pose.at<float32_t>(3,1) = 0;
      ccs_head_pose.at<float32_t>(0,2) = rotation_matrix.at<float32_t>(0,2);
      ccs_head_pose.at<float32_t>(1,2) = rotation_matrix.at<float32_t>(1,2);
      ccs_head_pose.at<float32_t>(2,2) = rotation_matrix.at<float32_t>(2,2);
      ccs_head_pose.at<float32_t>(3,2) = 0;
      ccs_head_pose.at<float32_t>(0,3) = translation_vector.at<float32_t>(0);
      ccs_head_pose.at<float32_t>(1,3) = translation_vector.at<float32_t>(1);
      ccs_head_pose.at<float32_t>(2,3) = translation_vector.at<float32_t>(2);
      ccs_head_pose.at<float32_t>(3,3) = 1;
 
      // CCS Euler Angle computation
      cv::Mat ccs_head_pose_subset;
      ccs_head_pose(cv::Rect(0,0,4,3)).copyTo(ccs_head_pose_subset);
      cv::decomposeProjectionMatrix(ccs_head_pose_subset,calculated_cam_matrix,calculated_rot_matrix,calculated_trans_matrix,rot_matrix_x,rot_matrix_y,rot_matrix_z,euler_angles);

      //Quality calculation 
      float32_t head_pose_quality = std::accumulate(landmark_qualities.begin(),landmark_qualities.end(),0.0) /landmark_qualities.size();

      // Transform to VCS
      cv::Mat vcs_head_pose(4,4,CV_32F);
      vcs_head_pose = car_T_cam * ccs_head_pose;
      
      // VCS Euler Angle computation
      cv::Mat vcs_head_pose_subset;
      vcs_head_pose(cv::Rect(0,0,4,3)).copyTo(vcs_head_pose_subset);
 
      cv::decomposeProjectionMatrix(vcs_head_pose_subset,calculated_cam_matrix_vcs,calculated_rot_matrix_vcs,calculated_trans_matrix_vcs,rot_matrix_x_vcs,rot_matrix_y_vcs,rot_matrix_z_vcs,euler_angles_vcs);

      //Fulfill the output structure  
      head_pose->head_pose_valid =true;
      head_pose->head_pose_quality = head_pose_quality;
      
      head_pose->T[0]=translation_vector.at<float32_t>(0);
      head_pose->T[1]=translation_vector.at<float32_t>(1),
      head_pose->T[2]=translation_vector.at<float32_t>(2);

      head_pose->R[0]=rotation_matrix.at<float32_t>(0,0);
      head_pose->R[1]=rotation_matrix.at<float32_t>(1,0);
      head_pose->R[2]=rotation_matrix.at<float32_t>(2,0);
      head_pose->R[3]=rotation_matrix.at<float32_t>(0,1);
      head_pose->R[4]=rotation_matrix.at<float32_t>(1,1);
      head_pose->R[5]=rotation_matrix.at<float32_t>(2,1);
      head_pose->R[6]=rotation_matrix.at<float32_t>(0,2);
      head_pose->R[7]=rotation_matrix.at<float32_t>(1,2);
      head_pose->R[8]=rotation_matrix.at<float32_t>(2,2);

      head_pose->euler_angles[0]=static_cast<float32_t>(euler_angles.at<double>(0));
      head_pose->euler_angles[1]=static_cast<float32_t>(euler_angles.at<double>(1));
      head_pose->euler_angles[2]=static_cast<float32_t>(euler_angles.at<double>(2));

      //VCS Head Pose 
      head_pose->T_vcs[0]=vcs_head_pose.at<float32_t>(0,3);
      head_pose->T_vcs[1]=vcs_head_pose.at<float32_t>(1,3),
      head_pose->T_vcs[2]=vcs_head_pose.at<float32_t>(2,3);

      head_pose->R_vcs[0]=vcs_head_pose.at<float32_t>(0,0);
      head_pose->R_vcs[1]=vcs_head_pose.at<float32_t>(1,0);
      head_pose->R_vcs[2]=vcs_head_pose.at<float32_t>(2,0);
      head_pose->R_vcs[3]=vcs_head_pose.at<float32_t>(0,1);
      head_pose->R_vcs[4]=vcs_head_pose.at<float32_t>(1,1);
      head_pose->R_vcs[5]=vcs_head_pose.at<float32_t>(2,1);
      head_pose->R_vcs[6]=vcs_head_pose.at<float32_t>(0,2);
      head_pose->R_vcs[7]=vcs_head_pose.at<float32_t>(1,2);
      head_pose->R_vcs[8]=vcs_head_pose.at<float32_t>(2,2);

      head_pose->euler_angles_vcs[0]=static_cast<float32_t>(euler_angles_vcs.at<double>(0));
      head_pose->euler_angles_vcs[1]=static_cast<float32_t>(euler_angles_vcs.at<double>(1));
      head_pose->euler_angles_vcs[2]=static_cast<float32_t>(euler_angles_vcs.at<double>(2));

      // If output sanity check failed, return default pose
      if (!is_output_valid(*head_pose))
      {   
        head_pose->head_pose_valid = false;
        // uncomment to put tha values back to default value
        // pose_output default_pose{};
        // *head_pose = default_pose;
      }
    }
    catch(const std::exception &e) 
    {
      CppLogger::getInstance()->error(e.what());
      std::exit(EXIT_FAILURE);
    } 
    
  }
  else
  {
    // If input check failed, return default pose
    pose_output default_pose{};
     *head_pose = default_pose;
  }
}

bool_t PnPHeadPoseEstimator::is_input_valid(const intrinsic_camera_parameters& camera_intrinsic, const resolution_data& image_res, const flm_output& face_landmarks)
{

  // Validity flags for each input, may be better for debugging later on.
  bool_t is_intrinsic_valid = true;
  bool_t is_image_res = true;
  bool_t is_flm_valid = true;

  // TODO: Implement checkers for intrinsic and resolution data
  // TODO: face_tracked is currently always 'false'. Add this check, once signal is available. 

  if(face_landmarks.facial_landmark_validity)
  {
    // Check, if all used FLM points' quality is above the defined threshold
    std::vector<float32_t> flm_points;
    flm_points.push_back( face_landmarks.facial_landmark_quality[HPEDefines::IDX_NOSE_BOTTOM]);
    flm_points.push_back( face_landmarks.facial_landmark_quality[HPEDefines::IDX_NOSE_TOP]);
    flm_points.push_back( face_landmarks.facial_landmark_quality[HPEDefines::IDX_LEFTEYE_LEFT_CORNER]);
    flm_points.push_back( face_landmarks.facial_landmark_quality[HPEDefines::IDX_RIGHTEYE_RIGHT_CORNER]);
    flm_points.push_back( face_landmarks.facial_landmark_quality[HPEDefines::IDX_MOUTH_LEFT_CORNER]);
    flm_points.push_back( face_landmarks.facial_landmark_quality[HPEDefines::IDX_MOUTH_RIGHT_CORNER]);

    is_flm_valid = std::all_of(std::begin(flm_points), std::end(flm_points), [](float32_t pt_quality){return pt_quality >= HPEDefines::FL_QUALITY_THRESHOLD;});  
  }
  else
  {
    is_flm_valid = false;
  }
  
  return (is_intrinsic_valid && is_image_res && is_flm_valid);
}

bool_t PnPHeadPoseEstimator::is_output_valid(const pose_output& head_pose)
{
  // Validity flags for each output, may be better for debugging later on.
  bool_t is_translation_valid = true;
  bool_t is_rotation_valid = true;
  
  if(head_pose.head_pose_valid)
  {
    // X-Translation boundary check
    if((head_pose.T[0] < HPEDefines::X_BOUNDARY_MIN) || (head_pose.T[0] > HPEDefines::X_BOUNDARY_MAX)) {is_translation_valid=false;}
    
    // Y-Translation boundary check
    if((head_pose.T[1] < HPEDefines::Y_BOUNDARY_MIN) || (head_pose.T[1] > HPEDefines::Y_BOUNDARY_MAX)) {is_translation_valid=false;}
    
    // Z-Translation boundary check
    if((head_pose.T[2] < HPEDefines::Z_BOUNDARY_MIN) || (head_pose.T[2] > HPEDefines::Z_BOUNDARY_MAX)) {is_translation_valid=false;}

    // Yaw-Rotation boundary check
    if((head_pose.euler_angles_vcs[2] < HPEDefines::YAW_BOUNDARY_MIN) || (head_pose.euler_angles_vcs[2] > HPEDefines::YAW_BOUNDARY_MAX)) {is_rotation_valid=false;}

    // Pitch-Rotation boundary check
    if((head_pose.euler_angles_vcs[1] < HPEDefines::PITCH_BOUNDARY_MIN) || (head_pose.euler_angles_vcs[1] > HPEDefines::PITCH_BOUNDARY_MAX)) {is_rotation_valid=false;}
    
    // Roll-Rotation boundary check
    if((head_pose.euler_angles_vcs[0] < HPEDefines::ROLL_BOUNDARY_MIN) || (head_pose.euler_angles_vcs[0] > HPEDefines::ROLL_BOUNDARY_MAX)) {is_rotation_valid=false;}
  }
  else
  {
    is_translation_valid = false;
    is_rotation_valid = false;
  }
  
  return (is_translation_valid && is_rotation_valid);
}
