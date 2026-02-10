#ifndef __TRACK_MANAGER_H_
#define __TRACK_MANAGER_H_

#include "manager.h"
#include "track.h"
#include "audiostreamcontroller.h"
#include "midiportcontroller.h"

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
  MainTrack() : Track(true),
                p_audio_controller(std::make_shared<AudioStreamController>()),
                p_midi_controller(std::make_shared<MidiPortController>()) {} // is_main_track = true
  ~MainTrack() override = default;

  /** @brief Set the audio output device for the MainTrack.
   *  @param device The audio output device to use.
   */
  void set_audio_output_device(const AudioDevice& device)
  {
    m_audio_output_device = device;
    p_audio_controller->set_output_device(std::make_shared<AudioDevice>(device));
  }

  void open_midi_input_port(const MidiDevice& device)
  {
    p_midi_controller->open_input_port(device.id);
  }

  void register_audio_dataplane(data::AudioDataPlanePtr data_plane)
  {
    p_audio_controller->register_dataplane(data_plane);
  }

  void register_midi_dataplane(data::MidiDataPlanePtr midi_dataplane)
  {
    p_midi_controller->register_dataplane(midi_dataplane);
  }

  bool start()
  {
    return p_audio_controller->start() && p_midi_controller->start();
  }

  bool stop()
  {
    p_midi_controller->close_input_port();
    return p_audio_controller->stop() && p_midi_controller->stop();
  }

  bool is_playing() const;

private:
  AudioDevice m_audio_output_device;
  std::shared_ptr<IAudioController> p_audio_controller; // Only MainTrack owns the controller
  std::shared_ptr<IMidiController> p_midi_controller; // Only MainTrack owns the controller
};

/** @class TrackManager
 *  @brief The TrackManager manages a single-layer hierarchy with MainTrack as root.
 *  All regular Tracks are direct children of MainTrack and may not have children.
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

  /** @brief Create a new track as child of MainTrack.
   *  @param parent Ignored unless it is MainTrack (defaults to MainTrack if nullptr).
   *  @return Shared pointer to the new track.
   */
  TrackPtr create_child_track(TrackPtr parent = nullptr);

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
};

}  // namespace miniaudioengine::control

#endif  // __TRACK_MANAGER_H_