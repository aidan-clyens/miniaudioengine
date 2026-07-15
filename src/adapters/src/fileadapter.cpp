#include "fileadapter.h"
#include "logger.h"

#include <chrono>

using namespace miniaudioengine::adapters;

bool FileAudioStreamThread::start(FilePtr file, BufferPtr buffer)
{
  if (is_running())
  {
    LOG_WARNING("FileAudioStreamThread: start - Audio stream thread is already running!");
    return false;
  }

  Params params = { file, buffer };

  // Create new thread
  p_audio_stream_thread = std::make_unique<std::jthread>(FileAudioStreamThread::callback, params);

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

void FileAudioStreamThread::callback(std::stop_token stop_token, const Params &params)
{
  framework::set_thread_name("FileAudioStreamThread");

  while (true)
  {
    if (stop_token.stop_requested())
    {
      LOG_DEBUG("FileAudioStreamThread: callback - Stop requested. Exiting...");
      break;
    }

    switch (params.file->get_direction())
    {
      case framework::eInputOutputDirection::Input:
        LOG_DEBUG("FileAudioStreamThread: callback - File Input");
        break;
      case framework::eInputOutputDirection::Output:
        LOG_DEBUG("FileAudioStreamThread: callback - File Output");
        break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
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

bool FileAdapter::open_stream(FilePtr file)
{
  LOG_DEBUG("FileAdapter: open_stream - Opening audio stream for ", file->to_string());

  if (m_audio_stream_thread.is_running())
  {
    LOG_WARNING("FileAdapter: open_stream - Audio stream thread is already running!");
    if (!m_audio_stream_thread.stop())
    {
      LOG_ERROR("FileAdapter: open_stream - Failed to stop audio stream thread");
      throw std::runtime_error("FileAdapter: open_stream - Failed to stop audio stream thread");
    }
  }

  if (!m_audio_stream_thread.start(file, p_buffer))
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
  // TODO - Implement FileAdapter::open_midi_stream()
  return false;
}
