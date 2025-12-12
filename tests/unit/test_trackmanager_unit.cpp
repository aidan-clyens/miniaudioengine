#include <gtest/gtest.h>
#include <iostream>

#include "trackmanager.h"

using namespace MinimalAudioEngine;


/** @brief Track Manager - Add a Track 
 */
TEST(TrackManagerTest, AddTrack)
{
  TrackManager::instance().clear_tracks();

  // Verify initial track count is zero
  EXPECT_EQ(TrackManager::instance().get_track_count(), 0);

  // Add a new track
  size_t index = TrackManager::instance().add_track();

  // Get the track
  auto track = TrackManager::instance().get_track(index);

  // Verify the track was added successfully
  EXPECT_NE(track, nullptr);
  EXPECT_EQ(TrackManager::instance().get_track_count(), 1);
}

/** @brief Track Manager - Remove a Track
 */
TEST(TrackManagerTest, RemoveTrack)
{
  TrackManager::instance().clear_tracks();

  // Add a new track
  size_t index = TrackManager::instance().add_track();
  EXPECT_EQ(TrackManager::instance().get_track_count(), 1);

  // Remove the track
  TrackManager::instance().remove_track(index);

  // Attempt to get the removed track
  EXPECT_ANY_THROW(
    TrackManager::instance().get_track(index)
  );
  
  // Verify the track was removed successfully
  EXPECT_EQ(TrackManager::instance().get_track_count(), 0);
}