#include "lib/faciallandmark_detection/PIPNetFacialLandmarkDetector.hpp"
#include "lib/eye_opening_detection/FLMEyeOpeningDetector_Definitions.hpp"



#include "common/Utils.hpp"
#include "common/ImageProcessing.hpp"
#include "common/CppLogger.hpp"

#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/c/common.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>

const float32_t PIPNetFacialLandmarkDetector::DET_BBOX_SCALE = 1;
const int16_t PIPNetFacialLandmarkDetector::INPUT_IMAGE_SIZE = 256;
const std::vector<float32_t> PIPNetFacialLandmarkDetector::STD = {0.1075, 0.1075, 0.1075}; 
const std::vector<float32_t> PIPNetFacialLandmarkDetector::MEAN = {0.2566, 0.2566, 0.2566}; 
const int16_t PIPNetFacialLandmarkDetector::NETWORK_STRIDE = 32;
const int16_t PIPNetFacialLandmarkDetector::NUM_LD = 12;
const int16_t PIPNetFacialLandmarkDetector::NUM_NB = 10;
const float32_t PIPNetFacialLandmarkDetector::THRESHOLD_X_QUALITY = 0.95;
const float32_t PIPNetFacialLandmarkDetector::THRESHOLD_Y_QUALITY = 0.03;
const std::vector<int16_t> PIPNetFacialLandmarkDetector::REVERSE_INDEX1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 
  2, 5, 6, 7, 9, 11, 0, 1, 2, 0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 7, 8, 10, 11, 0, 0, 1, 2, 3, 
  4, 5, 6, 8, 9, 10, 11, 2, 3, 6, 7, 9, 10, 11, 2, 3, 6, 7, 0, 1, 3, 4, 5, 8, 10, 0, 1, 3, 4, 0, 1, 2, 3, 4, 5, 
  6, 7, 8, 9, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const std::vector<int16_t> PIPNetFacialLandmarkDetector::REVERSE_INDEX2 = {1, 2, 5, 2, 2, 2, 2, 5, 2, 0, 1, 1, 
  7, 7, 1, 1, 6, 6, 6, 3, 5, 6, 4, 7, 0, 6, 6, 3, 3, 2, 5, 3, 0, 9, 9, 1, 9, 9, 7, 7, 3, 8, 6, 5, 7, 2, 9, 0, 
  4, 4, 7, 8, 7, 2, 9, 5, 0, 8, 9, 0, 5, 5, 9, 6, 9, 7, 6, 6, 5, 4, 4, 4, 0, 8, 8, 3, 6, 2, 4, 3, 3, 3, 3, 0, 
  7, 9, 7, 2, 6, 6, 9, 9, 1, 1, 9, 6, 6, 9, 9, 8, 8, 8, 8, 8, 1, 2, 8, 8, 8, 8, 3, 5, 4, 2, 7, 7, 8, 8, 0, 0, 
  4, 0, 3, 0, 1, 5, 5, 1, 1, 4, 4, 4};
const int16_t PIPNetFacialLandmarkDetector::MAX_LEN = 11;
const bool PIPNetFacialLandmarkDetector::USE_NEIGHBOURS = true;



/**
 * @brief Construct a new PIPNetFacialLandmarkDetector:: PIPNetFacialLandmarkDetector ladmark detector object
 * 
 */
PIPNetFacialLandmarkDetector::PIPNetFacialLandmarkDetector() 
{
  m_model = TfLiteModelCreateFromFile("models/het/model_FLD_1b67be7231cbac03387f4368b7460634.tflite");
  m_options = TfLiteInterpreterOptionsCreate();

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
  m_outputs_cls = TfLiteInterpreterGetOutputTensor(m_interpreter,3);
  m_outputs_x = TfLiteInterpreterGetOutputTensor(m_interpreter,1);
  m_outputs_y = TfLiteInterpreterGetOutputTensor(m_interpreter,4);
  m_outputs_nb_x = TfLiteInterpreterGetOutputTensor(m_interpreter,2);
  m_outputs_nb_y = TfLiteInterpreterGetOutputTensor(m_interpreter,0);
}

PIPNetFacialLandmarkDetector::~PIPNetFacialLandmarkDetector()
{
  TfLiteInterpreterDelete(m_interpreter);
  TfLiteInterpreterOptionsDelete(m_options);
  TfLiteModelDelete(m_model);
}

