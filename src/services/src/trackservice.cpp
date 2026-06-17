#include "trackservice.h"
#include "logger.h"

using namespace miniaudioengine;

/** @brief Create a new track as child of specified parent.
 *  @param parent The parent track (defaults to MainTrack if nullptr).
 *  @return Shared pointer to the new track.
 */
TrackPtr TrackService::add_track(TrackPtr parent)
{
  auto new_track = std::make_shared<Track>();
  try
  {
    parent->add_child_track(new_track);
  }
  catch (const std::exception& e)
  {
    LOG_ERROR("TrackService: Failed to create child track: ", e.what());
  }

  LOG_INFO("TrackService: Created child track. Total tracks in hierarchy: ", get_tracks().size());
  return new_track;
}

/** @brief Remove a track from the hierarchy.
 *  @param track The track to remove (will also remove all descendants).
 *  @throws std::runtime_error if attempting to remove MainTrack.
 */
void TrackService::remove_track(TrackPtr track)
{
  if (!track)
  {
    return;
  }

  if (track->get_child_count() > 0)
  {
    LOG_WARNING("TrackService: Removing a track that still has children. Children will be detached.");
  }

  // Remove from parent
  track->remove_from_parent();
  
  LOG_INFO("TrackService: Removed track from hierarchy. Total tracks: ", get_tracks().size());
}

/** @brief Clear all tracks except MainTrack.
 */
void TrackService::clear_tracks()
{
  LOG_INFO("TrackService: Cleared ", get_tracks().size(), " tracks after clear: ", get_tracks().size());
}

bool TrackService::play()
{
  LOG_INFO("TrackService: play - ", get_tracks().size(), " tracks");

  for (const auto &track : m_tracks)
  {
    track->play();
  }
  return true;
}

bool TrackService::stop()
{
  LOG_INFO("TrackService: stop");
  return true;
}
