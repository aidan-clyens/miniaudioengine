#ifndef __TRACK_MANAGER_H_
#define __TRACK_MANAGER_H_

#include "manager.h"
#include "track.h"
#include "audiostreamcontroller.h"

#include <memory>
#include <vector>

namespace miniaudioengine::control
{

/** @class MainTrack
 *  @brief The MainTrack is the root of the track hierarchy and manages the audio output device.
 */
class MainTrack : public Track
{
public:
  MainTrack() : Track(true) {} // is_main_track = true
  ~MainTrack() = default;

  /** @brief Set the audio output device for the MainTrack.
   *  @param device The audio output device to use.
   */
  void set_audio_output_device(const AudioDevice& device)
  {
    m_audio_output_device = device;
    m_audio_controller.set_output_device(device);
  }

private:
  AudioDevice m_audio_output_device;
};

/** @class TrackManager
 *  @brief The TrackManager manages a hierarchical tree of tracks with MainTrack as root.
 *  All tracks route their output to their parent, forming a mixing tree.
 */
class TrackManager : public core::IManager
{
using MainTrackPtr = std::shared_ptr<MainTrack>;

public:
  static TrackManager& instance()
  {
    static TrackManager instance;
    return instance;
  }

  // MainTrack access
  
  /** @brief Get the main track (root of hierarchy).
   *  @return Shared pointer to MainTrack.
   */
  TrackPtr get_main_track() const { return m_main_track; }

  // Hierarchy operations (Control Plane)

  /** @brief Create a new detached track (not yet in hierarchy).
   *  @return Shared pointer to the new track.
   */
  TrackPtr create_track();

  /** @brief Create a new track as child of specified parent.
   *  @param parent The parent track (defaults to MainTrack if nullptr).
   *  @return Shared pointer to the new track.
   */
  TrackPtr create_child_track(TrackPtr parent = nullptr);

  /** @brief Remove a track from the hierarchy.
   *  @param track The track to remove (will also remove all descendants).
   *  @throws std::runtime_error if attempting to remove MainTrack.
   */
  void remove_track(TrackPtr track);

  // Tree traversal

  /** @brief Get all tracks in the hierarchy (breadth-first traversal).
   *  @return Vector of all track pointers including MainTrack.
   */
  std::vector<TrackPtr> get_all_tracks() const;

  /** @brief Get all leaf tracks (tracks with no children).
   *  @return Vector of leaf track pointers.
   */
  std::vector<TrackPtr> get_leaf_tracks() const;

  /** @brief Get total number of tracks including MainTrack.
   *  @return Total track count.
   */
  size_t get_track_count() const;

  // Data plane collection (for callbacks)

  /** @brief Get audio dataplanes for all active tracks.
   *  @return Vector of audio dataplane pointers.
   */
  std::vector<data::TrackAudioDataPlanePtr> get_track_audio_dataplanes();

  /** @brief Get MIDI dataplanes for all active tracks.
   *  @return Vector of MIDI dataplane pointers.
   */
  std::vector<data::TrackMidiDataPlanePtr> get_track_midi_dataplanes();

  /** @brief Clear all tracks except MainTrack.
   */
  void clear_tracks();

  // Audio output device management

  /** @brief Set the audio output device for the main track.
   *  @param device The audio output device to use.
   */
  void set_audio_output_device(const AudioDevice& device);

  // Legacy compatibility methods

  /** @brief Add a track as child of MainTrack (legacy compatibility).
   *  @return Index of the track in MainTrack's children (for backward compatibility).
   *  @deprecated Use create_child_track() instead.
   */
  size_t add_track();

  /** @brief Get a track by index from MainTrack's children (legacy compatibility).
   *  @param index The index in MainTrack's children.
   *  @return Shared pointer to the track.
   *  @throws std::out_of_range if index is invalid.
   *  @deprecated Use get_all_tracks() and navigate hierarchy instead.
   */
  TrackPtr get_track(size_t index);

  /** @brief Get all immediate children of MainTrack (legacy compatibility).
   *  @return Vector of track pointers.
   *  @deprecated Use get_main_track()->get_children() instead.
   */
  std::vector<TrackPtr> get_tracks() const;

private:
  TrackManager();
  virtual ~TrackManager() = default;

  MainTrackPtr m_main_track; // Root of track tree (owns hardware audio output)
  mutable std::mutex m_manager_mutex;

  // Helper methods
  void collect_all_tracks_recursive(TrackPtr track, std::vector<TrackPtr>& out) const;
  void collect_leaf_tracks_recursive(TrackPtr track, std::vector<TrackPtr>& out) const;
  void collect_active_dataplanes_recursive(TrackPtr track,
                                            std::vector<data::TrackAudioDataPlanePtr>& out);
  void collect_active_midi_dataplanes_recursive(TrackPtr track,
                                                 std::vector<data::TrackMidiDataPlanePtr>& out);
};

}  // namespace miniaudioengine::control

#endif  // __TRACK_MANAGER_H_