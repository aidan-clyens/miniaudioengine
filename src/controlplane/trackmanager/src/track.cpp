#include "track.h"

#include "wavfile.h"
#include "midifile.h"
#include "audiostreamcontroller.h"

#include <iostream>
#include <stdexcept>
#include <memory>

// Define M_PI if not already defined (Windows MSVC compatibility)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace MinimalAudioEngine;

/** @brief Adds an audio input to the track.
 *  @param input The audio input device or file.
 */
void Track::add_audio_input(const AudioIOVariant& input)
{
  if (has_audio_input())
  {
    throw std::runtime_error("This track already has an audio input.");
  }

  // Verify the audio device has input channels
  if (std::holds_alternative<AudioDevice>(input) && std::get<AudioDevice>(input).input_channels < 1)
  {
    throw std::runtime_error("Selected audio device " + std::get<AudioDevice>(input).name + " has no input channels.");
  }

  m_audio_input = input;

  if (std::holds_alternative<AudioDevice>(input))
  {
    LOG_INFO("Track: Added audio input device: ", std::get<AudioDevice>(input).to_string());
  }
  else if (std::holds_alternative<WavFilePtr>(input))
  {
    LOG_INFO("Track: Added audio input file: ", std::get<WavFilePtr>(input)->to_string());
  }
}

/** @brief Adds a MIDI input to the track.
 *  @param input The MIDI input device or file.
 */
void Track::add_midi_input(const MidiIOVariant& input)
{
  if (has_midi_input())
  {
    throw std::runtime_error("This track already has a MIDI input.");
  }

  m_midi_input = input;

  // TODO - Move to dataplane
  if (std::holds_alternative<MidiDevice>(input))
  {
    MinimalAudioEngine::MidiEngine::instance().open_input_port(std::get<MidiDevice>(input).id);
    MinimalAudioEngine::MidiEngine::instance().attach(shared_from_this());
  }

  LOG_INFO("Track: Added MIDI input: ", std::get<MidiDevice>(input).to_string());
}

/** @brief Adds a MIDI output to the track.
 *  @param device The MIDI output device or file.
 */
void Track::add_midi_output(const MidiIOVariant &output)
{
  if (has_midi_output())
  {
    throw std::runtime_error("This track already has a MIDI output.");
  }

  m_midi_output = output;

  LOG_INFO("Track: Added MIDI output device: ", std::get<MidiDevice>(output).name);
}

/** @brief Removes the audio input from the track.
 */
void Track::remove_audio_input()
{
  m_audio_input = std::nullopt;
}

/** @brief Removes the MIDI input from the track.
 */
void Track::remove_midi_input()
{
  m_midi_input = std::nullopt;

  // TODO - Move to dataplane
  MinimalAudioEngine::MidiEngine::instance().close_input_port();
  MinimalAudioEngine::MidiEngine::instance().detach(shared_from_this());
}

/** @brief Removes the MIDI output from the track.
 */
void Track::remove_midi_output()
{
  m_midi_output = std::nullopt;
}

/** @brief Checks if the track has an audio input configured.
 *  @return True if an audio input is configured, false otherwise.
 */
bool Track::has_audio_input() const
{
  return !std::holds_alternative<std::nullopt_t>(m_audio_input);
}

/** @brief Checks if the track has a MIDI input configured.
 *  @return True if a MIDI input is configured, false otherwise.
 */
bool Track::has_midi_input() const
{
  return !std::holds_alternative<std::nullopt_t>(m_midi_input);
}

/** @brief Checks if the track has a MIDI output configured.
 *  @return True if a MIDI output is configured, false otherwise.
 */
bool Track::has_midi_output() const
{
  return !std::holds_alternative<std::nullopt_t>(m_midi_output);
}

/** @brief Gets the audio input of the track.
 *  @return The audio input variant (device, file, or nullopt). 
 */
