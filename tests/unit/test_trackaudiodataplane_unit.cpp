#include <gtest/gtest.h>
#include <iostream>

#include "trackaudiodataplane.h"
#include "logger.h"

#define TEST_WAV_FILE_PATH "C:\\Projects\\minimal-audio-engine\\samples\\test.wav"

using namespace MinimalAudioEngine;

TrackAudioDataPlanePtr test_track_audio_dataplane;

class TrackAudioDataPlaneTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    test_track_audio_dataplane = std::make_shared<TrackAudioDataPlane>();
  }
};

/** @brief TrackAudioDataPlaneTest - Setup
 */
TEST_F(TrackAudioDataPlaneTest, Setup)
{
  EXPECT_NE(test_track_audio_dataplane, nullptr) << "TrackAudioDataPlane should not be null after creation";
  LOG_INFO("Created TrackAudioDataPlane: ", test_track_audio_dataplane->to_string());
}