#include "audiocontroller.h"

#include "audiodataplane.h"

using namespace miniaudioengine::core;
using namespace miniaudioengine::control;
using namespace miniaudioengine::data;

bool IAudioController::validate_start_preconditions() const
{
  if (m_stream_state == eStreamState::Playing)
  {
    LOG_WARNING("AudioController: Stream is already running. No action taken.");
    return false;
  }

  auto device = std::dynamic_pointer_cast<AudioDevice>(get_output_device());
  if (device == nullptr)
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

bool IAudioController::register_dataplanes()
{
  auto dataplanes = get_registered_dataplanes();
  if (dataplanes.empty())
  {
    LOG_ERROR("AudioController: No IDataPlanes registered. Cannot register dataplanes for audio callback.");
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

  // For each active track, set output channels in data
  auto device = std::dynamic_pointer_cast<AudioDevice>(get_output_device());
  if (device != nullptr)
  {
    for (const auto& track_dp : m_callback_context->active_tracks)
    {
      track_dp->set_output_channels(device->output_channels);
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
