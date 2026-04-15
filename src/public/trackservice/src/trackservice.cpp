#include "miniaudioengine/trackservice.h"

using namespace miniaudioengine::core;
using namespace miniaudioengine;
using namespace miniaudioengine::audio;
using namespace miniaudioengine::midi;

bool MainTrack::is_playing() const
{
  return p_audio_controller->get_stream_state() == eStreamState::Playing;
}

// ============================================================================
// Constructor
// ============================================================================

TrackService::TrackService()
{
  // Create MainTrack (root of hierarchy) with hardware output
  m_main_track = std::make_shared<MainTrack>(); // is_main_track = true
  LOG_INFO("TrackService: Created MainTrack (root of hierarchy)");
}

// ============================================================================
// Hierarchy Operations
// ============================================================================

/** @brief Create a new detached track (not yet in hierarchy).
 *  @return Shared pointer to the new track.
 */
TrackPtr TrackService::create_track()
{
  auto new_track = std::make_shared<Track>(false);
  LOG_INFO("TrackService: Created detached track");
  return new_track;
}

/** @brief Create a new track as child of specified parent.
 *  @param parent The parent track (defaults to MainTrack if nullptr).
 *  @return Shared pointer to the new track.
 */
TrackPtr TrackService::create_child_track(TrackPtr parent)
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

// ============================================================================
// Audio Output Device Management
// ============================================================================

/** @brief Set the audio output device for the main track.
 *  @param device The audio output device to use.
 */
void TrackService::set_audio_output_device(DeviceHandlePtr device)
{
  LOG_INFO("TrackService: Set audio output device: ", device->get_name());
  m_main_track->set_audio_output_device(device);
}

// ============================================================================
// Legacy Compatibility Methods
// ============================================================================

/** @brief Add a Track to the TrackService (legacy compatibility).
 *  @return The index of the newly added track in MainTrack's children.
 */
size_t TrackService::add_track()
{
  auto new_track = create_child_track(m_main_track);
  size_t index = m_main_track->get_child_count() - 1;
  LOG_INFO("TrackService: Adding a new track (legacy). Total tracks: ", get_track_count());
  return index;
}

/** @brief Get a Track from the TrackService by index (legacy compatibility).
 *  @param index The index of the track in MainTrack's children.
 *  @return A shared pointer to the Track at the specified index.
 *  @throws std::out_of_range if the index is invalid.
 */
TrackPtr TrackService::get_track(size_t index)
{
  auto children = m_main_track->get_children();
  if (index >= children.size())
  {
    LOG_ERROR("TrackService: Attempted to get track with invalid index: ", index);
    throw std::out_of_range("Track index out of range");
  }

  return children[index];
}

/** @brief Get all Tracks from the TrackService (legacy compatibility).
 *  @return A vector of shared pointers to all immediate children of MainTrack.
 */
std::vector<TrackPtr> TrackService::get_tracks() const
{
  return m_main_track->get_children();
}
