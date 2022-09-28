#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "common/HETCoreDataStructure.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <errno.h>
#include <mutex>

/**
 * @brief Logger class
 * 
 */
class CppLogger {
public:

  /**
   * @brief Get the Instance object
   * 
   * @return CppLogger* 
   */
  static CppLogger* getInstance()
  {
    static CppLogger instance;
    return &instance;
  }

  //delete these methods to make sure no copies appear 
  CppLogger(CppLogger const&)=delete;
  CppLogger& operator=(CppLogger const&) =delete;


  /**
   * @brief Init the Cpp Logger with config 
   * 
   * @param log_config[in] : log config structure 
   */
  void init(logger_config log_config);
  
  /**
   * @brief Cpp Logger default init 
   * 
   */
  void default_init();
  
  /**
   * @brief Init the different writters used by the logger
   * 
   */
  void init_writters ();
  
  /**
   * @brief Interface for Error Log
   * 
   * @param text 
   */
  void error(const char *text) throw();
  void error(std::string &text) throw();
  void error(std::ostringstream &stream) throw();

  /**
   * @brief Interface for Alarm Log
   * 
   * @param text 
   */
  void alarm(const char *text) throw();
  void alarm(std::string &text) throw();
  void alarm(std::ostringstream &stream) throw();

  /**
   * @brief Interface for Always Log
   * 
   * @param text 
   */
  void always(const char *text) throw();
  void always(std::string &text) throw();
  void always(std::ostringstream &stream) throw();

  /**
   * @brief Interface for Info Log
   * 
   * @param text 
   */
  void info(const char *text) throw();
  void info(std::string &text) throw();
  void info(std::ostringstream &stream) throw();

  /**
   * @brief Interface for Trace log
   * 
   * @param text 
   */
  //void trace(boost::stacktrace::basic_stacktrace *text);
  void trace(const char *text) throw();
  void trace(std::string &text) throw();
  void trace(std::ostringstream &stream) throw();

  /**
   * @brief Interface for Debug log
   * 
   * @param text 
   */
  void debug(const char *text) throw();
  void debug(std::string &text) throw();
  void debug(std::ostringstream &stream) throw();

  // Error and Alarm log must be always enable
  // Hence, there is no interfce to control error and alarm logs
  
  /**
   * @brief Interface for Debug log
   * 
   * @return std::string : current time 
   */
  std::string getCurrentTime();
  
private:
  /**
   * @brief Log into file or console
   * 
   * @param data 
   */

  void loggingIntoFile(std::string &data);
  void loggingOnConsole(std::string &data);

  /**
   * @brief private constructor
   * 
   */
  CppLogger();
  
  /**
   * @brief private destructor 
   * 
   */
  ~CppLogger();
private:
  std::mutex m_mutex;  // mutex
  std::ofstream m_file;   // File Writter
  LogLevel m_log_level;    // Log Level
  LogType m_log_type;      // Log Type
  std::string m_log_file_name;  //Log file name 
  static const std::string LOG_FILE_NAME; //Default log file name 
};  

#endif // End of _LOGGER_H_
