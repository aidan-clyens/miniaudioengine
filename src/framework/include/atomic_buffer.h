#ifndef __ATOMIC_BUFFER_H__
#define __ATOMIC_BUFFER_H__

#include <vector>
#include <atomic>

namespace MinimalAudioEngine
{

/** @class AtomicBuffer
 *  @brief A thread-safe atomic double buffer for audio data.
 *  This class provides methods to get read and write buffers, swap them atomically,
 *  and check if the read buffer is ready.
 *  @tparam T The type of data stored in the buffers.
 */
template <typename T>
class AtomicBuffer
{
public:
  AtomicBuffer() = default;
  ~AtomicBuffer() = default;

  void get_read_buffer()
  {
    // TODO - Implement this method
  }
  void get_write_buffer()
  {
    // TODO - Implement this method
  }
  void swap_buffers()
  {
    // TODO - Implement this method
  }

  bool is_read_buffer_ready() const
  {
    // TODO - Implement this method
    return false;
  }

private:
  std::vector<T> m_read_buffer;
  std::vector<T> m_write_buffer;

  std::atomic<size_t> m_read_index{0};
  std::atomic<size_t> m_write_index{0};
};

} // namespace MinimalAudioEngine

#endif // __ATOMIC_BUFFER_H__