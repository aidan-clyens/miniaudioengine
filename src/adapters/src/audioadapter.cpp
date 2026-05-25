#include "audioadapter.h"
#include "audiograph.h"

using namespace miniaudioengine;
using namespace miniaudioengine::adapters;
using namespace miniaudioengine::dataplane;

int AudioCallbackHandler::audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                                         double stream_time, AudioStreamStatus status, void *user_data) noexcept
{
  // Verify user data is a pointer to a AudioGraph object
  if (!user_data || !static_cast<AudioGraph*>(user_data))
  {
    LOG_ERROR("AudioCallbackHandler: Audio callback does not have pointer to AudioGraph");
    return 1;
  }

  // If input buffer is present, read audio input
  if (input_buffer != nullptr)
  {
    LOG_INFO("AudioCallbackHandler: Reading input...");
  }
  
  // If output buffer is present, write to audio output
  if (output_buffer != nullptr)
  {
    LOG_INFO("AudioCallbackHandler: Writing to output...");
  }

  return 1;
}

unsigned int AudioAdapter::get_device_count()
{
  return m_rtaudio.getDeviceCount();
}

std::vector<DevicePtr> AudioAdapter::get_devices()
{
  std::vector<DevicePtr> devices;
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
  if (callback_context == nullptr)
  {
    LOG_ERROR("AudioAdapter: Required callback context is empty");
    return false;
  }

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