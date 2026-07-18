#include "fileadapter.h"
#include "logger.h"

#include <chrono>

using namespace miniaudioengine::adapters;

bool FileAudioStreamThread::start(const Params &params)
{
  if (is_running())
  {
    LOG_WARNING("FileAudioStreamThread: start - Audio stream thread is already running!");
    return false;
  }

  if (params.snd_file == nullptr)
  {
    LOG_WARNING("FileAudioStreamThread: start - SndFile is null.");
    return false;
  }

  // Define input or output buffer
  void *input_buffer = nullptr;
  void *output_buffer = nullptr;

  switch (params.direction)
  {
    case framework::eInputOutputDirection::Input:
      input_buffer = params.snd_file;
      output_buffer = params.buffer.get();
      break;
    case framework::eInputOutputDirection::Output:
      input_buffer = params.buffer.get();
      output_buffer = params.snd_file;
      break;
  }

  // Create new thread
  p_audio_stream_thread = std::make_unique<std::jthread>(
    FileAudioStreamThread::callback,
    input_buffer,
    output_buffer,
    params
  );

  LOG_DEBUG("FileAudioStreamThread: start - Started audio stream thread");
  return true;
}

bool FileAudioStreamThread::stop()
{
  if (!is_running())
  {
    LOG_WARNING("FileAudioStreamThread: stop - Audio stream not running");
    return false;
  }

  LOG_DEBUG("FileAudioStreamThread: stop - Stopped audio stream thread");
  p_audio_stream_thread->request_stop();
  p_audio_stream_thread->join();
  return true;
}

void FileAudioStreamThread::callback(std::stop_token stop_token, void *input_buffer, void *output_buffer, const Params &params)
{
  framework::set_thread_name("FileAudioStreamThread");

  const size_t total_frames_to_read = params.n_frames_to_read * params.snd_file_info.channels;
  const float cycle_time_s = 1.0 / (params.snd_file_info.samplerate / total_frames_to_read);
  const unsigned int cycle_time_ms = (unsigned int)(cycle_time_s * 1000);

  LOG_DEBUG("FileAudioStreamThread: callback - Sample Rate = ", params.snd_file_info.samplerate, " Cycle Time = ", cycle_time_ms, " ms");

  while (true)
  {
    if (stop_token.stop_requested())
    {
      LOG_DEBUG("FileAudioStreamThread: callback - Stop requested. Exiting...");
      break;
    }

    switch (params.direction)
    {
      case framework::eInputOutputDirection::Input:
      {
        SndFile *file = static_cast<SndFile *>(input_buffer);
        Buffer *buffer = static_cast<Buffer *>(output_buffer);
        read_from_file(file, buffer, params.n_frames_to_read);
        break;
      }
      case framework::eInputOutputDirection::Output:
      {
        Buffer *buffer = static_cast<Buffer *>(input_buffer);
        SndFile *file = static_cast<SndFile *>(output_buffer);
        write_to_file(buffer, file, params.n_frames_to_read);
        break;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(cycle_time_ms));
  }
}

void FileAudioStreamThread::read_from_file(SndFile *file, Buffer *buffer, const size_t frames_to_read)
{
  LOG_DEBUG("FileAudioStreamThread: read_from_file: ", frames_to_read, " bytes");
  // TODO - Read from File to Buffer
  std::vector<float> buffer_data(frames_to_read);
  FileAdapter::read_frames(file, buffer_data, frames_to_read);

  // TODO - Transfer to Buffer
  for (const float val : buffer_data)
  {
    buffer->try_push(val);
  }
}

void FileAudioStreamThread::write_to_file(Buffer *buffer, SndFile *file, const size_t frames_to_read)
{
  (void)file;
  LOG_DEBUG("FileAudioStreamThread: write_to_file: ", frames_to_read, " bytes");
  // TODO - Write from Buffer to File
}

SndFile* FileAdapter::open(const char *filename)
{
  m_info = {};
  return sf_open(filename, SFM_READ, &m_info);
}

void FileAdapter::close(SndFile *file)
{
  if (file == nullptr)
  {
    LOG_WARNING("FileAdapter: close - Cannot close null SndFile");
    return;
  }
  sf_close(file);
}

bool FileAdapter::open_stream(const std::filesystem::path &filename, const framework::eInputOutputDirection &direction)
{
  LOG_DEBUG("FileAdapter: open_stream - Opening audio stream");

  if (m_audio_stream_thread.is_running())
  {
    LOG_WARNING("FileAdapter: open_stream - Audio stream thread is already running!");
    if (!m_audio_stream_thread.stop())
    {
      LOG_ERROR("FileAdapter: open_stream - Failed to stop audio stream thread");
      throw std::runtime_error("FileAdapter: open_stream - Failed to stop audio stream thread");
    }
  }

  SndFile *file = open(filename.string().c_str());
  if (file == nullptr)
  {
    LOG_WARNING("FileAdapter: open_stream - Failed to open SndFile: ", filename);
    return false;
  }

  FileAudioStreamThread::Params params =
  {
    direction,
    p_buffer,
    file,
    m_info,
    1024
  };

  if (!m_audio_stream_thread.start(params))
  {
    LOG_ERROR("FileAdapter: open_stream - Failed to start audio stream thread");
    return false;
  }

  return m_audio_stream_thread.is_running();
}

bool FileAdapter::close_stream()
{
  if (!m_audio_stream_thread.stop())
  {
    LOG_ERROR("FileAdapter: open_stream - Failed to stop audio stream thread");
    return false;
  }
  return true;
}

bool FileAdapter::is_stream_open()
{
  return m_audio_stream_thread.is_running();
}

long long FileAdapter::read_frames(SndFile *file, std::vector<float> &buffer, long long frames_to_read)
{
  if (file == nullptr)
  {
    LOG_WARNING("FileAdapter: read_frames - Cannot read from null SndFile.");
    return 0LL;
  }
  return sf_readf_float(file,
                        buffer.data(),
                        static_cast<sf_count_t>(frames_to_read));
}

void FileAdapter::seek(SndFile *file, long long frame_offset)
{
  if (file == nullptr)
  {
    LOG_WARNING("FileAdapter: seek - Cannot read from null SndFile.");
    return;
  }
  sf_seek(file, static_cast<sf_count_t>(frame_offset), SEEK_SET);
}
