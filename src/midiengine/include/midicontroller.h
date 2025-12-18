#ifndef __MIDI_CONTROLLER_H__
#define __MIDI_CONTROLLER_H__

#include <functional>
#include <map>

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

} // namespace MinimalAudioEngine

#endif // __MIDI_CONTROLLER_H__