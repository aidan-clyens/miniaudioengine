#ifndef __TRACK_MANAGER_H_
#define __TRACK_MANAGER_H_

#include "track.h"
#include "maintrack.h"

#include <memory>
#include <vector>

namespace miniaudioengine
{

// Forward declarations
namespace adapters
{
using AudioAdapterPtr = std::shared_ptr<class AudioAdapter>;
using MidiAdapterPtr = std::shared_ptr<class MidiAdapter>;
}

/** @class TrackService
 *  @brief The TrackService manages a single-layer hierarchy with MainTrack as root.
 *  All regular Tracks are direct children of MainTrack and may not have children.
 */
class TrackService
{

using MainTrackPtr = std::shared_ptr<MainTrack>;

public:
  TrackService(adapters::AudioAdapterPtr audio_adapter, adapters::MidiAdapterPtr midi_adapter);
  ~TrackService() = default;

  /** @brief Get the main track (root of hierarchy).
   *  @return Shared pointer to MainTrack.
   */
  TrackPtr get_main_track() const { return m_main_track; }

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
  std::vector<TrackPtr> get_all_tracks() const;

  /** @brief Get total number of tracks including MainTrack.
   *  @return Total track count.
   */
  size_t get_track_count() const;

  /** @brief Clear all tracks except MainTrack.
   */
  void clear_tracks();

private:
  MainTrackPtr m_main_track; // Root of track tree (owns hardware audio output)
  mutable std::mutex m_manager_mutex;

  adapters::AudioAdapterPtr p_audio_adapter;
  adapters::MidiAdapterPtr p_midi_adapter;
};

}  // namespace miniaudioengine

#endif  // __TRACK_MANAGER_H_