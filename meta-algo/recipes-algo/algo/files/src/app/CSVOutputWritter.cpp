#include "app/CSVOutputWritter.hpp"


void CSVOutputWritter::create_output_csv(std::ofstream &output_csv, const std::string& csv_name)
{
  output_csv.open(csv_name);

  output_csv << "version;frame_number;timestamp;frame_rate;return_code;face_bb_valid;face_"
              "bb_x_px;face_bb_y_px;face_bb_width_px;face_bb_height_px"
              ";facial_landmark_valid"
              ";left_eye_corner_inner_quality;left_eye_corner_inner_valid;left_eye_corner_inner_x_px;left_eye_corner_inner_y_px"
              ";left_eye_corner_outer_quality;left_eye_corner_outer_valid;left_eye_corner_outer_x_px;left_eye_corner_outer_y_px"
              ";right_eye_corner_inner_quality;right_eye_corner_inner_valid;right_eye_corner_inner_x_px;right_eye_corner_inner_y_px"
              ";right_eye_corner_outer_quality;right_eye_corner_outer_valid;right_eye_corner_outer_x_px;right_eye_corner_outer_y_px"
              ";left_upper_eyelid_contour_quality;left_upper_eyelid_contour_valid;left_upper_eyelid_contour_x_px;left_upper_eyelid_contour_y_px"
              ";left_lower_eyelid_contour_quality;left_lower_eyelid_contour_valid;left_lower_eyelid_contour_x_px;left_lower_eyelid_contour_y_px"
              ";right_upper_eyelid_contour_quality;right_upper_eyelid_contour_valid;right_upper_eyelid_contour_x_px;right_upper_eyelid_contour_y_px"
              ";right_lower_eyelid_contour_quality;right_lower_eyelid_contour_valid;right_lower_eyelid_contour_x_px;right_lower_eyelid_contour_y_px"
              ";mouth_corner_right_quality;mouth_corner_right_valid;mouth_corner_right_x_px;mouth_corner_right_y_px"
              ";mouth_corner_left_quality;mouth_corner_left_valid;mouth_corner_left_x_px;mouth_corner_left_y_px"
              ";subnasal_point_quality;subnasal_point_valid;subnasal_point_x_px;subnasal_point_y_px"
              ";root_nose_quality;root_nose_valid;root_nose_x_px;root_nose_y_px"
              ";headpose_valid;headpose_quality;headpose_ccs_pos_x_m;headpose_ccs_pos_y_m;headpose_ccs_pos_z_m"
              ";headpose_ccs_rot_x_deg;headpose_ccs_rot_y_deg;headpose_ccs_rot_z_deg"
              ";headpose_vcs_pos_x_m;headpose_vcs_pos_y_m;headpose_vcs_pos_z_m"
              ";headpose_vcs_rot_x_deg;headpose_vcs_rot_y_deg;headpose_vcs_rot_z_deg"
              ";left_eyelid_opening_valid;left_eyelid_opening_mm;right_eyelid_opening_valid;right_eyelid_opening_mm"
              "\n";

}


void CSVOutputWritter::check_ouput_file(std::ofstream &output_csv, const std::string& csv_name)
{
  if(!output_csv.is_open())
  {
    create_output_csv(output_csv,csv_name);
  }
}
