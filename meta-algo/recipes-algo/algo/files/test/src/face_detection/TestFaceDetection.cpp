#include <gtest/gtest.h>
#include "lib/face_detection/FaceDetection.hpp"
#include "app/RawImageReader.hpp"
#include "common/AlgoFactory.hpp"
#include "common/Metrics.hpp"

TEST(TestBase, test_face_detector_created)
{
	int res = 2;   //TBD
	int res_expected = 2;  //TBD
  uint8_t image_data[] = {0,0,0,0};
  facedetection_output face_detected;
  //call Cocoveia Face Detector
  std::shared_ptr<FaceDetection> detector=AlgoFactory::get_instance()->get_face_detector();

	ASSERT_EQ( res, res_expected );
}

TEST(TestBase, test_face_inference_on_raw_data)
{
  std::string video_path = "../../../res/test_file.raw";
  std::fstream f(video_path.c_str());

  uint16_t img_width =1600;
  uint16_t img_height =1301;
  facedetection_output face_detected;
  uint8_t* image = new uint8_t ();
  image = (uint8_t*)malloc(img_width*img_height*sizeof(uint8_t));
  RawImageReader::reading_input_file(f, 0, img_width, img_height,image);

  //call Cocoveia Face Detector
  std::shared_ptr<FaceDetection> pSC=AlgoFactory::get_instance()->get_face_detector();
	pSC->detect_face(image,img_height,img_width,&face_detected);
}

TEST(TestBase, test_face_detected)
{
  std::string video_path = "../../../res/test_file.raw";
  std::fstream f(video_path.c_str());

  uint16_t img_width =1600;
  uint16_t img_height =1301;
  facedetection_output face_detected;
  uint8_t* image = new uint8_t ();
  image = (uint8_t*)malloc(img_width*img_height*sizeof(uint8_t));
  RawImageReader::reading_input_file(f, 0, img_width, img_height,image);

  //call Cocoveia Face Detector
  std::shared_ptr<FaceDetection> pSC=AlgoFactory::get_instance()->get_face_detector();
	pSC->detect_face(image,img_height,img_width,&face_detected);

  ASSERT_EQ( face_detected.valid, true );
}

TEST(TestBase, test_intersection_over_union)
{
  std::string video_path = "../../../res/test_file.raw";
  std::fstream f(video_path.c_str());
  float32_t ground_truth[] ={545,688,405,406};
  uint16_t img_width =1600;
  uint16_t img_height =1301;
  facedetection_output face_detected;
  uint8_t* image = new uint8_t ();
  image = (uint8_t*)malloc(img_width*img_height*sizeof(uint8_t));
  RawImageReader::reading_input_file(f, 0, img_width, img_height,image);

  //call Cocoveia Face Detector
  std::shared_ptr<FaceDetection> pSC=AlgoFactory::get_instance()->get_face_detector();
	pSC->detect_face(image,img_height,img_width,&face_detected);

  float32_t iou = Metrics::compute_IoU(face_detected.face_rect, ground_truth); 

  ASSERT_GE( iou, 0.7 );
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}