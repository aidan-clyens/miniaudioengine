#ifndef __FILE_ADAPTER_H__
#define __FILE_ADAPTER_H__

#include "file.h"

#include <sndfile.h>
#include <filesystem>
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

  // TODO - Implement file streaming in FileAdapter
  // File contents should be tranferred between file and a lock-free buffer in the data thread 

  bool open_audio_stream(FilePtr file);
  bool open_midi_port(FilePtr file);

private:
  std::shared_ptr<SndFile> p_file = nullptr;
  SndFileInfo m_info = {};

  static FilePtr make_wav_file_handle(const std::filesystem::path &path)
  {
    return FileHandleFactory::make_wav(path);
  }

  static FilePtr make_midi_file_handle(const std::filesystem::path &path)
  {
    return FileHandleFactory::make_midi(path);
  }
};

} // namespace miniaudioengine::adapters

#endif // __FILE_ADAPTER_H__