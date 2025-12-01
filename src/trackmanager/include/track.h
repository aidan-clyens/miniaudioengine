#ifndef __TRACK_H__
#define __TRACK_H__

#include <queue>
#include <mutex>
#include <memory>
#include <optional>

#include "observer.h"
#include "midiengine.h"

// Forward declaration
namespace Audio
{
  struct AudioMessage;
}

namespace Files
{
  class WavFile;
  class MidiFile;
}

namespace Tracks
{

/** @class Track
 *  @brief The Track class represents a track in the Digital Audio Workstation.
 */
class Track : public Observer<Midi::MidiMessage>, 
          public Observer<Audio::AudioMessage>,
          public std::enable_shared_from_this<Track>
{
public:
  Track() = default;

  void add_audio_input(const unsigned int device_id = 0);
  void add_audio_file_input(const std::shared_ptr<Files::WavFile> &wav_file);
  void add_midi_input(const unsigned int device_id = 0);
  void add_midi_file_input(const Files::MidiFile &midi_file);
  void add_audio_output(const unsigned int device_id = 0);

  bool has_audio_input() const { return m_audio_input_device_id.has_value(); }
  bool has_midi_input() const { return m_midi_input_device_id.has_value(); }
  bool has_audio_output() const { return m_audio_output_device_id.has_value(); }

  unsigned int get_audio_input_id() const { return m_audio_input_device_id.value_or(std::numeric_limits<unsigned int>::max()); }
  unsigned int get_midi_input_id() const { return m_midi_input_device_id.value_or(std::numeric_limits<unsigned int>::max()); }
  unsigned int get_audio_output() const { return m_audio_output_device_id.value_or(std::numeric_limits<unsigned int>::max()); }

  void play();
  void stop();

  // Observer interface
  void update(const Midi::MidiMessage& message) override;
  void update(const Audio::AudioMessage& message) override;

  void handle_midi_message();

  void get_next_audio_frame(float *output_buffer, unsigned int n_frames);

  std::string to_string() const
  {
    return "Track(AudioInputID=" + std::to_string(get_audio_input_id()) +
           ", MidiInputID=" + std::to_string(get_midi_input_id()) +
           ", AudioOutputID=" + std::to_string(get_audio_output()) + ")";
  }

private:
  std::queue<Midi::MidiMessage> m_message_queue;
  std::mutex m_queue_mutex;

  std::optional<unsigned int> m_audio_input_device_id;
  std::optional<unsigned int> m_midi_input_device_id;
  std::optional<unsigned int> m_audio_output_device_id;
};

}  // namespace Tracks

#endif  // __TRACK_H__