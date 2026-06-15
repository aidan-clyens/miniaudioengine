#include "maintrack.h"
#include "audioadapter.h"
#include "midiadapter.h"
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

  // Get inputs by searching for leaf nodes in AudioGraph
  std::vector<framework::IAudioGraphNodePtr> input_nodes = audio_graph->get_leaf_nodes();
  for (auto node : input_nodes)
  {
    dataplane::InputNodePtr input_node = std::dynamic_pointer_cast<dataplane::InputNode>(node);
    if (!input_node) continue;

    framework::IInputOutputPtr io = input_node->get_io();

    if (io->get_type() == framework::eInputOutputType_Device)
    {
      DevicePtr device = std::dynamic_pointer_cast<Device>(io);
      Device::eDeviceType type = device->get_device_type();
      LOG_DEBUG("MainTrack: play - Using Input Device - ", io->to_string());

      if (type == Device::eDeviceType::Audio)
      {
        LOG_WARNING("MainTrack: play - Read from audio input device not implemented.");
      }
      else if (type == Device::eDeviceType::Midi)
      {
        // LOG_WARNING("MainTrack: play - Read from MIDI input device not implemented.");
        p_midi_adapter->open_input_port(device->get_id(), nullptr);
      }
    }
    else if (io->get_type() == framework::eInputOutputType_File)
    {
      // TODO - Read from input file
      LOG_WARNING("MainTrack: play - Read from input file not implemented.");
    }
  }

  // Check output by checking root node or AudioGraph
  framework::IAudioGraphNodePtr root_node = audio_graph->get_root_node();
  dataplane::OutputNodePtr output_node = std::dynamic_pointer_cast<dataplane::OutputNode>(root_node);

  if (output_node)
  {
    framework::IInputOutputPtr io = output_node->get_io();
  
    if (io->get_type() == framework::eInputOutputType_Device)
    {
      DevicePtr device = std::dynamic_pointer_cast<Device>(io);
      Device::eDeviceType type = device->get_device_type();
      LOG_DEBUG("MainTrack: play - Using Output Device - ", io->to_string());

      if (type == Device::eDeviceType::Audio)
      {
        LOG_INFO("MainTrack: play - Opening audio stream with Device: ", device->to_string());

        bool ret = p_audio_adapter->open_stream(device, audio_graph);
        if (!ret)
        {
          LOG_ERROR("MainTrack: play - Failed to open audio stream!");
          return false;
        }
      }
      else if (type == Device::eDeviceType::Midi)
      {
        // TODO - Output to MIDI device
        LOG_WARNING("MainTrack: play - Open MIDI Device not implemented.");
      }
    }
    else if (io->get_type() == framework::eInputOutputType_File)
    {
      // TODO - Write to output file
      LOG_WARNING("MainTracK: play - Open File not implemented.");
    }
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

  ret = p_midi_adapter->close_input_port();
  if (!ret)
  {
    LOG_WARNING("MainTrack: stop - Failed to close MIDI port!");
    return false;
  }

  LOG_INFO("MainTrack: stop - Stopping...");
  return true;
}

bool MainTrack::is_playing()
{
  return (p_audio_adapter->is_stream_open() && p_audio_adapter->is_stream_running())
  || p_midi_adapter->is_port_open();
}

dataplane::AudioGraphPtr MainTrack::compile_audio_graph() const
{
  LOG_INFO("MainTrack: Compiling AudioGraph for current track hierarchy...");
  dataplane::AudioGraphPtr audio_graph = std::make_shared<dataplane::AudioGraph>();

  // if (!has_audio_output() && !has_midi_output())
  // {
  //   LOG_WARNING("MainTrack: Cannot compile AudioGraph. No Audio/MIDI Output is set.");
  //   return nullptr;
  // }

  // For the main track, first add an output node
  dataplane::OutputNodePtr output_node = nullptr;
  if (has_audio_output() || has_midi_output())
  {
    framework::IInputOutputPtr output = (has_audio_output() ? get_audio_output() : (has_midi_output() ? get_midi_output() : nullptr));
    if (output == nullptr)
    {
      LOG_WARNING("MainTrack: Cannot assign null Output");
      return nullptr;
    }
  
    output_node = audio_graph->add_output_node(output);
  }

  // Then, add a mixer node
  dataplane::MixerNodePtr mixer_node = audio_graph->add_mixer_node(output_node);

  // Iterate through main track's children
  for (const auto &child : get_children())
  {
    framework::IAudioGraphNodePtr next_node = mixer_node;

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

    // Add track processor nodes
    // auto processors = child->get_effects_processors();
    // if (processors.size() > 0)
    // {
    //   dataplane::ProcessorNodePtr processor_node = audio_graph->add_processor_node(next_node);
    //   next_node = processor_node;
    // }
  }

  return audio_graph;
}