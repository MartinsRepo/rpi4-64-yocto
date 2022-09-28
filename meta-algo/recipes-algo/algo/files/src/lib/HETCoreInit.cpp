#include "lib/HETCoreInit.hpp"

#include "common/AlgoFactory.hpp"
#include "common/CppLogger.hpp"

void init_het_core ()
{
  //Init Algo Factory
  AlgoFactory::get_instance()->init();
  
  //Init Cpp Logger with default init
  CppLogger::getInstance()->default_init();
  CppLogger::getInstance()->init_writters();
}

void init_het_core (init_config config)
{
  //Init Algo Factory
  AlgoFactory::get_instance()->init();

  //Init Cpp Logger
  CppLogger::getInstance()->init(config.log_config);
  CppLogger::getInstance()->init_writters();
}