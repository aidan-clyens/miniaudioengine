#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <thread>

namespace miniaudioengine::core
{

/** @class IProcessor
 *  @brief Base class for all processors in the framework.
 *  Processors are responsible for handling data processing,
 *  such as audio processing, MIDI processing, or effects processing.
 *  concurrently to main application.
 *  This class provides a common interface and shared functionality for all processor types.
 */
class IProcessor
{
public:
  virtual ~IProcessor() = default;

  /** @brief Start the processor's processing thread.
   *  This method launches a separate thread to handle processing tasks.
   */
  void start()
  {
    m_running.store(true, std::memory_order_release);
    m_thread = std::jthread([this]() { this->process(); });
  }

  /** @brief Stop the processor's processing thread.
   *  This method signals the processing thread to stop and waits for it to finish.
   */
  void stop()
  {
    m_running.store(false, std::memory_order_release);
    // std::jthread joins automatically in destructor, but we can request stop explicitly
    if (m_thread.joinable())
    {
      m_thread.request_stop();
    }
  }

protected:
  /** @brief The main processing loop to be implemented by derived classes.
   *  This method contains the core processing logic and runs in a separate thread.
   */
  virtual void process() = 0;

private:
  std::jthread m_thread;
  
  std::atomic<bool> m_running{false};
};

};

#endif // __PROCESSOR_H__