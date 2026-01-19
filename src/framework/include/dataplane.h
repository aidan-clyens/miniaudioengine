#ifndef __DATA_PLANE_H__
#define __DATA_PLANE_H__

#include <memory>

namespace miniaudioengine::core
{

/** @class IDataPlane
 *  @brief Abstract base class for data planes in the framework.
 *  Data planes handle the processing and management of data streams,
 *  such as audio or MIDI data. They provide interfaces for starting,
 *  stopping, and configuring data flow.
 */
class IDataPlane
{
public:
  virtual ~IDataPlane()
  {
    stop();
  }

  /** @brief Set the number of input channels of the audio input.
   *  @param channels Number of input channels.
   */
  void set_input_channels(unsigned int channels)
  {
    m_input_channels = channels;
  }

  /** @brief Set the number of output channels of the audio output.
   *  @param channels Number of output channels.
   */
  void set_output_channels(unsigned int channels)
  {
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
    return !m_stop_command.load(std::memory_order_acquire);
  }

  /** @brief Start audio processing.
   */
  virtual void start()
  {
    m_stop_command.store(false, std::memory_order_release);
  }

  /** @brief Stop audio processing and clear buffers.
   */
  virtual void stop()
  {
    m_stop_command.store(true, std::memory_order_release);
  }

protected:
  std::atomic<bool> m_stop_command{true};

  unsigned int m_input_channels{0};
  unsigned int m_output_channels{0};
};

} // namespace miniaudioengine::core

#endif // __DATA_PLANE_H__