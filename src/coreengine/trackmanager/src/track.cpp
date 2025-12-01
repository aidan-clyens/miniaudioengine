#include "track.h"

#include "devicemanager.h"
#include "wavfile.h"
#include "midifile.h"
#include "audioengine.h"

#include <iostream>
#include <stdexcept>
#include <memory>

using namespace Tracks;

/** @brief Adds an audio input to the track.
 *  @param device_id The ID of the audio input device. Defaults to 0 (the default input device).
 */
void Track::add_audio_input(const unsigned int device_id)
{
  Devices::AudioDevice device = Devices::DeviceManager::instance().get_audio_device(device_id);

  // Verify the audio device has input channels
  if (device.input_channels < 1)
  {
    throw std::runtime_error("Selected audio device " + device.name + " has no input channels.");
  }

  m_audio_input_device_id = device_id;

  Audio::AudioEngine::instance().set_output_device(device_id);

  LOG_INFO("Track: Added audio input device: ", device.name);
}

/** @brief Adds a MIDI input device to the track.
 *  @param device_id The ID of the MIDI input device. Defaults to 0 (the default input device)
 */
void Track::add_midi_input(const unsigned int device_id)
{
  Devices::MidiDevice device = Devices::DeviceManager::instance().get_midi_device(device_id);

  Midi::MidiEngine::instance().open_input_port(device_id);

  m_midi_input_device_id = device_id;

  LOG_INFO("Track: Added MIDI input device: ", device.name);
}

/** @brief Adds a WAV file input to the track.
 *  @param wav_file The WAV file.
 */
void Track::add_audio_file_input(const std::shared_ptr<Files::WavFile> &wav_file)
{
  LOG_INFO("Track: Added WAV file input: ", wav_file->get_filename());
  LOG_INFO("Sample Rate: ", wav_file->get_sample_rate(),
           ", Channels: ", wav_file->get_channels(),
           ", Format: ", wav_file->get_format());

  Audio::AudioEngine::instance().set_stream_parameters(wav_file->get_channels(), wav_file->get_sample_rate(), 512);
}

/** @brief Adds a MIDI file input to the track.
 *  @param midi_file The MIDI file.
 */
void Track::add_midi_file_input(const Files::MidiFile &midi_file)
{
  LOG_INFO("Track: Added MIDI file input: ", midi_file.get_filename());
}

/** @brief Adds an audio output to the track.
 *  @param device_id The ID of the audio output device. Defaults to 0 (the default output device).
 */
void Track::add_audio_output(const unsigned int device_id)
{
  Devices::AudioDevice device = Devices::DeviceManager::instance().get_audio_device(device_id);

  // Verify the audio device has output channels
  if (device.output_channels < 1)
  {
    throw std::runtime_error("Selected audio device " + device.name + " has no output channels.");
  }

  m_audio_output_device_id = device_id;
  LOG_INFO("Track: Added audio output device: ", device.name);
}

void Track::play()
{
  LOG_INFO("Track: Play...");
  Audio::AudioEngine::instance().play();
}

void Track::stop()
{
  LOG_INFO("Track: Stop...");
  Audio::AudioEngine::instance().stop();
}

/** @brief Updates the track with a new MIDI message.
 *  This function is called by the MidiEngine when a new MIDI message is received.
 *  @param message The MIDI message to process.
 */
void Track::update(const Midi::MidiMessage& message)
{
  std::lock_guard<std::mutex> lock(m_queue_mutex);
  m_message_queue.push(message);
}

/** @brief Updates the track with a new audio message.
 *  This function is called by the AudioEngine when a new audio message is received.
 *  @param message The audio message to process.
 */
void Track::update(const Audio::AudioMessage &message)
{
  (void)message;
}

/** @brief Handles a MIDI message.
 *  This function processes the MIDI message received from the MidiEngine.
 *  @param message The MIDI message to handle.
 */
void Track::handle_midi_message()
{
  Midi::MidiMessage message;
  {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    if (m_message_queue.empty())
      return;
  
    message = m_message_queue.front();
    m_message_queue.pop();
  }

  // Process the MIDI message here
  switch (message.type)
  {
    case Midi::eMidiMessageType::NoteOn:
      LOG_INFO("Track: Note On - Channel: ", static_cast<int>(message.channel),
               ", Note: ", static_cast<int>(message.data1),
               ", Velocity: ", static_cast<int>(message.data2));
      break;
    case Midi::eMidiMessageType::NoteOff:
      LOG_INFO("Track: Note Off - Channel: ", static_cast<int>(message.channel),
               ", Note: ", static_cast<int>(message.data1));
      break;
    case Midi::eMidiMessageType::ControlChange:
      LOG_INFO("Track: Control Change - Channel: ", static_cast<int>(message.channel),
               ", Controller: ", static_cast<int>(message.data1),
               ", Value: ", static_cast<int>(message.data2));
      break;
    default:
      LOG_INFO("Track: Unknown MIDI Message Type - ", message.type_name);
      break;
  }
}

/** @brief Fill the audio output buffer with the next available data
 *  @param output_buffer Pointer to the output buffer where audio data will be written.
 *  @param n_frames Number of frames to fill in the output buffer.
 */
void Track::get_next_audio_frame(float *output_buffer, unsigned int n_frames)
{
  
}
