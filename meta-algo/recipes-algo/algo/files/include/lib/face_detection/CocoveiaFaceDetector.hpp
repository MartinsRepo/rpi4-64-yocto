#ifndef _CocoveiaFaceDetector_H
#define _CocoveiaFaceDetector_H

#include "lib/face_detection/FaceDetection.hpp"
#include <opencv2/opencv.hpp>

class TfLiteModel;
class TfLiteInterpreterOptions;
class TfLiteInterpreter;
class TfLiteTensor;


/**
 * @brief Derived class to detect face on input data
 *
 * Attributes:
 *  p_input_image : pointer to input image 
 *  output_array  : array to save predicted bbox coordinates on input image
 * 
 */
class CocoveiaFaceDetector : public FaceDetection {
public:
  /**
   * @brief Construct a new Cocoveia Face Detector:: Cocoveia Face Detector object
   * 
   */
  CocoveiaFaceDetector();

  /**
   * @brief Public Destructor
   * 
   */
  
  ~CocoveiaFaceDetector();

  /**
   * @brief Load model and do inference on input images. 
   * 
   * @param input_image[in] : Input image data 
   * @param image_height[in] : Input image height ratio 
   * @param image_width [in] : Input image width ratio
   * @param face_detected [out] : Face detection output
   */
  void detect_face(uint8_t *input_image, const int16_t image_height,
                   const int16_t image_width,facedetection_output* face_detected);

private:

  struct Face 
  {
    cv::Rect m_box;     //! > The face box
    float32_t m_pose_yaw;   //! > Face pose yaw
    float32_t m_pose_pitch; //! > Face pose pitch
    float32_t m_pose_roll;  //! > Face pose roll
  };

  /**
   * @brief Compute bounding box coordinates from detection output
   * 
   * @param x1[in,out] : x (top left x-coordinate)
   * @param x2[in,out] : w (width)
   * @param y1[in,out] : y (top left y-coordinate)
   * @param y2[in,out] : h (height)
   * @param regression_output_data[in] : regression output from detector
   * @param image_width[in] : input image width
   * @param image_height[in] : input image height
   */
  void compute_bounding_box(std::vector<float32_t> &x1, std::vector<float32_t> &x2,
                            std::vector<float32_t> &y1, std::vector<float32_t> &y2,
                            std::vector<float32_t> &regression_output_data,
                            int16_t image_width, int16_t image_height);

  /**
   * @brief check bbox with score greater than threshold
   * 
   * @param x1 [in,out] : x (top left x-coordinate)
   * @param x2 [in,out] : w (width)
   * @param y1 [in,out] : y (top left y-coordinate)
   * @param y2 [in,out] : h (height)
   * @param classification_output_data : classification output from detector
   * @return std::multimap<float32_t, Face>[in,out] : output from detector 
   */
  std::multimap<float32_t, Face>
  check_scores(std::vector<float32_t> &x1, std::vector<float32_t> &x2,
               std::vector<float32_t> &y1, std::vector<float32_t> &y2,
               std::vector<float32_t> &classification_output_data);

  /**
   * @brief initalize anchors for detection 
   * 
   * @param m_anchors[in, out] : vector for anchors
   */
  void initializeAnchors();

  /**
   * @brief apply non maximum suppression on face detection output
   * 
   * @param input_faces[in] : face detectiion output
   * @param faces[out] : output vector 
   */
  void non_max_suppression(std::multimap<float32_t, Face> &input_faces,
                         std::vector<Face> &faces);

  std::vector<std::vector<float32_t>> m_anchors; //! > Anchor vector

  static const std::vector<float32_t> MEAN; //! > Mean Vector

  static const std::vector<float32_t> STD; //! > STD Vector

  static const int MODEL_IMAGE_HEIGHT; //! > Image width taken by model
  static const int MODEL_IMAGE_WIDTH;  //! > Image height taken by model
  static const float32_t CLASSIFICATION_SCORE_THRESHOLD;  //! > Classification threshold on output scores

  static const float32_t NMS_THRESHOLD;  //! > non-maximum supression threshold
  static const int NMS_NEIGHBORS;
  static const float32_t NMS_MIN_SCORE_SUM;  //! > non-maximum minimum

  TfLiteModel* m_model; //! > TF lite model 
  TfLiteInterpreterOptions* m_options; //! > TF lite interpreter options
  TfLiteInterpreter* m_interpreter; //! > TF lite interpreter
  TfLiteTensor* m_input_tensor =nullptr; //! > Input tensor
  const TfLiteTensor* m_output_regression_0 =nullptr; //! > Output Regression 0 tensor
  const TfLiteTensor* m_output_regression_1 =nullptr; //! > Output Regression 1 tensor
  const TfLiteTensor* m_output_regression_2 =nullptr; //! > Output Regression 2 tensor
  const TfLiteTensor* m_output_classification_0 =nullptr; //! > Output Classification 0 tensor
  const TfLiteTensor* m_output_classification_1 =nullptr; //! > Output Classification 1 tensor
  const TfLiteTensor* m_output_classification_2 =nullptr; //! > Output Classification 2 tensor
  const TfLiteTensor* m_output_pose_0 =nullptr; //! > Output Pose 0 tensor
  const TfLiteTensor* m_output_pose_1 =nullptr; //! > Output Pose 1 tensor
  const TfLiteTensor* m_output_pose_2 =nullptr; //! > Output Pose 2 tensor

  static const std::vector<float32_t> ANCHORS_SIZES;  //! > Anchor size 
  static const std::vector<float32_t> ANCHORS_SCALES;  //! > Anchor scales
  static const std::vector<float32_t> ANCHORS_RATIOS;  //! > Anchor ratios
  static const std::vector<std::vector<float32_t>> ANCHORS_LEVEL_SHAPES;  //! > Anchor Levels
};

#endif // !1