AudioIOVariant Track::get_audio_input() const
{
  return m_audio_input;
}

/** @brief Gets the MIDI input of the track.
 *  @return The MIDI input variant (device, file, or nullopt).
 */
MidiIOVariant Track::get_midi_input() const
{
  return m_midi_input;
}

/** @brief Gets the MIDI output of the track.
 *  @return The MIDI output variant (device, file, or nullopt).
 */
MidiIOVariant Track::get_midi_output() const
{
  return m_midi_output;
}

void Track::play()
{
  LOG_INFO("Track: Play...");
  m_is_playing = true;
  AudioStreamController::instance().start_stream();
}

void Track::stop()
{
  LOG_INFO("Track: Stop...");
  m_is_playing = false;
  AudioStreamController::instance().stop_stream();
}

/** @brief Updates the track with a new MIDI message.
 *  This function is called by the MidiEngine when a new MIDI message is received.
 *  @param message The MIDI message to process.
 */
void Track::update(const MidiMessage& message)
{
  handle_midi_message(message);
}

/** @brief Updates the track with a new audio message.
 *  This function is called by the AudioEngine when a new audio message is received.
 *  @param message The audio message to process.
 */
void Track::update(const AudioMessage &message)
{
  // if (message.command == eAudioEngineCommand::StoppedPlayback)
  // {
  //   LOG_INFO("Track: Received AudioEngine Stop notification.");
  //   if (m_event_callback)
  //   {
  //     m_event_callback(eTrackEvent::PlaybackFinished);
  //   }
  // }
}

/** @brief Handles a MIDI message.
 *  This function processes the MIDI message received from the MidiEngine.
 *  @param message The MIDI message to handle.
 */
void Track::handle_midi_message(const MidiMessage& message)
{
  LOG_INFO("Track: Handling MIDI message: ", message.to_string());

  // Process the MIDI message here
  switch (message.type)
  {
    case MinimalAudioEngine::eMidiMessageType::NoteOn:
    {
      MidiNoteMessage note_on_msg = static_cast<const MidiNoteMessage&>(message);
      LOG_INFO("Track: Note On - ", note_on_msg.to_string());
      m_note_on_callback(note_on_msg, shared_from_this());
      break;
    }
    case MinimalAudioEngine::eMidiMessageType::NoteOff:
    {
      MidiNoteMessage note_off_msg = static_cast<const MidiNoteMessage&>(message);
      LOG_INFO("Track: Note Off - ", note_off_msg.to_string());
      m_note_off_callback(note_off_msg, shared_from_this());
      break;
    }
    case MinimalAudioEngine::eMidiMessageType::ControlChange:
    {
      MidiControlMessage control_change_msg = static_cast<const MidiControlMessage&>(message);
      LOG_INFO("Track: Control Change - ", control_change_msg.to_string());
      m_control_change_callback(control_change_msg, shared_from_this());
      break;
    }
    default:
      LOG_INFO("Track: Unknown MIDI Message Type - ", message.type_name);
      break;
  }
}

/** @brief Fill the audio output buffer with the next available data
 *  @param output_buffer Pointer to the output buffer where audio data will be written.
 *  @param frames Number of frames to fill in the output buffer.
 *  @param channels Number of output audio channels.
 *  @param sample_rate Sample rate of the audio data.
 */
