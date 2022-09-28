#ifndef _RAW_IMAGE_READER_H
#define _RAW_IMAGE_READER_H

#include <cstdint>
#include <fstream>
#include <iostream>


class RawImageReader 
{
public :
    /**
     * @brief check input file extension
     * 
     * @param video_path[in] : input file path
     * @return std::string[out] : checked input file path
     */
    static std::string check_file_extension(std::string &video_path);

    /**
     * @brief read input data file
     * 
     * @param f[in] : stream object
     * @param current_frame[in] : framenumber 
     * @param width[in] : image width 
     * @param height[in] : image height 
     * @param image[out] : image buffer
     */
    static void reading_input_file(std::fstream &f, uint64_t current_frame,
                            uint16_t width, uint16_t height, uint8_t* image);

    /**
     * @brief check whether the file is valid or not
     * 
     * @param f[in] : stream object
     * @param width[in] : image width 
     * @param height[in] : image height 
     * @return uint64_t [out] : number of frames contained in the recording
     */
    static uint64_t check_input_file(std::fstream &f, uint16_t width, uint16_t height);

};

#endif