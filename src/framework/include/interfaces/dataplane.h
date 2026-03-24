#ifndef __DATA_PLANE_H__
#define __DATA_PLANE_H__

#include <memory>
#include <atomic>
#include <string>

#include "logger.h"

namespace miniaudioengine::core
{

class IDataPlaneStatistics
{
public:
  IDataPlaneStatistics() = default;
  IDataPlaneStatistics(const IDataPlaneStatistics&) = default;
  IDataPlaneStatistics& operator=(const IDataPlaneStatistics&) = default;
  virtual ~IDataPlaneStatistics() = default;

  virtual void reset() = 0;

  virtual std::string to_string() const
  {
    return "IDataPlaneStatistics";
  }
};

using IDataPlaneStatisticsPtr = std::shared_ptr<IDataPlaneStatistics>;

/** @class IDataPlane
 *  @brief Abstract base class for data planes in the framework.
 *  Data planes handle the processing and management of data streams,
 *  such as audio or MIDI data. They provide interfaces for starting,
 *  stopping, and configuring data flow.
 */
class IDataPlane
{
public:
  IDataPlane() = default;
  IDataPlane(const IDataPlane&) = delete; // Disable copy constructor
  IDataPlane& operator=(const IDataPlane&) = delete; // Disable copy assignment

  virtual ~IDataPlane() = default;

  /** @brief Set the number of input channels of the audio input.
   *  @param channels Number of input channels.
   */
  void set_input_channels(unsigned int channels)
  {
    if (static_cast<int>(channels) < 0)
    {
      throw std::invalid_argument("Input channels cannot be negative");
    }
    m_input_channels = channels;
  }

  /** @brief Set the number of output channels of the audio output.
   *  @param channels Number of output channels.
   */
  void set_output_channels(unsigned int channels)
  {
    if (static_cast<int>(channels) < 0)
    {
      throw std::invalid_argument("Output channels cannot be negative");
    }
    m_output_channels = channels;
  }

  /** @brief Get the number of input channels.
   *  @return Number of input channels.
   */
  inline unsigned int get_input_channels() const
  {
    return m_input_channels;
  }

  /** @brief Get the number of output channels.
   *  @return Number of output channels.
   */
  inline unsigned int get_output_channels() const
  {
    return m_output_channels;
  }

  /** @brief Check if the track is currently running.
   *  @return True if running, false if stopped.
   */
  bool is_running() const
  {
    return m_is_running.load(std::memory_order_acquire);
  }

  /** @brief Start audio processing.
   *  @return True if started successfully, false otherwise.
   */
  bool start()
  {
    LOG_DEBUG(m_name, ": Requested Start.");

    if (p_statistics)
    {
      p_statistics->reset();
    }

    m_stop_command.store(false, std::memory_order_release);
    m_is_running.store(true, std::memory_order_release);
    return _start();
  }

  /** @brief Stop audio processing and clear buffers.
   *  @return True if stopped successfully, false otherwise.
   */
  bool stop()
  {
    LOG_DEBUG(m_name, ": Requested Stop.");
    m_stop_command.store(true, std::memory_order_release);
    m_is_running.store(false, std::memory_order_release);
    return _stop();
  }

  /** @brief Get the statistics object for this data plane.
   *  @return Shared pointer to the statistics object.
   */
  virtual IDataPlaneStatisticsPtr get_statistics() const
  {
    return p_statistics;
  }

  std::string to_string() const
  {
    return "IDataPlane(" + m_name + ", Input Channels=" + std::to_string(m_input_channels) +
           ", Output Channels=" + std::to_string(m_output_channels) + ")";
  }

protected:
  std::atomic<bool> m_stop_command{true};
  std::atomic<bool> m_is_running{false};

  std::string m_name{"IDataPlane"};

  IDataPlaneStatisticsPtr p_statistics;

  unsigned int m_input_channels{0};
  unsigned int m_output_channels{0};

  virtual bool _start() = 0;
  virtual bool _stop() = 0;
};

} // namespace miniaudioengine::core

#endif // __DATA_PLANE_H__