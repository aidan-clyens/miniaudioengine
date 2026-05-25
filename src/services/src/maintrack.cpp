#include "maintrack.h"
#include "audioadapter.h"
#include "audiograph.h"
#include "mixernode.h"
#include "outputnode.h"
#include "processornode.h"
#include "inputnode.h"

using namespace miniaudioengine;

bool MainTrack::play()
{
  // Compile audio dataplane graph
  dataplane::AudioGraphPtr audio_graph = compile_audio_graph();
  LOG_INFO("MainTrack: play - Compiled ", audio_graph->to_string());

  unsigned int device_id = 0;
  unsigned int channels = 0;

  // Get inputs by searching for leaf nodes
  std::vector<framework::IAudioGraphNodePtr> input_nodes = audio_graph->get_leaf_nodes();
  for (auto node : input_nodes)
  {
    dataplane::InputNodePtr input_node = std::dynamic_pointer_cast<dataplane::InputNode>(node);
    framework::IInputOutputPtr io = input_node->get_io();

    switch (io->get_type())
    {
      case framework::eInputOutputType_Device:
        device_id = std::dynamic_pointer_cast<Device>(io)->get_id();
        channels = std::dynamic_pointer_cast<Device>(io)->get_output_channels();
        LOG_DEBUG("MainTrack: play - Using Input Device - ", io->to_string());
        break;
      case framework::eInputOutputType_File:
        LOG_DEBUG("MainTrack: play - Using Input File - ", io->to_string());
        break;
      case framework::eInputOutputType_None:
      default:
        LOG_WARNING("MainTrack: play - Invalid Input - ", io->to_string());
        break;
    }
  }

  if (has_audio_output())
  {
    LOG_DEBUG("MainTrack: play - Using Audio Output - ", get_audio_output()->to_string());

    device_id = p_audio_output->get_type() == framework::eInputOutputType_Device ? std::dynamic_pointer_cast<Device>(p_audio_output)->get_id() : 0;
    channels = p_audio_output->get_type() == framework::eInputOutputType_Device ? std::dynamic_pointer_cast<Device>(p_audio_output)->get_output_channels() : 0;
  }

  // Set audio output I/O parameters
  adapters::AudioStreamParameters params = {
    device_id,
    channels,
    0
  };

  unsigned int sample_rate = p_audio_output ? (p_audio_output->get_type() == framework::eInputOutputType_Device ? std::dynamic_pointer_cast<Device>(p_audio_output)->get_preferred_sample_rate() : 0) : 0;

  LOG_INFO("MainTrack: play - Opening audio stream with Device: ", device_id, ", ", channels, " Channels, Sample Rate: ", sample_rate, " Hz");

  // TODO - Convert open_stream() to use IInputOutputPtr as an input argument instead
  return p_audio_adapter->open_stream(
    params,
    sample_rate,
    1024,
    audio_graph.get()
  );
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
      next_node = output_node;
    }
    else if (child->has_midi_output())
    {
      dataplane::OutputNodePtr output_node = audio_graph->add_output_node(next_node);
      next_node = output_node;
    }
    else
    {
      LOG_INFO("MainTrack: Compiling AudioGraph - No Output");
    }

    // TODO - Add track processor nodes
    dataplane::ProcessorNodePtr processor_node = audio_graph->add_processor_node(next_node);
    next_node = processor_node;

    // TODO - Add track input node
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