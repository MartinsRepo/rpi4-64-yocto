#include "common/Metrics.hpp"
#include <algorithm>

float32_t Metrics::compute_IoU( float32_t *box1,  float32_t *box2){
  float32_t x_inter1, x_inter2, y_inter1, y_inter2, w_inter, h_inter, area_inter,s_inter,s_union,iou;
  x_inter1 = std::max<float32_t>(box1[0],box2[0]);
  y_inter1 = std::max<float32_t>(box1[1],box2[1]);
  x_inter2 = std::min<float32_t>(box1[0]+box1[2],box2[0]+box2[2]);
  y_inter2 = std::min<float32_t>(box1[1]+box1[3],box2[1]+box2[3]);
  w_inter = std::abs(x_inter2-x_inter1);
  h_inter = std::abs(y_inter2-y_inter1);
  s_inter = w_inter*h_inter;
  s_union = box1[2]*box1[3]+ box2[2]*box2[3]-s_inter;
  iou = s_inter/s_union;
  return iou;
}