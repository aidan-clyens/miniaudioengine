#ifndef __TRACK_H__
#define __TRACK_H__

#include <queue>
#include <mutex>
#include <memory>
#include <optional>
#include <variant>
#include <atomic>
#include <string>
#include <functional>

#include "filemanager.h"
#include "devicemanager.h"
#include "audiocontroller.h"
#include "audiostreamcontroller.h"
#include "audiodevice.h"
#include "audiodataplane.h"
#include "midicontroller.h"
#include "midiportcontroller.h"
#include "mididataplane.h"
#include "miditypes.h"

namespace MinimalAudioEngine::Control
{

// Forward declarations
struct AudioMessage;
class WavFile;
class MidiFile;

// Type definitions
typedef std::shared_ptr<class Track> TrackPtr;
typedef std::variant<AudioDevice, WavFilePtr, std::nullopt_t> AudioIOVariant;
typedef std::variant<MidiDevice, MidiFilePtr, std::nullopt_t> MidiIOVariant;

typedef std::function<void(const MidiNoteMessage&, TrackPtr)> MidiNoteOnCallbackFunc;
typedef std::function<void(const MidiNoteMessage&, TrackPtr)> MidiNoteOffCallbackFunc;
typedef std::function<void(const MidiControlMessage&, TrackPtr)> MidiControlCallbackFunc;

/** @enum eTrackEvent
 *  @brief Track events to handle in callbacks.
 */
enum class eTrackEvent
{
  PlaybackFinished
};

typedef std::function<void(eTrackEvent)> TrackEventCallback;

/** @struct TrackStatistics
 *  @brief Statistics related to Track operations.
 */
struct TrackStatistics
{
  Data::AudioOutputStatistics audio_output_stats;
  Data::MidiInputStatistics midi_input_stats;

  std::string to_string() const
  {
    return "TrackStatistics(\nAudio Output = " + audio_output_stats.to_string() + "\nMIDI Input = " + midi_input_stats.to_string() + "\n)";
  }
};

/** @class Track
 *  @brief The Track can one handle audio or MIDI input and output.
 */
class Track : public std::enable_shared_from_this<Track>
{
public:
  Track():
    m_audio_input(std::nullopt),
    m_midi_input(std::nullopt),
    m_midi_output(std::nullopt),
    m_audio_controller(AudioStreamController::instance()),
    m_midi_controller(MidiPortController::instance()),
    p_audio_dataplane(std::make_shared<Data::AudioDataPlane>()),
    p_midi_dataplane(std::make_shared<Data::MidiDataPlane>())
  {}

  ~Track() = default;

  // Audio/MIDI IO

  /** @brief Add an audio input to the track. 
   *  @param device The audio input device or file retrieved from DeviceManager or FileManager.
   */
  void add_audio_input(const AudioIOVariant &input);

  /** @brief Add a MIDI input to the track.
   *  @param device The MIDI input device or file retrieved from DeviceManager or FileManager.
   */
  void add_midi_input(const MidiIOVariant &input);

  /** @brief Add a MIDI output to the track.
   *  @param device The MIDI output device or file retrieved from DeviceManager or FileManager.
   */
  void add_midi_output(const MidiIOVariant &output);

  /** @brief Remove the audio input from the track. */
  void remove_audio_input();

  /** @brief Remove the MIDI input from the track. */
  void remove_midi_input();

  /** @brief Remove the MIDI output from the track. */
  void remove_midi_output();

  /** @brief Check if the track has an audio input.
   *  @return True if an audio input is configured, false otherwise.
   */
  bool has_audio_input() const;

  /** @brief Check if the track has a MIDI input.
   *   @return True if a MIDI input is configured, false otherwise.
   */
  bool has_midi_input() const;

  /** @brief Check if the track has a MIDI output.
   *  @return True if a MIDI output is configured, false otherwise.
   */
  bool has_midi_output() const;

  /** @brief Gets the audio input of the track.
   *  @return An audio input variant type (AudioDevice, WavFilePtr, std::nullopt_t).
   */
  AudioIOVariant get_audio_input() const;

  /** @brief Gets the MIDI input of the track.
   *  @return A MIDI input variant type (MidiDevice, MidiFilePtr, std::nullopt_t).
   */
  MidiIOVariant get_midi_input() const;

  /** @brief Gets the MIDI output of the track.
   *  @return The MIDI output variant (device, file, or nullopt).
   */
  MidiIOVariant get_midi_output() const;

  // Playback control
  /** @brief Start playback of the track. */
  void play();

  /** @brief Stop playback of the track. */
  void stop();

  /** @brief Check if the track is currently playing.
   *  @return True if the track is playing, false otherwise.
   */
  bool is_playing() const
  {
    return m_audio_controller.get_stream_state() == eAudioState::Playing;
  }
  
  TrackStatistics get_statistics() const
  {
    TrackStatistics stats;
    stats.audio_output_stats = p_audio_dataplane->get_audio_output_statistics();
    stats.midi_input_stats = p_midi_dataplane->get_statistics();
    return stats;
  }

  /** @brief Set a callback function for track events.
   *  @param callback The callback function to set e.g. `void playback_func(MinimalAudioEngine::eTrackEvent event)`.
   */
  void set_event_callback(TrackEventCallback callback)
  {
    m_event_callback = callback;
  }

  // MIDI Callbacks

  /** @brief Set a callback function for MIDI note on events.
   *  @param callback The callback function to set e.g. `void note_on_func(const MinimalAudioEngine::MidiNoteMessage& message)`.
   */
  void set_midi_note_on_callback(MidiNoteOnCallbackFunc callback)
  {
    m_note_on_callback = callback;
  }
  /** @brief Set a callback function for MIDI note off events.
   *  @param callback The callback function to set e.g. `void note_off_func(const MinimalAudioEngine::MidiNoteMessage& message)`.
   */
  void set_midi_note_off_callback(MidiNoteOffCallbackFunc callback)
  {
    m_note_off_callback = callback;
  }

  /** @brief Set a callback function for MIDI control change events.
   *  @param callback The callback function to set e.g. `void control_change_func(const MinimalAudioEngine::MidiControlMessage& message)`.
   */
  void set_midi_control_change_callback(MidiControlCallbackFunc callback)
  {
    m_control_change_callback = callback;
  }

  void handle_midi_message(const MidiMessage& message); // TODO - Remove

  Data::TrackAudioDataPlanePtr get_audio_dataplane() const
  {
    return p_audio_dataplane;
  }

  Data::TrackMidiDataPlanePtr get_midi_dataplane() const
  {
    return p_midi_dataplane;
  }

  std::string to_string() const;

private:
  std::queue<MidiMessage> m_message_queue; // TODO - Remove?
  std::mutex m_queue_mutex; // TODO - Remove?

  IAudioController &m_audio_controller;
  IMidiController &m_midi_controller;

  Data::TrackAudioDataPlanePtr p_audio_dataplane;
  Data::TrackMidiDataPlanePtr p_midi_dataplane;

  TrackEventCallback m_event_callback;

  AudioIOVariant m_audio_input;
  MidiIOVariant m_midi_input;
  MidiIOVariant m_midi_output;

  MidiNoteOnCallbackFunc m_note_on_callback;
  MidiNoteOffCallbackFunc m_note_off_callback;
  MidiControlCallbackFunc m_control_change_callback;
};

}  // namespace MinimalAudioEngine::Control

#endif  // __TRACK_H__