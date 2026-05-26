#include "trackservice.h"
#include "logger.h"

using namespace miniaudioengine;

// ============================================================================
// Constructor
// ============================================================================

TrackService::TrackService(adapters::AudioAdapterPtr audio_adapter, adapters::MidiAdapterPtr midi_adapter)
    : p_audio_adapter(audio_adapter), p_midi_adapter(midi_adapter)
{
  // Create MainTrack (root of hierarchy) with hardware output
  m_main_track = std::make_shared<MainTrack>(audio_adapter); // is_main_track = true
  LOG_INFO("TrackService: Created MainTrack (root of hierarchy)");
}

// ============================================================================
// Hierarchy Operations
// ============================================================================

/** @brief Create a new track as child of specified parent.
 *  @param parent The parent track (defaults to MainTrack if nullptr).
 *  @return Shared pointer to the new track.
 */
TrackPtr TrackService::add_track(TrackPtr parent)
{
  if (!parent || !parent->is_main_track())
  {
    if (parent && !parent->is_main_track())
    {
      LOG_WARNING("TrackService: Non-MainTrack parent provided. Defaulting to MainTrack.");
    }
    parent = m_main_track;
  }

  auto new_track = std::make_shared<Track>(false);
  try
  {
    parent->add_child_track(new_track);
  }
  catch (const std::exception& e)
  {
    LOG_ERROR("TrackService: Failed to create child track: ", e.what());
  }

  LOG_INFO("TrackService: Created child track. Total tracks in hierarchy: ", get_track_count());
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

  if (track->is_main_track())
  {
    throw std::runtime_error("Cannot remove MainTrack from hierarchy.");
  }

  if (track->get_child_count() > 0)
  {
    LOG_WARNING("TrackService: Removing a track that still has children. Children will be detached.");
  }

  // Remove from parent (MainTrack)
  track->remove_from_parent();
  
  LOG_INFO("TrackService: Removed track from hierarchy. Total tracks: ", get_track_count());
}

// ============================================================================
// Tree Traversal
// ============================================================================

/** @brief Get all tracks in the hierarchy (breadth-first traversal).
 *  @return Vector of all track pointers including MainTrack.
 */
std::vector<TrackPtr> TrackService::get_all_tracks() const
{
  std::vector<TrackPtr> all_tracks;

  all_tracks.push_back(m_main_track);

  const auto &children = m_main_track->get_children();
  for (const auto &child : children)
  {
    all_tracks.push_back(child);
  }

  return all_tracks;
}

/** @brief Get total number of tracks including MainTrack.
 *  @return Total track count.
 */
size_t TrackService::get_track_count() const
{
  return get_all_tracks().size();
}

/** @brief Clear all tracks except MainTrack.
 */
void TrackService::clear_tracks()
{
  LOG_INFO("TrackService: Clearing all tracks except MainTrack. Total tracks before clear: ", get_track_count());
  
  // Get all children of MainTrack (without holding the lock during removal)
  std::vector<TrackPtr> children;
  {
    std::lock_guard<std::mutex> lock(m_manager_mutex);
    children = m_main_track->get_children();
  }
  
  // Remove all children without holding the manager mutex
  for (auto& child : children)
  {
    m_main_track->remove_child_track(child);
  }
  
  LOG_INFO("TrackService: All tracks cleared. Total tracks after clear: ", get_track_count());
}

