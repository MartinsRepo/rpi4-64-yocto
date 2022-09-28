#include "common/ImageProcessing.hpp"

cv::Mat ImageProcessing::image_normalization(cv::Mat scaledImageMat) 
{
  double min, max;
  cv::minMaxLoc(scaledImageMat, &min, &max);
  cv::Mat normalized_image_mat;
  //normalized image
  scaledImageMat.convertTo(normalized_image_mat, CV_32FC1);
  normalized_image_mat -= min;
  normalized_image_mat *= 1.0 / max;

  return normalized_image_mat;
}   

cv::Mat ImageProcessing::image_normalization(cv::Mat image_mat, std::vector<float32_t> std, std::vector<float32_t> mean){
  image_mat.convertTo(image_mat, CV_32FC3);cv::Scalar std_scalar(std[0]*255,std[1]*255,std[2]*255);
  cv::Scalar mean_scalar(mean[0]*255,mean[1]*255,mean[2]*255);
  cv::subtract(image_mat,mean_scalar,image_mat);
  cv::divide(image_mat,std_scalar,image_mat);

  return image_mat;
}

cv::Mat ImageProcessing::crop_image(cv::Mat input_image_mat,
                int16_t x1, int16_t x2, int16_t y1, int16_t y2) {
  cv::Mat cropped_image = input_image_mat(cv::Range(y1,y2),
                                    cv::Range(x1,x2));
  return cropped_image;
}