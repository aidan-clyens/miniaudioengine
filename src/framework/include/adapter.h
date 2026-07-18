#ifndef __ADAPTER_H__
#define __ADAPTER_H__

#include "io.h"

#include <memory>

namespace miniaudioengine::framework
{

class IAdapterCallback
{
public:
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
  IAdapter() : p_buffer(std::make_shared<framework::Buffer>())
  {}

  virtual bool open_stream(const T &info, const framework::BufferPtr &buffer, const eInputOutputDirection &direction) = 0;
  virtual bool close_stream() = 0;
  virtual bool stop_stream() = 0;

  virtual bool is_stream_open() = 0;
  virtual bool is_stream_running() = 0;

protected:
  framework::BufferPtr p_buffer;
  IAdapterCallback::IParams m_params;
};

} // miniaudioengine::framework

#endif // __ADAPTER_H__