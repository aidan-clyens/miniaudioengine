#include "audiostreamcontroller.h"

#include "trackmanager.h"
#include "logger.h"

using namespace miniaudioengine::control;
using namespace miniaudioengine::data;

std::vector<RtAudio::DeviceInfo> AudioStreamController::get_audio_devices()
{
  std::vector<RtAudio::DeviceInfo> devices;
  unsigned int device_count = m_rtaudio.getDeviceCount();
  devices.reserve(device_count);

  std::vector<unsigned int> device_ids = m_rtaudio.getDeviceIds();
  for (const unsigned int i : device_ids)
  {
    RtAudio::DeviceInfo info = m_rtaudio.getDeviceInfo(i);
    devices.push_back(info);
  }
  
  return devices;
}

void AudioStreamController::set_output_device(const AudioDevice &device)
{
  // Check if device is an output device
  if (!device.is_output())
  {
    LOG_ERROR("AudioStreamController: Device ", device.name, " is not an output device.");
    throw std::invalid_argument("AudioStreamController: Device " + device.name + " is not an output device.");
  }

  // Close stream if already open
  if (m_rtaudio.isStreamOpen())
  {
    LOG_INFO("AudioStreamController: Closing existing RtAudio stream...");
    m_rtaudio.closeStream();
    LOG_INFO("AudioStreamController: Closed existing RtAudio stream.");
  }

  LOG_INFO("AudioStreamController: Output device set to ", device.name);
  m_audio_output_device = device;
}

bool AudioStreamController::start_stream()
{
  // Use base class validation
  if (!validate_start_preconditions())
  {
    return false;
  }

  // Use base class track registration
  if (!register_dataplanes())
  {
    return false;
  }

  RtAudio::StreamParameters params = {
    .deviceId = m_audio_output_device->id,
    .nChannels = m_audio_output_device->output_channels,
    .firstChannel = 0
  };

  unsigned int sample_rate = m_audio_output_device->preferred_sample_rate;
  unsigned int buffer_frames = 4096;

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
    LOG_ERROR("AudioStreamController: Failed to open RtAudio stream.");
    return false;
  }

  rc = m_rtaudio.startStream();
  if (rc != RTAUDIO_NO_ERROR)
  {
    LOG_ERROR("AudioStreamController: Failed to start RtAudio stream.");
    return false;
  }

  LOG_INFO("AudioStreamController: RtAudio stream started successfully.");
  m_stream_state = eAudioState::Playing;

  return true;
}

bool AudioStreamController::stop_stream()
{
  if (m_stream_state != eAudioState::Playing)
  {
    LOG_WARNING("AudioStreamController: Stream is not running. No action taken.");
    return false;
  }

  // If stream is running, stop it
  if (m_rtaudio.isStreamRunning())
  {
    RtAudioErrorType rc = m_rtaudio.stopStream();
    if (rc != RTAUDIO_NO_ERROR)
    {
      LOG_ERROR("AudioStreamController: Failed to stop RtAudio stream.");
      return false;
    }
  }

  // If stream is open, close it
  if (m_rtaudio.isStreamOpen())
  {
    m_rtaudio.closeStream();
  }

  LOG_INFO("AudioStreamController: RtAudio stream stopped successfully.");
  m_stream_state = eAudioState::Stopped;

  return true;
}
