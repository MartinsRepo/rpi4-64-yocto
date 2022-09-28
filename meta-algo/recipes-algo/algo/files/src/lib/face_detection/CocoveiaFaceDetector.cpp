#include "lib/face_detection/CocoveiaFaceDetector.hpp"

#include "common/Utils.hpp"
#include "common/ImageProcessing.hpp"
#include "common/CppLogger.hpp"

#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/c/common.h"

#include <iostream>
#include <stdexcept>

//Init static const values 
const std::vector<float32_t> CocoveiaFaceDetector::MEAN = {-0.02335974047877825, -0.02324655647226458,
                                                        0.022932373674175874, 0.02272149453906862}; 
const std::vector<float32_t> CocoveiaFaceDetector::STD = {0.17248740286552908, 0.17228514329199424,
                                                      0.17215003388303982, 0.17228301250742473};

const int CocoveiaFaceDetector::MODEL_IMAGE_HEIGHT = 240; 
const int CocoveiaFaceDetector::MODEL_IMAGE_WIDTH = 320;

const float32_t CocoveiaFaceDetector::CLASSIFICATION_SCORE_THRESHOLD = 0.8;

const float32_t CocoveiaFaceDetector::NMS_THRESHOLD = 0.05;  
const int CocoveiaFaceDetector::NMS_NEIGHBORS = 1;
const float32_t CocoveiaFaceDetector::NMS_MIN_SCORE_SUM = 0.8f;  

const std::vector<float32_t> CocoveiaFaceDetector::ANCHORS_SIZES = {5, 50, 200};   
const std::vector<float32_t> CocoveiaFaceDetector::ANCHORS_SCALES = {1, 1.25, 1.5, 1.75};
const std::vector<float32_t> CocoveiaFaceDetector::ANCHORS_RATIOS = {0.5, 1, 1.3};;  
const std::vector<std::vector<float32_t>> CocoveiaFaceDetector::ANCHORS_LEVEL_SHAPES= {{30, 40}, {15, 20}, {8, 10}};


/**
 * @brief Construct a new Cocoveia Face Detector:: Cocoveia Face Detector object
 * 
 */
CocoveiaFaceDetector::CocoveiaFaceDetector() 
{
  m_model =TfLiteModelCreateFromFile("models/hetd/model_FD_ab8077855ca386a0afb040bb3904621a.tflite");
  m_options = TfLiteInterpreterOptionsCreate();

  try {
    // verify if the model was loaded
    if (m_model == nullptr) {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      std::ostringstream msg;
      throw std::out_of_range(" Error loading the model, check the model name");
   }
  }
  catch (const std::exception &e) {
    CppLogger::getInstance()->error(e.what());
    std::exit(EXIT_FAILURE);
  }  

  //Create the interpreter
  m_interpreter = TfLiteInterpreterCreate(m_model,m_options);
  
  //Allocate tensor buffers
  if (TfLiteInterpreterAllocateTensors(m_interpreter) !=kTfLiteOk)
  {
    std::cout<<"Failed to allocate tensors !"<<std::endl;
  }


  if (TfLiteInterpreterGetInputTensorCount(m_interpreter) <1)
  {
    std::cout<<"Detection model graph needs to have 1 and only 1 input"<<std::endl;
  }

  //Initialize input tensor 
  m_input_tensor =TfLiteInterpreterGetInputTensor(m_interpreter,0);

  //Initialize output tensors 
  m_output_regression_0=TfLiteInterpreterGetOutputTensor(m_interpreter,8);
  m_output_regression_1=TfLiteInterpreterGetOutputTensor(m_interpreter,6);
  m_output_regression_2=TfLiteInterpreterGetOutputTensor(m_interpreter,1);
  
  m_output_classification_0=TfLiteInterpreterGetOutputTensor(m_interpreter,3);
  m_output_classification_1=TfLiteInterpreterGetOutputTensor(m_interpreter,7);
  m_output_classification_2=TfLiteInterpreterGetOutputTensor(m_interpreter,4);
 
  m_output_pose_0=TfLiteInterpreterGetOutputTensor(m_interpreter,2);
  m_output_pose_1=TfLiteInterpreterGetOutputTensor(m_interpreter,0);
  m_output_pose_2=TfLiteInterpreterGetOutputTensor(m_interpreter,5);

  // Intialize Anchors
  initializeAnchors();
}

