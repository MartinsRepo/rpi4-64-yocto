#ifndef _EYE_OPENING_DETECTOR_H
#define _EYE_OPENING_DETECTOR_H

#include "common/HETCoreDataStructure.hpp"

namespace HETS
{

  /**
   * @brief Using base class, we can call different eye opening detection algorithms
   *
   */
  class EyeOpeningDetector
  {
  public:
    /**
     * @brief Virtual function for eye aperture detection on input data.
     *
     * @param image_data[in] : Pointer to image data
     * @param image_res [in] : Image resolution structure 
     * @param fd_data   [in] : Face Detection Output 
     * @param flm_data  [in] : Face Landmark Detection Output
     * @param pose_data [in] : Head Pose Output
     * @param eye_flm_data [out] : Struct for Eye Landmark output
     * @param eyelid_data  [out] : Struct for Eyelid Data Output
     */
    virtual void detect_eye_opening(uint8_t* image_data, resolution_data& image_res,
                                    facedetection_output& fd_data, flm_output& flm_data, pose_output& pose_data,
                                    eye_flm_output& eye_flm_data, eyelid_output& eyelid_data) = 0;

    /**
     * @brief Virtual function for blink detection on input data
     *
     * @param eye_flm_data [in] : Eye Landmark output
     * @param eyelid_data  [in] : Eyelid Data Output
     * @param blink_data  [out] : Struct for Blink Data Output
     * @param timestamp_ns [in] : Timestamp in nanoseconds
     */
    virtual void detect_blink(eye_flm_output& eye_flm_data, eyelid_output& eyelid_data, blink_output& blink_data, uint64_t timestamp_ns) = 0;                                    

  };
}
#endif