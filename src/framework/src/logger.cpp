#include "logger.h"

// Single per-thread instance for the whole program
thread_local std::string thread_name = "unnamed";

void MinimalAudioEngine::set_thread_name(const std::string &name)
{
  thread_name = name;
}

const std::string &MinimalAudioEngine::get_thread_name()
{
  return thread_name;
}