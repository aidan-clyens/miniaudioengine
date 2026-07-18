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

  // Define input or output buffer
  SndFilePtr input_buffer = (params.direction == framework::eInputOutputDirection::Input)
    ? params.snd_file : nullptr;
  SndFilePtr output_buffer = (params.direction == framework::eInputOutputDirection::Output)
    ? params.snd_file : nullptr;

  // Create new thread
  p_audio_stream_thread = std::make_unique<std::jthread>(
    FileAudioStreamThread::callback,
    &input_buffer,
    &output_buffer,
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

void FileAudioStreamThread::callback(std::stop_token stop_token, void *output_buffer, void *input_buffer, const Params &params)
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
        SndFilePtr *file = static_cast<SndFilePtr *>(input_buffer);
        read_from_file(*file, params.n_frames_to_read);
        break;
      }
      case framework::eInputOutputDirection::Output:
      {
        SndFilePtr *file = static_cast<SndFilePtr *>(output_buffer);
        write_to_file(*file, params.n_frames_to_read);
        break;
      }
    }

    // TODO - Calculate cycle time from File bitrate
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void FileAudioStreamThread::read_from_file(const SndFilePtr &file, const size_t frames_to_read)
{
  LOG_DEBUG("FileAudioStreamThread: read_from_file: ", frames_to_read, " bytes");
  // TODO - Read from File to Buffer
  std::vector<float> buffer(frames_to_read);
  FileAdapter::read_frames(file, buffer, frames_to_read);
}

void FileAudioStreamThread::write_to_file(const SndFilePtr &file, const size_t frames_to_read)
{
  (void)file;
  LOG_DEBUG("FileAudioStreamThread: write_to_file: ", frames_to_read, " bytes");
  // TODO - Write from Buffer to File
}

FileAdapter::FileAdapter(): p_buffer(std::make_shared<FileAudioStreamThread::Buffer>())
{

}

bool FileAdapter::open(const char *filename)
{
  m_info = {};
  p_file = std::shared_ptr<SndFile>(
      sf_open(filename, SFM_READ, &m_info),
      [](SndFile *f)
      { if (f) sf_close(f); });

  if (!p_file)
  {
    return false;
  }

  return true;
}

void FileAdapter::close()
{
  p_file.reset();
}

bool FileAdapter::open_stream(const framework::eInputOutputDirection &direction)
{
  if (p_file == nullptr)
  {
    LOG_WARNING("FileAdapter: open_stream - No file opened, cannot open stream.");
    return false;
  }

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

  FileAudioStreamThread::Params params =
  {
    p_buffer,
    p_file,
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

long long FileAdapter::read_frames(const SndFilePtr &file, std::vector<float> &buffer, long long frames_to_read)
{
  if (file == nullptr)
    return 0LL;
  return sf_readf_float(file.get(),
                        buffer.data(),
                        static_cast<sf_count_t>(frames_to_read));
}

void FileAdapter::seek(const SndFilePtr &file, long long frame_offset)
{
  if (file == nullptr)
    return;
  sf_seek(file.get(), static_cast<sf_count_t>(frame_offset), SEEK_SET);
}