CocoveiaFaceDetector::~CocoveiaFaceDetector()
{
  TfLiteInterpreterDelete(m_interpreter);
  TfLiteInterpreterOptionsDelete(m_options);
  TfLiteModelDelete(m_model);
}

void CocoveiaFaceDetector::initializeAnchors() 
{
  // Process all size level of the model

  int level = 0;
  for (const auto &size : ANCHORS_SIZES) {
    auto number_new_anchors = ANCHORS_RATIOS.size() * ANCHORS_SCALES.size();

    // Get areas
    std::vector<float32_t> areas;
    auto scale_iterator = ANCHORS_SCALES.begin();
    for (int i = 0; i < number_new_anchors; i++) {
      float32_t area = (*scale_iterator * size) * (*scale_iterator * size);
      areas.push_back(area);

      ++scale_iterator;
      if (scale_iterator == ANCHORS_SCALES.end()) {
        scale_iterator = ANCHORS_SCALES.begin();
      }
    }
    
    // Calculate anchors
    scale_iterator = ANCHORS_SCALES.begin();
    auto area_iterator = areas.begin();
    auto ratio_iterator = ANCHORS_RATIOS.begin();
    std::vector<std::vector<float32_t>> new_anchors;
    for (int i = 0; i < number_new_anchors; i++) {
      float32_t a2 = sqrt(*area_iterator / *ratio_iterator);
      float32_t a3 = a2 * *ratio_iterator;
      float32_t a0 = -a2 * 0.5;
      float32_t a1 = -a3 * 0.5;
      a2 -= a2 * 0.5;
      a3 -= a3 * 0.5;
      new_anchors.push_back({a0, a1, a2, a3});
      ++scale_iterator;
      if (scale_iterator == ANCHORS_SCALES.end()) {
        scale_iterator = ANCHORS_SCALES.begin();

        ++ratio_iterator;
        if (ratio_iterator == ANCHORS_RATIOS.end()) {
          ratio_iterator = ANCHORS_RATIOS.begin();
        }
      }
      ++area_iterator;
      if (area_iterator == areas.end()) {
        area_iterator = areas.begin();
      }
    }
    // Calculate stride vectors
    float32_t level_height = ANCHORS_LEVEL_SHAPES.at(level).at(0);
    float32_t level_width = ANCHORS_LEVEL_SHAPES.at(level).at(1);
    float32_t stride_x = MODEL_IMAGE_WIDTH / level_width;
    float32_t stride_y = MODEL_IMAGE_HEIGHT / level_height;
    std::vector<float32_t> stride_x_vector;
    for (int j = 0; j < level_height; j++) {
      for (int i = 0; i < level_width; i++) {
        stride_x_vector.push_back((i + 0.5) * stride_x);
      }
    }
    std::vector<float32_t> stride_y_vector;
    for (int i = 0; i < level_height; i++) {
      for (int j = 0; j < level_width; j++) {
        stride_y_vector.push_back((i + 0.5) * stride_y);
      }
    }

    // Shift anchors
    auto stride_x_iterator = stride_x_vector.begin();
    auto stride_y_iterator = stride_y_vector.begin();
    for (int i = 0; i < stride_x_vector.size(); i++) {
      for (const auto &new_anchor : new_anchors) {
        m_anchors.push_back({*stride_x_iterator + new_anchor.at(0),
                             *stride_y_iterator + new_anchor.at(1),
                             *stride_x_iterator + new_anchor.at(2),
                             *stride_y_iterator + new_anchor.at(3)});
      }

      ++stride_x_iterator;
      ++stride_y_iterator;
    }
    // Go to next level
    ++level;
  }
}

