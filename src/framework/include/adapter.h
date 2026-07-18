#ifndef __ADAPTER_H__
#define __ADAPTER_H__

namespace miniaudioengine::framework
{

class IAdapterCallback
{

};

template <typename T>
class IAdapter
{
public:

  virtual bool open_stream(const T &info, const framework::eInputOutputDirection &direction) = 0;
  virtual bool close_stream() = 0;
  virtual bool stop_stream() = 0;

  virtual bool is_stream_open() = 0;
  virtual bool is_stream_running() = 0;
};

} // miniaudioengine::framework

#endif // __ADAPTER_H__