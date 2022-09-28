#ifndef _HET_CORE_TYPES_HPP
#define _HET_CORE_TYPES_HPP

#include <stdint.h>
#include <string>

typedef int8_t          bool_t;
typedef float           float32_t;
typedef double          float64_t;
typedef unsigned char   byte_t;


// enum for LogLevel
typedef enum  {
  DISABLE_LOG = 0,
  LOG_LEVEL_INFO = 1,
  LOG_LEVEL_TRACE = 2,
  LOG_LEVEL_DEBUG = 3,
} LogLevel;

// enum for LogType
typedef enum  {
  NO_LOG = 0,
  CONSOLE = 1,
  FILE_LOG = 2,
} LogType;



#endif