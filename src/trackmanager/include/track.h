#ifndef __TRACK_H__
#define __TRACK_H__

#include <queue>
#include <mutex>
#include <memory>
#include <optional>
#include <variant>
#include <atomic>
#include <string>

#include "observer.h"
#include "midiengine.h"
#include "filemanager.h"
#include "devicemanager.h"
#include "audiodevice.h"

// Forward declarations
namespace MinimalAudioEngine
{
  struct AudioMessage;
}

namespace MinimalAudioEngine
{
  class WavFile;
  class MidiFile;
}

namespace MinimalAudioEngine
{

// Type definitions
typedef std::shared_ptr<class Track> TrackPtr;
typedef std::variant<MinimalAudioEngine::AudioDevice, MinimalAudioEngine::WavFilePtr, std::nullopt_t> AudioIOVariant;
typedef std::variant<MinimalAudioEngine::MidiDevice, MinimalAudioEngine::MidiFilePtr, std::nullopt_t> MidiIOVariant;

/** @class Track
 *  @brief The Track class represents a track in the Digital Audio Workstation.
 */
class Track : public Observer<MinimalAudioEngine::MidiMessage>, 
          public Observer<MinimalAudioEngine::AudioMessage>,
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
  void add_audio_device_input(const MinimalAudioEngine::AudioDevice &device);
  void add_audio_file_input(const MinimalAudioEngine::WavFilePtr wav_file);
  void add_midi_device_input(const MinimalAudioEngine::MidiDevice &device);
  void add_midi_file_input(const MinimalAudioEngine::MidiFilePtr midi_file);

  // Audio/MIDI Outputs
  void add_audio_device_output(const MinimalAudioEngine::AudioDevice& device);
  void add_midi_device_output(const MinimalAudioEngine::MidiDevice& device);

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

  // Observer interface
  void update(const MinimalAudioEngine::MidiMessage& message) override;
  void update(const MinimalAudioEngine::AudioMessage& message) override;

  void handle_midi_message();

  void get_next_audio_frame(float *output_buffer, unsigned int frames, unsigned int channels, unsigned int sample_rate);

  std::string to_string() const;

private:
  std::queue<MinimalAudioEngine::MidiMessage> m_message_queue;
  std::mutex m_queue_mutex;

  AudioIOVariant m_audio_input;
  MidiIOVariant m_midi_input;
  AudioIOVariant m_audio_output;
  MidiIOVariant m_midi_output;

  // TEST
  std::atomic<double> m_test_tone_phase{0.0};
};

}  // namespace MinimalAudioEngine

#endif  // __TRACK_H__