#include "audiocontroller.h"
#include "devicehandle_factory.h"

#include "miniaudioengine/trackmanager.h"
#include "logger.h"

using namespace miniaudioengine::core;
using namespace miniaudioengine::audio;
using namespace miniaudioengine;

// Converts RtAudio::DeviceInfo to DeviceHandlePtr without exposing RtAudio in any header
static DeviceHandlePtr make_device_handle(const RtAudio::DeviceInfo& info, unsigned int id)
{
  return DeviceHandleFactory::make_audio(
    id,
    info.name,
    info.isDefaultInput,
    info.isDefaultOutput,
    info.outputChannels,
    info.inputChannels,
    info.duplexChannels,
    info.preferredSampleRate,
    info.sampleRates);
}

std::vector<DeviceHandlePtr> AudioController::get_audio_devices()
{
  std::vector<DeviceHandlePtr> devices;
  unsigned int device_count = m_rtaudio.getDeviceCount();
  devices.reserve(device_count);

  std::vector<unsigned int> device_ids = m_rtaudio.getDeviceIds();
  for (const unsigned int id : device_ids)
  {
    RtAudio::DeviceInfo info = m_rtaudio.getDeviceInfo(id);
    devices.push_back(make_device_handle(info, id));
  }

  return devices;
}

bool AudioController::_start()
{
  LOG_DEBUG("AudioController: Starting audio stream.");

  // Use base class validation
  if (!validate_start_preconditions())
  {
    LOG_ERROR("AudioController: Stream preconditions not met. Cannot start stream.");
    return false;
  }

  // Use base class track registration
  if (!register_dataplanes())
  {
    LOG_ERROR("AudioController: No active dataplanes registered. Cannot start stream.");
    return false;
  }

  auto device = m_device_handle;
  if (!device)
  {
    LOG_ERROR("AudioController: Output device is null after validation. Cannot start stream.");
    return false;
  }

  RtAudio::StreamParameters params = {
    .deviceId  = device->get_id(),
    .nChannels = device->get_output_channels(),
    .firstChannel = 0
  };

  unsigned int sample_rate   = device->get_preferred_sample_rate();
  unsigned int buffer_frames = 4096;

  LOG_DEBUG("AudioController: Opening RtAudio stream with device ", device->get_name(),
            ", Sample Rate: ", sample_rate,
            ", Buffer Frames: ", buffer_frames);

  RtAudioErrorType rc;
  rc = m_rtaudio.openStream(&params,
                            nullptr,
                            RTAUDIO_FLOAT32,
                            sample_rate,
                            &buffer_frames,
                            &AudioCallbackHandler::audio_callback,
                            m_callback_context.get());

  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioController: Failed to open RtAudio stream.");
    return false;
  }

  rc = m_rtaudio.startStream();
  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioController: Failed to start RtAudio stream.");
    return false;
  }

  LOG_DEBUG("AudioController: RtAudio stream Started with output device ", device->get_name());
  m_stream_state = eStreamState::Playing;

  return true;
}

bool AudioController::_stop()
{
  if (m_stream_state != eStreamState::Playing)
  {
    LOG_WARNING("AudioController: Stream is not running. No action taken.");
    return false;
  }

  // If stream is running, stop it
  if (m_rtaudio.isStreamRunning())
  {
    RtAudioErrorType rc = m_rtaudio.stopStream();
    if (rc != RTAUDIO_NO_ERROR)
    {
      LOG_ERROR("AudioController: Failed to stop RtAudio stream.");
      return false;
    }
  }

  // If stream is open, close it
  if (m_rtaudio.isStreamOpen())
  {
    m_rtaudio.closeStream();
  }

  clear_registered_dataplane();

  LOG_DEBUG("AudioController: RtAudio stream stopped successfully.");
  m_stream_state = eStreamState::Stopped;

  return true;
}
