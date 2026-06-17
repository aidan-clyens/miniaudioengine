#ifndef __TRACK_MANAGER_H_
#define __TRACK_MANAGER_H_

#include "track.h"

#include <memory>
#include <vector>

namespace miniaudioengine
{

/** @class TrackService
 *  @brief The TrackService manages a single-layer hierarchy with MainTrack as root.
 *  All regular Tracks are direct children of MainTrack and may not have children.
 */
class TrackService
{

public:
  TrackService() = default;
  ~TrackService() = default;

  /** @brief Create a new track as child of MainTrack.
   *  @param parent Ignored unless it is MainTrack (defaults to MainTrack if nullptr).
   *  @return Shared pointer to the new track.
   */
  TrackPtr add_track(TrackPtr parent = nullptr);

  /** @brief Remove a track from the hierarchy.
   *  @param track The track to remove.
   *  @throws std::runtime_error if attempting to remove MainTrack.
   */
  void remove_track(TrackPtr track);

  // Tree traversal

  /** @brief Get all tracks in the hierarchy (MainTrack + direct children).
   *  @return Vector of all track pointers including MainTrack.
   */
  inline std::vector<TrackPtr> get_tracks() const { return m_tracks; }

  /** @brief Clear all tracks.
   */
  void clear_tracks();

  bool play();
  bool stop();

private:
  mutable std::mutex m_manager_mutex;

  std::vector<TrackPtr> m_tracks;
};

}  // namespace miniaudioengine

#endif  // __TRACK_MANAGER_H_