#include <gtest/gtest.h>
#include <iostream>

#include "trackmanager.h"

using namespace MinimalAudioEngine::Control;


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

/** @brief Track Manager - List All Tracks
 */
TEST(TrackManagerTest, ListAllTracks)
{
  TrackManager::instance().clear_tracks();
  EXPECT_EQ(TrackManager::instance().get_track_count(), 0);

  // Add multiple tracks
  const size_t num_tracks_to_add = 5;
  for (size_t i = 0; i < num_tracks_to_add; ++i)
  {
    TrackManager::instance().add_track();
  }

  // Get all tracks
  auto tracks = TrackManager::instance().get_tracks();
  auto track_dataplanes = TrackManager::instance().get_track_audio_dataplanes();

  // Verify the number of tracks matches
  EXPECT_EQ(tracks.size(), num_tracks_to_add);
  EXPECT_EQ(track_dataplanes.size(), num_tracks_to_add);
  EXPECT_EQ(TrackManager::instance().get_track_count(), num_tracks_to_add);
}
