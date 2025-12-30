#include <gtest/gtest.h>

#include <memory>

#include "trackmididataplane.h"
#include "miditypes.h"
#include "logger.h"

using namespace MinimalAudioEngine;

class TrackMidiDataPlaneTest : public ::testing::Test
{ 
public:
  TrackMidiDataPlanePtr track_midi_data_plane;

  void SetUp() override
  {
    track_midi_data_plane = std::make_shared<TrackMidiDataPlane>();
  }
};

TEST_F(TrackMidiDataPlaneTest, ProcessMidiMessage)
{
  MidiMessage message;
  message.deltatime = 0;
  message.status = 0x90; // Note On
  message.type = eMidiMessageType::NoteOn;
  message.channel = 0;
  message.data1 = 60; // Middle C
  message.data2 = 100; // Velocity
  message.type_name = "Note On";

  // Get MIDI input statistics
  MidiInputStatistics stats = track_midi_data_plane->get_statistics();
  EXPECT_EQ(stats.total_messages_processed, 0) << "Total messages processed should be 0 before processing.";

  EXPECT_NO_THROW(track_midi_data_plane->process_midi_message(message));

  // Get updated MIDI input statistics
  stats = track_midi_data_plane->get_statistics();
  EXPECT_EQ(stats.total_messages_processed, 1) << "Total messages processed should be 1 after processing one message.";
}
