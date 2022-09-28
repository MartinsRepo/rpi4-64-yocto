#include "lib/HETDModule.hpp"

#include "lib/face_detection/FaceDetection.hpp"
#include "common/AlgoFactory.hpp"

#include <algorithm>

void hetd_find_face(uint8_t *image_data, resolution_data image_res,
                       facedetection_output *output_ptr) 
{

  //call Cocoveia Face Detector
  std::shared_ptr<FaceDetection> detector=AlgoFactory::get_instance()->get_face_detector();

  //do inference on input image
  detector->detect_face(image_data, image_res.image_rows, image_res.image_cols,output_ptr);
}