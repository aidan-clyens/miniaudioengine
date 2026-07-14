#include "track.h"
#include "trackservice.h"
#include "io.h"
#include "device.h"
#include "file.h"
#include "miditypes.h"
#include "logger.h"

#include <iostream>
#include <stdexcept>
#include <memory>

// Define M_PI if not already defined (Windows MSVC compatibility)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace miniaudioengine::framework;
using namespace miniaudioengine;

// ============================================================================
// Audio/MIDI Input/Output
// ============================================================================

/** @brief Adds an audio input to the track.
 *  @param input The audio input device or file.
 */
void Track::add_audio_input(IInputOutputPtr input)
{
  if (has_audio_input())
  {
    throw std::runtime_error("This Track already has an Audio Input.");
  }

  if (input->get_type() == framework::eInputOutputType_Device)
  {
    DevicePtr device = std::dynamic_pointer_cast<Device>(input);
    if (!device->is_input())
    {
      throw std::runtime_error("Selected audio device " + device->get_name() + " has no input channels.");
    }

    LOG_INFO("Track: Added Audio Input - ", device->to_string());
    p_audio_input = input;
  }

  if (input->get_type() == framework::eInputOutputType_File)
  {
    FilePtr file = std::dynamic_pointer_cast<File>(input);
    LOG_INFO("Track: Added Audio Input - ", file->to_string());
    p_audio_input = input;
  }
}

/** @brief Adds a MIDI input to the track.
 *  @param input The MIDI input device or file.
 */
void Track::add_midi_input(IInputOutputPtr input)
{
  if (has_midi_input())
  {
    LOG_ERROR("Track: Cannot add MIDI Input - already has one configured.");
    throw std::runtime_error("This track already has a MIDI Input.");
  }

  if (input->get_type() == framework::eInputOutputType_Device)
  {
    auto device = std::dynamic_pointer_cast<Device>(input);
    if (!device->is_input())
    {
      throw std::runtime_error("Selected MIDI device " + device->get_name() + " has no input channels.");
    }

    LOG_INFO("Track: Added MIDI Input Device: ", device->to_string());
    p_midi_input = input;
  }
}

/** @brief Adds a MIDI output to the track.
 *  @param output The MIDI output device or file.
 */
void Track::add_audio_output(IInputOutputPtr output)
{
  if (has_audio_output())
  {
    LOG_ERROR("Track: Cannot add Audio output - already has one configured.");
    throw std::runtime_error("This track already has an Audio Output.");
  }

  if (output->get_type() == framework::eInputOutputType_Device)
  {
    auto device = std::dynamic_pointer_cast<Device>(output);
    if (!device->is_output())
    {
      throw std::runtime_error("Selected Audio Device " + device->get_name() + " has no output channels.");
    }

    LOG_INFO("Track: Added Audio Output - ", device->to_string());
    p_audio_output = output;
  }

  if (output->get_type() == framework::eInputOutputType_File)
  {
    auto file = std::dynamic_pointer_cast<File>(output);
    LOG_INFO("Track: Added Audio Output - ", file->to_string());
    p_audio_output = output;
  }
}

/** @brief Adds a MIDI output to the track.
 *  @param device The MIDI output device or file.
 */
void Track::add_midi_output(IInputOutputPtr output)
{
  if (has_midi_output())
  {
    LOG_ERROR("Track: Cannot add MIDI output - already has one configured.");
    throw std::runtime_error("This track already has a MIDI output.");
  }

  if (output->get_type() == framework::eInputOutputType_Device)
  {
    auto device = std::dynamic_pointer_cast<Device>(output);
    if (!device->is_output())
    {
      LOG_ERROR("Track: Selected MIDI device ", device->get_name(), " has no output channels.");
      throw std::runtime_error("Selected MIDI device has no output channels.");
    }

    LOG_INFO("Track: Added MIDI output device: ", device->to_string());
    p_midi_output = output;
  }
}

/** @brief Removes the audio input from the track.
 */
void Track::remove_audio_input()
{
  p_audio_input = nullptr;
}

/** @brief Removes the MIDI input from the track.
 */
void Track::remove_midi_input()
{
  p_midi_input = nullptr;
}

/** @brief Removes the audio output from the track.
 */
void Track::remove_audio_output()
{
  p_audio_output = nullptr;
}

/** @brief Removes the MIDI output from the track.
 */
void Track::remove_midi_output()
{
  p_midi_output = nullptr;
}

/** @brief Checks if the track has an audio input configured.
 *  @return True if an audio input is configured, false otherwise.
 */
bool Track::has_audio_input() const
{
  return p_audio_input != nullptr;
}