std::multimap<float32_t, CocoveiaFaceDetector::Face> CocoveiaFaceDetector::check_scores(
    std::vector<float32_t> &x1, std::vector<float32_t> &x2, std::vector<float32_t> &y1,
    std::vector<float32_t> &y2, std::vector<float32_t> &classification_output_data) {
  auto x1_iterator = x1.begin();
  auto x2_iterator = x2.begin();
  auto y1_iterator = y1.begin();
  auto y2_iterator = y2.begin();
  // auto pose_iterator = pose_output_data.begin();

  // Get boxes and pose by score
  std::multimap<float32_t, Face> input_face;
  for (const auto &classification : classification_output_data) {
    // If classification is good enough
    if (classification > CLASSIFICATION_SCORE_THRESHOLD)
    {

      input_face.emplace(classification,
                         Face({cv::Rect(cv::Point(*x1_iterator, *y1_iterator),
                                        cv::Point(*x2_iterator, *y2_iterator)),
                               0, 0, 0}));
    }
    ++x1_iterator;
    ++x2_iterator;
    ++y1_iterator;
    ++y2_iterator;
  }

  return input_face;
}

void CocoveiaFaceDetector::compute_bounding_box(
    std::vector<float32_t> &x1, std::vector<float32_t> &x2, std::vector<float32_t> &y1,
    std::vector<float32_t> &y2, std::vector<float32_t> &regression_output_data,
    int16_t image_width, int16_t image_height) 
{
  // Compute the bounding boxes parameters from the regression outputs
  int index = 0;
  auto anchor_iterator = m_anchors.begin();
  
  for (const auto &regression : regression_output_data) {
    if (index % 4 == 0) {
      auto anchor = anchor_iterator->at(0);
      auto w = anchor_iterator->at(2) - anchor_iterator->at(0);
      x1.push_back(
          Utils::clip(anchor + ((regression * STD[0] + MEAN[0]) * w), 0, static_cast<float>(image_width)));
    } else if (index % 4 == 1) {
      auto anchor = anchor_iterator->at(1);
      auto h = anchor_iterator->at(3) - anchor_iterator->at(1);
      y1.push_back(
          Utils::clip(anchor + ((regression * STD[1] + MEAN[1]) * h), 0, static_cast<float>(image_height)));
    } else if (index % 4 == 2) {
      auto anchor = anchor_iterator->at(2);
      auto w = anchor_iterator->at(2) - anchor_iterator->at(0);
      x2.push_back(
          Utils::clip(anchor + ((regression * STD[2] + MEAN[2]) * w), 0, static_cast<float>(image_width)));
    } else if (index % 4 == 3) {
      auto anchor = anchor_iterator->at(3);
      auto h = anchor_iterator->at(3) - anchor_iterator->at(1);
      y2.push_back(
          Utils::clip(anchor + ((regression * STD[3] + MEAN[3]) * h), 0, static_cast<float>(image_height)));
      ++anchor_iterator;
    }

    ++index;
  }
}

void CocoveiaFaceDetector::non_max_suppression(std::multimap<float32_t, Face> &input_faces,
                                               std::vector<Face> &faces)
{
  // keep looping while some indexes still remain in the indexes list
  while (input_faces.size() > 0) {
    // get the last rectangle
    auto last_element = --std::end(input_faces);
    const cv::Rect &last_rectangle = last_element->second.m_box;

    int neighbors_count = 0;
    float32_t scores_sum = last_element->first;

    input_faces.erase(last_element);

    for (auto pos = std::begin(input_faces); pos != std::end(input_faces);)
    {
      // get the current rectangle
      const cv::Rect &current_rectangle = pos->second.m_box;

      // compute overlap
      float32_t inter_area =
          static_cast<float32_t>((last_rectangle & current_rectangle).area());
      float32_t union_area =
          last_rectangle.area() + current_rectangle.area() - inter_area;

      float32_t overlap = inter_area / union_area;

      // if there's a sufficient overlap, delete the current bounding box
      if (overlap > NMS_THRESHOLD) 
      {
        scores_sum += pos->first;
        pos = input_faces.erase(
            pos); // point to the element next to the erased one
        neighbors_count++;
      } 
      else 
      {
        pos++;
      }
    }

    if (neighbors_count >= NMS_NEIGHBORS && scores_sum >= NMS_MIN_SCORE_SUM) 
    {
      faces.push_back(last_element->second);
    }
  }
}

