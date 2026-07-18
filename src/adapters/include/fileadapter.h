#ifndef __FILE_ADAPTER_H__
#define __FILE_ADAPTER_H__

#include "file.h"
#include "ringbuffer.h"

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

constexpr size_t BUFFER_SIZE = 1024;

// TODO - Should FileAudioStreamThread be moved to FileService instead?
class FileAudioStreamThread
{
public:

  using Buffer = framework::RingBuffer<unsigned int, BUFFER_SIZE>;
  using BufferPtr = std::shared_ptr<Buffer>;

  struct Params
  {
    BufferPtr buffer;
    SndFile* snd_file;
    SndFileInfo snd_file_info;
    framework::eInputOutputDirection direction;
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

  static void read_from_file(SndFile *file, const size_t frames_to_read);
  static void write_to_file(SndFile *file, const size_t frames_to_read);

  std::unique_ptr<std::jthread> p_audio_stream_thread;
};

/** @class FileAdapter 
  * @brief Interface to backend audio file library. e.g. sndfile. 
  */
class FileAdapter
{
public:
  FileAdapter();
  ~FileAdapter() = default;

  SndFile* open(const char* filename);
  void close(SndFile *file);

  SndFileInfo get_info() const { return m_info; }

  // TODO - Implement file streaming in FileAdapter
  // File contents should be tranferred between file and a lock-free buffer in the data thread

  bool open_stream(const std::filesystem::path &filename, const framework::eInputOutputDirection &direction);
  bool close_stream();
  bool is_stream_open();

  bool open_midi_stream(FilePtr file);  // TOOD - Remove FilePtr reference

  static long long read_frames(SndFile *file, std::vector<float> &buffer, long long frames_to_read);
  static void seek(SndFile *file, long long frame_offset);

private:
  SndFileInfo m_info = {};

  FileAudioStreamThread m_audio_stream_thread;
  FileAudioStreamThread::BufferPtr p_buffer;

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