#ifndef __FILE_ADAPTER_H__
#define __FILE_ADAPTER_H__

#include "file.h"
#include "ringbuffer.h"
#include "adapter.h"

#include <sndfile.h>
#include <filesystem>
#include <string>
#include <memory>
#include <stdexcept>
#include <thread>

namespace miniaudioengine::adapters
{

typedef SNDFILE SndFile;
typedef SF_INFO SndFileInfo;

// TODO - Should FileAudioStreamThread be moved to FileService instead?
class FileAudioStreamThread : public framework::IAdapterCallback
{
public:
  struct Params : public framework::IAdapterCallback::IParams
  {
    SndFile* snd_file;
    SndFileInfo snd_file_info;
    size_t n_frames_to_read;
  };

  FileAudioStreamThread() = default;
  ~FileAudioStreamThread() = default;
  // TODO - Cannot be copied

  bool start(const Params &params);
  bool stop();
  bool is_running() { return p_audio_stream_thread != nullptr; }

  static void callback(std::stop_token stop_token, void *input_buffer, void *output_buffer, const Params &params);

private:

  static void read_from_file(SndFile *file, framework::Buffer *buffer, const size_t frames_to_read);
  static void write_to_file(framework::Buffer * buffer, SndFile * file, const size_t frames_to_read);

  std::unique_ptr<std::jthread> p_audio_stream_thread;
};

/** @class FileAdapter 
  * @brief Interface to backend audio file library. e.g. sndfile. 
  */
class FileAdapter : public framework::IAdapter<std::filesystem::path>
{
public:
  FileAdapter() = default;
  FileAdapter(const FileAdapter &) = default;
  FileAdapter &operator=(const FileAdapter &) = default;
  ~FileAdapter() = default;

  SndFileInfo get_info() const { return m_info; }

  bool open_stream(const std::filesystem::path &filename, const framework::BufferPtr &buffer, const framework::eInputOutputDirection &direction);
  bool close_stream();
  bool stop_stream() { return false; }  // TODO
  
  bool is_stream_open();
  bool is_stream_running() { return false; }  // TODO

  static long long read_frames(SndFile *file, std::vector<float> &buffer, long long frames_to_read);
  static void seek(SndFile *file, long long frame_offset);

private:
  SndFileInfo m_info = {};

  FileAudioStreamThread m_audio_stream_thread;

  SndFile *open(const char *filename);
  void close(SndFile *file);

  static FilePtr make_wav_file_handle(const std::filesystem::path &path)
  {
    return FileHandleFactory::make_wav(path);
  }

  static FilePtr make_midi_file_handle(const std::filesystem::path &path)
  {
    return FileHandleFactory::make_midi(path);
  }
};

using FileAdapterPtr = std::shared_ptr<FileAdapter>;

} // namespace miniaudioengine::adapters

#endif // __FILE_ADAPTER_H__