/** @brief Checks if the track has an audio output configured.
 *  @return True if an audio output is configured, false otherwise.
 */
bool Track::has_audio_output() const
{
  return p_audio_output != nullptr;
}

/** @brief Checks if the track has a MIDI input configured.
 *  @return True if a MIDI input is configured, false otherwise.
 */
bool Track::has_midi_input() const
{
  return p_midi_input != nullptr;
}

/** @brief Checks if the track has a MIDI output configured.
 *  @return True if a MIDI output is configured, false otherwise.
 */
bool Track::has_midi_output() const
{
  return p_midi_output != nullptr;
}

/** @brief Gets the audio input of the track.
 *  @return The audio input (DevicePtr, FilePtr). 
 */
IInputOutputPtr Track::get_audio_input() const
{
  return p_audio_input;
}

/** @brief Gets the audio output of the track.
 *  @return The audio output (DevicePtr, FilePtr).
 */
IInputOutputPtr Track::get_audio_output() const
{
  return p_audio_output;
}

/** @brief Gets the MIDI input of the track.
 *  @return The MIDI input variant (DevicePtr, FilePtr).
 */
IInputOutputPtr Track::get_midi_input() const
{
  return p_midi_input;
}

/** @brief Gets the MIDI output of the track.
 *  @return The MIDI output variant (DevicePtr, FilePtr).
 */
IInputOutputPtr Track::get_midi_output() const
{
  return p_midi_output;
}

void Track::add_effects_processor(const IProcessorPtr processor)
{
  m_effects_processors.push_back(processor);
}

std::vector<framework::IProcessorPtr> Track::get_effects_processors() const
{
  return m_effects_processors;
}

/** @brief Starts playback of the track.
 *  If the track has an audio input file, it preloads the data and starts the audio dataplane.
 *  If the track has a MIDI input device, it opens the port and starts the MIDI dataplane.
 *  Then the audio stream is started via the audio controller.
 */
bool Track::play()
{
  // If already playing, do nothing
  if (is_playing())
  {
    LOG_WARNING("Track: Already playing.");
    return false;
  }

  m_state = eTrackState::Stopped;

  // Audio Input
  if (has_audio_input())
  {
    LOG_INFO("Track: play - Opening audio input ", get_audio_input()->to_string());
    if (!open_audio_stream(get_audio_input()))
      return false;
  }

  // Audio Output
  if (has_audio_output())
  {
    LOG_INFO("Track: play - Opening audio output ", get_audio_output()->to_string());
    if (!open_audio_stream(get_audio_output()))
      return false;
  }

  // MIDI Input
  if (has_midi_input())
  {
    LOG_INFO("Track: play - Opening MIDI input ", get_midi_input()->to_string());
    if (!open_midi_port(get_midi_input()))
      return false;
  }

  // MIDI Output
  if (has_midi_output())
  {
    LOG_INFO("Track: play - Opening MIDI output ", get_midi_output()->to_string());
    if (!open_midi_port(get_midi_output()))
      return false;
  }

  m_state = eTrackState::Playing;

  LOG_INFO("Track: Started playing.");
  return true;
}

/** @brief Stops playback of the track.
 */
bool Track::stop()
{
  LOG_INFO("Track: Stop...");

  // If not playing, do nothing
  if (!is_playing())
  {
    LOG_WARNING("Track: Not currently playing.");
  }

  if (p_file_adapter->is_audio_stream_open())
    p_file_adapter->close_audio_stream();

  m_state = eTrackState::Stopped;

  return true;
}

/** @brief Handles a MIDI message.
 *  This function processes the MIDI message received from the MidiEngine.
 *  @param message The MIDI message to handle.
 */
void Track::handle_midi_message(const midi::MidiMessage& message)
{
  LOG_INFO("Track: Handling MIDI message: ", message.to_string());

  // Process the MIDI message here
  switch (message.type)
  {
    case midi::eMidiMessageType::NoteOn:
    {
      midi::MidiNoteMessage note_on_msg = static_cast<const midi::MidiNoteMessage&>(message);
      LOG_INFO("Track: Note On - ", note_on_msg.to_string());
      m_note_on_callback(note_on_msg, shared_from_this());
      break;
    }
    case midi::eMidiMessageType::NoteOff:
    {
      midi::MidiNoteMessage note_off_msg = static_cast<const midi::MidiNoteMessage&>(message);
      LOG_INFO("Track: Note Off - ", note_off_msg.to_string());
      m_note_off_callback(note_off_msg, shared_from_this());
      break;
    }
    case midi::eMidiMessageType::ControlChange:
    {
      midi::MidiControlMessage control_change_msg = static_cast<const midi::MidiControlMessage &>(message);
      LOG_INFO("Track: Control Change - ", control_change_msg.to_string());
      m_control_change_callback(control_change_msg, shared_from_this());
      break;
    }
    default:
      LOG_INFO("Track: Unknown MIDI Message Type - ", message.type_name);
      break;
  }
}