void PIPNetFacialLandmarkDetector::detect_landmark(uint8_t *input_image, resolution_data image_res, facedetection_output detected_face,flm_output* facial_landmarks)
{


  int16_t det_x1 = detected_face.face_rect[0];
  int16_t det_y1 = detected_face.face_rect[1];
  int16_t det_w = detected_face.face_rect[2];
  int16_t det_h = detected_face.face_rect[3];
  int16_t det_x2 = det_x1+det_w-1;
  int16_t det_y2 = det_y1+det_h-1;

  //scale the face bbox to 1.2 of its size
  det_x1 -= (int)(det_w * (DET_BBOX_SCALE-1)/2);
  det_x2 += (int)(det_w * (DET_BBOX_SCALE-1)/2);
  det_y1 -= (int)(det_h * (DET_BBOX_SCALE-1)/2);
  det_y2 += (int)(det_h * (DET_BBOX_SCALE-1)/2);
  
  //avoid the bbox going out of the image
  det_x1 = std::max<int16_t>(det_x1,0);
  det_x2 = std::min<int16_t>(det_x2,image_res.image_cols-1);
  det_y1 = std::max<int16_t>(det_y1,0);
  det_y2 = std::min<int16_t>(det_y2,image_res.image_rows-1);
  det_w = det_x2-det_x1+1;
  det_h = det_y2-det_y1+1;


  //read image as cv::Mat and crop it
  cv::Mat input_image_mat(image_res.image_rows,image_res.image_cols, CV_8UC1, input_image);
  cv::Mat cropped_image = ImageProcessing::crop_image(input_image_mat,det_x1,det_x2,det_y1,det_y2);
  
  // raise contrast on low contrast images, low contrast images have a low mean value of the blurred image
  cv::Mat blur_image;
  cv::blur(cropped_image,blur_image,cv::Size(5,5));
  cv::Scalar mean_blur = cv::mean(blur_image);
  if(mean_blur[0]<40) {
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(7);
    clahe->setTilesGridSize(cv::Size(7,7));
    clahe->apply(cropped_image,cropped_image);
  }
  
  cv::Mat scaled_image_mat,RGB_image;
  cv::resize(cropped_image, scaled_image_mat,
               cv::Size(INPUT_IMAGE_SIZE, INPUT_IMAGE_SIZE),
               cv::INTER_CUBIC);
  cv::cvtColor(scaled_image_mat, RGB_image, cv::COLOR_GRAY2RGB);
  // normalize image using std and mean
  cv::Mat normalized_image_mat = ImageProcessing::image_normalization(RGB_image, STD, MEAN);
  cv::Mat channel[3];
  cv::split(normalized_image_mat,channel);

  //order data channel by channel rather than pixel by pixel: from rgbrgbrgb to rrrgggbbb 
  std::vector<float32_t> vectorImageR((float32_t*)channel[0].data,(float32_t*)channel[0].data+channel[0].rows*channel[0].cols);
  std::vector<float32_t> vectorImageG((float32_t*)channel[1].data,(float32_t*)channel[1].data+channel[1].rows*channel[1].cols);
  std::vector<float32_t> vectorImageB((float32_t*)channel[2].data,(float32_t*)channel[2].data+channel[2].rows*channel[2].cols);
  vectorImageR.insert(vectorImageR.end(),vectorImageG.begin(),vectorImageG.end());
  vectorImageR.insert(vectorImageR.end(),vectorImageB.begin(),vectorImageB.end());
  
  memcpy(m_input_tensor->data.f,vectorImageR.data() , INPUT_IMAGE_SIZE *
                                          INPUT_IMAGE_SIZE*sizeof(float)*3);
  try 
  {
    // run the inference
    if (TfLiteInterpreterInvoke(m_interpreter) !=kTfLiteOk)
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      std::ostringstream msg;
      throw std::out_of_range(" Error invoking the face detection model ");

    }
  }
  catch (const std::exception &e) 
  {
    CppLogger::getInstance()->error(e.what());
    std::exit(EXIT_FAILURE);
  }
  //Get output data
  float *cls_output_rawdata = m_outputs_cls->data.f;
  size_t cls_output_rawdata_size = m_outputs_cls->bytes/sizeof(float);
  float *x_output_rawdata = m_outputs_x->data.f;
  size_t x_output_rawdata_size = m_outputs_x->bytes/sizeof(float);
  float *y_output_rawdata = m_outputs_y->data.f;
  size_t y_output_rawdata_size = m_outputs_y->bytes/sizeof(float);
  float *nb_x_output_rawdata = m_outputs_nb_x->data.f;
  size_t nb_x_output_rawdata_size = m_outputs_nb_x->bytes/sizeof(float);
  float *nb_y_output_rawdata = m_outputs_nb_y->data.f;
  size_t nb_y_output_rawdata_size = m_outputs_nb_y->bytes/sizeof(float);
  int16_t  tmp_height, tmp_width, num_regions;

  // calculate number of regions. note! there is no information if the x number of region and y number of regions are similar
  // currently image size should  be divisible by netstride
  tmp_height = INPUT_IMAGE_SIZE/NETWORK_STRIDE;
  tmp_width = INPUT_IMAGE_SIZE/NETWORK_STRIDE;
  num_regions = tmp_height*tmp_width;

  std::vector<int16_t> max_ids;
  std::vector<float32_t> max_cls, ld_x, ld_y ;
  std::vector<std::vector<float32_t>> ld_nb_x, ld_nb_y;

  int16_t max_id; 
  float32_t max_cl;
  // for each landmark get the region with the maximum classification and its id
  for(int16_t ld=0;ld<NUM_LD;ld++)
  {
    max_id = 0;
    max_cl = 0;
    for(int16_t k=0; k<num_regions;k++)
    {
      float32_t val = cls_output_rawdata[ld*num_regions+k];
      if(val>max_cl)
      {
        max_cl =val;
        max_id = k;
      }
    }
    max_ids.push_back(max_id);
    max_cls.push_back(max_cl);
  }

  // for each landmark get the associated maximum x y values for the landmarks and neighbours 
  float32_t x_selected, y_selected;
  int16_t count = 0;
  for (int16_t max_id : max_ids)
  {
    std::vector<float32_t> x_selected_neighbours, y_selected_neighbours;
    x_selected = x_output_rawdata[max_id+count*num_regions];
    y_selected = y_output_rawdata[max_id+count*num_regions];
    for(int16_t neighbour = 0; neighbour<NUM_NB;neighbour++)
    {
      x_selected_neighbours.push_back(nb_x_output_rawdata[max_id+count*num_regions*NUM_NB+neighbour*num_regions]);
      y_selected_neighbours.push_back(nb_y_output_rawdata[max_id+count*num_regions*NUM_NB+neighbour*num_regions]);
    }
    ld_x.push_back(x_selected);
    ld_y.push_back(y_selected);
    ld_nb_x.push_back(x_selected_neighbours);
    ld_nb_y.push_back(y_selected_neighbours);
    count++;
  }

  // add the top left coordinates of each region to the landmarks and neighbours
  //the maximum and minimum is added as an insurance to not get negative values, doesn't exist on the post processing part
  count = 0;
  float32_t image_size_per_stride = (float32_t)(INPUT_IMAGE_SIZE/NETWORK_STRIDE);
  for (int16_t max_id : max_ids)
  {
    ld_x[count] = (ld_x[count] + (float32_t)(max_id%tmp_width))/(image_size_per_stride);
    ld_y[count] = (ld_y[count] + ((float32_t)round(max_id/tmp_width)))/(image_size_per_stride);
    for(int16_t neighbour=0;neighbour<NUM_NB;neighbour++)
    {
      ld_nb_x[count][neighbour] = (ld_nb_x[count][neighbour] + (float32_t)(max_id%tmp_width))/(image_size_per_stride);
      ld_nb_y[count][neighbour] = (ld_nb_y[count][neighbour] + ((float32_t)round(max_id/tmp_width)))/(image_size_per_stride);
      ld_nb_x[count][neighbour] =  std::max<float32_t>(ld_nb_x[count][neighbour],0);
      ld_nb_y[count][neighbour] =  std::max<float32_t>(ld_nb_y[count][neighbour],0);
      ld_nb_x[count][neighbour] =  std::min<float32_t>(ld_nb_x[count][neighbour],1);
      ld_nb_y[count][neighbour] =  std::min<float32_t>(ld_nb_y[count][neighbour],1);
    }
    ld_x[count] =  std::max<float32_t>(ld_x[count],0);
    ld_y[count] =  std::max<float32_t>(ld_y[count],0);
    ld_x[count] =  std::min<float32_t>(ld_x[count],1);
    ld_y[count] =  std::min<float32_t>(ld_y[count],1);
    count++;
  }  

  // use the values generated from the meanface function(need to investigate deeper its role) 
  // and calculate the mean of all the values
  std::vector<float32_t> ld_mean_x, ld_mean_y;
  for(int16_t count = 0; count<NUM_LD;count++)
  {
    float32_t mean_x = ld_x[count];
    float32_t mean_y = ld_y[count];
    for(int16_t neighbour = 0; neighbour<MAX_LEN;neighbour++)
    {
      mean_x += ld_nb_x[REVERSE_INDEX1[count*MAX_LEN+neighbour]][REVERSE_INDEX2[count*MAX_LEN+neighbour]];
      mean_y += ld_nb_y[REVERSE_INDEX1[count*MAX_LEN+neighbour]][REVERSE_INDEX2[count*MAX_LEN+neighbour]];
    }
    mean_x/=NUM_LD;
    mean_y/=NUM_LD;
    ld_mean_x.push_back(mean_x);
    ld_mean_y.push_back(mean_y);
  }

  if(USE_NEIGHBOURS)
  {
    ld_x = ld_mean_x;
    ld_y = ld_mean_y;
  }
  
  // multiply the landmark by the width of the image and add th top left corner of the bbox
  count = 0;
  
  bool_t left_eyelid_valid = true;
  bool_t right_eyelid_valid = true;

  // Hard coded
  if ((ld_y[HETSDefines::IDX_LEFTEYE_UPPER_APEX]<PIPNetFacialLandmarkDetector::THRESHOLD_Y_QUALITY &&
       ld_y[HETSDefines::IDX_LEFTEYE_LOWER_APEX]<PIPNetFacialLandmarkDetector::THRESHOLD_Y_QUALITY)||
      (ld_x[HETSDefines::IDX_LEFTEYE_UPPER_APEX]>PIPNetFacialLandmarkDetector::THRESHOLD_X_QUALITY &&
       ld_x[HETSDefines::IDX_LEFTEYE_LOWER_APEX]>PIPNetFacialLandmarkDetector::THRESHOLD_X_QUALITY))
  {
    left_eyelid_valid = false;
  }

  // Hard coded
  if ((ld_y[HETSDefines::IDX_RIGHTEYE_UPPER_APEX]<PIPNetFacialLandmarkDetector::THRESHOLD_Y_QUALITY &&
       ld_y[HETSDefines::IDX_RIGHTEYE_LOWER_APEX]<PIPNetFacialLandmarkDetector::THRESHOLD_Y_QUALITY)||
      (ld_x[HETSDefines::IDX_RIGHTEYE_UPPER_APEX]>PIPNetFacialLandmarkDetector::THRESHOLD_X_QUALITY &&
       ld_x[HETSDefines::IDX_RIGHTEYE_LOWER_APEX]>PIPNetFacialLandmarkDetector::THRESHOLD_X_QUALITY))
  {
    right_eyelid_valid = false;
  }  
  
  for (int16_t max_id : max_ids)
  {
    ld_x[count] = det_x1 + ld_x[count] * det_w;
    ld_y[count] = det_y1 + ld_y[count] * det_h;
    count++;
  }



  //Fulfill output structure
  facial_landmarks->face_tracked =false;
  facial_landmarks->tracking_quality=0.0f;
  facial_landmarks->facial_roi =0;
  bool is_empty = true;
  for(int i=0;i<ld_x.size();i++)
  {
    facial_landmarks->facial_landmarks_2d[i][0] = floor(ld_x[i]);
    facial_landmarks->facial_landmarks_2d[i][1] = floor(ld_y[i]);
    facial_landmarks->facial_landmark_quality[i] = (max_cls[i]>1.0f) ? 1.0f : max_cls[i];

    is_empty = false;
  }
  if(right_eyelid_valid == false){
    facial_landmarks->facial_landmark_quality[HETSDefines::IDX_RIGHTEYE_UPPER_APEX] = 0;
    facial_landmarks->facial_landmark_quality[HETSDefines::IDX_RIGHTEYE_LOWER_APEX] = 0;
  }
  if(left_eyelid_valid == false){
    facial_landmarks->facial_landmark_quality[HETSDefines::IDX_LEFTEYE_UPPER_APEX] = 0;
    facial_landmarks->facial_landmark_quality[HETSDefines::IDX_LEFTEYE_LOWER_APEX] = 0;
  }
  if (is_empty == true) 
  {
    for(int i=0;i<NUM_LD;i++)
    {
      facial_landmarks->facial_landmarks_2d[i][0] = -1;
      facial_landmarks->facial_landmarks_2d[i][1] = -1;
      facial_landmarks->facial_landmark_quality[i] = 0;
    }
    facial_landmarks->facial_landmark_validity = false;
  }
  else
  {
    facial_landmarks->facial_landmark_validity = true;
  }
}



