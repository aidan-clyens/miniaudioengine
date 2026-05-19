#include "trackservice.h"
#include "audiograph.h"
#include "mixernode.h"
#include "outputnode.h"
#include "processornode.h"
#include "inputnode.h"
#include "logger.h"

using namespace miniaudioengine;

bool MainTrack::start()
{
  LOG_INFO("MainTrack: Start");
  // Compile audio dataplane graph
  dataplane::AudioGraphPtr audio_graph = compile_audio_graph();
  LOG_INFO("MainTrack: Compiled ", audio_graph->to_string());

  return true;
}

dataplane::AudioGraphPtr MainTrack::compile_audio_graph() const
{
  LOG_INFO("MainTrack: Compiling AudioGraph for current track hierarchy...");
  dataplane::AudioGraphPtr audio_graph = std::make_shared<dataplane::AudioGraph>();

  // For the main track, add a mixer node
  dataplane::MixerNodePtr mixer_node = audio_graph->add_mixer_node();

  // TODO - Iterate through main track's children
  for (const auto &child : get_children())
  {
    framework::IAudioGraphNodePtr next_node = mixer_node;

    // Add track output node
    if (child->has_audio_output())
    {
      dataplane::OutputNodePtr output_node = audio_graph->add_output_node(next_node);
      LOG_INFO("MainTrack: Compiling AudioGraph - Added ", output_node->to_string());
      next_node = output_node;
    }
    else if (child->has_midi_output())
    {
      dataplane::OutputNodePtr output_node = audio_graph->add_output_node(next_node);
      LOG_INFO("MainTrack: Compiling AudioGraph - Added ", output_node->to_string());
      next_node = output_node;
    }
    else
    {
      LOG_INFO("MainTrack: Compiling AudioGraph - No Output");
    }

    // TODO - Add track processor nodes
    dataplane::ProcessorNodePtr processor_node = audio_graph->add_processor_node(next_node);
    next_node = processor_node;
    LOG_INFO("MainTrack: Compiling AudioGraph - Added ", processor_node->to_string());

    // TODO - Add track input node
    if (child->has_audio_input())
    {
      auto input = child->get_audio_input();
      dataplane::InputNodePtr input_node = audio_graph->add_input_node(next_node);
      next_node = input_node;
      LOG_INFO("MainTrack: Compiling AudioGraph - Added ", input_node->to_string());
    }
    else if (child->has_midi_input())
    {
      auto input = child->get_midi_input();
      dataplane::InputNodePtr input_node = audio_graph->add_input_node(next_node);
      next_node = input_node;
      LOG_INFO("MainTrack: Compiling AudioGraph - Added ", input_node->to_string());
    }
    else
    {
      LOG_INFO("MainTrack: Compiling AudioGraph - No Input");
    }
  }

  return audio_graph;
}

// ============================================================================
// Constructor
// ============================================================================

TrackService::TrackService()
{
  // Create MainTrack (root of hierarchy) with hardware output
  m_main_track = std::make_shared<MainTrack>(); // is_main_track = true
  LOG_INFO("TrackService: Created MainTrack (root of hierarchy)");
}

// ============================================================================
// Hierarchy Operations
// ============================================================================

/** @brief Create a new track as child of specified parent.
 *  @param parent The parent track (defaults to MainTrack if nullptr).
 *  @return Shared pointer to the new track.
 */
TrackPtr TrackService::add_track(TrackPtr parent)
{
  if (!parent || !parent->is_main_track())
  {
    if (parent && !parent->is_main_track())
    {
      LOG_WARNING("TrackService: Non-MainTrack parent provided. Defaulting to MainTrack.");
    }
    parent = m_main_track;
  }

  auto new_track = std::make_shared<Track>(false);
  try
  {
    parent->add_child_track(new_track);
  }
  catch (const std::exception& e)
  {
    LOG_ERROR("TrackService: Failed to create child track: ", e.what());
  }

  LOG_INFO("TrackService: Created child track. Total tracks in hierarchy: ", get_track_count());
  return new_track;
}

/** @brief Remove a track from the hierarchy.
 *  @param track The track to remove (will also remove all descendants).
 *  @throws std::runtime_error if attempting to remove MainTrack.
 */
void TrackService::remove_track(TrackPtr track)
{
  if (!track)
  {
    return;
  }

  if (track->is_main_track())
  {
    throw std::runtime_error("Cannot remove MainTrack from hierarchy.");
  }

  if (track->get_child_count() > 0)
  {
    LOG_WARNING("TrackService: Removing a track that still has children. Children will be detached.");
  }

  // Remove from parent (MainTrack)
  track->remove_from_parent();
  
  LOG_INFO("TrackService: Removed track from hierarchy. Total tracks: ", get_track_count());
}

// ============================================================================
// Tree Traversal
// ============================================================================

/** @brief Get all tracks in the hierarchy (breadth-first traversal).
 *  @return Vector of all track pointers including MainTrack.
 */
std::vector<TrackPtr> TrackService::get_all_tracks() const
{
  std::vector<TrackPtr> all_tracks;

  all_tracks.push_back(m_main_track);

  const auto &children = m_main_track->get_children();
  for (const auto &child : children)
  {
    all_tracks.push_back(child);
  }

  return all_tracks;
}

/** @brief Get total number of tracks including MainTrack.
 *  @return Total track count.
 */
size_t TrackService::get_track_count() const
{
  return get_all_tracks().size();
}

/** @brief Clear all tracks except MainTrack.
 */
void TrackService::clear_tracks()
{
  LOG_INFO("TrackService: Clearing all tracks except MainTrack. Total tracks before clear: ", get_track_count());
  
  // Get all children of MainTrack (without holding the lock during removal)
  std::vector<TrackPtr> children;
  {
    std::lock_guard<std::mutex> lock(m_manager_mutex);
    children = m_main_track->get_children();
  }
  
  // Remove all children without holding the manager mutex
  for (auto& child : children)
  {
    m_main_track->remove_child_track(child);
  }
  
  LOG_INFO("TrackService: All tracks cleared. Total tracks after clear: ", get_track_count());
}

