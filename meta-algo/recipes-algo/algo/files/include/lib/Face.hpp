
/*!   Face.hpp file declare Face class to provide
**    face detection output
!*/

#ifndef _FACE_HPP
#define _FACE_HPP

#include <stdint.h>

typedef float float32_t;

class Face {

  /*! set image ,BBox coordinates and landmarks
  !*/

private:
  struct FaceOutput {
    bool valid;
    float32_t face_bbox[4];
    int32_t orientation;
  };
  uint8_t *p_output_image; // input image
  int32_t *p_landmarks;    // outputlandmarks

public:
  Face();

  // getter to get image
  uint8_t get_image();

  // get coordinates
  float32_t get_coordinate();

  // setter to set coordinates
  void set_coordinates(FaceOutput *face_output);

  // set image
  void set_image(uint32_t *image);

  // crop image
  void clip_face_image(int value);

  uint32_t * get_landmarks();

  int32_t * get_resolution();

  ~Face();
};
#endif // !1