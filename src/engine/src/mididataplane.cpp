#include "mididataplane.h"

#include "miditypes.h"
#include "logger.h"

using namespace miniaudioengine::midi;
using namespace miniaudioengine::framework;

void MidiDataPlane::process_midi_message(const MidiMessage &midi_message)
{
  // Check stop command once at start
  if (m_stop_command.load(std::memory_order_acquire))
  {
    return;
  }

  LOG_INFO("MidiDataPlane received MIDI message: " + midi_message.to_string());
  

  update_midi_input_statistics(midi_message);
}

void MidiDataPlane::update_midi_input_statistics(const MidiMessage &midi_message)
{
  (void)midi_message; // Unused for now, but can be used to update statistics based on message type, channel, etc.
  if (p_statistics)
  {
    auto midi_stats = std::dynamic_pointer_cast<MidiInputStatistics>(p_statistics);
    if (midi_stats)
    {
      midi_stats->total_messages_processed++;
    }
  }
  else
  {
    LOG_WARNING("MidiDataPlane: No statistics object available to update.");
  }
}
