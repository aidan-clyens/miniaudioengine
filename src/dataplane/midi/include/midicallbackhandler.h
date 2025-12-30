#ifndef __MIDI_CALLBACK_HANDLER_H__
#define __MIDI_CALLBACK_HANDLER_H__

#include <memory>
#include <vector>

// #include <rtaudio/RtAudio.h>

namespace MinimalAudioEngine
{

// typedef std::shared_ptr<class TrackAudioDataPlane> TrackAudioDataplanePtr;

struct MidiCallbackContext
{
  // std::vector<TrackAudioDataplanePtr> active_tracks;
};

class MidiCallbackHandler
{
public:
  /** @brief Callback function to handle incoming MIDI messages.
   *  This function is called by the RtMidi library when a MIDI message is received.
   *  It prints the received MIDI message to the console.
   *
   *  @param deltatime The time in seconds since the last message was received.
   *  @param message A vector containing the MIDI message bytes.
   *  @param user_data A pointer to the MidiEngine object.
   */
  static void midi_callback(double deltatime, std::vector<unsigned char> *message, void *user_data);
};

} // namespace MinimalAudioEngine

#endif // __MIDI_CALLBACK_HANDLER_H__