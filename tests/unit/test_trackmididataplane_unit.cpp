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

  void TearDown() override
  {
    track_midi_data_plane->stop();
  }
};

TEST_F(TrackMidiDataPlaneTest, Setup)
{
  EXPECT_NE(track_midi_data_plane, nullptr) << "TrackMidiDataPlane should be initialized.";
  EXPECT_FALSE(track_midi_data_plane->is_running()) << "TrackMidiDataPlane should be stopped initially.";
}

TEST_F(TrackMidiDataPlaneTest, StartStop)
{
  track_midi_data_plane->start();
  EXPECT_TRUE(track_midi_data_plane->is_running()) << "TrackMidiDataPlane should be running after start().";

  track_midi_data_plane->stop();
  EXPECT_FALSE(track_midi_data_plane->is_running()) << "TrackMidiDataPlane should be stopped after stop().";
}

TEST_F(TrackMidiDataPlaneTest, ProcessMidiMessageAndStatistics)
{
  MidiMessage message;
  message.deltatime = 0;
  message.status = 0x90; // Note On
  message.type = eMidiMessageType::NoteOn;
  message.channel = 0;
  message.data1 = 60; // Middle C
  message.data2 = 100; // Velocity
  message.type_name = "Note On";

  // Start the data plane
  track_midi_data_plane->start();
  EXPECT_TRUE(track_midi_data_plane->is_running()) << "TrackMidiDataPlane should be running after start().";

  // Get MIDI input statistics
  MidiInputStatistics stats = track_midi_data_plane->get_statistics();
  EXPECT_EQ(stats.total_messages_processed, 0) << "Total messages processed should be 0 before processing.";

  EXPECT_NO_THROW(track_midi_data_plane->process_midi_message(message));

  // Get updated MIDI input statistics
  stats = track_midi_data_plane->get_statistics();
  EXPECT_EQ(stats.total_messages_processed, 1) << "Total messages processed should be 1 after processing one message.";
}

TEST_F(TrackMidiDataPlaneTest, DoNotProcessMidiMessageWhenStopped)
{
  MidiMessage message;
  message.deltatime = 0;
  message.status = 0x90; // Note On
  message.type = eMidiMessageType::NoteOn;
  message.channel = 0;
  message.data1 = 60; // Middle C
  message.data2 = 100; // Velocity
  message.type_name = "Note On";

  // Ensure the data plane is stopped
  track_midi_data_plane->stop();
  EXPECT_FALSE(track_midi_data_plane->is_running()) << "TrackMidiDataPlane should be stopped.";

  // Get MIDI input statistics
  MidiInputStatistics stats = track_midi_data_plane->get_statistics();
  EXPECT_EQ(stats.total_messages_processed, 0) << "Total messages processed should be 0 before processing.";

  EXPECT_NO_THROW(track_midi_data_plane->process_midi_message(message));

  // Get updated MIDI input statistics
  stats = track_midi_data_plane->get_statistics();
  EXPECT_EQ(stats.total_messages_processed, 0) << "Total messages processed should remain 0 when stopped.";
}