/** @brief Check if the track is currently playing.
 *  @return True if the track is playing, false otherwise.
 */
bool Track::is_playing()
{
  return m_state == eTrackState::Playing;
}

bool Track::open_audio_stream(framework::IInputOutputPtr stream)
{
  switch (stream->get_type())
  {
  case framework::eInputOutputType_Device:
  {
    DevicePtr device = std::dynamic_pointer_cast<Device>(stream);
    if (p_audio_adapter->is_stream_open())
    {
      LOG_WARNING("Track: play - Audio stream is already open ", device->to_string());
      if (!p_audio_adapter->close_stream())
      {
        LOG_ERROR("Track: play - Failed to close audio stream ", device->to_string());
        return false;
      }
    }

    if (!p_audio_adapter->open_stream(device))
    {
      LOG_ERROR("Track: play - Failed to open audio stream ", device->to_string());
      return false;
    }
    break;
  }
  case framework::eInputOutputType_File:
  {
    FilePtr file = std::dynamic_pointer_cast<File>(stream);
    if (p_file_adapter->is_audio_stream_open())
    {
      LOG_WARNING("Track: play - Audio stream is already open ", file->to_string());
      if (!p_file_adapter->close_audio_stream())
      {
        LOG_ERROR("Track: play - Failed to close audio stream ", file->to_string());
        return false;
      }
    }

    if (!p_file_adapter->open_audio_stream(file))
    {
      LOG_ERROR("Track: play - Failed to open audio stream ", file->to_string());
      return false;
    }
    break;
  }
  default:
    LOG_WARNING("Track: play - Unsupported audio stream type.");
    return false;
  }

  return true;
}

bool Track::open_midi_port(framework::IInputOutputPtr port)
{
  switch (port->get_type())
  {
  case framework::eInputOutputType_Device:
  {
    DevicePtr device = std::dynamic_pointer_cast<Device>(port);
    if (p_midi_adapter->is_port_open())
    {
      LOG_WARNING("Track: play - MIDI port is already open ", device->to_string());
      if (!p_midi_adapter->close_input_port())
      {
        LOG_ERROR("Track: play - Failed to close MIDI port ", device->to_string());
        return false;
      }
    }

    // TODO - Move MidiAdapter & AudioAdapter to Device class?
    if (!p_midi_adapter->open_input_port(device, nullptr))
    {
      LOG_ERROR("Track: play - Failed to open MIDI port ", device->to_string());
      return false;
    }

    break;
  }
  case framework::eInputOutputType_File:
  {
    FilePtr file = std::dynamic_pointer_cast<File>(port);
    // TODO - Implement file streaming in FileAdapter
    // p_file_adapter->open_midi_port()
    break;
  }
  default:
    LOG_WARNING("Track: play - Unsupported MIDI port type.");
    return false;
  }

  return true;
}

std::string Track::to_string() const
{
  IInputOutputPtr audio_input = get_audio_input();
  IInputOutputPtr audio_output = get_audio_output();
  IInputOutputPtr midi_input = get_midi_input();
  IInputOutputPtr midi_output = get_midi_output();

  std::string audio_input_str = audio_input ?
    (audio_input->get_type() == framework::eInputOutputType_Device  ?
    std::dynamic_pointer_cast<Device>(audio_input)->to_string() : std::dynamic_pointer_cast<File>(audio_input)->to_string()) : "None";

  std::string audio_output_str = audio_output ?
    (audio_output->get_type() == framework::eInputOutputType_Device  ?
    std::dynamic_pointer_cast<Device>(audio_output)->to_string() : std::dynamic_pointer_cast<File>(audio_output)->to_string()) : "None";

  std::string midi_input_str = midi_input ?
    (midi_input->get_type() == framework::eInputOutputType_Device  ?
    std::dynamic_pointer_cast<Device>(midi_input)->to_string() : std::dynamic_pointer_cast<File>(midi_input)->to_string()) : "None";

  std::string midi_output_str = midi_output ?
    (midi_output->get_type() == framework::eInputOutputType_Device  ?
    std::dynamic_pointer_cast<Device>(midi_output)->to_string() : std::dynamic_pointer_cast<File>(midi_output)->to_string()) : "None";

  return "Track(AudioInput=" + audio_input_str +
         ", AudioOutput=" + audio_output_str +
         ", MidiInput=" + midi_input_str +
         ", MidiOutput=" + midi_output_str + ")";
}