#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <thread>

#include "logger.h"

constexpr const unsigned int DEFAULT_BPM = 120;

class IClockObservable
{
public:
  virtual void callback() = 0;
};

using IClockObservablePtr = std::shared_ptr<IClockObservable>;

class Clock
{
public:
  Clock() = default;
  virtual ~Clock() = default;

  void start()
  {
    if (m_running.load())
      return;

    _update_bpm();
    LOG_INFO("Clock: start - BPM = ", m_bpm, " Period = ", m_period);

    m_thread = std::jthread([this](std::stop_token stop_token)
                            { _run(stop_token); });
  }

  void stop()
  {
    if (!m_running.load())
      return;

    m_thread.request_stop();
    m_thread.join();
  }

  void set_bpm(const unsigned int bpm)
  {
    m_bpm = bpm;
  }

  unsigned int get_bpm() const
  {
    return m_bpm;
  }

  void register_observer(const IClockObservablePtr &observer)
  {
    m_observers.push_back(observer);
  }

private:
  unsigned int m_bpm = DEFAULT_BPM;
  std::chrono::milliseconds m_period;
  std::jthread m_thread;
  std::atomic<bool> m_running = false;
  std::vector<IClockObservablePtr> m_observers;

  void _run(std::stop_token stoken)
  {
    miniaudioengine::framework::set_thread_name("Clock");
    m_running.store(true);
    while (!stoken.stop_requested())
    {
      for (const IClockObservablePtr observer : m_observers)
      {
        observer->callback();
      }

      auto now = std::chrono::steady_clock::now();
      std::this_thread::sleep_for(m_period);
    }
    m_running.store(false);
  }

  void _update_bpm()
  {
    // beats / minute -> ms / beat
    double ms = (60.0 / (2 * m_bpm)) * 1000;
    m_period = std::chrono::milliseconds(static_cast<long long>(ms));
  }
};

using ClockPtr = std::shared_ptr<Clock>;

#endif // __CLOCK_H__