#include <gtest/gtest.h>
#include <iostream>

#include "midiengine.h"
#include "devicemanager.h"
#include "trackmanager.h"
#include "filemanager.h"
#include "midifile.h"
#include "logger.h"

using namespace MinimalAudioEngine;
using namespace MinimalAudioEngine;
using namespace MinimalAudioEngine;

TEST(MidiInputIntegrationTest, MidiInput)
{
  set_thread_name("Main");

  // Start the MIDI engine
  MidiEngine::instance().start_thread();

  // Add a track
  ASSERT_EQ(TrackManager::instance().get_track_count(), 0);
  size_t track_index = TrackManager::instance().add_track();
  ASSERT_EQ(TrackManager::instance().get_track_count(), 1);

  auto track = TrackManager::instance().get_track(track_index);

  LOG_INFO("Track added with index: ", track_index);

  // Open a test MIDI file and load it into the track
  std::string test_midi_file = "samples/midi_c_major_monophonic.mid";

  MidiFile midi_file = FileManager::instance().read_midi_file(test_midi_file);
  ASSERT_EQ(midi_file.get_filepath(), FileManager::instance().convert_to_absolute(test_midi_file));
  ASSERT_EQ(midi_file.get_filename(), FileManager::instance().convert_to_absolute(test_midi_file).filename().string());

  LOG_INFO("MIDI file loaded: ", midi_file.get_filepath());

  track->add_midi_file_input(midi_file);

  // Stop the MIDI engine
  MidiEngine::instance().stop_thread();
}