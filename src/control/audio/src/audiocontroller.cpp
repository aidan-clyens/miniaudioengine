#include "audiocontroller.h"
#include "trackmanager.h"

using namespace miniaudioengine::control;
using namespace miniaudioengine::data;

bool IAudioController::validate_start_preconditions() const
{
  if (m_stream_state == eAudioState::Playing)
  {
    LOG_WARNING("AudioController: Stream is already running. No action taken.");
    return false;
  }

  if (!m_audio_output_device.has_value())
  {
    LOG_WARNING("AudioController: No output device set. Cannot start stream.");
    return false;
  }

  if (!m_callback_context)
  {
    LOG_ERROR("AudioController: No AudioCallbackContext registered. Cannot start stream.");
    throw std::runtime_error("AudioController: No AudioCallbackContext registered. Cannot start stream.");
  }

  return true;
}

bool IAudioController::register_dataplanes()
{
  m_callback_context->active_tracks.clear();
  // TODO - Remove dependency on TrackManager singleton, use dependency injection for
  // list of IDataplane types
  m_callback_context->active_tracks = TrackManager::instance().get_track_audio_dataplanes();

  // For each active track, set output channels in data
  if (m_audio_output_device.has_value())
  {
    for (const auto& track_dp : m_callback_context->active_tracks)
    {
      track_dp->set_output_channels(m_audio_output_device->output_channels);
    }
  }

  if (m_callback_context->active_tracks.empty())
  {
    LOG_WARNING("AudioController: No active tracks found in TrackManager.");
    return false;
  }

  LOG_DEBUG("AudioController: Registered ", m_callback_context->active_tracks.size(), " active tracks for audio callback. (", 
            TrackManager::instance().get_track_count(), " total tracks in TrackManager)");

  return true;
}
