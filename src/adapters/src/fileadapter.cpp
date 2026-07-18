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
  SndFile *input_buffer = (params.direction == framework::eInputOutputDirection::Input)
    ? params.snd_file : nullptr;
  SndFile *output_buffer = (params.direction == framework::eInputOutputDirection::Output)
    ? params.snd_file : nullptr;

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
        read_from_file(file, params.n_frames_to_read);
        break;
      }
      case framework::eInputOutputDirection::Output:
      {
        SndFile *file = static_cast<SndFile *>(output_buffer);
        write_to_file(file, params.n_frames_to_read);
        break;
      }
    }

    // TODO - Calculate cycle time from File bitrate
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void FileAudioStreamThread::read_from_file(SndFile *file, const size_t frames_to_read)
{
  LOG_DEBUG("FileAudioStreamThread: read_from_file: ", frames_to_read, " bytes");
  // TODO - Read from File to Buffer
  std::vector<float> buffer(frames_to_read);
  FileAdapter::read_frames(file, buffer, frames_to_read);
}

void FileAudioStreamThread::write_to_file(SndFile *file, const size_t frames_to_read)
{
  (void)file;
  LOG_DEBUG("FileAudioStreamThread: write_to_file: ", frames_to_read, " bytes");
  // TODO - Write from Buffer to File
}

FileAdapter::FileAdapter(): p_buffer(std::make_shared<FileAudioStreamThread::Buffer>())
{

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
    p_buffer,
    file,
    m_info,
    direction,
    2048 // TODO - Use BUFFER_SIZE constant for n_frames_to_read
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

bool FileAdapter::open_midi_stream(FilePtr file)
{
  (void)file;
  // TODO - Implement FileAdapter open_midi_stream
  throw std::runtime_error("FileAdapter: open_midi_stream not implemented!");
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
