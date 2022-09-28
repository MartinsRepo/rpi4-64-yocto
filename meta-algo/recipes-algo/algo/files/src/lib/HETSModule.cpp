#include "lib/HETSModule.hpp"
#include "common/AlgoFactory.hpp"

void HETS::het_find_eyeopening(uint8_t* image_data, resolution_data& image_res,
                      facedetection_output& fd_data, flm_output& flm_data, pose_output& pose_data, uint64_t timestamp_ns,
                      eye_flm_output& eye_flm_data, eyelid_output& eyelid_data, blink_output& blink_data)
{
    std::shared_ptr<HETS::EyeOpeningDetector> detector = AlgoFactory::get_instance()->get_eye_opening_detector();

    detector->detect_eye_opening(image_data, image_res,
                                 fd_data, flm_data, pose_data,
                                 eye_flm_data, eyelid_data);
    detector->detect_blink(eye_flm_data, eyelid_data, blink_data, timestamp_ns);   
}
