#include "maintrack.h"
#include "audioadapter.h"
#include "audiograph.h"
#include "mixernode.h"
#include "outputnode.h"
#include "processornode.h"
#include "inputnode.h"
#include "device.h"

using namespace miniaudioengine;

bool MainTrack::play()
{
  // Compile audio dataplane graph
  dataplane::AudioGraphPtr audio_graph = compile_audio_graph();
  LOG_INFO("MainTrack: play - Compiled ", audio_graph->to_string());

  unsigned int device_id = 0;
  unsigned int channels = 0;
  unsigned int sample_rate = 0;

  DevicePtr device = nullptr;

  // Get inputs by searching for leaf nodes in AudioGraph
  std::vector<framework::IAudioGraphNodePtr> input_nodes = audio_graph->get_leaf_nodes();
  for (auto node : input_nodes)
  {
    dataplane::InputNodePtr input_node = std::dynamic_pointer_cast<dataplane::InputNode>(node);
    framework::IInputOutputPtr io = input_node->get_io();

    if (io->get_type() == framework::eInputOutputType_Device)
    {
      device = std::dynamic_pointer_cast<Device>(io);
      LOG_DEBUG("MainTrack: play - Using Input Device - ", io->to_string());
      break;
    }
  }

  // Check output by checking root node or AudioGraph
  {
    framework::IAudioGraphNodePtr root_node = audio_graph->get_root_node();
    dataplane::OutputNodePtr output_node = std::dynamic_pointer_cast<dataplane::OutputNode>(root_node);
    framework::IInputOutputPtr io = output_node->get_io();
  
    if (io->get_type() == framework::eInputOutputType_Device)
    {
      device = std::dynamic_pointer_cast<Device>(io);
      LOG_DEBUG("MainTrack: play - Using Output Device - ", io->to_string());
    }
  }

  if (device == nullptr)
  {
    LOG_WARNING("MainTrack: play - Cannot open audio stream because there are no input/output devices.");
    return false;
  }

  LOG_INFO("MainTrack: play - Opening audio stream with Device: ", device->to_string());

  bool ret = p_audio_adapter->open_stream(device, audio_graph);
  if (!ret)
  {
    LOG_ERROR("MainTrack: play - Failed to open audio stream!");
    return false;
  }

  if (!is_playing())
  {
    LOG_ERROR("MainTrack: play - Audio stream not playing after starting!");
    return false;
  }

  return true;
}

bool MainTrack::stop()
{
  if (!is_playing())
  {
    LOG_WARNING("MainTrack: stop - Audio stream is not playing!");
    return true;
  }

  bool ret = p_audio_adapter->stop_stream();
  if (!ret)
  {
    LOG_WARNING("MainTrack: stop - Failed to stop audio stream!");
    return false;
  }

  LOG_INFO("MainTrack: stop - Stopping...");
  return true;
}

bool MainTrack::is_playing()
{
  return p_audio_adapter->is_stream_open() && p_audio_adapter->is_stream_running();
}

dataplane::AudioGraphPtr MainTrack::compile_audio_graph() const
{
  LOG_INFO("MainTrack: Compiling AudioGraph for current track hierarchy...");
  dataplane::AudioGraphPtr audio_graph = std::make_shared<dataplane::AudioGraph>();

  if (!has_audio_output() && !has_midi_output())
  {
    LOG_WARNING("MainTrack: Cannot compile AudioGraph. No Audio/MIDI Output is set.");
    return nullptr;
  }

  // For the main track, first add an output node
  framework::IInputOutputPtr output = (has_audio_output() ? get_audio_output() : (has_midi_output() ? get_midi_output() : nullptr));
  if (output == nullptr)
  {
    LOG_WARNING("MainTrack: Cannot assign null Output");
    return nullptr;
  }

  dataplane::OutputNodePtr output_node = audio_graph->add_output_node(output);

  // Then, add a mixer node
  dataplane::MixerNodePtr mixer_node = audio_graph->add_mixer_node(output_node);

  // Iterate through main track's children
  for (const auto &child : get_children())
  {
    framework::IAudioGraphNodePtr next_node = mixer_node;

    // Add track processor nodes
    dataplane::ProcessorNodePtr processor_node = audio_graph->add_processor_node(next_node);
    next_node = processor_node;

    // Add track input node
    if (child->has_audio_input())
    {
      auto input = child->get_audio_input();
      dataplane::InputNodePtr input_node = audio_graph->add_input_node(input, next_node);
      next_node = input_node;
    }
    else if (child->has_midi_input())
    {
      auto input = child->get_midi_input();
      dataplane::InputNodePtr input_node = audio_graph->add_input_node(input, next_node);
      next_node = input_node;
    }
    else
    {
      LOG_INFO("MainTrack: Compiling AudioGraph - No Input");
    }
  }

  return audio_graph;
}