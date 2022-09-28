// Library Include
#include "lib/HETDModule.hpp"
#include "lib/HETModule.hpp"
#include "lib/HETSModule.hpp"
#include "lib/HETCoreVersion.hpp"
#include "lib/HETCoreInit.hpp"

// App Include 
#include "app/CSVOutputWritter.hpp"
#include "app/RawImageReader.hpp"

// C++ Include 
#include <dirent.h>
#include <fstream>
#include <memory>
#include <string>
#include <errno.h>
#include <chrono>
#include <iostream>
#include <cstring>

int main(int argc, char *argv[]) 
{

  //start time
  auto start = std::chrono::high_resolution_clock::now();

  //Print project and version name
  uint8_t het_core_version[3]{};
  get_het_core_version(het_core_version);
  std::string version_number =std::to_string(het_core_version[0])+"."+std::to_string(het_core_version[1])+"."+std::to_string(het_core_version[2]);
  std::cout<<"Project name : dmap-het-core"<<" version : " <<version_number<<std::endl;
  
  int16_t return_code = 0;
  
  // Default argument values
  std::string video_path = "";
  int16_t frame_rate = 35;
  uint16_t img_width = 1600;
  uint16_t img_height = 1301;
  bool_t are_arguments_valid = true;

  // We do require matching parameter sets for each argument. So the number or arguments is always uneven
  if (((argc % 2) == 0) || (argc < 2))
  {
    are_arguments_valid = false;
  }
  else
  {
    // Cycle through all provided arguments
    for (int16_t i = 1; i < argc; (i+=2)) 
    {
      std::string arg_name = argv[i];
      std::string arg_value = argv[i+1];

      if (arg_name == "--input") 
      {     
        video_path = arg_value; 
        
        if (video_path == "") 
        {
          std::cerr << "--input argument value invalid!" << std::endl;
          are_arguments_valid = false;
        }
      }

      if (arg_name == "--fps") 
      {
        frame_rate = std::stoi(arg_value);   
        if (frame_rate < 1) 
        {
          std::cerr << "--fps argument value invalid!" << std::endl;
          are_arguments_valid = false;
        }
      }

      if (arg_name == "--resolution") 
      {
          std::string separator = "x";
          auto sep_pos = arg_value.find(separator);
          if(sep_pos != std::string::npos)
          {
            std::string width_string = arg_value.substr(0, sep_pos);
            std::string height_string = arg_value.substr((sep_pos +1), arg_value.size());
            img_width = std::stoi(width_string);
            img_height = std::stoi(height_string);
          }
          else
          {
            std::cerr << "--resolution argument value invalid, required format: WIDTHxHEIGHT!" << std::endl;
            are_arguments_valid = false;
          }
      }
    }    
  }

  // App cannot continue if not at least an input file was specified. Also abort, if an optional parameter was specified, but had a bad value
  if((are_arguments_valid == 0) || (video_path == ""))
  {
    std::cout << "Usage: " << argv[0] << " --input INPUT_VIDEO_PATH\nOptional Arguments:\n--resolution WIDTHxHEIGHT\n--fps FPS" << std::endl;
    return 1;
  }

  std::cout<<"Input file: " << video_path << "\nResolution: " << img_width << "x" << img_height << "\nFramerate: " << frame_rate << " fps" << std::endl;
  
  //Create csv output file 
  std::ofstream bbox_csv; 

  std::string video_name = video_path.substr(video_path.find_last_of("/") + 1);
  std::string csv_name =
      video_name.substr(0, video_name.find_last_of(".")) + "_bbox.csv";

  CSVOutputWritter::create_output_csv(bbox_csv ,csv_name);

  //input image size
  int size = img_width * img_height;

  //init the img buffer
  uint8_t* image = new uint8_t ();
  image = (uint8_t*)malloc(img_width*img_height*sizeof(uint8_t));

  //frame count and timestamp 
  uint64_t frame_nb = 0;
  uint64_t nb_landmarks_run = 0;
  uint64_t nb_headpose_run =0;
  uint64_t nb_eye_opening_run =0;
  uint64_t timestamp =0;

  //Init timer for framerate calculation
  uint64_t find_face_time = 0;
  uint64_t detect_landmark_time = 0;
  uint64_t find_headpose_time = 0;
  uint64_t find_eyeopening_time = 0;

  //Camera Intrinsic parameters 
  intrinsic_camera_parameters intrinsic_cam_param = 
  {
    1980.235250,
    1980.39866,
    793.564247,
    634.515820,
    0,
    {-0.347872,0.230107,0,0,0}
  };

  //init the library algorithms
  auto start_init = std::chrono::high_resolution_clock::now();

  init_config config;
  config.log_config.log_level = LogLevel::LOG_LEVEL_TRACE;
  config.log_config.log_type =LogType::FILE_LOG;
  config.log_config.log_file_name="DMAP_log_file.log";
  init_het_core(config);

  auto end_init = std::chrono::high_resolution_clock::now();
  auto duration_init =
  std::chrono::duration_cast<std::chrono::milliseconds>(end_init - start_init);

  //Display time and framerate 
  std::cout<<" ----->  Init execution time: " << duration_init.count() << "ms"<<std::endl;

  //validity threshold for the landmark
  const float32_t landmark_validity_threshold= 0.3;

  try 
  {
    std::cout << " ----->  Reading file: " << video_path.c_str() <<std::endl;

    //check whether the input file extension is valid or not
    std::cout <<" ----->  Checking whether the file extension is valid or not "<<std::endl;
    video_path = RawImageReader::check_file_extension(video_path);

    //read input file
    std::fstream f(video_path.c_str());

    //check for exception 
    std::cout <<" ----->  Checking whether the input file is valid or not"<<std::endl;
    uint64_t num_frames = RawImageReader::check_input_file(f,img_width,img_height);

    {
      std::cout<<" ----->  Performing detection on input dataset \n";

      while (f.good() && num_frames!=frame_nb) 
      {

        if(frame_nb % 100 == 0 )
        {
          std::cout<<" ----->  Processing frame : " << frame_nb << " to " << frame_nb+100 << std::endl;
        }
        // creating image
        RawImageReader::reading_input_file(f, frame_nb, img_width, img_height,image);


        //get input images rows and columns
        resolution_data image_res = {
          .image_rows = (int16_t)img_height, // image_rows
          .image_cols = (int16_t)img_width  // image_cols
        };

        //Init feature output structures
        facedetection_output out_hetd_fd=
        {
          false,
          {0.0,0.0,0.0,0.0}
        };

        flm_output output_het_flm= 
        {         
          false,
          0,
          0,
          false,
          {0,0,0,0,0,0,0,0,0,0,0,0},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
        };
        
        pose_output output_het_pose_cam =
        {
          false,
          0.0,
          {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
          {0.0,0.0,0.0}
        };

        eye_flm_output output_eyelid_flm_data{};
        eyelid_output  output_eyelid_data{};
        blink_output   output_blink_data{};
        std::memset(&output_eyelid_flm_data, 0, sizeof (output_eyelid_flm_data));
        std::memset(&output_eyelid_data,     0, sizeof (output_eyelid_data));
        std::memset(&output_blink_data,      0, sizeof (output_blink_data));

        try 
        {

          //Find face in input images
          //start time
          auto start_find_face = std::chrono::high_resolution_clock::now();
          hetd_find_face(image, image_res, &out_hetd_fd);
          return_code = 1;
          auto end_find_face = std::chrono::high_resolution_clock::now();
          find_face_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_find_face - start_find_face).count();


          //Check whether the face is detected or not
          if (out_hetd_fd.valid) 
          {
            //detect the landmarks
            //start time
            auto start_detect_landmark = std::chrono::high_resolution_clock::now();
            het_find_landmark(image, image_res,out_hetd_fd,&output_het_flm);
            auto end_detect_landmark = std::chrono::high_resolution_clock::now();
            detect_landmark_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_detect_landmark - start_detect_landmark).count();
            nb_landmarks_run++;
          } 
          //Check if the face and the landmarks are detected
          if (out_hetd_fd.valid && output_het_flm.facial_landmark_validity)
          {
            //Run Head Pose Estimation
            //start time
            auto start_find_headpose = std::chrono::high_resolution_clock::now();
            het_find_headposition(intrinsic_cam_param,image_res,output_het_flm,&output_het_pose_cam); 
            auto end_find_headpose = std::chrono::high_resolution_clock::now();
            find_headpose_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_find_headpose - start_find_headpose).count();
            nb_headpose_run++;
          }

          //Check if the face and the landmarks are detected
          if (out_hetd_fd.valid && output_het_flm.facial_landmark_validity)
          {
            //Run Eye Aperture Detection
            //start time
            auto start_eye_opening = std::chrono::high_resolution_clock::now();
            HETS::het_find_eyeopening(image, image_res, out_hetd_fd, output_het_flm, output_het_pose_cam, start_eye_opening.time_since_epoch().count(),
                                      output_eyelid_flm_data, output_eyelid_data, output_blink_data); 
            auto end_eye_opening = std::chrono::high_resolution_clock::now();
            find_eyeopening_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_eye_opening - start_eye_opening).count();
            nb_eye_opening_run++;
            }
          
          //check whether the output csv file exists or not
          // if not create a output csv file
          CSVOutputWritter::check_ouput_file(bbox_csv, csv_name);

          //timestamp calculation in ms
          timestamp = (frame_nb*1000)/frame_rate;

          //save output in the bbox_csv
          bbox_csv << version_number << ";" << frame_nb << ";"<< timestamp << ";" << frame_rate << ";"
                  << return_code << ";" << out_hetd_fd.valid << ";"
                  << out_hetd_fd.face_rect[0] << ";" << out_hetd_fd.face_rect[1] << ";"
                  << out_hetd_fd.face_rect[2] << ";" << out_hetd_fd.face_rect[3]<< ";" << output_het_flm.facial_landmark_validity;
          int16_t landmark_validity = 0;
          for(int k=0;k<12;k++)
          {
            if(output_het_flm.facial_landmark_quality[k]> landmark_validity_threshold)
            {
              landmark_validity=1;
            }
            bbox_csv << ";"<<output_het_flm.facial_landmark_quality[k] << ";"<<landmark_validity<<";"<< output_het_flm.facial_landmarks_2d[k][0]<<";"<< output_het_flm.facial_landmarks_2d[k][1];
          }
          bbox_csv << ";"<< output_het_pose_cam.head_pose_valid<<";"<< output_het_pose_cam.head_pose_quality<<";"
          <<output_het_pose_cam.T[0] <<";"<<output_het_pose_cam.T[1] <<";"<< output_het_pose_cam.T[2]<<";"
          <<output_het_pose_cam.euler_angles[0]<<";"<<output_het_pose_cam.euler_angles[1] <<";"<<output_het_pose_cam.euler_angles[2] <<";"
          <<output_het_pose_cam.T_vcs[0] <<";"<<output_het_pose_cam.T_vcs[1] <<";"<< output_het_pose_cam.T_vcs[2]<<";"
          <<output_het_pose_cam.euler_angles_vcs[0]<<";"<<output_het_pose_cam.euler_angles_vcs[1] <<";"<<output_het_pose_cam.euler_angles_vcs[2];

          //Add Eye Opening Output to CSV
          bbox_csv << ";"<< std::to_string(output_eyelid_data.is_eye_opening_valid[0]) << ";" << output_eyelid_data.eye_opening_mm[0]
                   << ";"<< std::to_string(output_eyelid_data.is_eye_opening_valid[1]) << ";" << output_eyelid_data.eye_opening_mm[1];
          bbox_csv << std::endl;
        } 
        catch (...) 
        {
          //if no face detected by the detector
          std::cout<<" ----->  Error occured ... check log file for more info"<<std::endl;
        }
        frame_nb++;
      }
    }
    //close file
    std::cout<<" ----->  Detection Done on input data file "<<std::endl;
    f.close();
    free(image);
  }
  catch (const char *cstr) 
  {
    std::cerr << cstr << std::endl;
    exit(1);
  }

  //close bbox_csv file
  std::cout<<" ----->  Done saving face detection output in CSV file"<<std::endl;
  bbox_csv.close();

  //end time
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  //Display time and framerate 
  std::cout<<" ----->  Total execution time: " << duration.count() << "ms"<<std::endl;
  if (duration.count()>0)
  {
    std::cout<<" ----->  Framerate: " << (1000 * frame_nb) / duration.count()<<std::endl;
  }
  else
  {
    std::cout<<" ----->  Framerate: Not computable." <<std::endl; 
  }
  std::cout<<" ----->  Total face detection time: " << find_face_time << "ms for " << frame_nb << " frames"<<std::endl;
  if (find_face_time>0)
  {
    std::cout<<" ----->  Framerate: " << (1000 * frame_nb) / find_face_time<<std::endl;
  }
  else
  {
    std::cout<<" ----->  Framerate: Not computable." <<std::endl; 
  }
  std::cout<<" ----->  Total landmark detection time: " << detect_landmark_time << "ms for " << nb_landmarks_run << " frames"<<std::endl;
  if (detect_landmark_time>0)
  {
    std::cout<<" ----->  Framerate: " << (1000 * nb_landmarks_run) / detect_landmark_time<<std::endl;
  }
  else
  {
    std::cout<<" ----->  Framerate: Not computable." <<std::endl; 
  }
  std::cout<<" ----->  Total head pose estimation time: " << find_headpose_time << "ms for " << nb_headpose_run << " frames"<<std::endl;
  if (find_headpose_time>0)
  {
    std::cout<<" ----->  Framerate: " << (1000 * nb_headpose_run) / find_headpose_time<<std::endl;
  }
  else
  {
    std::cout<<" ----->  Framerate: Not computable." <<std::endl; 
  }
  std::cout<<" ----->  Total eye opening and blink detection time: " << find_eyeopening_time << "ms for " << nb_eye_opening_run << " frames"<<std::endl;
  if (find_eyeopening_time>0)
  {
    std::cout<<" ----->  Framerate: " << (1000 * nb_eye_opening_run) / find_eyeopening_time<<std::endl;
  }
  else
  {
    std::cout<<" ----->  Framerate: Not computable." <<std::endl; 
  }  
  std::cout<<" ----->  Detection done on input dataset\n";
}