void CocoveiaFaceDetector::detect_face(uint8_t *image_data,
                                       const int16_t img_height,
                                       const int16_t img_width,
                                       facedetection_output* face_detected) 
{


  cv::Mat input_image_mat(img_height, img_width, CV_8UC1, image_data);

  // calculate ratio height and width with the required input for the tensorflow
  double ratio_height =
      double(MODEL_IMAGE_HEIGHT) / double(input_image_mat.size().height);
  double ratio_width =
      double(MODEL_IMAGE_WIDTH) / double(input_image_mat.size().width);

  // scaling image
  cv::Mat scaled_image_mat;
  
  cv::resize(input_image_mat, scaled_image_mat,
               cv::Size(MODEL_IMAGE_WIDTH, MODEL_IMAGE_HEIGHT),
               cv::INTER_CUBIC);

  // raise contrast on low contrast images
  cv::Mat blur_image;
  cv::blur(scaled_image_mat,blur_image,cv::Size(5,5));
  cv::Scalar mean_blur = cv::mean(blur_image);
  if(mean_blur[0]<40) {
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(7);
    clahe->setTilesGridSize(cv::Size(7,7));
    clahe->apply(scaled_image_mat,scaled_image_mat);
  }

  // do img normalization
  cv::Mat normalized_image_mat = ImageProcessing::image_normalization(scaled_image_mat);

  int16_t image_width = normalized_image_mat.cols;
  int16_t image_height = normalized_image_mat.rows;

  // Copy the image to the input tensor
  memcpy(m_input_tensor->data.f,normalized_image_mat.data , normalized_image_mat.rows *
                                          normalized_image_mat.cols*sizeof(float));
  try {
    // run the inference
    if (TfLiteInterpreterInvoke(m_interpreter) !=kTfLiteOk)
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      std::ostringstream msg;
      throw std::out_of_range(" Error invoking the face detection model ");

    }
  }

  catch (const std::exception &e) {
    CppLogger::getInstance()->error(e.what());
    std::exit(EXIT_FAILURE);
  }                   
  //Get the regression output
  float *regression_output_rawdata_0 = m_output_regression_0->data.f;
  size_t regression_output_rawdata_0_size = m_output_regression_0->bytes/sizeof(float);
  float *regression_output_rawdata_1 = m_output_regression_1->data.f;
  size_t regression_output_rawdata_1_size =m_output_regression_1->bytes/sizeof(float);
  float *regression_output_rawdata_2 = m_output_regression_2->data.f;
  size_t regression_output_rawdata_2_size =m_output_regression_2->bytes/sizeof(float);
  

  // Concatenate the 3 regression outputs into one vector
  std::vector<float32_t> regression_output_data;
  regression_output_data.insert(
      regression_output_data.end(), regression_output_rawdata_0,
      regression_output_rawdata_0 + regression_output_rawdata_0_size);
  regression_output_data.insert(
      regression_output_data.end(), regression_output_rawdata_1,
      regression_output_rawdata_1 + regression_output_rawdata_1_size);
  regression_output_data.insert(
      regression_output_data.end(), regression_output_rawdata_2,
      regression_output_rawdata_2 + regression_output_rawdata_2_size);
  
  try {
    if (regression_output_data.size() < 75840)
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      std::ostringstream msg;
      throw std::out_of_range(" Exception thrown in CocoveiaFaceDetector::regression_output : ERROR_ALGORITHM::REGRESSION_OUTPUT_NULL ");

    }
  }

  catch (const std::exception &e) {
    CppLogger::getInstance()->error(e.what());
    std::exit(EXIT_FAILURE);
  }


  // create the result vector
  std::vector<float> classification_output_data;
  float *classification_output_rawdata_0 = m_output_classification_0->data.f;
  size_t classification_output_rawdata_0_size= m_output_classification_0->bytes/sizeof(float);
  float *classification_output_rawdata_1 = m_output_classification_1->data.f;
  size_t classification_output_rawdata_1_size= m_output_classification_1->bytes/sizeof(float);
  float *classification_output_rawdata_2 = m_output_classification_2->data.f;
  size_t classification_output_rawdata_2_size= m_output_classification_2->bytes/sizeof(float);


  // Concatenate the 3 classification outputs into one vector
  classification_output_data.insert(
      classification_output_data.end(), classification_output_rawdata_0,
      classification_output_rawdata_0 + classification_output_rawdata_0_size);
  classification_output_data.insert(
      classification_output_data.end(), classification_output_rawdata_1,
      classification_output_rawdata_1 + classification_output_rawdata_1_size);
  classification_output_data.insert(
      classification_output_data.end(), classification_output_rawdata_2,
      classification_output_rawdata_2 + classification_output_rawdata_2_size);

  try 
  {
    if (classification_output_data.size()  < 18960)
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      throw std::out_of_range(" Exception thrown in CocoveiaFaceDetector::classification_output : ERROR_ALGORITHM::CLASSIFICATION_OUTPUT_NULL ");

    }
  }
  catch (const std::exception &e) 
  {
    CppLogger::getInstance()->error(e.what());
    std::exit(EXIT_FAILURE);
  }

  // apply sigmoid the classification output vector
  std::for_each(classification_output_data.begin(),
                classification_output_data.end(),
                [&](float32_t &value) { Utils::fsigmoid(value); });

  // create the result vector
  std::vector<float> pose_output_data;
  float *pose_output_rawdata_0 = m_output_pose_0->data.f;
  size_t pose_output_rawdata_0_size= m_output_pose_0->bytes/sizeof(float);
  float *pose_output_rawdata_1 = m_output_pose_1->data.f;
  size_t pose_output_rawdata_1_size= m_output_pose_1->bytes/sizeof(float);
  float *pose_output_rawdata_2 = m_output_pose_2->data.f;
  size_t pose_output_rawdata_2_size= m_output_pose_2->bytes/sizeof(float);


  // Concatenate the 3 classification outputs into one vector
  pose_output_data.insert(pose_output_data.end(), pose_output_rawdata_0,
                          pose_output_rawdata_0 + pose_output_rawdata_0_size);
  pose_output_data.insert(pose_output_data.end(), pose_output_rawdata_1,
                          pose_output_rawdata_1 + pose_output_rawdata_1_size);
  pose_output_data.insert(pose_output_data.end(), pose_output_rawdata_2,
                          pose_output_rawdata_2 + pose_output_rawdata_2_size);

  // Apply tanh on the pose output vector
  std::for_each(pose_output_data.begin(), pose_output_data.end(),
                [&](float32_t &value) { Utils::ftanh(value); });

  // compute bbox coordinates
  std::vector<float32_t> x1, y1, x2, y2;
  compute_bounding_box(x1, x2, y1, y2, regression_output_data, image_width,
                       image_height);

  // check classification scores 
  std::multimap<float32_t, Face> input_face =
      check_scores(x1, x2, y1, y2, classification_output_data);

  auto max_val = 0.00001;
  for (const auto &classification : classification_output_data) 
  {
    if (classification > max_val) 
    {
      max_val = classification;
    }
  }

  // apply non max supp
  std::vector<Face> output_faces;
  non_max_suppression(input_face, output_faces);

  // Fulfill output structure

  bool is_empty = true;
  // update output_array with bbox coordinates
  for (const auto &face : output_faces) 
  {
    face_detected->face_rect[0] = floor(float32_t(face.m_box.x / ratio_width));
    face_detected->face_rect[1] = floor(float32_t(face.m_box.y / ratio_height));
    face_detected->face_rect[2] = floor(float32_t(face.m_box.width / ratio_width));
    face_detected->face_rect[3] = floor(float32_t(face.m_box.height / ratio_height));

    is_empty = false;
  }
  if (is_empty == true) 
  {
    face_detected->face_rect[0] = -1;
    face_detected->face_rect[1] = -1;
    face_detected->face_rect[2] = -1;
    face_detected->face_rect[3] = -1;

    face_detected->valid=false;
  }
  else
  {
    face_detected->valid=true;
  }
}