void Track::get_next_audio_frame(float *output_buffer, unsigned int frames, unsigned int channels, unsigned int sample_rate)
{
  LOG_INFO("Track: get_next_audio_frame with ", frames, " frames.", 
           " Channels: ", channels, 
           " Sample Rate: ", sample_rate);

  if (output_buffer == nullptr)
  {
    LOG_ERROR("Track: Null output buffer in get_next_audio_frame");
    return;
  }

  if (frames == 0)
  {
    LOG_ERROR("Track: Zero frames requested in get_next_audio_frame");
    return;
  }

  if (channels == 0)
  {
    LOG_ERROR("Track: Zero channels requested in get_next_audio_frame");
    return;
  }

  if (sample_rate == 0)
  {
    LOG_ERROR("Track: Zero sample rate requested in get_next_audio_frame");
    return;
  }

  // if (!has_audio_input())
  // {
  //   // No audio input configured, fill with silence
  //   LOG_INFO("Track: No audio input configured, filling output buffer with silence.");
  //   std::fill(output_buffer, output_buffer + frames * channels, 0.0f);
  //   return;
  // }

  // If audio input is a WAV file, read data from it
  if (std::holds_alternative<MinimalAudioEngine::WavFilePtr>(m_audio_input))
  {
    MinimalAudioEngine::WavFilePtr wav_file = std::get<MinimalAudioEngine::WavFilePtr>(m_audio_input);
    LOG_INFO("Track: Reading next audio frame from WAV file: ", wav_file->to_string());

    unsigned int file_channels = wav_file->get_channels();
    unsigned int samples_to_read = frames * file_channels;

    std::vector<float> file_buffer(samples_to_read, 0.0f);
    sf_count_t read_frames = wav_file->read_frames(file_buffer, frames);

    if (read_frames != frames)
    {
      LOG_INFO("Track: Reached end of WAV file or read less frames than requested. Stopping playback.");
      // Fill remaining buffer with silence
      for (unsigned int i = read_frames * channels; i < frames * channels; ++i)
      {
        output_buffer[i] = 0.0f;
      }

      // Stop playback if end of file reached
      stop();
    }

    // Add data to output buffer, handling channel mismatch
    for (unsigned int i = 0; i < static_cast<unsigned int>(read_frames); ++i)
    {
      for (unsigned int ch = 0; ch < channels; ++ch)
      {
        if (ch < file_channels)
        {
          output_buffer[i * channels + ch] = file_buffer[i * file_channels + ch];
        }
        else
        {
          output_buffer[i * channels + ch] = 0.0f; // Fill extra channels with silence
        }
      }
    }
  }

  // Audio processing nodes
  for (auto node_ptr : m_audio_processing_nodes)
  {
    LOG_INFO("Track: Processing audio frame with processing node: ", node_ptr->to_string());
    node_ptr->get_next_audio_frame(output_buffer, frames, channels, sample_rate);
  }
}

std::string Track::to_string() const
{
  AudioIOVariant audio_input = get_audio_input();
  MidiIOVariant midi_input = get_midi_input();
  MidiIOVariant midi_output = get_midi_output();

  std::string audio_input_str = std::holds_alternative<std::nullopt_t>(audio_input) ? "None" :
                                std::holds_alternative<MinimalAudioEngine::AudioDevice>(audio_input) ? std::get<MinimalAudioEngine::AudioDevice>(audio_input).to_string() :
                                std::get<MinimalAudioEngine::WavFilePtr>(audio_input)->to_string();

  std::string midi_input_str = std::holds_alternative<std::nullopt_t>(midi_input) ? "None" :
                               std::holds_alternative<MinimalAudioEngine::MidiDevice>(midi_input) ? std::get<MinimalAudioEngine::MidiDevice>(midi_input).to_string() :
                               std::get<MinimalAudioEngine::MidiFilePtr>(midi_input)->to_string();

  std::string midi_output_str = std::holds_alternative<std::nullopt_t>(midi_output) ? "None" :
                                std::holds_alternative<MinimalAudioEngine::MidiDevice>(midi_output) ? std::get<MinimalAudioEngine::MidiDevice>(midi_output).to_string() :
                                std::get<MinimalAudioEngine::MidiFilePtr>(midi_output)->to_string();

  // TODO - Include audio processing nodes in the string representation

  return "Track(AudioInput=" + audio_input_str +
         ", MidiInput=" + midi_input_str +
         ", MidiOutput=" + midi_output_str + ")";
}