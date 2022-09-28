#include "common/AlgoFactory.hpp"

AlgoFactory::AlgoFactory()
{

}
  
AlgoFactory::~AlgoFactory()
{

}

void AlgoFactory::init()
{
    if(!m_is_algo_initialized)
    {
        m_face_detector=std::make_shared<CocoveiaFaceDetector>();
        m_facial_landmark_detector=std::make_shared<PIPNetFacialLandmarkDetector>();
        m_head_pose_estimator=std::make_shared<PnPHeadPoseEstimator>();
        m_eye_opening_detector=std::make_shared<HETS::FLMEyeOpeningDetector>();
        m_is_algo_initialized=true;
    }
}

std::shared_ptr<FaceDetection> AlgoFactory::get_face_detector()
{   
    if(!m_is_algo_initialized)
    {
        init();
    }
    return m_face_detector;
}

std::shared_ptr<FacialLandmarkDetector> AlgoFactory::get_facial_landmark_detector()
{
    if(!m_is_algo_initialized)
    {
        init();
    }
    return m_facial_landmark_detector;
}

std::shared_ptr<HeadPoseEstimator> AlgoFactory::get_head_pose_estimator()
{
    if(!m_is_algo_initialized)
    {
        init();
    }
    return m_head_pose_estimator;
}

std::shared_ptr<HETS::EyeOpeningDetector> AlgoFactory::get_eye_opening_detector()
{
    if(!m_is_algo_initialized)
    {
        init();
    }
    return m_eye_opening_detector;
}
