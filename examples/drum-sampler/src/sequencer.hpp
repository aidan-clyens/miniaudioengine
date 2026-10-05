#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

#include <memory>

#include "clock.hpp"

constexpr const unsigned int DEFAULT_LOOP_SIZE = 8;

class ISequencerObservable : public std::enable_shared_from_this<ISequencerObservable>
{
public:
  virtual void callback(unsigned int beat_num) = 0;
};

using ISequencerObservablePtr = std::shared_ptr<ISequencerObservable>;

class Sequencer : public IClockObservable
{
public:
  Sequencer() = default;
  virtual ~Sequencer() = default;

  void start()
  {
    m_running.store(true);
  }

  void stop()
  {
    m_running.store(false);
  }

  void set_loop_size(const unsigned int loop_size)
  {
    m_loop_size = loop_size;
  }

  unsigned int get_loop_size() const
  {
    return m_loop_size;
  }

  void register_observer(const ISequencerObservablePtr &observer)
  {
    m_observers.push_back(observer);
  }

  void callback() override
  {
    if (!m_running.load())
      return;

    for (const ISequencerObservablePtr observer : m_observers)
    {
      observer->callback(m_beat_counter);
    }
    m_beat_counter = (m_beat_counter + 1) % m_loop_size;

    if (m_beat_counter % m_loop_size == 0)
    {
      m_loop_counter++;
    }

    LOG_INFO("Sequencer: callback - Loop = ", m_loop_counter, " Beat = ", m_beat_counter);
  }

private:
  unsigned int m_loop_size = DEFAULT_LOOP_SIZE;
  unsigned int m_beat_counter = 0;
  unsigned int m_loop_counter = 0;
  std::atomic<bool> m_running = false;

  std::vector<ISequencerObservablePtr> m_observers;
};

using SequencerPtr = std::shared_ptr<Sequencer>;

#endif // __SEQUENCER_H__