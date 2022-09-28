#ifndef _CSV_OUTPUT_WRITTER_H
#define _CSV_OUTPUT_WRITTER_H

#include <cstdint>
#include <fstream>
#include <iostream>

class CSVOutputWritter 
{
public : 
    /**
     * @brief Create a output csv file
     * 
     * @param csv_name[in,out]  : output csv file name
     */
    static void create_output_csv(std::ofstream &output_csv,const std::string& csv_name);


    /**
     * @brief check whetehr the output csv file exists or not
     *        if not exists create .
     * 
     * @param video_path[in] : input video path
     * @param csv_name[in,out] : output csv file name 
     */
    static void check_ouput_file(std::ofstream &output_csv,const std::string& csv_name);

};

#endif