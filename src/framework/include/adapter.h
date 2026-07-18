#ifndef __ADAPTER_H__
#define __ADAPTER_H__

#include "ringbuffer.h"

#include <memory>

namespace miniaudioengine::framework
{

class IAdapterCallback
{
public:
  using Buffer = RingBuffer<unsigned int, BUFFER_SIZE>;
  using BufferPtr = std::shared_ptr<Buffer>;

  struct IParams
  {
    eInputOutputDirection direction;
    BufferPtr buffer;
  };
};

template <typename T>
class IAdapter
{
public:
  IAdapter() : p_buffer(std::make_shared<IAdapterCallback::Buffer>())
  {}

  virtual bool open_stream(const T &info, const eInputOutputDirection &direction) = 0;
  virtual bool close_stream() = 0;
  virtual bool stop_stream() = 0;

  virtual bool is_stream_open() = 0;
  virtual bool is_stream_running() = 0;

protected:
  IAdapterCallback::BufferPtr p_buffer;
  IAdapterCallback::IParams m_params;
};

} // miniaudioengine::framework

#endif // __ADAPTER_H__