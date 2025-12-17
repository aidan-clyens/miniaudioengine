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

#include "observer.h"
#include "midiengine.h"
#include "filemanager.h"
#include "devicemanager.h"
#include "audiodevice.h"

namespace MinimalAudioEngine
{

// Forward declarations
struct AudioMessage;
class WavFile;
class MidiFile;
  
// Type definitions
typedef std::shared_ptr<class Track> TrackPtr;
typedef std::variant<AudioDevice, WavFilePtr, std::nullopt_t> AudioIOVariant;
typedef std::variant<MidiDevice, MidiFilePtr, std::nullopt_t> MidiIOVariant;

/** @enum eTrackEvent
 *  @brief Track events to handle in callbacks.
 */
enum class eTrackEvent
{
  PlaybackFinished
};

typedef std::function<void(eTrackEvent)> TrackEventCallback;

/** @class Track
 *  @brief The Track can one handle audio or MIDI input and output.
 *  It implements the Observer pattern to receive MIDI and audio messages.
 */
class Track : public Observer<MidiMessage>, 
          public Observer<AudioMessage>,
          public std::enable_shared_from_this<Track>
{
public:
  Track():
    m_audio_input(std::nullopt),
    m_midi_input(std::nullopt),
    m_audio_output(std::nullopt),
    m_midi_output(std::nullopt)
  {}

  ~Track() = default;

  // Audio/MIDI Inputs
  void add_audio_device_input(const AudioDevice &device);
  void add_audio_file_input(const WavFilePtr wav_file);
  void add_midi_device_input(const MidiDevice &device);
  void add_midi_file_input(const MidiFilePtr midi_file);

  // Audio/MIDI Outputs
  void add_audio_device_output(const AudioDevice& device);
  void add_midi_device_output(const MidiDevice& device);
  void remove_audio_input();
  void remove_midi_input();
  void remove_audio_output();
  void remove_midi_output();

  bool has_audio_input() const;
  bool has_midi_input() const;
  bool has_audio_output() const;
  bool has_midi_output() const;

  AudioIOVariant get_audio_input() const;
  MidiIOVariant get_midi_input() const;
  AudioIOVariant get_audio_output() const;
  MidiIOVariant get_midi_output() const;

  void play();
  void stop();

  void set_event_callback(TrackEventCallback callback)
  {
    m_event_callback = callback;
  }

  // Observer interface
  void update(const MidiMessage& message) override;
  void update(const AudioMessage& message) override;

  void handle_midi_message(const MidiMessage& message);

  void get_next_audio_frame(float *output_buffer, unsigned int frames, unsigned int channels, unsigned int sample_rate);

  std::string to_string() const;

private:
  std::queue<MidiMessage> m_message_queue;
  std::mutex m_queue_mutex;

  TrackEventCallback m_event_callback;

  AudioIOVariant m_audio_input;
  MidiIOVariant m_midi_input;
  AudioIOVariant m_audio_output;
  MidiIOVariant m_midi_output;

  // TEST
  std::atomic<double> m_test_tone_phase{0.0};
};

}  // namespace MinimalAudioEngine

#endif  // __TRACK_H__