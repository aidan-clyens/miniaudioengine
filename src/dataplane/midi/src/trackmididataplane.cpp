#include "trackmididataplane.h"

#include "miditypes.h"
#include "logger.h"

using namespace MinimalAudioEngine;

void TrackMidiDataPlane::process_midi_message(const MidiMessage &midi_message)
{
  // Check stop command once at start
  if (m_stop_command.load(std::memory_order_acquire))
  {
    return;
  }

  LOG_INFO("TrackMidiDataPlane received MIDI message: " + midi_message.to_string());
  

  update_midi_input_statistics(midi_message);
}

void TrackMidiDataPlane::update_midi_input_statistics(const MidiMessage &midi_message)
{
  m_midi_input_stats.total_messages_processed++;
}
