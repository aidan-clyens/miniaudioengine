#ifndef __FILE_ADAPTER_H__
#define __FILE_ADAPTER_H__

#include <sndfile.h>
#include <string>
#include <memory>
#include <stdexcept>

namespace miniaudioengine::adapters
{

typedef SNDFILE SndFile;
typedef SF_INFO SndFileInfo;

class FileAdapter
{
public:
  FileAdapter() = default;
  ~FileAdapter() = default;

  bool open(const char* filename);
  void close();

  SndFile *get_file() const { return p_file.get(); }
  SndFileInfo get_info() const { return m_info; }

private:
  std::shared_ptr<SndFile> p_file = nullptr;
  SndFileInfo m_info = {};
};

} // namespace miniaudioengine::adapters

#endif // __FILE_ADAPTER_H__