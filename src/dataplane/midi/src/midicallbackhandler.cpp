#include "midicallbackhandler.h"

#include "trackmididataplane.h"
#include "miditypes.h"
#include "logger.h"

using namespace MinimalAudioEngine::Data;
using namespace MinimalAudioEngine::Control;
using namespace MinimalAudioEngine::Core;

/** @brief Callback function to handle incoming MIDI messages.
 *  This function is called by the RtMidi library when a MIDI message is received.
 *  It prints the received MIDI message to the console.
 *
 *  @param deltatime The time in seconds since the last message was received.
 *  @param message A vector containing the MIDI message bytes.
 *  @param user_data A pointer to the MidiEngine object.
 */
void MidiCallbackHandler::midi_callback(double deltatime, std::vector<unsigned char> *message, void *user_data)
{
  try
  {
    if (message == nullptr || user_data == nullptr)
    {
      LOG_ERROR("Received null MIDI message or user data");
      return;
    }

    MidiCallbackContext *context = reinterpret_cast<MidiCallbackContext *>(user_data);
    if (context == nullptr)
    {
      LOG_ERROR("MidiCallbackContext instance is null in MIDI callback");
      return;
    }

    // Parse incoming MIDI messages
    MidiMessage midi_message;

    midi_message.deltatime = deltatime;
    midi_message.status = message->at(0);
    midi_message.type = static_cast<eMidiMessageType>(midi_message.status & 0xF0);
    midi_message.channel = midi_message.status & 0x0F;
    midi_message.data1 = message->size() > 1 ? message->at(1) : 0;
    midi_message.data2 = message->size() > 2 ? message->at(2) : 0;

    auto it = std::find_if(midi_message_type_names.begin(), midi_message_type_names.end(),
                           [&midi_message](const auto &pair)
                           { return pair.first == midi_message.type; });

    if (it != midi_message_type_names.end())
    {
      midi_message.type_name = it->second;
    }
    else
    {
      midi_message.type_name = "Unknown MIDI Message";
    }

    // Update active tracks in context and forward message to them
    for (const auto &track_dp : context->active_tracks)
    {
      track_dp->process_midi_message(midi_message);
    }
  }
  catch (const std::exception &e)
  {
    LOG_ERROR("Exception in MIDI callback: ", e.what());
  }
}