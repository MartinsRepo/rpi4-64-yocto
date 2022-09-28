#ifndef _IMAGE_PROCESSING_H
#define _IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include "common/HETCoreTypes.hpp"

class ImageProcessing
{
public : 
    /**
     * @brief Normalize the image between 0 and 1 
     * 
     * @param scaledImageMat[in] : input image for model
     * @return cv::Mat[out] : image after applying pre-processing 
     */
    static cv::Mat image_normalization(cv::Mat scaledImageMat);

    /**
     * @brief Normalize the image between 0 and 1 with mean and std
     * 
     * @param image_mat[in] : input image to normalize
     * @param std[in] : standard deviation to use
     * @param mean[in] : mean of the data to use
     * @return cv::Mat[out] : image after applying normalization 
     */
    static cv::Mat image_normalization(cv::Mat image_mat, std::vector<float32_t> std, std::vector<float32_t> mean);

    /**
     * @brief crop input image
     * 
     * @param image_data[in] : input image data 
     * @param x1[in] : input top  left x coordinates
     * @param y1[in] : input top left y coordinate
     * @param w[in] : input width
     * @param h[in] : input height
     * @return uint8_t[out] :data of the cropped image
     */
    static cv::Mat crop_image(cv::Mat input_image_mat,
                      int16_t x1, int16_t x2, int16_t y1, int16_t y2
                        );

};

#endif