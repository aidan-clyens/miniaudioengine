#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <array>
#include <cstddef>
#include <mutex>

#include "logger.h"

namespace miniaudioengine::framework
{

constexpr size_t BUFFER_SIZE = 8 * 1024;

/** @enum eDirection
 *  @brief Read Audio/MIDI stream as input or output
 */
enum class eDirection : unsigned int
{
  Input,
  Output
};

/** @class RingBuffer
 *  @brief A Ring Buffer implementation for audio streaming.
 *  The data structure is single producer, single-consumer (SPSC).
 *  @tparam T The type of elements stored in the ring buffer.
 *  @tparam Size The maximum number of elements the ring buffer can hold.
 */
template <typename T, size_t Size>
class RingBuffer
{
public:
  RingBuffer() = default;
  ~RingBuffer() = default;

  /** @brief Attempts to push an item into the ring buffer.
   *  @return true if the item was successfully pushed, false if the buffer is full.
   *  @param item The item to be pushed into the buffer.
   *  @note If the buffer is full, the item will not be added and the method will return false.
   *  @return false if the buffer is full. True otherwise.
   */
  bool try_push(const T &item)
  {
    std::lock_guard<std::mutex> guard(m_mutex);

    // Check if the buffer is full
    size_t current_write = m_write_index;
    size_t next_write = (current_write + 1) % Size;
    size_t current_read = m_read_index;

    if (next_write == current_read)
    {
      // Buffer is full, cannot push
      return false;
    }

    m_buffer[current_write] = item;
    m_write_index = next_write;
    return true;
  }

  /** @brief Attempts to pop an item from the ring buffer.
   *  @return true if an item was successfully popped, false if the buffer is empty.
   *  @param item Reference to store the popped item.
   *  @note If the buffer is empty, no item will be retrieved and the method will return false.
   *  @return false if the buffer is empty. True otherwise.
   */
  bool try_pop(T &item)
  {
    std::lock_guard<std::mutex> guard(m_mutex);

    // Check if the buffer is empty
    size_t current_read = m_read_index;
    size_t current_write = m_write_index;

    if (current_read == current_write)
    {
      // Buffer is empty, cannot pop
      return false;
    }

    item = m_buffer[current_read];
    size_t next_read = (current_read + 1) % Size;
    m_read_index = next_read;
    return true;
  }

  /** @brief Returns the current number of items in the ring buffer.
   *  @return The number of items currently stored in the buffer.
   *  @note This method is lock-free and safe for use in real-time contexts.
   */
  size_t size() const
  {
    std::lock_guard<std::mutex> guard(m_mutex);

    size_t current_write = m_write_index;
    size_t current_read = m_read_index;

    if (current_write >= current_read)
    {
      return current_write - current_read;
    }
    else
    {
      return Size - (current_read - current_write);
    }
  }

  /** @brief Returns the maximum capacity of the ring buffer.
   *  @return The maximum number of items the buffer can hold.
   *  @note This method is lock-free and safe for use in real-time contexts.
   *  @note The usable capacity is Size - 1 to distinguish between full and empty states.
   */
  size_t capacity() const
  {
    return Size - 1; // One slot is used to distinguish full vs empty
  }

  /** @brief Clears the ring buffer, resetting it to an empty state.
   *  @note This method is not thread-safe and should only be called when no other threads are accessing the buffer.
   */
  void clear()
  {
    m_write_index = 0;
    m_read_index = 0;
  }

private:
  std::array<T, Size> m_buffer;
  std::mutex m_mutex;

  size_t m_write_index{0}; // Producer index
  size_t m_read_index{0}; // Consumer index
};

} // namespace miniaudioengine::framework

#endif // __RINGBUFFER_H__