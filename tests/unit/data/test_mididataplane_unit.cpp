#include <gtest/gtest.h>

#include <memory>

#include "mididataplane.h"
#include "miditypes.h"
#include "logger.h"

using namespace miniaudioengine;
using namespace miniaudioengine::midi;
using namespace miniaudioengine::framework;

class MidiDataPlaneTest : public ::testing::Test
{ 
public:
  MidiDataPlanePtr midi_data_plane;

  void SetUp() override
  {
    midi_data_plane = std::make_shared<MidiDataPlane>();
  }

  void TearDown() override
  {
    midi_data_plane->stop();
    midi_data_plane.reset();
  }
};

TEST_F(MidiDataPlaneTest, Setup)
{
  EXPECT_NE(midi_data_plane, nullptr) << "MidiDataPlane should be initialized.";
  EXPECT_FALSE(midi_data_plane->is_running()) << "MidiDataPlane should be stopped initially.";
}

TEST_F(MidiDataPlaneTest, StartStop)
{
  midi_data_plane->start();
  EXPECT_TRUE(midi_data_plane->is_running()) << "MidiDataPlane should be running after start().";

  midi_data_plane->stop();
  EXPECT_FALSE(midi_data_plane->is_running()) << "MidiDataPlane should be stopped after stop().";
}

TEST_F(MidiDataPlaneTest, ProcessMidiMessageAndStatistics)
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
  midi_data_plane->start();
  EXPECT_TRUE(midi_data_plane->is_running()) << "MidiDataPlane should be running after start().";

  // Get MIDI input statistics
  auto stats = midi_data_plane->get_statistics();
  auto midi_stats = std::dynamic_pointer_cast<MidiInputStatistics>(stats);
  ASSERT_NE(midi_stats, nullptr) << "Statistics object should be of type MidiInputStatistics";

  EXPECT_EQ(midi_stats->total_messages_processed, 0) << "Total messages processed should be 0 before processing.";

  EXPECT_NO_THROW(midi_data_plane->process_midi_message(message));

  // Get updated MIDI input statistics
  stats = midi_data_plane->get_statistics();
  midi_stats = std::dynamic_pointer_cast<MidiInputStatistics>(stats);

  ASSERT_NE(midi_stats, nullptr) << "Statistics object should be of type MidiInputStatistics";
  EXPECT_EQ(midi_stats->total_messages_processed, 1) << "Total messages processed should be 1 after processing one message.";
}

TEST_F(MidiDataPlaneTest, DoNotProcessMidiMessageWhenStopped)
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
  midi_data_plane->stop();
  EXPECT_FALSE(midi_data_plane->is_running()) << "MidiDataPlane should be stopped.";

  // Get MIDI input statistics
  auto stats = midi_data_plane->get_statistics();
  auto midi_stats = std::dynamic_pointer_cast<MidiInputStatistics>(stats);
  ASSERT_NE(midi_stats, nullptr) << "Statistics object should be of type MidiInputStatistics";

  EXPECT_EQ(midi_stats->total_messages_processed, 0) << "Total messages processed should be 0 before processing.";

  EXPECT_NO_THROW(midi_data_plane->process_midi_message(message));

  // Get updated MIDI input statistics
  stats = midi_data_plane->get_statistics();
  midi_stats = std::dynamic_pointer_cast<MidiInputStatistics>(stats);
  ASSERT_NE(midi_stats, nullptr) << "Statistics object should be of type MidiInputStatistics";

  EXPECT_EQ(midi_stats->total_messages_processed, 0) << "Total messages processed should remain 0 when stopped.";
}