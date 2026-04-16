#include "audiocontroller.h"
#include "devicehandle_factory.h"
#include "audiodataplane.h"

#include "miniaudioengine/trackservice.h"
#include "logger.h"

using namespace miniaudioengine::core;
using namespace miniaudioengine::audio;
using namespace miniaudioengine;

AudioController::AudioController()
  : core::IController("AudioController"),
    m_callback_context(std::make_shared<AudioCallbackContext>())
{}

void AudioController::set_output_device(DeviceHandlePtr device)
{
  if (!device)
  {
    LOG_ERROR("AudioController: Attempted to set a null output device.");
    throw std::invalid_argument("AudioController: Output device cannot be null.");
  }

  if (!device->is_output())
  {
    LOG_ERROR("AudioController: Device ", device->get_name(), " is not an output device.");
    throw std::invalid_argument("AudioController: Device " + device->get_name() + " is not an output device.");
  }

  LOG_DEBUG("AudioController: Output device set to ", device->to_string());
  m_device_handle = device;
}

bool AudioController::validate_start_preconditions() const
{
  if (m_stream_state == eStreamState::Playing)
  {
    LOG_WARNING("AudioController: Stream is already running. No action taken.");
    return false;
  }

  if (!m_device_handle)
  {
    LOG_WARNING("AudioController: No output device set. Cannot start stream.");
    return false;
  }

  if (!m_callback_context)
  {
    LOG_ERROR("AudioController: No AudioCallbackContext registered. Cannot start stream.");
    throw std::runtime_error("AudioController: No AudioCallbackContext registered. Cannot start stream.");
  }

  LOG_DEBUG("AudioController: Start preconditions validated successfully.");
  return true;
}

bool AudioController::register_dataplanes()
{
  auto dataplanes = get_registered_dataplanes();
  if (dataplanes.empty())
  {
    LOG_ERROR("AudioController: No dataplanes registered. Cannot register dataplanes for audio callback.");
    return false;
  }

  m_callback_context->active_tracks.clear();
  for (auto & dp : dataplanes)
  {
    auto audio_dp = std::dynamic_pointer_cast<AudioDataPlane>(dp);
    if (audio_dp != nullptr)
    {
      m_callback_context->active_tracks.push_back(audio_dp);
    }
  }

  if (m_device_handle)
  {
    for (const auto& track_dp : m_callback_context->active_tracks)
    {
      track_dp->set_output_channels(m_device_handle->get_output_channels());
    }
  }

  if (m_callback_context->active_tracks.empty())
  {
    LOG_WARNING("AudioController: No active dataplanes registered.");
    return false;
  }

  LOG_DEBUG("AudioController: Registered ", m_callback_context->active_tracks.size(), " active dataplanes for audio callback.");

  return true;
}

std::vector<DeviceHandlePtr> AudioController::get_audio_devices()
{
  return m_adapter.get_devices();
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

  adapter::AudioStreamParameters params = {
    .deviceId  = device->get_id(),
    .nChannels = device->get_output_channels(),
    .firstChannel = 0
  };

  unsigned int sample_rate   = device->get_preferred_sample_rate();
  unsigned int buffer_frames = 4096;

  LOG_DEBUG("AudioController: Opening RtAudio stream with device ", device->get_name(),
            ", Sample Rate: ", sample_rate,
            ", Buffer Frames: ", buffer_frames);

  if (!m_adapter.open_stream(params, sample_rate, buffer_frames, m_callback_context.get()))
  {
    LOG_ERROR("AudioController: Failed to open RtAudio stream.");
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
  if (m_adapter.is_stream_running())
  {
    if (!m_adapter.stop_stream())
    {
      LOG_ERROR("AudioController: Failed to stop RtAudio stream.");
      return false;
    }
  }

  // If stream is open, close it
  if (m_adapter.is_stream_open())
  {
    m_adapter.close_stream();
  }

  clear_registered_dataplane();

  LOG_DEBUG("AudioController: RtAudio stream stopped successfully.");
  m_stream_state = eStreamState::Stopped;

  return true;
}
