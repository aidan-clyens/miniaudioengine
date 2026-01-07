#include "trackmanager.h"

#include "audiostreamcontroller.h"

using namespace MinimalAudioEngine::Control;
using namespace MinimalAudioEngine::Data;

// ============================================================================
// Constructor
// ============================================================================

TrackManager::TrackManager()
{
  // Create MainTrack (root of hierarchy) with hardware output
  m_main_track = std::make_shared<Track>(true); // is_main_track = true
  LOG_INFO("TrackManager: Created MainTrack (root of hierarchy)");
}

// ============================================================================
// Hierarchy Operations
// ============================================================================

/** @brief Create a new detached track (not yet in hierarchy).
 *  @return Shared pointer to the new track.
 */
TrackPtr TrackManager::create_track()
{
  auto new_track = std::make_shared<Track>(false);
  LOG_INFO("TrackManager: Created detached track");
  return new_track;
}

/** @brief Create a new track as child of specified parent.
 *  @param parent The parent track (defaults to MainTrack if nullptr).
 *  @return Shared pointer to the new track.
 */
TrackPtr TrackManager::create_child_track(TrackPtr parent)
{
  if (!parent)
  {
    parent = m_main_track;
  }

  auto new_track = std::make_shared<Track>(false);
  parent->add_child_track(new_track);
  
  LOG_INFO("TrackManager: Created child track. Total tracks in hierarchy: ", get_track_count());
  return new_track;
}

/** @brief Remove a track from the hierarchy.
 *  @param track The track to remove (will also remove all descendants).
 *  @throws std::runtime_error if attempting to remove MainTrack.
 */
void TrackManager::remove_track(TrackPtr track)
{
  if (!track)
  {
    return;
  }

  if (track->is_main_track())
  {
    throw std::runtime_error("Cannot remove MainTrack from hierarchy.");
  }

  // Remove all children recursively
  auto children = track->get_children();
  for (auto& child : children)
  {
    remove_track(child);
  }

  // Remove from parent
  track->remove_from_parent();
  
  LOG_INFO("TrackManager: Removed track from hierarchy. Total tracks: ", get_track_count());
}

// ============================================================================
// Tree Traversal
// ============================================================================

/** @brief Get all tracks in the hierarchy (breadth-first traversal).
 *  @return Vector of all track pointers including MainTrack.
 */
std::vector<TrackPtr> TrackManager::get_all_tracks() const
{
  std::vector<TrackPtr> all_tracks;
  collect_all_tracks_recursive(m_main_track, all_tracks);
  return all_tracks;
}

/** @brief Get all leaf tracks (tracks with no children).
 *  @return Vector of leaf track pointers.
 */
std::vector<TrackPtr> TrackManager::get_leaf_tracks() const
{
  std::vector<TrackPtr> leaf_tracks;
  collect_leaf_tracks_recursive(m_main_track, leaf_tracks);
  return leaf_tracks;
}

/** @brief Get total number of tracks including MainTrack.
 *  @return Total track count.
 */
size_t TrackManager::get_track_count() const
{
  return get_all_tracks().size();
}

// ============================================================================
// Data Plane Collection
// ============================================================================

/** @brief Get all TrackAudioDataPlanes from the TrackManager.
 *  @return A vector of shared pointers to all TrackAudioDataPlanes.
 */
std::vector<TrackAudioDataPlanePtr> TrackManager::get_track_audio_dataplanes()
{
  std::vector<TrackAudioDataPlanePtr> dataplanes;
  collect_active_dataplanes_recursive(m_main_track, dataplanes);
  return dataplanes;
}

/** @brief Get all TrackMidiDataPlanes from the TrackManager.
 *  @return A vector of shared pointers to all TrackMidiDataPlanes.
 */
std::vector<TrackMidiDataPlanePtr> TrackManager::get_track_midi_dataplanes()
{
  std::vector<TrackMidiDataPlanePtr> dataplanes;
  collect_active_midi_dataplanes_recursive(m_main_track, dataplanes);
  return dataplanes;
}

/** @brief Clear all tracks except MainTrack.
 */
void TrackManager::clear_tracks()
{
  LOG_INFO("TrackManager: Clearing all tracks except MainTrack. Total tracks before clear: ", get_track_count());
  
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
  
  LOG_INFO("TrackManager: All tracks cleared. Total tracks after clear: ", get_track_count());
}

// ============================================================================
// Legacy Compatibility Methods
// ============================================================================

/** @brief Add a Track to the TrackManager (legacy compatibility).
 *  @return The index of the newly added track in MainTrack's children.
 */
size_t TrackManager::add_track()
{
  auto new_track = create_child_track(m_main_track);
  size_t index = m_main_track->get_child_count() - 1;
  LOG_INFO("TrackManager: Adding a new track (legacy). Total tracks: ", get_track_count());
  return index;
}

/** @brief Get a Track from the TrackManager by index (legacy compatibility).
 *  @param index The index of the track in MainTrack's children.
 *  @return A shared pointer to the Track at the specified index.
 *  @throws std::out_of_range if the index is invalid.
 */
TrackPtr TrackManager::get_track(size_t index)
{
  auto children = m_main_track->get_children();
  if (index >= children.size())
  {
    LOG_ERROR("TrackManager: Attempted to get track with invalid index: ", index);
    throw std::out_of_range("Track index out of range");
  }

  return children[index];
}

/** @brief Get all Tracks from the TrackManager (legacy compatibility).
 *  @return A vector of shared pointers to all immediate children of MainTrack.
 */
std::vector<TrackPtr> TrackManager::get_tracks() const
{
  return m_main_track->get_children();
}

// ============================================================================
// Helper Methods
// ============================================================================

void TrackManager::collect_all_tracks_recursive(TrackPtr track, std::vector<TrackPtr>& out) const
{
  if (!track)
  {
    return;
  }

  out.push_back(track);
  
  for (const auto& child : track->get_children())
  {
    collect_all_tracks_recursive(child, out);
  }
}

void TrackManager::collect_leaf_tracks_recursive(TrackPtr track, std::vector<TrackPtr>& out) const
{
  if (!track)
  {
    return;
  }

  if (track->get_child_count() == 0)
  {
    out.push_back(track);
  }
  else
  {
    for (const auto& child : track->get_children())
    {
      collect_leaf_tracks_recursive(child, out);
    }
  }
}

void TrackManager::collect_active_dataplanes_recursive(TrackPtr track,
                                                       std::vector<Data::TrackAudioDataPlanePtr>& out)
{
  if (!track)
  {
    return;
  }

  auto dataplane = track->get_audio_dataplane();
  if (dataplane && dataplane->is_running())
  {
    out.push_back(dataplane);
  }

  for (const auto& child : track->get_children())
  {
    collect_active_dataplanes_recursive(child, out);
  }
}

void TrackManager::collect_active_midi_dataplanes_recursive(TrackPtr track,
                                                            std::vector<Data::TrackMidiDataPlanePtr>& out)
{
  if (!track)
  {
    return;
  }

  auto dataplane = track->get_midi_dataplane();
  if (dataplane && dataplane->is_running())
  {
    out.push_back(dataplane);
  }

  for (const auto& child : track->get_children())
  {
    collect_active_midi_dataplanes_recursive(child, out);
  }
}
