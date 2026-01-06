#ifndef __TRACK_MANAGER_H_
#define __TRACK_MANAGER_H_

#include "manager.h"
#include "track.h"
#include "audiostreamcontroller.h"

#include <memory>
#include <vector>

namespace MinimalAudioEngine::Control
{

/** @class TrackManager
 *  @brief The TrackManager class is responsible for managing tracks in the application.
 */
class TrackManager : public Core::IManager
{
public:
  static TrackManager& instance()
  {
    static TrackManager instance;
    return instance;
  }

  size_t add_track();
  void remove_track(size_t index);
  TrackPtr get_track(size_t index);
  std::vector<TrackPtr> get_tracks() const;
  std::vector<Data::TrackAudioDataPlanePtr> get_track_audio_dataplanes(); // TODO - Make private and accessible by AudioCallbackHandler only
  std::vector<Data::TrackMidiDataPlanePtr> get_track_midi_dataplanes(); // TODO - Make private and accessible by MidiCallbackHandler only

  void clear_tracks();

  size_t get_track_count() const { return m_tracks.size(); }

private:
  TrackManager() = default;
  virtual ~TrackManager() = default;

  std::vector<TrackPtr> m_tracks;
};

}  // namespace MinimalAudioEngine::Control

#endif  // __TRACK_MANAGER_H_