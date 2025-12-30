#ifndef __DATA_PLANE_H__
#define __DATA_PLANE_H__

#include <memory>

#include "lockfree_ringbuffer.h"

namespace MinimalAudioEngine
{

/** @struct DataPlaneStatistics
 *  @brief Base structure for data plane statistics.
 */
struct DataPlaneStatistics {};

/** @class IDataPlane
 *  @brief Base class for all data plane components.
 */
template <typename T, size_t BufferSize>
class IDataPlane
{
using LockfreeRingBufferPtr = std::shared_ptr<LockfreeRingBuffer<T, BufferSize>>;

public:
  IDataPlane() : p_output_buffer(std::make_shared<LockfreeRingBuffer<T, BufferSize>>()) {}
  virtual ~IDataPlane() = default;

  /** @brief Get the output lock-free ring buffer.
   *  @return Shared pointer to the output LockfreeRingBuffer.
   */
  LockfreeRingBufferPtr get_output_buffer() const
  {
    return p_output_buffer;
  }

protected:
  void push_to_output_buffer(const T &item)
  {
    // If buffer is full, skip the item
    if (!p_output_buffer->try_push(item))
    {
      // Optionally handle overflow (e.g., log a warning)
    }
  }

  std::optional<T> pop_from_output_buffer() const
  {
    T item;
    // If buffer is empty, return nullopt
    if (p_output_buffer->try_pop(item))
    {
      return item;
    }
    return std::nullopt;
  }

  LockfreeRingBufferPtr p_output_buffer;
};

} // namespace MinimalAudioEngine

#endif // __DATA_PLANE_H__