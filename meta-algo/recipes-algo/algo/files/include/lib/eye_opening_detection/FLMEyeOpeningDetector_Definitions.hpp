#ifndef _FLM_EYE_OPENING_DETECTOR_DEFINITIONS_H
#define _FLM_EYE_OPENING_DETECTOR_DEFINITIONS_H

#include "common/HETCoreDataStructure.hpp"

namespace HETSDefines
{
  // Static expressions for used landmark indexes.
  // TODO: Centralize these defines between HET, HETS and HPE so that there is no mismatch between the module indexes!
  static constexpr const uint8_t IDX_LEFTEYE_OUTER_CORNER = 1;
  static constexpr const uint8_t IDX_LEFTEYE_INNER_CORNER = 0;
  static constexpr const uint8_t IDX_LEFTEYE_UPPER_APEX = 4;
  static constexpr const uint8_t IDX_LEFTEYE_LOWER_APEX = 5;
  static constexpr const uint8_t IDX_RIGHTEYE_OUTER_CORNER = 3;
  static constexpr const uint8_t IDX_RIGHTEYE_INNER_CORNER = 2;
  static constexpr const uint8_t IDX_RIGHTEYE_UPPER_APEX = 6;
  static constexpr const uint8_t IDX_RIGHTEYE_LOWER_APEX = 7;
  static constexpr const uint8_t NUM_EYE_LM_IDX = NUM_EYE_LANDMARKS;

  static constexpr const uint8_t IDX_NOSE_BOTTOM = 10;
  static constexpr const uint8_t IDX_NOSE_TOP = 11;

  // Dummy thresholds for eye closure
  static constexpr const float32_t EAR_EYE_CLOSED_THRESHOLD = 0.2f;
  static constexpr const float32_t EAR_EYE_OPEN_THRESHOLD = 0.8f;

  //Dummy thresholds for FLM Quality invalidating eyelid data
  static constexpr const float32_t FLM_QUALITY_THRESHOLD = 0.1f;

  // Counterclockwise list of all landmark indexes of left eye, starting with outer eye corner
  static constexpr const uint8_t LEFT_EYE_INDEXES[NUM_EYE_LM_IDX] = {IDX_LEFTEYE_OUTER_CORNER, IDX_LEFTEYE_UPPER_APEX, IDX_LEFTEYE_INNER_CORNER, IDX_LEFTEYE_LOWER_APEX};

  // Clockwise list of all landmark indexes of right eye, starting with outer eye corner
  static constexpr const uint8_t RIGHT_EYE_INDEXES[NUM_EYE_LM_IDX] = {IDX_RIGHTEYE_OUTER_CORNER, IDX_RIGHTEYE_UPPER_APEX, IDX_RIGHTEYE_INNER_CORNER, IDX_RIGHTEYE_LOWER_APEX};  
}

#endif // !