#ifndef _FLM_EYE_OPENING_DETECTOR_H
#define _FLM_EYE_OPENING_DETECTOR_H

#include "lib/eye_opening_detection/EyeOpeningDetector.hpp"
#include "lib/eye_opening_detection/FLMEyeOpeningDetector_Definitions.hpp"

#include "common/HETCoreDataStructure.hpp"
#include "common/CppLogger.hpp"

#include <vector>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <utility>

namespace HETS
{

  class FLMEyeOpeningDetector : public EyeOpeningDetector 
  {
  public:
    /**
     * @brief Constructor. Resets all internal member variables
     * 
     */
    FLMEyeOpeningDetector();

    /**
     * @brief Destructor
     * 
     */    
    ~FLMEyeOpeningDetector();

    /**
     * @brief Detects the eye opening using the previously detected face region and face landmarks
     *
     * @param image_data[in] : Pointer to image data
     * @param image_res [in] : Image resolution structure 
     * @param fd_data   [in] : Face Detection Output 
     * @param flm_data  [in] : Face Landmark Detection Output
     * @param pose_data [in] : Head Pose Output
     * @param eye_flm_data [out] : Struct for Eye Landmark output
     * @param eyelid_data  [out] : Struct for Eyelid Data Output
     */
    void detect_eye_opening(uint8_t* image_data, resolution_data& image_res,
                            facedetection_output& fd_data, flm_output& flm_data, pose_output& pose_data,
                            eye_flm_output& eye_flm_data, eyelid_output& eyelid_data);

    /**
     * @brief Detects the blink behavior based on a previously detected eye aperture
     *
     * @param eye_flm_data [in] : Eye Landmark output
     * @param eyelid_data  [in] : Eyelid Data Output
     * @param blink_data  [out] : Struct for Blink Data Output
     * @param timestamp_ns [in] : Timestamp in nanoseconds
     */
    void detect_blink(eye_flm_output& eye_flm_data, eyelid_output& eyelid_data, blink_output& blink_data, uint64_t timestamp_ns);   
 
  private:


    // -------------------------
    // General Helper functions
    // -------------------------

    /**  
    * @brief This function sets all internally used member variables to their default values. (Zero)
    * Function accesses/modifies internal member space.
    * 
    * @return TRUE, if init was successful, FALSE otherwise
    */
    bool_t init_internal(void);

    /**  
    * @brief This function caluclates the euclidian distance between two points in 2d as defined by: d=sqrt((x_b-x_a)^2 + (y_b-y_a)^2)
    * Function is reentrant.
    * 
    * @param pt_a First 2d point
    * @param pt_b Second 2d point
    * @return The euclidian distance
    */
    float32_t euclidian_distance2d(cv::Point2f pt_a, cv::Point2f pt_b)
    { return static_cast<float32_t>(sqrt(pow((pt_b.x - pt_a.x),2) + pow((pt_b.y - pt_a.y),2)));};

    /**  
    * @brief This function caluclates the euclidian distance between two points in 3d as defined by: d=sqrt((x_b-x_a)^2 + (y_b-y_a)^2 + (z_b-z_a)^2)
    * Function is reentrant.
    * 
    * @param pt_a First 3d point
    * @param pt_b Second 3d point
    * @return The euclidian distance
    */
    float32_t euclidian_distance3d(cv::Point3f pt_a, cv::Point3f pt_b)
    { return static_cast<float32_t>(sqrt(pow((pt_b.x - pt_a.x),2) + pow((pt_b.y - pt_a.y),2) + pow((pt_b.z - pt_a.z),2)));};

    /**  
    * @brief This function caluclates the Eye-Aspect-Ratio (EAR) for any number of eyelid landmarks by:
    * 
    * ||p_u1-p_l1|| + ||p_u2-p_l2|| + ||p_un-p_lx||
    * ---------------------------------------------
    *                n*||p_oc-p_ic||
    * With p_ux as Point x on upper eyelid
    * With p_lx as Point x on lower eyelid
    * With p_oc as Outer Corner Point
    * With p_ic as Inner Corner Point
    * With n as the numer of eyelid landmarks
    * Function is reentrant.
    * 
    * @param upper_lm Vector of upper landmark points, same order as lower_lm required
    * @param lower_lm Vector of lower landmark points, same order as upper_lm required
    * @param corner_lm Vector of Corner Landmarks (Outer first, Inner second)
    * @return The EAR scalar value 
    */
    float32_t calculate_EAR2d(std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm, std::vector<cv::Point2f> corner_lm);


    // -------------------------------------
    // Functions for Eye Opening calculation
    // -------------------------------------

    /**  
    * @brief This function checks the eye opening detection inputs for validity and useability. It also transforms the input data in a format used internally by EyeOpening Detection.
    * Function accesses/modifies internal member space.
    * 
    * @param image_data[in] : Pointer to image data
    * @param image_res [in] : Image resolution structure 
    * @param fd_data   [in] : Face Detection Output 
    * @param flm_data  [in] : Face Landmark Detection Output
    * @return TRUE, if all inputs are valid and are available in a usable format, FALSE otherwise
    */
    bool_t prepare_eyeopening_inputs(uint8_t* image_data, resolution_data& image_res, facedetection_output& fd_data, flm_output& flm_data);

