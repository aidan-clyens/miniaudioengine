#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <iomanip>

#define LOG_INFO(...) \
  MinimalAudioEngine::Core::Logger::instance().log(MinimalAudioEngine::Core::eLogLevel::Info, __VA_ARGS__)

#define LOG_WARNING(...) \
  MinimalAudioEngine::Core::Logger::instance().log(MinimalAudioEngine::Core::eLogLevel::Warning, __VA_ARGS__)

#define LOG_ERROR(...) \
  MinimalAudioEngine::Core::Logger::instance().log(MinimalAudioEngine::Core::eLogLevel::Error, __VA_ARGS__)

#define LOG_DEBUG(...) \
  MinimalAudioEngine::Core::Logger::instance().log(MinimalAudioEngine::Core::eLogLevel::Debug, __VA_ARGS__)

namespace MinimalAudioEngine::Core
{

enum class eLogLevel
{
  Info,
  Warning,
  Error,
  Debug
};

void set_thread_name(const std::string &name);
const std::string &get_thread_name();

class Logger
{
public:
  static Logger &instance()
  {
    static Logger instance;
    return instance;
  }

  void set_log_file(const std::string &file_path)
  {
    std::lock_guard<std::mutex> lock(m_log_mutex);
    static std::ofstream file_stream;
    file_stream.open(file_path, std::ios::out | std::ios::app);
    if (file_stream.is_open())
    {
      p_file_out_stream = &file_stream;
    }
    else
    {
      p_file_out_stream = nullptr;
    }
  }

  void enable_console_output(bool enable)
  {
    m_console_output_enabled = enable;
  }

  void enable_colors(bool enable)
  {
    m_colors_enabled = enable;
  }

  template <typename... Args>
  void log(eLogLevel level, Args &&...args)
  {
    std::lock_guard<std::mutex> lock(m_log_mutex);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::tm local_time{};
#ifdef _WIN32
    localtime_s(&local_time, &time);  // Windows: parameters reversed
#else
    localtime_r(&time, &local_time);  // POSIX (Linux/Unix)
#endif

    std::ostringstream timestamp_stream;
    timestamp_stream << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << ms.count();

    std::ostringstream message_stream;
    (message_stream << ... << args); // C++17 fold expression

    if (m_console_output_enabled)
    {
      (*p_out_stream) << "[" << timestamp_stream.str() << "] "
        << get_color_code(level) << "[" << log_level_to_string(level) << "] " << get_reset_code();
      if (get_thread_name() != "unnamed")
        (*p_out_stream) << (m_colors_enabled ? "\033[1m" : "") << "[Thread: " << get_thread_name() << "]" << (m_colors_enabled ? "\033[0m" : "") << " ";
      (*p_out_stream) << message_stream.str() << "\n";
    }

    if (p_file_out_stream)
    {
      (*p_file_out_stream) << "[" << timestamp_stream.str() << "] "
                      << "[" << log_level_to_string(level) << "] ";
      if (get_thread_name() != "unnamed")
        (*p_file_out_stream) << "[Thread: " << get_thread_name() << "] ";
      (*p_file_out_stream) << message_stream.str() << "\n";
    }
  }

private:
  std::ostream *p_out_stream = &std::cout;
  std::ostream *p_file_out_stream;
  std::mutex m_log_mutex;

  bool m_console_output_enabled = true;
  bool m_colors_enabled = true;

  Logger() = default;
  ~Logger() = default;
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  std::string log_level_to_string(eLogLevel level)
  {
    switch (level)
    {
    case eLogLevel::Info:
      return "INFO";
    case eLogLevel::Warning:
      return "WARNING";
    case eLogLevel::Error:
      return "ERROR";
    case eLogLevel::Debug:
      return "DEBUG";
    default:
      return "UNKNOWN";
    }
  }

  const char* get_color_code(eLogLevel level)
  {
    if (!m_colors_enabled)
      return "";

    switch (level)
    {
    case eLogLevel::Info:
      return "\033[32m";  // Green
    case eLogLevel::Warning:
      return "\033[33m";  // Yellow
    case eLogLevel::Error:
      return "\033[31m";  // Red
    case eLogLevel::Debug:
      return "\033[36m";  // Cyan
    default:
      return "\033[0m";   // Reset
    }
  }

  const char* get_reset_code()
  {
    return m_colors_enabled ? "\033[0m" : "";
  }
};

} // namespace MinimalAudioEngine::Core

#endif // __LOGGER_H__