#ifndef _HEADPOSEESTIMATORDEFINITIONS_H
#define _HEADPOSEESTIMATORDEFINITIONS_H

#include "common/HETCoreTypes.hpp"

namespace HPEDefines
{
  /**
   * @brief Static expressions for used landmark indexes
   */
  static constexpr const uint8_t IDX_NOSE_BOTTOM = 10;
  static constexpr const uint8_t IDX_NOSE_TOP = 11;
  static constexpr const uint8_t IDX_LEFTEYE_LEFT_CORNER = 1;
  static constexpr const uint8_t IDX_RIGHTEYE_RIGHT_CORNER = 3;
  static constexpr const uint8_t IDX_MOUTH_LEFT_CORNER = 9;
  static constexpr const uint8_t IDX_MOUTH_RIGHT_CORNER = 8;

  /**
   * @brief Minimum quality threshold for the Face Landmark Quality.
   * If a landmark has lower quality than this threshold, HPE might not be able to compute a meaningful pose
   */
  static constexpr const float32_t FL_QUALITY_THRESHOLD = 0.25;

  /**
   * @brief Minimum/Maximum allowed x,y,z translations in meter.
   * If the translation is not within this range, the HPE output is most likely not sensible
   */
  static constexpr const float32_t X_BOUNDARY_MIN = -5.0f;
  static constexpr const float32_t X_BOUNDARY_MAX = 5.0f;
  static constexpr const float32_t Y_BOUNDARY_MIN = -5.0f;
  static constexpr const float32_t Y_BOUNDARY_MAX = 5.0f;
  static constexpr const float32_t Z_BOUNDARY_MIN = 0.0f;
  static constexpr const float32_t Z_BOUNDARY_MAX = 5.0f;
  
  /**
   * @brief Minimum/Maximum allowed yaw,pitch,roll rotations in degrees.
   * If the rotation is not within this range, the HPE output is most likely not sensible
   */
  static constexpr const float32_t YAW_BOUNDARY_MIN = -90.0f;
  static constexpr const float32_t YAW_BOUNDARY_MAX = 90.0f;
  static constexpr const float32_t PITCH_BOUNDARY_MIN = -40.0f;
  static constexpr const float32_t PITCH_BOUNDARY_MAX = 50.0f;
  static constexpr const float32_t ROLL_BOUNDARY_MIN = -45.0f;
  static constexpr const float32_t ROLL_BOUNDARY_MAX = 45.0f;
}
#endif // !