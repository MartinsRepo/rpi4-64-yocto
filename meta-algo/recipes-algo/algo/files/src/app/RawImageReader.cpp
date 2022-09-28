#include "app/RawImageReader.hpp"

uint64_t RawImageReader::check_input_file(std::fstream &f, uint16_t width, uint16_t height) {
  try 
  {
    if (!f)
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      throw std::ios::failure(" Error opening file! ");
    }
    if (f.peek() == EOF)
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      throw std::ios::failure(" Bad Input : No image found in file ");
    }
    uint64_t img_size = static_cast<uint64_t> (width*height*sizeof(uint8_t));
    f.seekg(0,f.end);
    size_t file_size = f.tellg();
    f.seekg(0, f.beg);
    if (static_cast<uint64_t>(file_size) %img_size !=0)
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      throw std::ios::failure(" Error file size not conguent to image resolution ");
    }
    return static_cast<uint64_t>(file_size) /img_size;

  } 
  catch (const std::exception &e) 
  {
    exit(1);
  }
}

void RawImageReader::reading_input_file(std::fstream &f, uint64_t current_frame,
                           uint16_t width, uint16_t height, uint8_t* image) 
{
  try 
  {
    if(current_frame<0)
    {
      throw std::ios::failure(" Error negative frame number ! ");
    }
    uint64_t img_size = width*height*sizeof(uint8_t);
    uint64_t next_read_pos = static_cast<uint64_t>(current_frame*img_size);
    f.seekg(next_read_pos);
    f.read((char*)(&image[0]),img_size);
    if(!f.good())
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      throw std::ios::failure(" Error coming from the frame reader ! ");
    }
    if (f.gcount()!=img_size)
    {
    std::cout<<" ----->  Error occured ... check log file for more info \n";
    throw std::ios::failure(" Error byte read different from the image size ! ");
    }
  }
  catch (const std::exception &e) 
  {
    exit(1);
  } 

}

std::string RawImageReader::check_file_extension(std::string &video_path) {
  try {

    if(video_path.substr(video_path.find_last_of(".") + 1) != "raw")
    {
      std::cout<<" ----->  Error occured ... check log file for more info \n";
      throw std::ios::failure(" Not a valid file extension ");
    }
    return video_path;
    
  } catch (const std::exception &e) 
  {
    exit(1);
  }
}