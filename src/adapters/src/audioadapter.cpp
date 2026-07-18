#include "audioadapter.h"

using namespace miniaudioengine;
using namespace miniaudioengine::adapters;


unsigned int BUFFER_SIZE = 1024;


int AudioCallbackHandler::audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                                         double stream_time, AudioStreamStatus status, void *user_data) noexcept
{
  framework::set_thread_name("AudioCallbackHandler");

  (void)output_buffer;
  (void)input_buffer;

  // Verify user data is a valid pointer
  if (user_data == nullptr)
  {
    LOG_ERROR("AudioCallbackHandler: Audio callback does not have user data pointer");
    return 1;
  }

  AudioCallbackHandler::Params *params = (AudioCallbackHandler::Params *)user_data;
  if (params == nullptr)
  {
    LOG_ERROR("AudioCallbackHandler: Audio callback user data does not reference a AudioCallbackHandler::Params object.");
    return 1;
  }

  switch (params->direction)
  {
    case framework::eInputOutputDirection::Input:
      LOG_DEBUG("AudioCallbackHandler: Reading input...");
      break;
    case framework::eInputOutputDirection::Output:
      LOG_DEBUG("AudioCallbackHandler: Writing ", n_frames, " to output buffer. Status=", status, ". Stream Time=", stream_time);
      break;
    default:
      break;
  }

  return 0;
}

AudioAdapter::AudioAdapter()
{
  // List available RtAudio APIs
  std::vector<RtAudio::Api> apis;
  RtAudio::getCompiledApi(apis);
  LOG_DEBUG("AudioAdapter: Compiled RtAudio APIs = ", apis.size());
  for (const auto api : apis)
  {
    LOG_DEBUG("AudioAdapter: RtAudio API - ", RtAudio::getApiDisplayName(api));
  }

  try
  {
    p_rtaudio = std::make_unique<RtAudio>();
    p_rtaudio->showWarnings(true);
  }
  catch(const std::exception& e)
  {
    LOG_ERROR("AudioAdapter: Failed to inialize RtAudio!");
    throw std::runtime_error("AudioAdapter: Failed to inialize RtAudio!");
  }
}

unsigned int AudioAdapter::get_device_count()
{
  return p_rtaudio->getDeviceCount();
}

std::vector<DevicePtr> AudioAdapter::get_devices()
{
  std::vector<DevicePtr> devices;
  unsigned int device_count = p_rtaudio->getDeviceCount();
  devices.reserve(device_count);

#if defined(RTAUDIO_VERSION_MAJOR) && RTAUDIO_VERSION_MAJOR >= 6
  std::vector<unsigned int> device_ids = p_rtaudio->getDeviceIds();
  for (const unsigned int id : device_ids)
  {
    RtAudio::DeviceInfo i = p_rtaudio->getDeviceInfo(id);
    DeviceInfo info = {
        i.ID,
        i.name,
        i.isDefaultInput,
        i.isDefaultOutput,
        i.outputChannels,
        i.inputChannels,
        i.duplexChannels,
        i.sampleRates,
        i.preferredSampleRate};
    devices.push_back(make_device_handle(info));
  }
#else
  for (unsigned int i = 0; i < device_count; ++i)
  {
    RtAudio::DeviceInfo info = p_rtaudio->getDeviceInfo(i);
    DeviceInfo info = {
        i.ID,
        i.name,
        i.isDefaultInput,
        i.isDefaultOutput,
        i.outputChannels,
        i.inputChannels,
        i.duplexChannels,
        i.sampleRates,
        i.preferredSampleRate};
    devices.push_back(make_device_handle(info));
  }
#endif

  return devices;
}

bool AudioAdapter::open_stream(const DeviceInfo &info, const framework::eInputOutputDirection &direction)
{
  unsigned int device_id = info.id;
  unsigned int sample_rate = info.preferred_sample_rate;
  unsigned int channels;

  switch (direction)
  {
    case framework::eInputOutputDirection::Input:
      channels = info.input_channels;
      break;
    case framework::eInputOutputDirection::Output:
      channels = info.output_channels;
      break;
    default:
      LOG_ERROR("AudioAdapter: open_stream - Cannot open stream unless direction is Input or Output: ", direction);
      return false;
  }

  // Set audio output I/O parameters
  adapters::AudioStreamParameters params = {
    device_id,
    channels,
    0
  };

  unsigned int buffer_size = BUFFER_SIZE;

  m_params = {
    direction,
    p_buffer
  };

#if defined(RTAUDIO_VERSION_MAJOR) && RTAUDIO_VERSION_MAJOR >= 6
  LOG_DEBUG("AudioAdapter: open_stream - Opening RtAudio audio stream with Device ID=", device_id, ", Channels=", channels, ", Sample Rate=", sample_rate, ", Buffer Size=", BUFFER_SIZE);

  RtAudioErrorType rc;
  rc = p_rtaudio->openStream(&params,
                             nullptr,
                             RTAUDIO_FLOAT32,
                             sample_rate,
                             &buffer_size,
                             &AudioCallbackHandler::audio_callback,
                             &m_params);

  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioAdapter: open_stream - Failed to open RtAudio stream.");
    return false;
  }

  rc = p_rtaudio->startStream();
  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioAdapter: open_stream - Failed to start RtAudio stream.");
    return false;
  }
#else
  try
  {
    p_rtaudio->openStream(&params,
                          nullptr,
                          RTAUDIO_FLOAT32,
                          sample_rate,
                          &buffer_size,
                          &AudioCallbackHandler::audio_callback,
                          &m_params);
    p_rtaudio->startStream();
  }
  catch (const RtAudioError &e)
  {
    LOG_ERROR("AudioAdapter: open_stream - Failed to open/start RtAudio stream: ", e.getMessage());
    return false;
  }
#endif
  LOG_DEBUG("AudioAdapter: open_stream - Opened audio stream");
  return true;
}

bool AudioAdapter::close_stream()
{
#if defined(RTAUDIO_VERSION_MAJOR) && RTAUDIO_VERSION_MAJOR >= 6
  p_rtaudio->closeStream();
  return true;
#else
  try
  {
    p_rtaudio->closeStream();
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
  RtAudioErrorType rc = p_rtaudio->stopStream();
  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioAdapter: Failed to stop RtAudio stream.");
    return false;
  }
#else
  try
  {
    p_rtaudio->stopStream();
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
  return p_rtaudio->isStreamOpen();
}

bool AudioAdapter::is_stream_running()
{
  return p_rtaudio->isStreamRunning();
}