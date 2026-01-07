#ifndef __MIDI_TYPES_H_
#define __MIDI_TYPES_H_

#include <string>
#include <string_view>
#include <array>
#include <iostream>

namespace MinimalAudioEngine::Control
{

/** @enum eMidiMessageType
 *  @brief MIDI message types.
 */
enum class eMidiMessageType : unsigned char
{
  NoteOff                   = 0x80,
  NoteOn                    = 0x90,
  PolyphonicKeyPressure     = 0xA0,
  ControlChange             = 0xB0,
  ProgramChange             = 0xC0,
  ChannelPressure           = 0xD0,
  PitchBendChange           = 0xE0,
  SystemExclusive           = 0xF0,
  MidiTimeCodeQuarterFrame  = 0xF1,
  SongPositionPointer       = 0xF2,
  SongSelect                = 0xF3,
  TuneRequest               = 0xF6,
  EndOfSysEx                = 0xF7,
  TimingClock               = 0xF8,
  Start                     = 0xFA,
  Continue                  = 0xFB,
  Stop                      = 0xFC,
  ActiveSensing             = 0xFE,
  SystemReset               = 0xFF
};

inline constexpr std::array<std::pair<eMidiMessageType, std::string_view>, 19> midi_message_type_names =
{{
  {eMidiMessageType::NoteOff, "Note Off"},
  {eMidiMessageType::NoteOn, "Note On"},
  {eMidiMessageType::PolyphonicKeyPressure, "Polyphonic Key Pressure"},
  {eMidiMessageType::ControlChange, "Control Change"},
  {eMidiMessageType::ProgramChange, "Program Change"},
  {eMidiMessageType::ChannelPressure, "Channel Pressure"},
  {eMidiMessageType::PitchBendChange, "Pitch Bend Change"},
  {eMidiMessageType::SystemExclusive, "System Exclusive"},
  {eMidiMessageType::MidiTimeCodeQuarterFrame, "MIDI Time Code Quarter Frame"},
  {eMidiMessageType::SongPositionPointer, "Song Position Pointer"},
  {eMidiMessageType::SongSelect, "Song Select"},
  {eMidiMessageType::TuneRequest, "Tune Request"},
  {eMidiMessageType::EndOfSysEx, "End of SysEx"},
  {eMidiMessageType::TimingClock, "Timing Clock"},
  {eMidiMessageType::Start, "Start"},
  {eMidiMessageType::Continue, "Continue"},
  {eMidiMessageType::Stop, "Stop"},
  {eMidiMessageType::ActiveSensing, "Active Sensing"},
  {eMidiMessageType::SystemReset, "System Reset"}
}};

/** @struct MidiPort
  * @brief Represents a MIDI port with its number and name.
  */
struct MidiPort
{
  unsigned int port_number;
  std::string port_name;
};

/** @struct MidiMessage
  * @brief Represents a MIDI message with its delta time received, status, data bytes, and type name.
  */
struct MidiMessage
{
  double deltatime;  // Time in seconds since the last message
  unsigned char status;  // Status byte of the MIDI message
  eMidiMessageType type; // Type of the MIDI message (e.g., Note On, Control Change)
  unsigned char channel; // MIDI channel (0-15)
  unsigned char data1;   // First data byte (e.g., note number, control change number)
  unsigned char data2;   // Second data byte (e.g., velocity, control change value)
  std::string_view type_name; // Human-readable name of the MIDI message type

  int channel_num() const
  {
    return static_cast<int>(channel);
  }

  virtual std::string to_string() const
  {
    return "MidiMessage(" +
           std::string("Deltatime=") + std::to_string(deltatime) +
           ", Status=0x" + std::to_string(static_cast<int>(status)) +
           ", Type=" + std::string(type_name) +
           ", Channel=" + std::to_string(channel_num()) +
           ", Data1=" + std::to_string(static_cast<int>(data1)) +
           ", Data2=" + std::to_string(static_cast<int>(data2)) +
           ")";
  }
};

/** @struct MidiNoteMessage 
 *  @brief Represents a MIDI note message, derived from MidiMessage, with helper methods to get note number and velocity.
 */
struct MidiNoteMessage : public MidiMessage
{
  int note_number() const
  {
    return static_cast<int>(data1);
  }

  int velocity() const
  {
    return static_cast<int>(data2);
  }

  std::string to_string() const override
  {
    return "MidiNoteMessage(" +
           std::string("Deltatime=") + std::to_string(deltatime) +
           ", Status=0x" + std::to_string(static_cast<int>(status)) +
           ", Type=" + std::string(type_name) +
           ", Channel=" + std::to_string(channel_num()) +
           ", Note Number=" + std::to_string(note_number()) +
           ", Velocity=" + std::to_string(velocity()) +
           ")";
  }
};

/** @struct MidiControlMessage 
 *  @brief Represents a MIDI control change message, derived from MidiMessage, with helper methods to get controller number and value.
 */
struct MidiControlMessage : public MidiMessage
{
  int controller_number() const
  {
    return static_cast<int>(data1);
  }

  int controller_value() const
  {
    return static_cast<int>(data2);
  }

  std::string to_string() const override
  {
    return "MidiControlMessage(" +
           std::string("Deltatime=") + std::to_string(deltatime) +
           ", Status=0x" + std::to_string(static_cast<int>(status)) +
           ", Type=" + std::string(type_name) +
           ", Channel=" + std::to_string(channel_num()) +
           ", Controller Number=" + std::to_string(controller_number()) +
           ", Controller Value=" + std::to_string(controller_value()) +
           ")";
  }
};

inline std::ostream &operator<<(std::ostream &os, const MidiMessage &msg)
{
  os << msg.to_string();
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const MidiNoteMessage &msg)
{
  os << msg.to_string();
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const MidiControlMessage &msg)
{
  os << msg.to_string();
  return os;
}

}  // namespace MinimalAudioEngine

#endif  // __MIDI_TYPES_H_