#ifndef __TRACK_MANAGER_H_
#define __TRACK_MANAGER_H_

#include "track.h"
#include "device.h"

#include <memory>
#include <vector>

namespace miniaudioengine
{

// Forward declarations
namespace dataplane { class AudioGraph; }
namespace dataplane { using AudioGraphPtr = std::shared_ptr<AudioGraph>; }

/** @class MainTrack
 *  @brief The MainTrack is the root of the track hierarchy and manages the audio output device.
 */
class MainTrack : public Track
{
public:
  MainTrack() : Track(true) {} // main_track = true
  ~MainTrack() override = default;

  bool start();
  bool stop() { return true; /* Placeholder implementation */ }
  bool is_playing() const { return false; /* Placeholder implementation */ }

  void set_audio_output_device(DevicePtr device)
  {
    p_audio_output_device = device;
  }

private:
  /** @brief Compile the audio graph for the current track hierarchy for audio processing.
   *  @return AudioGraph representing the current track hierarchy.
   */
  dataplane::AudioGraphPtr compile_audio_graph() const;

private:
  DevicePtr p_audio_output_device;
};

/** @class TrackService
 *  @brief The TrackService manages a single-layer hierarchy with MainTrack as root.
 *  All regular Tracks are direct children of MainTrack and may not have children.
 */
class TrackService
{
using MainTrackPtr = std::shared_ptr<MainTrack>;

public:
  TrackService();
  ~TrackService() = default;

  static TrackService& instance()
  {
    static TrackService instance;
    return instance;
  }

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
};

}  // namespace miniaudioengine

#endif  // __TRACK_MANAGER_H_