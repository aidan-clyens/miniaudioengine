#ifndef __AUDIO_GRAPH_H__
#define __AUDIO_GRAPH_H__

#include "audiographnode.h"
#include "io.h"

#include <memory>
#include <string>

namespace miniaudioengine::dataplane
{

// Forward declarations
class MixerNode;
class InputNode;
class OutputNode;
class ProcessorNode;

using IAudioGraphNodePtr = framework::IAudioGraphNodePtr;
using IInputOutputPtr = framework::IInputOutputPtr;

using MixerNodePtr = std::shared_ptr<MixerNode>;
using InputNodePtr = std::shared_ptr<InputNode>;
using OutputNodePtr = std::shared_ptr<OutputNode>;
using ProcessorNodePtr = std::shared_ptr<ProcessorNode>;

/** @class AudioGraph
 *  @brief Represents the audio processing graph.
 */
class AudioGraph
{
public:
  AudioGraph() = default;
  ~AudioGraph() = default;

  MixerNodePtr add_mixer_node(IAudioGraphNodePtr parent = nullptr);
  InputNodePtr add_input_node(IInputOutputPtr input, IAudioGraphNodePtr parent = nullptr);
  OutputNodePtr add_output_node(IInputOutputPtr output, IAudioGraphNodePtr parent = nullptr);
  ProcessorNodePtr add_processor_node(IAudioGraphNodePtr parent = nullptr);

  IAudioGraphNodePtr get_root_node() const;
  std::vector<IAudioGraphNodePtr> get_leaf_nodes() const;

  std::string to_string() const;

private:
  IAudioGraphNodePtr add_node(IAudioGraphNodePtr node, IAudioGraphNodePtr parent = nullptr);

  void get_leaf_nodes_impl(IAudioGraphNodePtr parent, std::vector<IAudioGraphNodePtr> &leaf_nodes) const;

private:
  IAudioGraphNodePtr p_root_node;
};

} // namespace miniaudioengine::dataplane

#endif // __AUDIO_GRAPH_H__