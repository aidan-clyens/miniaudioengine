#ifndef __MAIN_TRACK_H__
#define __MAIN_TRACK_H__

#include "track.h"

#include <memory>

namespace miniaudioengine
{

// Forward declarations
namespace dataplane { using AudioGraphPtr = std::shared_ptr<class AudioGraph>; }
namespace adapters { using AudioAdapterPtr = std::shared_ptr<class AudioAdapter>; }

using DevicePtr = std::shared_ptr<class Device>;

/** @class MainTrack
 *  @brief The MainTrack is the root of the track hierarchy and manages the audio output device.
 */
class MainTrack : public Track
{
public:
  MainTrack(adapters::AudioAdapterPtr audio_adapter, adapters::MidiAdapterPtr midi_adapter) : Track(true), // main_track = true
                                                                                              p_audio_adapter(audio_adapter),
                                                                                              p_midi_adapter(midi_adapter)
  {}
  ~MainTrack() override = default;

  bool play() override;
  bool stop() override;
  bool is_playing() override;

private:
  /** @brief Compile the audio graph for the current track hierarchy for audio processing.
   *  @return AudioGraph representing the current track hierarchy.
   */
  dataplane::AudioGraphPtr compile_audio_graph() const;

private:
  adapters::AudioAdapterPtr p_audio_adapter = nullptr;
  adapters::MidiAdapterPtr p_midi_adapter = nullptr;
};


} // namespace miniaudioengine

#endif // __MAIN_TRACK_H__