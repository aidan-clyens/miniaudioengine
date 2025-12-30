#include "trackmididataplane.h"

#include "miditypes.h"
#include "logger.h"

using namespace MinimalAudioEngine;

void TrackMidiDataPlane::process_midi_message(const MidiMessage &midi_message)
{
  LOG_INFO("TrackMidiDataPlane received MIDI message: " + midi_message.to_string());
  // Push the message to the track's MIDI event queue
  push_to_output_buffer(midi_message);
  update_midi_input_statistics(midi_message);
}

void TrackMidiDataPlane::update_midi_input_statistics(const MidiMessage &midi_message)
{
  m_midi_input_stats.total_messages_processed++;
}
