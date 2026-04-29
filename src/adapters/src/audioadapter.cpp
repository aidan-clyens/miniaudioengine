#include "audioadapter.h"

using namespace miniaudioengine;
using namespace miniaudioengine::adapters;

unsigned int AudioAdapter::get_device_count()
{
  return m_rtaudio.getDeviceCount();
}

std::vector<DeviceHandlePtr> AudioAdapter::get_devices()
{
  std::vector<DeviceHandlePtr> devices;
  unsigned int device_count = m_rtaudio.getDeviceCount();
  devices.reserve(device_count);

#if defined(RTAUDIO_VERSION_MAJOR) && RTAUDIO_VERSION_MAJOR >= 6
  std::vector<unsigned int> device_ids = m_rtaudio.getDeviceIds();
  for (const unsigned int id : device_ids)
  {
    RtAudio::DeviceInfo info = m_rtaudio.getDeviceInfo(id);
    devices.push_back(make_device_handle(info, id));
  }
#else
  for (unsigned int i = 0; i < device_count; ++i)
  {
    RtAudio::DeviceInfo info = m_rtaudio.getDeviceInfo(i);
    devices.push_back(make_device_handle(info, i));
  }
#endif

  return devices;
}

bool AudioAdapter::open_stream(AudioStreamParameters &params,
                 unsigned int sample_rate,
                 unsigned int buffer_frames,
                 void *callback_context)
{
#if defined(RTAUDIO_VERSION_MAJOR) && RTAUDIO_VERSION_MAJOR >= 6
  RtAudioErrorType rc;
  rc = m_rtaudio.openStream(&params,
                            nullptr,
                            RTAUDIO_FLOAT32,
                            sample_rate,
                            &buffer_frames,
                            &AudioCallbackHandler::audio_callback,
                            callback_context);

  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioAdapter: Failed to open RtAudio stream.");
    return false;
  }
#else
  try
  {
    m_rtaudio.openStream(&params,
                         nullptr,
                         RTAUDIO_FLOAT32,
                         sample_rate,
                         &buffer_frames,
                         &AudioCallbackHandler::audio_callback,
                         callback_context);
    m_rtaudio.startStream();
  }
  catch (const RtAudioError &e)
  {
    LOG_ERROR("AudioAdapter: Failed to open/start RtAudio stream: ", e.getMessage());
    return false;
  }
#endif
  return true;
}

bool AudioAdapter::close_stream()
{
#if defined(RTAUDIO_VERSION_MAJOR) && RTAUDIO_VERSION_MAJOR >= 6
  m_rtaudio.closeStream();
  return true;
#else
  try
  {
    m_rtaudio.closeStream();
  }
  catch (const RtAudioError &e)
  {
    LOG_ERROR("AudioAdapter: Failed to close RtAudio stream: ", e.getMessage());
    return false;
  }
#endif
  return true;
}

bool AudioAdapter::stop_stream()
{
#if defined(RTAUDIO_VERSION_MAJOR) && RTAUDIO_VERSION_MAJOR >= 6
  RtAudioErrorType rc = m_rtaudio.stopStream();
  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioAdapter: Failed to stop RtAudio stream.");
    return false;
  }
#else
  try
  {
    m_rtaudio.stopStream();
  }
  catch (const RtAudioError &e)
  {
    LOG_ERROR("AudioAdapter: Failed to stop RtAudio stream: ", e.getMessage());
    return false;
  }
#endif
  return true;
}

bool AudioAdapter::is_stream_open()
{
  return m_rtaudio.isStreamOpen();
}

bool AudioAdapter::is_stream_running()
{
  return m_rtaudio.isStreamRunning();
}