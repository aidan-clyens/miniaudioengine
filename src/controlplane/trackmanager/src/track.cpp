#include "track.h"

#include "wavfile.h"
#include "midifile.h"
#include "midiportcontroller.h"
#include "logger.h"

#include <iostream>
#include <stdexcept>
#include <memory>

// Define M_PI if not already defined (Windows MSVC compatibility)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace MinimalAudioEngine::Control;
using namespace MinimalAudioEngine::Data;

// ============================================================================
// Hierarchy Management
// ============================================================================

/** @brief Add a child track to this track.
 *  @param child The child track to add.
 *  @throws std::runtime_error if the child already has a parent or if adding would create a cycle.
 */
void Track::add_child_track(TrackPtr child)
{
  if (!child)
  {
    throw std::runtime_error("Cannot add null child track.");
  }

  if (child->has_parent())
  {
    throw std::runtime_error("Child track already has a parent. Remove from parent first.");
  }

  // Prevent adding self as child
  if (child.get() == this)
  {
    throw std::runtime_error("Cannot add track as its own child.");
  }

  // Prevent cycles: check if this track is a descendant of child
  TrackPtr current = shared_from_this();
  while (current)
  {
    if (current == child)
    {
      throw std::runtime_error("Cannot add child: would create a cycle in hierarchy.");
    }
    current = current->get_parent();
  }

  std::lock_guard<std::mutex> lock(m_hierarchy_mutex);
  m_children.push_back(child);
  child->m_parent = shared_from_this();

  LOG_INFO("Track: Added child track. Total children: ", m_children.size());
}

/** @brief Remove a child track from this track.
 *  @param child The child track to remove.
 */
void Track::remove_child_track(TrackPtr child)
{
  if (!child)
  {
    return;
  }

  std::lock_guard<std::mutex> lock(m_hierarchy_mutex);
  auto it = std::find(m_children.begin(), m_children.end(), child);
  if (it != m_children.end())
  {
    child->m_parent.reset();
    m_children.erase(it);
    LOG_INFO("Track: Removed child track. Total children: ", m_children.size());
  }
}

/** @brief Remove this track from its parent.
 */
void Track::remove_from_parent()
{
  TrackPtr parent = get_parent();
  if (parent)
  {
    parent->remove_child_track(shared_from_this());
  }
}

/** @brief Get the parent track.
 *  @return Shared pointer to parent track, or nullptr if no parent.
 */
TrackPtr Track::get_parent() const
{
  return m_parent.lock();
}

/** @brief Check if this track has a parent.
 *  @return True if track has a parent.
 */
bool Track::has_parent() const
{
  return !m_parent.expired();
}

// ============================================================================
// Audio/MIDI Input/Output
// ============================================================================

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
    p_audio_dataplane->set_input_channels(std::get<AudioDevice>(input).input_channels);
  }
  else if (std::holds_alternative<WavFilePtr>(input))
  {
    LOG_INFO("Track: Added audio input file: ", std::get<WavFilePtr>(input)->to_string());
    p_audio_dataplane->set_input_channels(std::get<WavFilePtr>(input)->get_channels());
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

  if (std::holds_alternative<MidiDevice>(input))
  {
    LOG_INFO("Track: Added MIDI input: ", std::get<MidiDevice>(input).to_string());
    m_midi_controller.open_input_port(std::get<MidiDevice>(input).id);
  }
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
  m_midi_controller.close_input_port();
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

/** @brief Starts playback of the track.
 */
void Track::play()
{
  LOG_INFO("Track: Play...");

  // If already playing, do nothing
  if (is_playing())
  {
    LOG_WARNING("Track: Already playing.");
    return;
  }

  // If audio input is a WAV file, start producer thread BEFORE starting audio stream
  if (std::holds_alternative<WavFilePtr>(m_audio_input))
  {
    p_audio_dataplane->start();
    WavFilePtr wav_file = std::get<WavFilePtr>(m_audio_input);
    p_audio_dataplane->preload_wav_file(wav_file); // Preload WAV file data
  }

  // If MIDI input is a MIDI device, ensure the port is open
  if (std::holds_alternative<MidiDevice>(m_midi_input))
  {
    p_midi_dataplane->start();
    MidiDevice midi_device = std::get<MidiDevice>(m_midi_input);
    m_midi_controller.open_input_port(midi_device.id);
  }

  if (!m_audio_controller.start_stream())
  {
    return;
  }
}

/** @brief Stops playback of the track.
 */
void Track::stop()
{
  LOG_INFO("Track: Stop...");

  // If not playing, do nothing
  if (!is_playing())
  {
    LOG_WARNING("Track: Not currently playing.");
    return;
  }

  // Clear dataplane buffers and stop any data processing threads
  p_audio_dataplane->stop();
  m_audio_controller.stop_stream();
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
    case eMidiMessageType::NoteOn:
    {
      MidiNoteMessage note_on_msg = static_cast<const MidiNoteMessage&>(message);
      LOG_INFO("Track: Note On - ", note_on_msg.to_string());
      m_note_on_callback(note_on_msg, shared_from_this());
      break;
    }
    case eMidiMessageType::NoteOff:
    {
      MidiNoteMessage note_off_msg = static_cast<const MidiNoteMessage&>(message);
      LOG_INFO("Track: Note Off - ", note_off_msg.to_string());
      m_note_off_callback(note_off_msg, shared_from_this());
      break;
    }
    case eMidiMessageType::ControlChange:
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

std::string Track::to_string() const
{
  AudioIOVariant audio_input = get_audio_input();
  MidiIOVariant midi_input = get_midi_input();
  MidiIOVariant midi_output = get_midi_output();

  std::string audio_input_str = std::holds_alternative<std::nullopt_t>(audio_input) ? "None" :
                                std::holds_alternative<AudioDevice>(audio_input) ? std::get<AudioDevice>(audio_input).to_string() :
                                std::get<WavFilePtr>(audio_input)->to_string();

  std::string midi_input_str = std::holds_alternative<std::nullopt_t>(midi_input) ? "None" :
                               std::holds_alternative<MidiDevice>(midi_input) ? std::get<MidiDevice>(midi_input).to_string() :
                               std::get<MidiFilePtr>(midi_input)->to_string();

  std::string midi_output_str = std::holds_alternative<std::nullopt_t>(midi_output) ? "None" :
                                std::holds_alternative<MidiDevice>(midi_output) ? std::get<MidiDevice>(midi_output).to_string() :
                                std::get<MidiFilePtr>(midi_output)->to_string();

  return "Track(AudioInput=" + audio_input_str +
         ", MidiInput=" + midi_input_str +
         ", MidiOutput=" + midi_output_str + ")";
}