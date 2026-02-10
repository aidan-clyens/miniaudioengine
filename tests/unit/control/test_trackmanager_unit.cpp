#include <gtest/gtest.h>
#include <iostream>

#include "trackmanager.h"

using namespace miniaudioengine::control;

// Configure test setup and teardown
class TrackManagerTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    m_track_manager.clear_tracks();
    m_track_manager.set_audio_output_device(
        DeviceManager::instance().get_default_audio_output_device().value());
  }

  void TearDown() override
  {
    m_track_manager.clear_tracks();
  }

  TrackManager &get_track_manager()
  {
    return m_track_manager;
  }

  TrackPtr create_track(TrackPtr parent = nullptr)
  {
    size_t current_track_count = m_track_manager.get_track_count();
    auto track_ptr = m_track_manager.create_child_track(parent);
    size_t new_track_count = m_track_manager.get_track_count();
    EXPECT_EQ(new_track_count, current_track_count + 1);
    return track_ptr;
  }

private:
  TrackManager &m_track_manager = TrackManager::instance();
};

TEST_F(TrackManagerTest, Initial)
{
  EXPECT_EQ(get_track_manager().get_track_count(), 1);
  EXPECT_EQ(get_track_manager().get_all_tracks().size(), 1);

  auto main_track = get_track_manager().get_main_track();
  EXPECT_TRUE(main_track->is_main_track());
  EXPECT_EQ(main_track->get_child_count(), 0);
  EXPECT_FALSE(main_track->has_parent());
}

TEST_F(TrackManagerTest, CreateSingleChildTrack)
{
  auto main_track = get_track_manager().get_main_track();

  auto track1 = create_track(main_track);
  EXPECT_EQ(track1->get_parent(), main_track);
  EXPECT_EQ(main_track->get_child_count(), 1);

  EXPECT_EQ(get_track_manager().get_track_count(), 2);
  EXPECT_EQ(get_track_manager().get_all_tracks().size(), 2);
}

TEST_F(TrackManagerTest, CreateNestedTracks)
{
  auto main_track = get_track_manager().get_main_track();

  auto track1 = create_track(main_track);
  EXPECT_EQ(track1->get_parent(), main_track);
  EXPECT_EQ(main_track->get_child_count(), 1);

  auto track2 = create_track(track1);
  EXPECT_EQ(track2->get_parent(), track1);
  EXPECT_EQ(track1->get_child_count(), 1);

  auto track3 = create_track(main_track);
  EXPECT_EQ(track3->get_parent(), main_track);
  EXPECT_EQ(main_track->get_child_count(), 2);

  EXPECT_EQ(get_track_manager().get_track_count(), 4);
  EXPECT_EQ(get_track_manager().get_all_tracks().size(), 4);
}

TEST_F(TrackManagerTest, RemoveTrackWithChildren)
{
  auto main_track = get_track_manager().get_main_track();

  auto track1 = create_track(main_track);
  auto track2 = create_track(track1);
  auto track3 = create_track(main_track);

  EXPECT_EQ(get_track_manager().get_track_count(), 4);

  get_track_manager().remove_track(track1);

  EXPECT_EQ(get_track_manager().get_track_count(), 2);
  EXPECT_EQ(main_track->get_child_count(), 1);                // only track3 remains
}