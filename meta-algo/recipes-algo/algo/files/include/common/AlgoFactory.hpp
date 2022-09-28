#ifndef _ALGO_CONTAINER_H
#define _ALGO_CONTAINER_H

#include "common/HETCoreDataStructure.hpp"
#include "lib/face_detection/CocoveiaFaceDetector.hpp"
#include "lib/faciallandmark_detection/PIPNetFacialLandmarkDetector.hpp"
#include "lib/headpose_estimation/PnPHeadPoseEstimator.hpp"
#include "lib/eye_opening_detection/FLMEyeOpeningDetector.hpp"

#include <memory>


class AlgoFactory
{
public:

  /**
   * @brief Get the Instance object
   * 
   * @return AlgoFactory* 
   */
  static AlgoFactory* get_instance()
  {
    static AlgoFactory instance;
    return &instance;
  }

  //delete these methods to make sure no copies appear 
  AlgoFactory(AlgoFactory const&)=delete;
  AlgoFactory& operator=(AlgoFactory const&) =delete;
  
  /**
   * @brief Get the Instance object
   * 
   * @return AlgoFactory* 
   */ 
  void init();
  
  /**
   * @brief Get the face detector algorithm 
   * 
   * @return std::shared_ptr<FaceDetection> 
   */
  std::shared_ptr<FaceDetection> get_face_detector();
  
   /**
   * @brief Get the facial landmark detector algorithm
   * 
   * @return std::shared_ptr<FacialLandmarkDetector>
   */
  std::shared_ptr<FacialLandmarkDetector> get_facial_landmark_detector();
  
   /**
   * @brief Get the head pose estimator algorithm
   * 
   * @return std::shared_ptr<HeadPoseEstimator>
   */
  std::shared_ptr<HeadPoseEstimator> get_head_pose_estimator();

   /**
   * @brief Get the eye opening detection algorithm
   * 
   * @return std::shared_ptr<EyeOpeningDetector>
   */
  std::shared_ptr<HETS::EyeOpeningDetector> get_eye_opening_detector();  

private: 

  /**
   * @brief private constructor
   * 
   */
  AlgoFactory();
  
  /**
   * @brief private destructor 
   * 
   */
  ~AlgoFactory();

  std::shared_ptr<CocoveiaFaceDetector> m_face_detector; //! > Cocoveia Face Detector
  std::shared_ptr<PIPNetFacialLandmarkDetector> m_facial_landmark_detector; //! > PIPNet Facial Landmark Detector
  std::shared_ptr<PnPHeadPoseEstimator> m_head_pose_estimator; //! > Pnp headpose estimator
  std::shared_ptr<HETS::EyeOpeningDetector> m_eye_opening_detector{}; //! > Eye Opening Detector
  bool_t m_is_algo_initialized{false}; //! > Is algo initialized
};

#endif 