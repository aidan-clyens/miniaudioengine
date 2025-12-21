#ifndef __LOCKFREE_RINGBUFFER_H__
#define __LOCKFREE_RINGBUFFER_H__

namespace MinimalAudioEngine
{

/** @class LockfreeRingBuffer
 *  @brief A lock-free ring buffer implementation.
 *  The data structure is single producer, single-consumer (SPSC).
 *  @tparam T The type of elements stored in the ring buffer.
 *  @tparam Size The maximum number of elements the ring buffer can hold.
 */
template <typename T, size_t Size>
class LockfreeRingBuffer
{
public:
    LockfreeRingBuffer() = default;
    ~LockfreeRingBuffer() = default;

    void try_push(const T& item)
    {
        // TODO - Implementation goes here
    }

    void try_pop(T& item)
    {
        //  TODO - Implementation goes here
    }

    size_t size() const
    {
        //  TODO - Implementation goes here
        return 0;
    }

    size_t capacity() const
    {
        return Size;
    }

private:
};

} // namespace MinimalAudioEngine

#endif // __LOCKFREE_RINGBUFFER_H__