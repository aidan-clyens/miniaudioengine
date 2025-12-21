#ifndef __MIDI_CONTROLLER_H__
#define __MIDI_CONTROLLER_H__

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <ostream>

namespace MinimalAudioEngine
{

/** @enum eMidiController
 *  @brief MIDI controller numbers for the Novation Launchkey Mini
 */
enum class eMidiController : int
{
  Play = 115,
  Record = 117,
  ModulationWheel = 1,
  Pot1 = 21,
  Pot2 = 22,
  Pot3 = 23,
  Pot4 = 24,
  Pot5 = 25,
  Pot6 = 26,
  Pot7 = 27,
  Pot8 = 28,
  PreviousTrack = 103,
  NextTrack = 102,
  Up = 104,
  Down = 105
};

static inline const std::map<eMidiController, std::string_view> midi_controller_names =
{
    {eMidiController::Play, "Play"},
    {eMidiController::Record, "Record"},
    {eMidiController::ModulationWheel, "Modulation Wheel"},
    {eMidiController::Pot1, "Pot 1"},
    {eMidiController::Pot2, "Pot 2"},
    {eMidiController::Pot3, "Pot 3"},
    {eMidiController::Pot4, "Pot 4"},
    {eMidiController::Pot5, "Pot 5"},
    {eMidiController::Pot6, "Pot 6"},
    {eMidiController::Pot7, "Pot 7"},
    {eMidiController::Pot8, "Pot 8"},
    {eMidiController::PreviousTrack, "Previous Track"},
    {eMidiController::NextTrack, "Next Track"},
    {eMidiController::Up, "Up"},
    {eMidiController::Down, "Down"}
};

/** @enum eMidiContollerValues
 *  @brief MIDI controller values for pressed and released states for the Novation Launchkey Mini
 */
enum class eMidiControllerValues : int
{
  Pressed = 127,
  Released = 0
};

typedef std::function<void(int value)> MidiControllerActionFunc;
typedef std::map<MinimalAudioEngine::eMidiController, MidiControllerActionFunc> MidiControllerActionMap;

/** @enum eMidiNoteValues
 *  @brief MIDI note values
 */
enum class eMidiNoteValues : int
{
  Min = 0,
  A_1 = 21,
  A_Sharp_1 = 22,
  B_1 = 23,
  C_1 = 24,
  C_Sharp_1 = 25,
  D_1 = 26,
  D_Sharp_1 = 27,
  E_1 = 28,
  F_1 = 29,
  F_Sharp_1 = 30,
  G_1 = 31,
  G_Sharp_1 = 32,
  A_2 = 33,
  A_Sharp_2 = 34,
  B_2 = 35,
  C_2 = 36,
  C_Sharp_2 = 37,
  D_2 = 38,
  D_Sharp_2 = 39,
  E_2 = 40,
  F_2 = 41,
  F_Sharp_2 = 42,
  G_2 = 43,
  G_Sharp_2 = 44,
  A_3 = 45,
  A_Sharp_3 = 46,
  B_3 = 47,
  C_3 = 48,
  C_Sharp_3 = 49,
  D_3 = 50,
  D_Sharp_3 = 51,
  E_3 = 52,
  F_3 = 53,
  F_Sharp_3 = 54,
  G_3 = 55,
  G_Sharp_3 = 56,
  A_4 = 57,
  A_Sharp_4 = 58,
  B_4 = 59,
  C_4 = 60,
  C_Sharp_4 = 61,
  D_4 = 62,
  D_Sharp_4 = 63,
  E_4 = 64,
  F_4 = 65,
  F_Sharp_4 = 66,
  G_4 = 67,
  G_Sharp_4 = 68,
  A_5 = 69,
  A_Sharp_5 = 70,
  B_5 = 71,
  C_5 = 72,
  C_Sharp_5 = 73,
  D_5 = 74,
  D_Sharp_5 = 75,
  E_5 = 76,
  F_5 = 77,
  F_Sharp_5 = 78,
  G_5 = 79,
  G_Sharp_5 = 80,
  A_6 = 81,
  A_Sharp_6 = 82,
  B_6 = 83,
  C_6 = 84,
  C_Sharp_6 = 85,
  D_6 = 86,
  D_Sharp_6 = 87,
  E_6 = 88,
  F_6 = 89,
  F_Sharp_6 = 90,
  G_6 = 91,
  G_Sharp_6 = 92,
  A_7 = 93,
  A_Sharp_7 = 94,
  B_7 = 95,
  C_7 = 96,
  C_Sharp_7 = 97,
  D_7 = 98,
  D_Sharp_7 = 99,
  E_7 = 100,
  F_7 = 101,
  F_Sharp_7 = 102,
  G_7 = 103,
  G_Sharp_7 = 104,
  A_8 = 105,
  A_Sharp_8 = 106,
  B_8 = 107,
  C_8 = 108,
  D_8 = 109,
  D_Sharp_8 = 110,
  E_8 = 111,
  F_8 = 112,
  F_Sharp_8 = 113,
  G_8 = 114,
  G_Sharp_8 = 115,
  A_9 = 116,
  A_Sharp_9 = 117,
  B_9 = 118,
  C_9 = 119,
  C_Sharp_9 = 120,
  D_9 = 121,
  D_Sharp_9 = 122,
  E_9 = 123,
  F_9 = 124,
  F_Sharp_9 = 125,
  G_9 = 126,
  G_Sharp_9 = 127,
  Max = 127
};

static inline const std::map<eMidiNoteValues, std::string_view> midi_note_names =
{
    {eMidiNoteValues::A_1, "A1"},
    {eMidiNoteValues::A_Sharp_1, "A#1"},
    {eMidiNoteValues::B_1, "B1"},
    {eMidiNoteValues::C_1, "C1"},
    {eMidiNoteValues::C_Sharp_1, "C#1"},
    {eMidiNoteValues::D_1, "D1"},
    {eMidiNoteValues::D_Sharp_1, "D#1"},
    {eMidiNoteValues::E_1, "E1"},
    {eMidiNoteValues::F_1, "F1"},
    {eMidiNoteValues::F_Sharp_1, "F#1"},
    {eMidiNoteValues::G_1, "G1"},
    {eMidiNoteValues::G_Sharp_1, "G#1"},
    {eMidiNoteValues::A_2, "A2"},
    {eMidiNoteValues::A_Sharp_2, "A#2"},
    {eMidiNoteValues::B_2, "B2"},
    {eMidiNoteValues::C_2, "C2"},
    {eMidiNoteValues::C_Sharp_2, "C#2"},
    {eMidiNoteValues::D_2, "D2"},
    {eMidiNoteValues::D_Sharp_2, "D#2"},
    {eMidiNoteValues::E_2, "E2"},
    {eMidiNoteValues::F_2, "F2"},
    {eMidiNoteValues::F_Sharp_2, "F#2"},
    {eMidiNoteValues::G_2, "G2"},
    {eMidiNoteValues::G_Sharp_2, "G#2"},
    {eMidiNoteValues::A_3, "A3"},
    {eMidiNoteValues::A_Sharp_3, "A#3"},
    {eMidiNoteValues::B_3, "B3"},
    {eMidiNoteValues::C_3, "C3"},
    {eMidiNoteValues::C_Sharp_3, "C#3"},
    {eMidiNoteValues::D_3, "D3"},
    {eMidiNoteValues::D_Sharp_3, "D#3"},
    {eMidiNoteValues::E_3, "E3"},
    {eMidiNoteValues::F_3, "F3"},
    {eMidiNoteValues::F_Sharp_3, "F#3"},
    {eMidiNoteValues::G_3, "G3"},
    {eMidiNoteValues::G_Sharp_3, "G#3"},
    {eMidiNoteValues::A_4, "A4"},
    {eMidiNoteValues::A_Sharp_4, "A#4"},
    {eMidiNoteValues::B_4, "B4"},
    {eMidiNoteValues::C_4, "C4"},
    {eMidiNoteValues::C_Sharp_4, "C#4"},
    {eMidiNoteValues::D_4, "D4"},
    {eMidiNoteValues::D_Sharp_4, "D#4"},
    {eMidiNoteValues::E_4, "E4"},
    {eMidiNoteValues::F_4, "F4"},
    {eMidiNoteValues::F_Sharp_4, "F#4"},
    {eMidiNoteValues::G_4, "G4"},
    {eMidiNoteValues::G_Sharp_4, "G#4"},
    {eMidiNoteValues::A_5, "A5"},
    {eMidiNoteValues::A_Sharp_5, "A#5"},
    {eMidiNoteValues::B_5, "B5"},
    {eMidiNoteValues::C_5, "C5"},
    {eMidiNoteValues::C_Sharp_5, "C#5"},
    {eMidiNoteValues::D_5, "D5"},
    {eMidiNoteValues::D_Sharp_5, "D#5"},
    {eMidiNoteValues::E_5, "E5"},
    {eMidiNoteValues::F_5, "F5"},
    {eMidiNoteValues::F_Sharp_5, "F#5"},
    {eMidiNoteValues::G_5, "G5"},
    {eMidiNoteValues::G_Sharp_5, "G#5"},
    {eMidiNoteValues::A_6, "A6"},
    {eMidiNoteValues::A_Sharp_6, "A#6"},
    {eMidiNoteValues::B_6, "B6"},
    {eMidiNoteValues::C_6, "C6"},
    {eMidiNoteValues::C_Sharp_6, "C#6"},
    {eMidiNoteValues::D_6, "D6"},
    {eMidiNoteValues::D_Sharp_6, "D#6"},
    {eMidiNoteValues::E_6, "E6"},
    {eMidiNoteValues::F_6, "F6"},
    {eMidiNoteValues::F_Sharp_6, "F#6"},
    {eMidiNoteValues::G_6, "G6"},
    {eMidiNoteValues::G_Sharp_6, "G#6"},
    {eMidiNoteValues::A_7, "A7"},
    {eMidiNoteValues::A_Sharp_7, "A#7"},
    {eMidiNoteValues::B_7, "B7"},
    {eMidiNoteValues::C_7, "C7"},
    {eMidiNoteValues::C_Sharp_7, "C#7"},
    {eMidiNoteValues::D_7, "D7"},
    {eMidiNoteValues::D_Sharp_7, "D#7"},
    {eMidiNoteValues::E_7, "E7"},
    {eMidiNoteValues::F_7, "F7"},
    {eMidiNoteValues::F_Sharp_7, "F#7"},
    {eMidiNoteValues::G_7, "G7"},
    {eMidiNoteValues::G_Sharp_7, "G#7"},
    {eMidiNoteValues::A_8, "A8"},
    {eMidiNoteValues::A_Sharp_8, "A#8"},
    {eMidiNoteValues::B_8, "B8"},
    {eMidiNoteValues::C_8, "C8"},
    {eMidiNoteValues::D_8, "D8"},
    {eMidiNoteValues::D_Sharp_8, "D#8"},
    {eMidiNoteValues::E_8, "E8"},
    {eMidiNoteValues::F_8, "F8"},
    {eMidiNoteValues::F_Sharp_8, "F#8"},
    {eMidiNoteValues::G_8, "G8"},
    {eMidiNoteValues::G_Sharp_8, "G#8"},
    {eMidiNoteValues::A_9, "A9"},
    {eMidiNoteValues::A_Sharp_9, "A#9"},
    {eMidiNoteValues::B_9, "B9"},
    {eMidiNoteValues::C_9, "C9"},
    {eMidiNoteValues::C_Sharp_9, "C#9"},
    {eMidiNoteValues::D_9, "D9"},
    {eMidiNoteValues::D_Sharp_9, "D#9"},
    {eMidiNoteValues::E_9, "E9"},
    {eMidiNoteValues::F_9, "F9"},
    {eMidiNoteValues::F_Sharp_9, "F#9"},
    {eMidiNoteValues::G_9, "G9"},
    {eMidiNoteValues::G_Sharp_9, "G#9"}
};

/** @brief Returns the name of the given MIDI controller.
 *  @param controller The MIDI controller.
 *  @return The name of the MIDI controller.
 *  @throws std::out_of_range if the controller is invalid.
 */
std::string get_midi_controller_name(eMidiController controller)
{
  std::string name;
  try
  {
    name = std::string(midi_controller_names.at(controller));
  }
  catch (const std::out_of_range& e) 
  {
    name = "Unknown: " + std::to_string(static_cast<int>(controller));
  }

  return name;
}

/** @brief Returns the name of the given MIDI note value.
 *  @param note_value The MIDI note value.
 *  @return The name of the MIDI note.
 *  @throws std::out_of_range if the note value is invalid.
 */
std::string get_midi_note_name(eMidiNoteValues note_value)
{
  std::string name;
  try
  {
    name = std::string(midi_note_names.at(note_value));
  }
  catch (const std::out_of_range& e) 
  {
    name = "Unknown: " + std::to_string(static_cast<int>(note_value));
  }

  return name;
}

/** @brief Overload the output stream operator for eMidiController.
 *  @param os The output stream.
 *  @param controller The MIDI controller.
 *  @return The output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const eMidiController& controller)
{
  os << get_midi_controller_name(controller);
  return os;
}

/** @brief Overload the output stream operator for eMidiControllerValues.
 *  @param os The output stream.
 *  @param controller_value The MIDI controller value.
 *  @return The output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const eMidiControllerValues& controller_value)
{
  os << static_cast<int>(controller_value);
  return os;
}

/** @brief Overload the output stream operator for eMidiNoteValues.
 *  @param os The output stream.
 *  @param note_value The MIDI note value.
 *  @return The output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const eMidiNoteValues& note_value)
{
  os << get_midi_note_name(note_value);
  return os;
}

} // namespace MinimalAudioEngine

#endif // __MIDI_CONTROLLER_H__