#ifndef _METRICS_H
#define _METRICS_H

#include "common/HETCoreTypes.hpp"

class Metrics
{
public : 
    /**
     * @brief calculate intersection over union 
     * 
     * @param box1[in] : bounding box 1 with x,y,w,h
     * @param box2[in] : bounding box 2 with x,y,w,h
     * @return float32_t[out] : iou between the 2 bounding boxes 
     */
    static float32_t compute_IoU( float32_t *box1,  float32_t *box2);

};

#endif