#ifndef __DOUBLE_BUFFER_H__
#define __DOUBLE_BUFFER_H__

#include <vector>
#include <atomic>

namespace MinimalAudioEngine::Core
{

/** @class DoubleBuffer
 *  @brief A thread-safe atomic double buffer for audio data.
 *  This class provides methods to get read and write buffers, swap them atomically,
 *  and check if the read buffer is ready.
 *  @tparam T The type of data stored in the buffers.
 */
template <typename T>
class DoubleBuffer
{
public:
  explicit DoubleBuffer(size_t capacity):
    m_read_ready(false)
  {
    m_buffer_a.resize(capacity);
    m_buffer_b.resize(capacity);
    m_read_buffer.store(&m_buffer_a, std::memory_order_relaxed);
    m_write_buffer.store(&m_buffer_b, std::memory_order_relaxed);
  } 
  ~DoubleBuffer() = default;

  std::vector<T>& get_read_buffer()
  {
    m_read_ready.store(false, std::memory_order_release);
    return *m_read_buffer.load(std::memory_order_acquire);
  }

  std::vector<T>& get_write_buffer()
  {
    return *m_write_buffer.load(std::memory_order_acquire);
  }

  void publish()
  {
    std::vector<T>* current_write = m_write_buffer.load(std::memory_order_relaxed);
    std::vector<T>* current_read = m_read_buffer.exchange(current_write, std::memory_order_release);
    m_write_buffer.store(current_read, std::memory_order_relaxed);
    m_read_ready.store(true, std::memory_order_release);
  }

  bool is_read_ready() const
  {
    return m_read_ready.load(std::memory_order_acquire);
  }

private:
  std::vector<T> m_buffer_a;
  std::vector<T> m_buffer_b;

  std::atomic<std::vector<T>*> m_read_buffer;;
  std::atomic<std::vector<T>*> m_write_buffer;

  std::atomic<bool> m_read_ready;
};

} // namespace MinimalAudioEngine::Core

#endif // __DOUBLE_BUFFER_H__