    /**  
    * @brief This function checks the eye opening output for validity and useability. It also fills in the output space with internal process image data.
    * Function accesses/modifies internal member space.
    * 
    * @param flm_data     [in] : FLM Detector Landmark output
    * @param eye_flm_data [in] : Eye Landmark output
    * @param eyelid_data  [in] : Eyelid Data Output
    * @return TRUE, if all outputs are valid and have been written to the provided space.
    */
    bool_t fill_eyeopening_outputs(flm_output& flm_data, eye_flm_output& eye_flm_data, eyelid_output& eyelid_data);

    /**  
    * @brief This function calculates the eye aperture. Internally, it tries multiple approaches for distance calculation, from most accurate to most rough.
    * Function accesses/modifies internal member space.
    * 
    * @param image_data[in] : Pointer to image data
    * @param image_res [in] : Image resolution structure 
    * @param fd_data   [in] : Face Detection Output 
    * @param flm_data  [in] : Face Landmark Detection Output
    * @param upper_lm  [in] : Vector of upper landmark points, same order as lower_lm required
    * @param lower_lm  [in] : Vector of lower landmark points, same order as upper_lm required
    * @return A pair of <eye aperture in millimeter, confidence value for the opening>
    */
    std::pair<float32_t,float32_t> calculate_eye_opening(uint8_t* image_data, resolution_data& image_res, facedetection_output& fd_data, flm_output& flm_data, std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm);

    /**  
    * @brief This function calculates a rough eyelid aperture value by comparing eyelid landmark pixel distance to HPE nose size.
    * Function is reentrant.
    * 
    * @param upper_lm  [in] : Vector of upper landmark points, same order as lower_lm required
    * @param lower_lm  [in] : Vector of lower landmark points, same order as upper_lm required
    * @param nose_lm   [in] : Vector of nose landmark points, order from top to bottom
    * @return A pair of <eye aperture in millimeter, confidence value for the opening>
    */
    std::pair<float32_t,float32_t> calculate_eye_opening_HPE_based(std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm, std::vector<cv::Point2f> nose_lm);

    /**  
    * @brief This function caluclates an eyelid aperture value by detecting the Iris and using its size (~11mm) for mapping the eyelid distance to millimeter.
    * Function is reentrant.
    * 
    * @param image_data[in] : Pointer to image data
    * @param image_res [in] : Image resolution structure 
    * @param flm_data  [in] : Face Landmark Detection Output
    * @param upper_lm  [in] : Vector of upper landmark points, same order as lower_lm required
    * @param lower_lm  [in] : Vector of lower landmark points, same order as upper_lm required
    * @return A pair of <eye aperture in millimeter, confidence value for the opening>
    */
    std::pair<float32_t,float32_t> calculate_eye_opening_iris_based(uint8_t* image_data, resolution_data& image_res, flm_output& flm_data, std::vector<cv::Point2f> upper_lm, std::vector<cv::Point2f> lower_lm);

    // ------------------------------
    // Functions for Blink Detection
    // ------------------------------

    /**  
    * @brief This function checks the blink inputs for validity and useability. It also transforms the input data in a format used internally by EyeOpening Detection.
    * Function accesses/modifies internal member space.
    * 
    * @param eye_flm_data [in] : Eye Landmark output
    * @param eyelid_data  [in] : Eyelid Data Output
    * @param blink_data  [out] : Struct for Blink Data Output
    * @param timestamp_ns [in] : Timestamp in nanoseconds
    * @return TRUE, if all inputs are valid and are available in a usable format, FALSE otherwise
    */
    bool_t prepare_blink_inputs(eye_flm_output& eye_flm_data, eyelid_output& eyelid_data, blink_output& blink_data, uint64_t timestamp_ns);

    /**  
    * @brief This function checks the blink output for validity and useability. It also fills in the output space with internal process image data.
    * Function accesses/modifies internal member space.
    * 
    * @param blink_data   [in] : Struct for Blink Data Output
    * @return TRUE, if all outputs are valid and have been written to the provided space.
    */
    bool_t fill_blink_outputs(blink_output& blink_data);

    // -----------------------------------
    // Member variables for internal state
    // -----------------------------------

    /**
    * @brief Vector of Left Eye Corner Landmarks (Outer first, Inner Second)
    */
    std::vector<cv::Point2f> m_eye_corner_lms_left{};

    /**
    * @brief Vector of Right Eye Corner Landmarks (Outer first, Inner Second)
    */
    std::vector<cv::Point2f> m_eye_corner_lms_right{};

    /**
    * @brief Vector of Left Upper Eyelid Landmarks (From Outer to Inner)
    */
    std::vector<cv::Point2f> m_upper_eyelid_lms_left{};

    /**
    * @brief Vector of Left Lower Eyelid Landmarks (From Outer to Inner)
    */
    std::vector<cv::Point2f> m_lower_eyelid_lms_left{};

    /**
    * @brief Vector of Right Upper Eyelid Landmarks (From Outer to Inner)
    */
    std::vector<cv::Point2f> m_upper_eyelid_lms_right{};

    /**
    * @brief Vector of Right Lower Eyelid Landmarks (From Outer to Inner)
    */
    std::vector<cv::Point2f> m_lower_eyelid_lms_right{};

    /**
    * @brief Vector of Nose Landmarks (From Root to Bottom)
    */ 
    std::vector<cv::Point2f> m_nose_lms{};    

    /**
     * @brief Internal volatile process image. Will be cleared on every cycle.
     */
    eyelid_output m_eyelid_data{};
    eye_flm_output m_eye_flm_data{};
    blink_output m_blink_data{};
  };
}
#endif // !
