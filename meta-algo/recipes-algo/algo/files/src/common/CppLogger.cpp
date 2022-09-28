#include <iostream>

#include "common/CppLogger.hpp"

#include <cstdlib>
#include <ctime>

// Log file name. File name should be change from here only
const std::string CppLogger::LOG_FILE_NAME = "DMAaPLogFile.log";

CppLogger::CppLogger()
{
  default_init();
}

CppLogger::~CppLogger() 
{ 
  if(m_file.is_open())
  {
    m_file.close(); 
  }
}

void CppLogger::default_init()
{
  m_log_level = LOG_LEVEL_TRACE;
  m_log_type = FILE_LOG ;//FILE_LOG; // CONSOLE ;
  m_log_file_name = LOG_FILE_NAME.c_str();
}

void CppLogger::init(logger_config log_config)
{
  m_log_level = log_config.log_level;
  m_log_type = log_config.log_type ;
  m_log_file_name = log_config.log_file_name;
}

void CppLogger::init_writters ()
{
  if (m_log_type==FILE_LOG && m_log_level >=DISABLE_LOG && !m_file.is_open())
  {  
    m_file.open(m_log_file_name, std::ios::out | std::ios::app);
  }
}

void CppLogger::loggingIntoFile(std::string &data) 
{
  if(m_file.is_open())
  {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_file << getCurrentTime() << "  " << data << std::endl;
  }
}

void CppLogger::loggingOnConsole(std::string &data) 
{
  std::unique_lock<std::mutex> lock{m_mutex};
  std::cout << getCurrentTime() << "  " << data << std::endl;
}

std::string CppLogger::getCurrentTime() 
{
  std::string currTime;
  // Current date/time based on current time
  time_t now = time(0);
  // Convert current time to string
  currTime.assign(ctime(&now));

  // Last charactor of currentTime is "\n", so remove it
  std::string currentTime = currTime.substr(0, currTime.size() - 1);
  return currentTime;
}

// Interface for Error Log
void CppLogger::error(const char *text) throw()
{
  std::string data;
  data.append("[ERROR]: ");
  data.append(text);

  // ERROR must be capture
  if (m_log_type == FILE_LOG) 
  {
    loggingIntoFile(data);
  } 
  else if (m_log_type == CONSOLE) 
  {
    loggingOnConsole(data);
  }
}

void CppLogger::error(std::string &text) throw() { error(text.data()); }

void CppLogger::error(std::ostringstream &stream) throw() 
{
  std::string text = stream.str();
  error(text.data());
}

// Interface for Alarm Log
void CppLogger::alarm(const char *text) throw() 
{
  std::string data;
  data.append("[ALARM]: ");
  data.append(text);

  // ALARM must be capture
  if (m_log_type == FILE_LOG) 
  {
    loggingIntoFile(data);
  } 
  else if (m_log_type == CONSOLE) 
  {
    loggingOnConsole(data);
  }
}

void CppLogger::alarm(std::string &text) throw() { alarm(text.data()); }

void CppLogger::alarm(std::ostringstream &stream) throw() 
{
  std::string text = stream.str();
  alarm(text.data());
}

// Interface for Always Log
void CppLogger::always(const char *text) throw()
{
  std::string data;
  data.append("[ALWAYS]: ");
  data.append(text);

  // No check for ALWAYS logs
  if (m_log_type == FILE_LOG)
  {
    loggingIntoFile(data);
  }
  else if (m_log_type == CONSOLE) 
  {
    loggingOnConsole(data);
  }
}

void CppLogger::always(std::string &text) throw() { always(text.data()); }

void CppLogger::always(std::ostringstream &stream) throw()
{
  std::string text = stream.str();
  always(text.data());
}

// Interface for Info Log
void CppLogger::info(const char *text) throw() 
{
  std::string data;
  data.append("[INFO]: ");
  data.append(text);

  //m_log_level = LOG_LEVEL_INFO;
  if ((m_log_type == FILE_LOG) && (m_log_level >= LOG_LEVEL_INFO)) 
  {
    loggingIntoFile(data);
  } 
  else if ((m_log_type == CONSOLE) && (m_log_level >= LOG_LEVEL_INFO)) 
  {
    loggingOnConsole(data);
  }
}

void CppLogger::info(std::string &text) throw() { info(text.data()); }

void CppLogger::info(std::ostringstream &stream) throw() 
{
  std::string text = stream.str();
  info(text.data());
}

// Interface for Trace Log


void CppLogger::trace(const char *text) throw() 
{
  std::string data;
  data.append("[Trace]: ");
  data.append(text);

  if ((m_log_type == FILE_LOG) && (m_log_level >= LOG_LEVEL_TRACE))
  {
    loggingIntoFile(data);
  } 
  else if ((m_log_type == CONSOLE) && (m_log_level >= LOG_LEVEL_TRACE))
  {
    loggingOnConsole(data);
  }
}

void CppLogger::trace(std::string &text) throw() { trace(text.data()); }

void CppLogger::trace(std::ostringstream &stream) throw() 
{
  std::string text = stream.str();
  trace(text.data());
}

// Interface for Debug Log
void CppLogger::debug(const char *text) throw() 
{
  std::string data;
  data.append("[DEBUG]: ");
  data.append(text);

  if ((m_log_type == FILE_LOG) && (m_log_level >= LOG_LEVEL_DEBUG))
  {
    loggingIntoFile(data);
  } 
  else if ((m_log_type == CONSOLE) && (m_log_level >= LOG_LEVEL_DEBUG))
  {
    loggingOnConsole(data);
  }
}

void CppLogger::debug(std::string &text) throw() { debug(text.data()); }

void CppLogger::debug(std::ostringstream &stream) throw()
{
  std::string text = stream.str();
  debug(text.data());
}
